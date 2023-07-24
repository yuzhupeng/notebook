# 2023-07-04-shellcode木马攻击

小常识:  APT海莲花的英文名字  OceanLotus

# 身份证

<aside>
💡 名字: 36 ASEAN Summit 26-06-2020 Conference.doc~.exe

SHA256: dbde2b710bee38eb3ff1a72b673f756c27faa45d5c38cbe0f8a5dfccb16c18ba

MD5: 

SHA1: 4a41bc81b27374b8a711794a7b27d51700403341

样本格式: SFX(RAX自解压)

回连C2: tripplekill.mentosfontcmb.com

</aside>

长这个样子

![Untitled](2023-07-04-shellcode%E6%9C%A8%E9%A9%AC%E6%94%BB%E5%87%BB%20f32ccbf80a8b4901b6fda574a8b756f1/Untitled.png)

看上去是一个word, 但他本来就是一个exe

# 运行效果

通过网上的样本分析得, 很多东西都是为了解密一段shellcode,然后执行它

于是我用了比较直接的形式解密了shellcode然后执行它

直接的效果就是wps打开了一个word,然后就没了

# 基本PE信息

![Untitled](2023-07-04-shellcode%E6%9C%A8%E9%A9%AC%E6%94%BB%E5%87%BB%20f32ccbf80a8b4901b6fda574a8b756f1/Untitled%201.png)

Die中,出现一些关键字, PE32和Zip

经过查询资料，推断该文件为自解压压缩文件

通过7-zip打开文件

![Untitled](2023-07-04-shellcode%E6%9C%A8%E9%A9%AC%E6%94%BB%E5%87%BB%20f32ccbf80a8b4901b6fda574a8b756f1/Untitled%202.png)

发现这些信息

APT组织加载恶意样本的方式多种多样，

其中白加黑的加载方式是一种常用的APT攻击手法，

也是海莲花APT组织最常用的APT攻击手法之一，统计最近几年国内发布的OceanLotus(海莲花)APT组织报告中的白+黑程序名，

如下所示

![Untitled](2023-07-04-shellcode%E6%9C%A8%E9%A9%AC%E6%94%BB%E5%87%BB%20f32ccbf80a8b4901b6fda574a8b756f1/Untitled%203.png)

之前我们用7-zip打开过exe文件,我们可以解压打开它(woc,既然可以把一个exe 解压缩)

内含有

```c
-MicrosoftUpdate.exe
-SoftwareUpdateFiles.locale
-SoftwareUpdateFiles.dll
-SoftwareUpdateFiles.Resources
	-en.lproj
		- SoftwareUpdateFilesLocalized.dll
```

本次样本采用白+黑的模式运行

启动白程序，然后利用升级程序MicrosoftUpdate.exe，

加载SoftwareUpdateFiles.Resources目录下的恶意程序SoftwareUpdateFilesLocalized.dll

SoftwareUpdateFilesLocalized.dll会读取目录下的SoftwareUpdateFiles.locale文件

然后解密SoftwareUpdateFiles.locale并执行

# Dll 加载

主要是寻找SoftwareUpdateFilesLocalized.dll如何被加载的

MicrosoftUpdate.exe的导入表里面有SoftwareUpdateFiles.dll,但是没有SoftwareUpdateFilesLocalized.dll

而且查看MicrosoftUpdate.exe,SoftwareUpdateFiles.dll,SoftwareUpdateFilesLocalized.dll的字符串信息

并没发现SoftwareUpdateFilesLocalized.dll的字符串信息

查资料后推测: SoftwareUpdateFilesLocalized.dll由其他系统dll加载 

(后来证实发现,是SoftwareUpdateFiles.dll,加载的SoftwareUpdateFilesLocalized.dll)

还有一种情况是把数据解密后才会有目标字符,只是猜测罢了串

