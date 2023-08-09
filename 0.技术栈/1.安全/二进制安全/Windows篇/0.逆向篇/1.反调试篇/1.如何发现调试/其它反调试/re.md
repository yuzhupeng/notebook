# 其它反调试

# 引言

高级反调试技术的程序包含大量垃圾代码、条件分支语句、循环语句、加密/解密代码以及“深不见底”的调用树（Call-Tree)，

# 扰乱代码对齐

其中int 0x2D 就是一个很好的扰乱代码对齐的方式

花指令是其中一个实践运用

# 代码重组

比如

```
009E24F0 MOV EAX, 009E24F8
009E24F5 XOR DWORD PTR DS:[EAX], 10
009E24F8 INC DWORD PTR DS:[ESI]
;执行这2条指令后
;009E24F8的指令将发生一个很大的变化
```

感觉提取了SMC的精华

但是代码重组的构造会比较的难

该技术的另一个优点是，用户在解码的代码处设置软件断点（0xCC)后，程序运行就会引发运行时错误。这是因为，

设有断点的区域被0xCC取代，从而出现完全不同的计算结果（OllyDbg 调试器中，为了保护设有软件断点的地址中的数据，干脆禁止写入新值

# 防附加(失败)



为了使运行中进程能够即时中断到调试器中，操作系统提供了一个函数DbgUiRemoteBreakin，

其内部通过调用DbgBreakPoint产生一个中断异常从而被调试器捕获，

为了实现及时中断我们需要在运行中的进程中创建远程线程，

线程回调函数就是DbgUiRemoteBreakin函数。

实际在我们附加进程时，调试器就时这么做的。

所以我们通过hookDbgUiRemoteBreakin函数可以达到反附加的目的。

自己的理解:

调试器通过调用DbgUiRemoteBreakin达到调试的一些效果

然后我们hook这个API,然后等调试器一调用就退出了

```
#define _CRT_SECURE_NO_WARNINGS
#include<windows.h>
#include<stdio.h>

int main(int argc, char* argv[])
{
	BYTE	bBuffer[0x10] = { 0 };
	DWORD	dwBreakAddress;					//DbgUiRemoteBreakin函数的地址
	DWORD	dwOldProtect;
	DWORD	dwNum;
	char flag1[] = "flag{xxxxxxxx}";
	char flag2[16];
	dwBreakAddress = (DWORD)GetProcAddress(LoadLibrary("ntdll.dll"), "DbgUiRemoteBreakin");
	bBuffer[0] = 0xE9;												//jmp指令字节码
	*((DWORD*)(bBuffer + 1)) = (DWORD)ExitProcess - dwBreakAddress;	//ExitProcess函数偏移地址

	VirtualProtect((LPVOID)dwBreakAddress, 0x10, PAGE_EXECUTE_READWRITE, &dwOldProtect);
	WriteProcessMemory(GetCurrentProcess(), (LPVOID)dwBreakAddress, bBuffer, 5, &dwNum);
	printf("%x %d\n", dwBreakAddress,dwNum);
	for (int i = 0; i < 5;i++)
	{
		printf("%X", bBuffer[i]);
	}
	puts("");
	VirtualProtect((LPVOID)dwBreakAddress, 0x10, dwOldProtect, &dwOldProtect);

	//此死循环是为了检测
	printf("PLZ flag:");
	scanf("%s", flag2);
	if (!strcmp(flag1, flag2))
	{
		printf("You Got it\n");
	}
	else
	{
		printf("Wrong input\n");
	}
	system("pause");
	return 0;
}
```



# **探测虚拟机运行**

Vmware为真主机与虚拟机之间提供了相互沟通的通讯机制,

它使用IN指令来读取特定端口的数据以进行两机通讯,

但由于IN指令属于特权指令,在真机中运行将会触发`EXCEPTION_PRIV_INSTRUCTION`异常,

而在虚拟机中并不会发生异常,我们可以利用这个特性判断代码是否在虚拟机中.

```
 #include <windows.h>
 #include <stdio.h>
 
 bool IsInsideVM()
 {
     bool VmWare = true;
     __try
     {
         __asm
         {
             mov    eax, 'VMXh'
             mov    ebx, 0
             mov    ecx, 10// 指定功能号
             mov    edx, 'VX'
             in     eax, dx// 从端口dx读取VMware版本到eax
             cmp    ebx, 'VMXh'// 判断ebx中是否包含VMware版本VMXh
             setz[VmWare]// 设置返回值 True/False
         }
     }
     __except (EXCEPTION_EXECUTE_HANDLER)
     {
         VmWare = false;// 如果未处于虚拟机中,将会产生异常
     }
     return VmWare;
 }
 
 int main()
 {
     int ret = IsInsideVM();
 
     if (ret == 1)
         printf("当前代码在虚拟机中 \n");
     else
         printf("宿主机 \n");
 
     system("pause");
     return 0;
 }
 
```