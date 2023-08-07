

# 待学习的链接

[How to Reverse Engineer Software (Windows) in a Right Way](https://www.apriorit.com/dev-blog/364-how-to-reverse-engineer-software-windows-in-a-right-way)

[sanqiuai](https://blog.csdn.net/sanqiuai?type=blog) 这个人的反调试,我是没怎么看懂,好像很创新



# 时间记录器



被调试时，进程的运行速度大大降低

下面是一些用于时间记录的API

```c
GetLocalTime(&stStart);
dwStart = GetTickCount();
QueryPerformanceCounter(&liStart);
```



下面主要使用汇编指令来做相关的时间记录,API相比来说过于显眼

缺点: 你需要去触发时间记录器,也就是去单步运行它,它才会有一个时间差

如果你F9完整运行和你双击运行,其实没差别的,无法检测调试器的存在

只有你把时间记录器内联在关键步骤里面才会有一个很好的效果

所以这些API就可以上场了,就不用内联汇编了



x86版本

```c
#include <stdio.h>
#include <windows.h>

void Anti_dbg()
{
	DWORD dwDelta = 0;
	printf("Timing Check");
	__asm {
			pushad

			; --------------------------------------------------------
			rdtsc		//这里就会把t0计入寄存器
			push edx
			push eax	//把获取的数据放入栈中

			; ------------------------------
			xor eax, eax	//eax从0开始累计1,到0x3e8
			mov ecx, 1000 //一个循环的次数

			time_spend:
			inc eax
			loop time_spend
			; ------------------------------

			rdtsc //记录一下t1
			pop esi	// eax
			pop edi	// edx

			; ---------------------------------- - (t1)edx:eax - (t0)edi : esi

			cmp edx, edi
			ja you_are_debugging //如果存在很大的时间差,肯定就在调试,这是比较高位的原因
			sub eax, esi		//比较低位,
			cmp eax, 0xffffff
			jb no_one_debugging //如果时间差很小.就是正常运行

			//发现你在调试,就故意引发了一个异常

			you_are_debugging:
			xor eax, eax
			mov[eax], eax

			// debugger not found
			no_one_debugging :
			popad
	}
	MessageBox(0, "where are you?", "Xsir", 0);
	return;
}

int main()
{
	Anti_dbg();
	return 0;
}
```



x64版本

asm文件

```assembly
.CODE

MY_TEST PROC
	push rax;
	push rbx;
	push rcx;
	push rdx;
	push rsi;
	push rdi;

	rdtsc;		这里就会把t0计入寄存器
	push rdx;
	push rax;	把获取的数据放入栈中

 
	xor rax, rax;	eax从0开始累计1,到0x3e8
	mov rcx, 1000; 一个循环的次数

time_spend:
	inc rax;
	loop time_spend;
			 
	rdtsc; 记录一下t1
	pop rsi;	 eax
	pop rdi;	 edx

	; ---------------------------------- - (t1)edx:eax - (t0)edi : esi

	cmp rdx, rdi;
	ja you_are_debugging; 如果存在很大的时间差,肯定就在调试,这是比较高位的原因
	sub rax, rsi;		比较低位
	cmp rax, 16777215
	jb no_one_debugging ;如果时间差很小.就是正常运行

	;发现你在调试,就故意引发了一个异常

you_are_debugging:
	xor rax, rax;
	mov[rax], rax;

	;debugger not found
	no_one_debugging :
	pop rdi;
	pop rsi;
	pop rdx;
	pop rcx;
	pop rbx;
	pop rax;
	ret;

MY_TEST ENDP
 
END
```

c文件

```c
 
#include <Windows.h>
#include <stdio.h>

extern  int _stdcall MY_TEST();


void Anti_dbg()
{
	MY_TEST();
	MessageBoxA(0, "where are you?", "re@dqx", 0);
	return;
}

int main()
{
	Anti_dbg();
	return 0;
}
```





如果专用API的话

```c

#include <stdio.h>
#include <windows.h>

DWORD NormallCode(int x, int y)
{
	
	DWORD xx= GetTickCount();
	int z = x + y;
	z = z ^ x;
	z = z ^ y;
	z = z | (x + y);
	xx = GetTickCount() - xx;
	if (xx > 1000)
	{
		printf("you are dbg me!");
	}
	else
	{
		printf("no find you\n");
	}
	return z;
}

int main()
{
	NormallCode(1, 2);
	return 0;
}
```







# 软件|硬件的断点检测



## 软件断点0xCC



### 0xCC个数检测



在没有下断点的情况下,0xCC的个数是固定的

如果下了多个软件断点,0xCC的个数会增加

所以我们可以统计0xCC的个数来判断是否下了断点



本来想开一个线程,来特意的检测0xCC的个数

哪知道我用IDA调试,CPU的控制权一直落在主线程

我主线程运行完毕,CreateThread的线程都还没有开始运行

所以只能来一个顺序执行检测0xCC断点



```c
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <Windows.h>


DWORD WINAPI CheckDebug(PVOID pParam);
DWORD NormallCode(int x, int y);

BYTE* __stdcall getRealCall(BYTE* lp)
{
	return  *(DWORD*)(lp + 1) + (BYTE*)lp + 5;
}
int main()
{

	//HANDLE hThread = CreateThread(NULL, 0, CheckDebug, "re@dqx", 0, NULL);
	CheckDebug("check you");
	NormallCode(1, 2);
	//WaitForSingleObject(hThread, INFINITE);
	return 0;
}
DWORD CALLBACK CheckDebug(PVOID pParam)
{
	BYTE* bAddr_start = getRealCall((BYTE*)NormallCode);//开始的地址
	DWORD func_size =   99;
	DWORD cnt_0xcc;
	int i;
	//while (1)
	//{
		cnt_0xcc = 0;
		for (i = 0; i< func_size; i++)//结束的地址
		{
			if (bAddr_start[i] == 0xCC)
				cnt_0xcc++;
		}
		if (cnt_0xcc > 6)
		{
			printf("Find YOu %d\n", cnt_0xcc - 6);
		}
		//Sleep(1000);//慢点检测
	//}
	return 0;
}
DWORD NormallCode(int x, int y)
{
	int z = x + y;
	z = z ^ x;
	z = z ^ y;
	z = z | (x + y);
	return z;
}
```

 

效果图

![image-20230807180128949](img/image-20230807180128949.png)



### 哈希校验

其实就是指定一个区段, 然后计算一个摘要值

如果该摘要值发生了变化,那么就说明被动过了

对于调试器来说,0xCC就算一种情况



 

```c
#include <stdio.h>
#include <windows.h>

DWORD g_dwOrgChecksum = 0xF5934986;

int main();
void Checksum()
{
    BOOL bDebugging = FALSE;
    DWORD count = 0;

    __asm {
        mov ecx, offset main
        mov esi, offset Checksum
        sub ecx, esi            // ecx : loop count (buf size)
        xor eax, eax            // eax : checksum
        xor ebx, ebx

 _CALC_CHECKSUM :
        movzx ebx, byte ptr ds : [esi]
        add eax, ebx
        rol eax, 1
        inc esi
        loop _CALC_CHECKSUM

        cmp eax, g_dwOrgChecksum
        je _NOT_DEBUGGING
        mov bDebugging, 1

        _NOT_DEBUGGING:
    }

    if (bDebugging)
        printf("  => Debugging!!!\n\n");
    else
        printf("  => Not debugging...\n\n");
}

int main()
{
    Checksum();
    return 0;
}
```





### 软件断点去除|软件断点修改



其实是基于0xCC的检测

原理:

这里我么用的是读取原来的字节码

在调试中,开启一个线程,不断的去循环校验,然后去读取字节码

读取过程中就两两比对,发现差异

如果有差异,就说明该字节码被修改为了0xCC

就前面提到的MD5哈希校验,,,其实就是对原始代码的一个摘要

但是我们并没有说是直接拷贝一份原始字节码.写死在内存里面

然后在调试运行的时候,去动态的读取调试器下的字节码,

然后把读取的字节码和我们写死在内存里面的字节码来一个一一的比较

如果这样做确实很准确...但是会显得比较臃肿

我们采取的措施是程序运行的时候开一个线程

动态的读取字节码到一个内存空间叫做asm_old

该线程不断的死循环,不断的读取字节码和以前的asm_old做一个比较

然后根据比较结果,如果不同就说明下了有0xCC断点

但是这样有一个问题

我们无法预料我们创建的线程是读取的字节码就是原始的字节码

此话怎讲,,,也就是如果IDA在启动前下了断点

比如在0x00411CBC处下来断点0xCC,原始的字节码是0xE8

于是程序启动

我们开的线程读取的字节码asm_old讲不会是0xE8,而是0xCC

包括后面的比较,,,也就是我们开的线程其实一直是在拿着断点处的0xCC == 0xCC

做一个判断,而不是拿着0xCC == 0xE8

但是,如果我们在动态调试的时候,取出了启动前下的断点

那么IDA就恢复了那个字节码

也就是asm_old[0x00411CBC]=0xCC 但是恢复后[0x00411CBC]=x0xE8

于是就会发生一个不相等的情况

此刻的检测就是启动前下了断点,然后动态调试的时候取消了,发生了字节码的差异,发现了在调试

也就是动态的取消断点或者动态的下断点就会发生检测

源码如下,需要根据PE文件的代码区信息做一些参数的修改

```
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<windows.h>
#include <Wincrypt.h>
#include <math.h>
#define I 20
#define R 340

BYTE* lp = 0x00411000;//去PE信息里面查看
int   len = 0x5E00;//去PE信息里面查看

DWORD WINAPI ThreadProc(LPVOID lpParam)
{
    DWORD oldProtect;
    BYTE* old_asm;
    int i;
    BOOL result = VirtualProtect(lp, len, PAGE_EXECUTE_READWRITE, &oldProtect);
    if (!result)
    {
        // 处理错误
        exit(-1);
    }
    old_asm = malloc(len + 1);
    if (old_asm)
    {
        memcpy(old_asm, lp, len);
    }
    else
    {
        exit(-1);
    }
    while (1)
    {
        for (i = 0; i < len; i++)
        {
            if (old_asm[i] != lp[i])//动态取消断点
            {
                //MessageBox(NULL, ":)", "do not dbg me", MB_OK);
                lp[i] = old_asm[i]+i;
            }
        }
    }
    return 0;
}
void testFunc()
{
    int time;
    for (float y = 1.5f; y > -1.5f; y -= 0.1f)
    {
        for (float x = -1.5f; x < 1.5f; x += 0.05f)
        {
            float a = x * x + y * y - 1;
            putchar(a * a * a - x * x * y * y * y <= 0.0f ? '*' : ' ');
        }
        putchar('\n');
    }

    for (;;)
    {
        system("color a");
        for (time = 0; time < 0xffff; time++);

        system("color b");
        for (time = 0; time < 0xffff; time++);

        system("color c");
        for (time = 0; time < 0xffff; time++);

        system("color d");
        for (time = 0; time < 0xffff; time++);

        system("color e");
        for (time = 0; time < 0xffff; time++);

        system("color f");
        for (time = 0; time < 0xffff; time++);

        system("color 0");
        for (time = 0; time < 0xffff; time++);

        system("color 1");
        for (time = 0; time < 0xffff; time++);

        system("color 2");
        for (time = 0; time < 0xffff; time++);

        system("color 3");
        for (time = 0; time < 0xffff; time++);

        system("color 4");
        for (time = 0; time < 0xffff; time++);

        system("color 5");
        for (time = 0; time < 0xffff; time++);

        system("color 6");
        for (time = 0; time < 0xffff; time++);

        system("color 7");
        for (time = 0; time < 0xffff; time++);

        system("color 8");
        for (time = 0; time < 0xffff; time++);

        system("color 9");
        for (time = 0; time < 0xffff; time++);

    }
    return ;
}
int main()
{
    DWORD threadID;
    HANDLE hThread;
    hThread = CreateThread(NULL, 0, ThreadProc, NULL, 0, &threadID);	//创建线程 但是最好先于main函数执行
    testFunc();
    return 0;
}
```

但是,,,,我们一般下断点,多是启动前下断点,启动后动态的下断点几率要小一些

所以如何在基于启动后读取字节码到old_asm然后检测?

其实上面所说的,如果对比没有发生差异,,,,就检测不出来

发生差异的时候,就是动态的下了一个断点或者动态的取消了一个断点

没办法做到一启动就发现你下了断点

## 硬件断点

基于硬件断点的监测

DR0、Dr1、Dr2、Dr3用于设置硬件断点，

由于只有4个硬件断点寄存器，所以同时最多只能设置4个硬件断点。

DR4、DR5由系统保留。

DR6、DR7用于记录Dr0-Dr3中断点的相关属性。

如果没有硬件断点，那么DR0、DR1、DR2、DR3这4个寄存器的值都为0

```
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <Windows.h>
#include <tlhelp32.h>

BOOL CheckDebug();
DWORD NormallCode(int x, int y);

int main()
{
    NormallCode(1, 2);
    if (CheckDebug())
    {
        printf("Get You\n");
    }
    else
    {
        printf("where are you\n");
    }
    NormallCode(3, 4);
	return 0;
}
BOOL CheckDebug()
{
    CONTEXT context;
    HANDLE hThread = GetCurrentThread();
    context.ContextFlags = CONTEXT_DEBUG_REGISTERS;
    GetThreadContext(hThread, &context);
    if (context.Dr0 != 0 || context.Dr1 != 0 || context.Dr2 != 0 || context.Dr3 != 0)
    {
        return TRUE;
    }
    return FALSE;
}
DWORD NormallCode(int x, int y)
{
    int z = x + y;
    z = z ^ x;
    z = z ^ y;
    z = z | (x + y);
    return z;
}
```

# 故意触发错误(非异常)

win32 编程

A P I

```
CloseHandle((HANDLE)0xBAAD); //引起异常
LoadLibiary("O(∩_∩)O");//返回值确定
```

CloseHandl会触发异常的(算是中级或者高级的异常)

```
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>

EXCEPTION_DISPOSITION ExceptionRoutine(
    PEXCEPTION_RECORD ExceptionRecord,
    PVOID             EstablisherFrame,
    PCONTEXT          ContextRecord,
    PVOID             DispatcherContext)
{
    if (EXCEPTION_INVALID_HANDLE == ExceptionRecord->ExceptionCode)
    {
        printf("Stop debugging program!\n");

    }
    return ExceptionContinueExecution;
}
int main()
{
    __asm
    {
        // set SEH handler
        push ExceptionRoutine
        push dword ptr fs : [0]
        mov  dword ptr fs : [0] , esp
    }
    CloseHandle((HANDLE)0xBAAD);
    __asm
    {
        // return original SEH handler
        mov  eax, [esp]
        mov  dword ptr fs : [0] , eax
        add  esp, 8
    }
    puts("Where are you?\n");
    return 0;
}
```

# 内存扫描之特征码监测(失败)

比如OD加载了xxx.exe

然后我们扫描xxx.exe,去发现也没有字符串OD

可能原理就是这个