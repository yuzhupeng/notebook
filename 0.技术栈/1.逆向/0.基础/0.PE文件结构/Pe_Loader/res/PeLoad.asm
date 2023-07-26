.386
.model flat,stdcall
option casemap:none
 
include windows.inc
include msvcrt.inc
include user32.inc
include kernel32.inc
 
includelib msvcrt.lib
includelib user32.lib
includelib kernel32.lib
 
.data
IMAGE_SIZE equ 2000h  ;程序加载更改项
 
.const
g_fileName db "helloPe.exe",0 ;程序加载更改项
 
.code
org IMAGE_SIZE - 1000h ;这里减1000是程序本身具有头文件可以不拷贝，直接利用
 
 
checkPE proc lpAddress:LPVOID
     
    mov esi,lpAddress
    assume esi : ptr IMAGE_DOS_HEADER
     
    .if [esi].e_magic != 'ZM'
        mov eax,0
        ret
    .endif
     
    ;指向PE文件头
    add esi,[esi].e_lfanew
    assume esi : ptr IMAGE_NT_HEADERS
    .if[esi].Signature !='EP'
        mov eax,0
        ret
    .endif
     
    ret
checkPE endp
 
LoadPe proc uses esi ebx edi hMod:HMODULE
    LOCAL @hFile:HANDLE
    LOCAL @hMap:HANDLE
    LOCAL @lpAddress:LPVOID
    LOCAL @NumberOfSections:dword ;节区表数量
    LOCAL @SizeOfOptional:dword ;可选头大小
    LOCAL @AddrOfEntryPoint:dword;程序执行入口
    LOCAL @SizeOfImage:dword;所有头部加节表大小
    LOCAL @SizeOfHeaders:dword;头部占空间的大小
    LOCAL @dwOld:dword
    LOCAL @SectionAddr:dword
    LOCAL @importAddr:dword
    LOCAL @zeroIDD :IMAGE_DATA_DIRECTORY
    LOCAL @DllhMod:HMODULE
     
    invoke RtlZeroMemory, addr @zeroIDD,sizeof IMAGE_DATA_DIRECTORY ;设置全0结构体检测地址是否为0
     
    ;读取文件
    invoke CreateFile,offset g_fileName,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0
    .if eax==INVALID_HANDLE_VALUE
        jmp DISPOSE_ERROR
    .endif 
    mov @hFile,eax
     
    ;创建内存位图
    invoke CreateFileMapping,@hFile,NULL,PAGE_READONLY,0,0,NULL
    .if eax==NULL
        jmp DISPOSE_ERROR
    .endif 
    mov @hMap,eax
     
    ;位图映射到进程,复制可执行文件的头部到PE装载器申请内存的首地址
    invoke MapViewOfFile,@hMap,FILE_MAP_READ,0,0,0
    .if eax==NULL
        jmp DISPOSE_ERROR
    .endif 
    mov @lpAddress,eax
     
    ;检查PE的头文件格式是否合法
    invoke checkPE,@lpAddress
    .if eax ==NULL
        jmp DISPOSE_ERROR
    .endif
     
 
    mov esi,@lpAddress
    assume esi : ptr IMAGE_DOS_HEADER
     
    ;需要拿到的属性filehead 的可选头大小，节区表数量，需要optionhead里AddressOfEntryPoint
    ;SizeOfHeaders
         
    add esi,[esi].e_lfanew
    assume esi : ptr IMAGE_NT_HEADERS
         
    movzx eax,[esi].FileHeader.NumberOfSections
    mov @NumberOfSections,eax
     
    movzx eax,[esi].FileHeader.SizeOfOptionalHeader
    mov @SizeOfOptional,eax
     
    mov eax,[esi].OptionalHeader.AddressOfEntryPoint ;拿到程序入口点地址
    mov @AddrOfEntryPoint,eax
    mov eax,[esi].OptionalHeader.SizeOfImage ;拿到头部+节区表的总长度
    mov @SizeOfImage,eax
    mov eax,[esi].OptionalHeader.SizeOfHeaders
    mov @SizeOfHeaders,eax
     
    ;这里获取导入表的地址,等于导入信息表的下标1*结构体的大小
    lea eax,[esi].OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT * sizeof IMAGE_DATA_DIRECTORY]
    mov @importAddr,eax
     
    ;拷贝可执行文件的头部到当前文件的模块载入地址
     
    ;更改代码段的属性
    invoke VirtualProtect,hMod,@SizeOfImage,PAGE_EXECUTE_READWRITE,addr @dwOld
 
    invoke crt_memcpy, hMod, @lpAddress,@SizeOfHeaders
    .if eax==NULL
        jmp DISPOSE_ERROR
    .endif
         
    ;5 复制区块表的相关数据到PE装载器申请内存的相应地址,注意：记得加模块偏移
    lea esi,[esi].OptionalHeader
    add esi,@SizeOfOptional
     
    ;拿到了区块表的起始地址
    mov @SectionAddr,esi
     
    ;转换esi地址为IMAGE_SECTION_HEADER结构体
    mov esi,@SectionAddr
    assume esi:ptr IMAGE_SECTION_HEADER
    xor ecx,ecx
     
    .while ecx<@NumberOfSections
        .if [esi].SizeOfRawData!=0 ;首先判断文件偏移是否为0，为0是未初始化的节区
        mov eax,[esi].PointerToRawData ;文件的偏移加文件在内存中的基地址得到文件区块的起始位置
        add eax,@lpAddress ;在文件中的偏移加文件的基地址
         
        mov edi,[esi].VirtualAddress;文件在内存中映射的位置
        add edi,hMod ;在内存中的偏移加内存中的基地址
         
        invoke crt_memcpy ,edi,eax,[esi].SizeOfRawData
        .if eax ==NULL
            jmp DISPOSE_ERROR
        .endif
         
        .endif 
        inc ecx
        add esi,sizeof IMAGE_SECTION_HEADER
    .endw
     
    ;加载导入表信息
    mov esi,@importAddr
    mov esi,dword ptr[esi]
    add esi,hMod
    assume esi:ptr IMAGE_IMPORT_DESCRIPTOR
     
    invoke crt_memcmp,esi,addr @zeroIDD,sizeof @zeroIDD
    .while eax!=0
        mov eax,[esi].FirstThunk
        add eax ,hMod
        .if dword ptr [eax] != 0 ;IAT表不为空
 
            mov ebx,[esi].OriginalFirstThunk
            .if ebx == 0
             mov ebx,[esi].FirstThunk
            .endif
             
            add ebx,hMod ;不管是IAT表为空还是INT表为空，值都会保存在Ebx
             
            mov edi,[esi].FirstThunk
            add edi,hMod ;edi得到iat的位置
             
            mov eax,[esi].Name1
            add eax,hMod
             
            ;获取模块的名称
            push ecx
            invoke LoadLibrary,eax
            pop ecx
            mov @DllhMod,eax
             
            mov ecx,0
            .while dword ptr[ebx+ecx*4] !=0
                mov eax,[ebx+ecx*4]
                .if eax&80000000h
                    ;序号导入
                    and eax,0ffffh ;拿到了低4位的值
                 
                .else
                    ;名称导入
                    add eax,hMod
                    assume eax:ptr IMAGE_IMPORT_BY_NAME
                    lea eax,[eax].Name1
                .endif         
                    ;此时eax保存了序号或者函数名
                    push ecx
                    invoke GetProcAddress, @DllhMod,eax
                    pop ecx
                    mov [edi+ecx*4],eax
                    inc ecx
            .endw
             
        .endif
         
        add esi,sizeof IMAGE_IMPORT_DESCRIPTOR
        invoke crt_memcmp,esi,addr @zeroIDD,sizeof @zeroIDD
    .endw
     
    ;跳转到程序入口点
    mov eax,hMod
    add @AddrOfEntryPoint,eax
    call @AddrOfEntryPoint
     
DISPOSE_ERROR:
 
    .if @lpAddress !=NULL
        invoke UnmapViewOfFile,@lpAddress
    .endif
     
    .if @hMap!=NULL
        invoke CloseHandle,@hMap
    .endif
 
    .if @hFile!=INVALID_HANDLE_VALUE
        invoke CloseHandle,@hFile
    .endif
    ret
LoadPe endp
 
 
START:
    ;获取程序载入模块的确切地址
    invoke GetModuleHandle,NULL
    invoke LoadPe,eax
    invoke ExitProcess,0
end START