```c
WOW64 process has been detected (pid=7284)
400000: process C:\Users\virus\Desktop\7579AEDE6A223C96231AD30472A060DB\MicrosoftUpdate.exe has started (pid=7284)
...
76070000: loaded C:\Windows\SysWOW64\msvcrt.dll
10000000: loaded C:\Users\virus\Desktop\7579AEDE6A223C96231AD30472A060DB\SoftwareUpdateFiles.dll
...
72D30000: loaded C:\Windows\SysWOW64\netutils.dll
76130000: loaded C:\Windows\SysWOW64\bcrypt.dll
74F30000: loaded C:\Windows\SysWOW64\IMM32.DLL
72C70000: loaded C:\Users\virus\Desktop\7579AEDE6A223C96231AD30472A060DB\SoftwareUpdateFiles.Resources\en.lproj\SoftwareUpdateFilesLocalized.dll
```

可以看见在 C:\Windows\SysWOW64\msvcrt.dll加载后,

就加载了C:\Users\virus\Desktop\7579AEDE6A223C96231AD30472A060DB\SoftwareUpdateFiles.dll

SoftwareUpdateFiles.dll是存在于导入表的

然后看SoftwareUpdateFilesLocalized.dll是怎么加载的..

通过xdebug和IDA 2次测试,发现IMM32.DLL加载后,才会加载SoftwareUpdateFilesLocalized.dll

然后看IMM32.DLL是否引入SoftwareUpdateFilesLocalized.dll

![Untitled](2023-07-04-shellcode%E6%9C%A8%E9%A9%AC%E6%94%BB%E5%87%BB%20f32ccbf80a8b4901b6fda574a8b756f1/Untitled%204.png)

发现,并没有引入

可能是IMM32.DLL动态加载的

然后猜测是IMM32.DLL调用了Kernel32的LoadLibiary加载的

![Untitled](2023-07-04-shellcode%E6%9C%A8%E9%A9%AC%E6%94%BB%E5%87%BB%20f32ccbf80a8b4901b6fda574a8b756f1/Untitled%205.png)

所以,可以去对应的API下断点

![Untitled](2023-07-04-shellcode%E6%9C%A8%E9%A9%AC%E6%94%BB%E5%87%BB%20f32ccbf80a8b4901b6fda574a8b756f1/Untitled%206.png)

虽然导入表只有LoadLibiaryExW,但是我还是在可能的地方都设置了断点,比如在上面

```c
72C80000: loaded C:\Windows\SysWOW64\oledlg.dll
74F30000: loaded C:\Windows\SysWOW64\IMM32.DLL
```

于是F9后

它停在了

![Untitled](2023-07-04-shellcode%E6%9C%A8%E9%A9%AC%E6%94%BB%E5%87%BB%20f32ccbf80a8b4901b6fda574a8b756f1/Untitled%207.png)

what?

难道不是IMM32.DLL加载的,,明明IMM32.DLL只调用了Kernel32的LoadLibiaryExW

为什么这里是LoadLibarayW?

观察函数的参数

![Untitled](2023-07-04-shellcode%E6%9C%A8%E9%A9%AC%E6%94%BB%E5%87%BB%20f32ccbf80a8b4901b6fda574a8b756f1/Untitled%208.png)

就是一个字符串,内容是一个dll地址

```c
7579AEDE6A223C96231AD30472A060DB\7579AEDE6A223C96231AD30472A060DB~\SoftwareUpdateFiles.Resources\en.lproj\SoftwareUpdateFilesLocalized.dll
```

之后再次F9,就不会停在LoadLibary相关的API,说明后面也没有加载SoftwareUpdateFilesLocalized.dll

于是我们算是知道了 是LoadLibaryW加载的SoftwareUpdateFilesLocalized.dll

但是很懵,到底是谁调用LoadLibiaryW加载的SoftwareUpdateFilesLocalized.dll

后来想了很久…死去的记忆再次复燃

到底是谁调用的LoadLibaryW, 其实你看LoadLibaryW函数返回到哪个模块

说明就是那个模块就调用了LoadLibaryW呗

这不就是<<逆向工程核心原理>>讲过的吗🤣

![Untitled](2023-07-04-shellcode%E6%9C%A8%E9%A9%AC%E6%94%BB%E5%87%BB%20f32ccbf80a8b4901b6fda574a8b756f1/Untitled%209.png)

于是去往返回地址就在SoftwareUpdateFiles.dll模块,真离谱

通过返回地址和IDA的静态分析,发现在

int __thiscall ASUResourceProxy::ASUResourceProxy(int this, wchar_t *a2)函数里面加载了dll

![Untitled](2023-07-04-shellcode%E6%9C%A8%E9%A9%AC%E6%94%BB%E5%87%BB%20f32ccbf80a8b4901b6fda574a8b756f1/Untitled%2010.png)

然后很奇怪的是

对于SoftwareUpdateFilesLocalized.dll的DllMain

```c
BOOL __stdcall DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
  if ( fdwReason == 1 )
    sub_10001020((int)hinstDLL);
  return 1;
}
```

进入sub_10001020((int)hinstDLL);是对SoftwareUpdateFiles.dll做一个API的Hook

![Untitled](2023-07-04-shellcode%E6%9C%A8%E9%A9%AC%E6%94%BB%E5%87%BB%20f32ccbf80a8b4901b6fda574a8b756f1/Untitled%2011.png)

也就是SoftwareUpdateFiles.dll (A)加载SoftwareUpdateFilesLocalized.dll(B)

然后SoftwareUpdateFilesLocalized.dll(B)调用dllMain又把SoftwareUpdateFiles.dll(A)给Hook

![Untitled](2023-07-04-shellcode%E6%9C%A8%E9%A9%AC%E6%94%BB%E5%87%BB%20f32ccbf80a8b4901b6fda574a8b756f1/Untitled%2012.png)

为什么这样做呢?

对于SoftwareUpdateFiles.dll加载SoftwareUpdateFilesLocalized.dll的时候

![Untitled](2023-07-04-shellcode%E6%9C%A8%E9%A9%AC%E6%94%BB%E5%87%BB%20f32ccbf80a8b4901b6fda574a8b756f1/Untitled%2013.png)

它意思就是说,如果SoftwareUpdateFilesLocalized.dll加载失败了,那就不调用没有下面这句话

```c
.text:10002F53 E8 08 F4 FF FF               
 call    ?ReportError@ASUResourceProxy@@AAEXXZ ; ASUResourceProxy::ReportError(void)
```

也就是SoftwareUpdateFilesLocalized.dll加载失败后不报错

![Untitled](2023-07-04-shellcode%E6%9C%A8%E9%A9%AC%E6%94%BB%E5%87%BB%20f32ccbf80a8b4901b6fda574a8b756f1/Untitled%2014.png)

然后对MicrosoftUpdate.exe的函数hook

# shellcode ****加载****

分析SoftwareUpdateFilesLocalized.dll

对于存在SHELLCODE的样本，其加载原理大多通过"VirtualAlloc"函数将SHELLCODE注入到某个内存空间中,

所以寻找VirtualAlloc的交叉引用即可

于是就可以发现一个函数 `void __noreturn sub_100010C0()`

在SoftwareUpdateFilesLocalized.dll被加载后,在一些关键位置下了断点

第1次 F9 来到SoftwareUpdateFilesLocalized.dll的HMODULE __usercall sub_10001020@<eax>(int a1@<eax>)

然后对SoftwareUpdateFiles.dll进行一个Hook,让它不报错

然后再次对 MicrosoftUpdate.exe Hook

![Untitled](2023-07-04-shellcode%E6%9C%A8%E9%A9%AC%E6%94%BB%E5%87%BB%20f32ccbf80a8b4901b6fda574a8b756f1/Untitled%2015.png)

可以看出,他是Hook了12个字节, 2+4+2+4

效果如下

Hook前

![Untitled](2023-07-04-shellcode%E6%9C%A8%E9%A9%AC%E6%94%BB%E5%87%BB%20f32ccbf80a8b4901b6fda574a8b756f1/Untitled%2016.png)

Hook后 去往SoftwareUpdateFilesLocalized.dll 解密二进制文件SoftwareUpdateFiles.locale让他执行

![Untitled](2023-07-04-shellcode%E6%9C%A8%E9%A9%AC%E6%94%BB%E5%87%BB%20f32ccbf80a8b4901b6fda574a8b756f1/Untitled%2017.png)

于是第二次F9,出现了很多C+= Exception, 也就是F9了很多次,每次把异常交个程序自己处理

后面又来到了SoftwareUpdateFiles.dll

然后它再次加载SoftwareUpdateFilesLocalized.dll

然后再F9,就发现它去往了 二进制文件解密函数

该函数前面干的事情就是获取路径

```c
GetModuleFileNameW(0, Filename, 0x104u);      // 7579AEDE6A223C96231AD30472A060DB\MicrosoftUpdate.exe
PathRemoveFileSpecW(Filename);                // 7579AEDE6A223C96231AD30472A060DB (字符串00截断) MicrosoftUpdate.exe
PathAppendW(Filename, L"SoftwareUpdateFiles.locale");// 7579AEDE6A223C96231AD30472A060DB\SoftwareUpdateFiles.locale
```

后面就是读取文件,然后分配内存,然后解密

![Untitled](2023-07-04-shellcode%E6%9C%A8%E9%A9%AC%E6%94%BB%E5%87%BB%20f32ccbf80a8b4901b6fda574a8b756f1/Untitled%2018.png)

注意对内容解密后

它是直接

```c
push 0
call eax
```

也侧面说明了该二进制文件不是可执行的PE文件,而是shellcode

# shellcode 分析

这一次算是真正的发现了shellcode长什么样子

ShellCode加入大量无意义运算和无效跳转花指令，用于干扰分析

## 重定位分析

```nasm
debug047:00B90000 fcmovb  st, st(5) ; 根据{CF标志位,[esp+4*0]和[esp+4*5]}来做一一些操作
debug047:00B90002 mov     eax, 0DAB1435Bh
debug047:00B90007 fnstenv byte ptr [esp-0Ch]
debug047:00B9000B pop     ebp
debug047:00B9000C xor     ecx, ecx
debug047:00B9000E mov     ecx, 56B1Ah
debug047:00B90013 add     ebp, 4
debug047:00B90016 xor     [ebp+17h], eax
```

第一条指令运行: 如果st0<st5,那么st5=st0,否则啥也没发生

指令运行前

![Untitled](2023-07-04-shellcode%E6%9C%A8%E9%A9%AC%E6%94%BB%E5%87%BB%20f32ccbf80a8b4901b6fda574a8b756f1/Untitled%2019.png)

运行后

![Untitled](2023-07-04-shellcode%E6%9C%A8%E9%A9%AC%E6%94%BB%E5%87%BB%20f32ccbf80a8b4901b6fda574a8b756f1/Untitled%2020.png)

其实到这里,我们还是很懵b 第一句话是干嘛的

ps: 后面一点点马上解释

执行fnstenv byte ptr [esp-0Ch]前

![Untitled](2023-07-04-shellcode%E6%9C%A8%E9%A9%AC%E6%94%BB%E5%87%BB%20f32ccbf80a8b4901b6fda574a8b756f1/Untitled%2021.png)

当 fnstenv byte ptr [esp-4*3] 执行之后

ps: 他就是把FPU的一部分环境拷贝到esp-4*3的地方

![Untitled](2023-07-04-shellcode%E6%9C%A8%E9%A9%AC%E6%94%BB%E5%87%BB%20f32ccbf80a8b4901b6fda574a8b756f1/Untitled%2022.png)

可以看到[esp]的地方,他的值=0x00BD0000,这个数据很特殊

也就是他的值就是shellcode开始的地方,也就是说这个是是一个EIP

我尝试运行了几遍,发现[esp]始终的执行一个shellcode起始地址

后面拷打了很多次chatgpt,他才告诉我这是为什么

首先是 提问 

<aside>
💡 x86 fnstenv 会存入多少字节?

</aside>

`fnstenv`指令保存的字节数取决于使用的保存格式。在x86汇编中，有两种保存格式：

1. 短格式：`fnstenv [目标]`，它保存28个字节的FPU环境信息。
2. 长格式：`fnstenv [目标]`，它保存108个字节的FPU环境信息。

因此，`fnstenv`指令会存储28个字节或108个字节，具体取决于您选择的保存格式。

在大多数情况下，推荐使用较小的短格式，除非您需要完整的FPU环境信息。

然后,对于我们这里,他属于28字节的情况

<aside>
💡 然后问他,这28字节分别是什么

</aside>

1. 控制寄存器（Control Register，CW）：2个字节
2. 状态寄存器（Status Register，SW）：2个字节
3. 标签寄存器（Tag Register，TW）：2个字节
4. FPU指令指针偏移量（Instruction Pointer Offset）：2个字节
5. FPU指令指针选择器（Instruction Pointer Selector）：2个字节
6. 数据指针偏移量（Operand Pointer Offset）：2个字节
7. 数据指针选择器（Operand Pointer Selector）：2个字节
8. FPU状态寄存器（FPU Status Word）：2个字节
9. FPU控制寄存器（FPU Control Word）：2个字节
10. FPU标签寄存器（FPU Tag Word）：2个字节
11. FPU数据寄存器（FPU Data Registers）：8个字节

通过观察发现 数据指针选择器和FPU状态寄存器构成的4字节是shellcode的起始地址

于是问chargpt

<aside>
💡 数据指针选择器和FPU状态寄存器是否和当前运行的EIP有关系?

</aside>

数据指针选择器（Data Pointer Selector）和FPU状态寄存器（FPU Status Register）

与当前运行的EIP（Extended Instruction Pointer）在x86架构中是有关系的。

当执行FPU指令时，数据指针选择器会帮助确定从哪里加载或存储FPU数据寄存器的值。

而FPU状态寄存器则包含了FPU的运算状态和控制信息。这些寄存器的值会影响FPU指令的执行过程。

EIP则是指向当前正在执行的指令的指针。在程序执行期间，如果涉及到FPU操作，FPU数据的加载和保存可能会涉及到EIP的信息，

以确定正确的数据寄存器位置和正确的FPU状态。

因此，数据指针选择器和FPU状态寄存器与当前运行的EIP密切相关，用于在执行浮点运算时正确管理和控制FPU环境。

对于chatgpt的回答,只能说是了解一下,可能就真的是那么一回事情

这就是为什么shellcode一开始就要fcmovb st, st(5)

只是为了构造一个FPU指令,方便后面做一个环境的保存

至于为什么是[esp-0Ch],是为了刚好把那个FPU据指针选择器给pop出来

## SMC-1

然后就是SMC解码

可以看出是通过Loop循环异或实现SMC

![Untitled](2023-07-04-shellcode%E6%9C%A8%E9%A9%AC%E6%94%BB%E5%87%BB%20f32ccbf80a8b4901b6fda574a8b756f1/Untitled%2023.png)

一开始对00E0001B地址处解密

解密之后才是loop循环

![Untitled](2023-07-04-shellcode%E6%9C%A8%E9%A9%AC%E6%94%BB%E5%87%BB%20f32ccbf80a8b4901b6fda574a8b756f1/Untitled%2024.png)

```c
for(ebp=00E0001B,ecx=00E0001B;ecx>0;ebp+=4,ecx--)
{
	dword ptr [ebp+0x17]^=eax;
	eax+=dword ptr [ebp+0x17];
}
```

感觉这种加密算法更加小小的复杂

## 找打kernel32并获GetProcAddress

函数分析struct _LIST_ENTRY *__stdcall sub_11F0059(_DWORD *a1)

关于那个结构体到底怎么会是

查看笔记 

[细说  LDR_DATA_TABLE_ENTRY](https://www.notion.so/LDR_DATA_TABLE_ENTRY-aca3da52fbe14f0380c8d770b79feaad?pvs=21)

```c
struct _LIST_ENTRY *__stdcall sub_11F0059(_DWORD *a1)
{
  // 定义局部变量
  // ...

  if (a1)
    *a1 = 0;

  // 初始化 v20 数组和变量
  // ...

  strcpy((char *)v20, "kernel32.dll");
  Flink = NtCurrentPeb()->Ldr->InLoadOrderModuleList.Flink;
  while (1)
  {
    // 获取当前模块的名称长度并保存到 v24 中
    // ...

    // 进行模块名称比较，直到找到 "kernel32.dll" 或遍历结束
    // ...
  }

  // 如果找到 "kernel32.dll"，则继续查找 "GetProcAddress" 函数地址
  // ...
  
  return v3;
}
```

然后具体说一下他是怎么回事的

关于模块寻找的函数如下

![Untitled](2023-07-04-shellcode%E6%9C%A8%E9%A9%AC%E6%94%BB%E5%87%BB%20f32ccbf80a8b4901b6fda574a8b756f1/Untitled%2025.png)

对于每次遍历，如何处理？

![Untitled](2023-07-04-shellcode%E6%9C%A8%E9%A9%AC%E6%94%BB%E5%87%BB%20f32ccbf80a8b4901b6fda574a8b756f1/Untitled%2026.png)

遍历到ｋｅｒｎｅｌ３２．ｄｌｌ后如何获取导出函数？　如下

![Untitled](2023-07-04-shellcode%E6%9C%A8%E9%A9%AC%E6%94%BB%E5%87%BB%20f32ccbf80a8b4901b6fda574a8b756f1/Untitled%2027.png)

## 加载API和其他操作

一开始加载了很多的API,接着才是调用API的一些操作

![Untitled](2023-07-04-shellcode%E6%9C%A8%E9%A9%AC%E6%94%BB%E5%87%BB%20f32ccbf80a8b4901b6fda574a8b756f1/Untitled%2028.png)

接着后面就是一些奇奇怪怪的文件操作了

![Untitled](2023-07-04-shellcode%E6%9C%A8%E9%A9%AC%E6%94%BB%E5%87%BB%20f32ccbf80a8b4901b6fda574a8b756f1/Untitled%2029.png)

## 后续

```c
debug042:00D7003C 55                            push    ebp
debug042:00D7003D 54                            push    esp
debug042:00D7003E E8 16 00 00 00                call    sub_D70059            ; GetProcAddress和kernel32的base获取
debug042:00D7003E
debug042:00D70043 5A                            pop     edx                   ; edx将会是GetProcAddress
debug042:00D70044 E8 00 00 00 00                call    $+5
debug042:00D70044
debug042:00D70049 59                            pop     ecx
debug042:00D7004A 81 C1 8B 0A 00 00             add     ecx, 0A8Bh            ; 重定位的操作
debug042:00D70050 51                            push    ecx                   ; edx：重定位的一个shellcode地址
debug042:00D70051 52                            push    edx                   ; edx： GetProcAddress地址
debug042:00D70052 50                            push    eax                   ; eax: kernel32的baes
debug042:00D70053 E8 68 01 00 00                call    near ptr unk_D701C0
debug042:00D70053
debug042:00D70058 C3                            retn
```

有点小多,看网上没分析,我也摆大烂了

# 其它发现

热补丁

![Untitled](2023-07-04-shellcode%E6%9C%A8%E9%A9%AC%E6%94%BB%E5%87%BB%20f32ccbf80a8b4901b6fda574a8b756f1/Untitled%2030.png)