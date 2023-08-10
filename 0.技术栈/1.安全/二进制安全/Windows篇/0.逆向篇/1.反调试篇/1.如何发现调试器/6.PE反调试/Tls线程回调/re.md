# Tls回调

Tls线程回调 只是一个媒介

他不是具体检测反调试的手法

而是通过它,更好的辅助去检测调试器





注册方法

```c
#include <Windows.h>
#include <stdio.h>
#pragma comment(linker, "/INCLUDE:__tls_used")


VOID _stdcall TLS_A(PVOID DllHandle, DWORD Reason, PVOID Reserved);
VOID _stdcall TLS_B(PVOID DllHandle, DWORD Reason, PVOID Reserved);
//然后巴拉巴拉注册
#pragma data_seg(".CRT$XLB")
PIMAGE_TLS_CALLBACK p_thread_callback[] = { TLS_B, TLS_A };
#pragma data_seg()

//然后巴拉巴拉
int main()
{
	printf("hello i am main\n");
	return 0;
}
VOID _stdcall TLS_A(PVOID DllHandle, DWORD Reason, PVOID Reserved)
{
	printf("hello i am TLS A\n");
	return ;
}
VOID _stdcall TLS_B(PVOID DllHandle, DWORD Reason, PVOID Reserved)
{
	printf("hello i am TLS B\n");
	return ;
}
```





# 基于Tls引申出来的C++全局class的钩子函数



如果是局部的话,没太大意思

```c
#include<stdio.h>
class sayHello
{
public:
	sayHello()
	{
		printf("start\n");
	}

	~sayHello()
	{
		printf("end\n");
	}
};

sayHello gclass;

void test()
{
	sayHello lclass;
	return;
}
int main()
{
	test();
	printf("I Am Main\n");
	return 0;
}
```

所以全局的class的

构造函数和析构函数是先于main和后于main

而局部的构造函数和析构函数就不多说



# 基于tls引申出来的退出函数注册







运行它就大概知道了

可以和c++的全局析构做一个类比

```c
#include <stdio.h>
#include <stdlib.h>

void exit_fn1(void)
{
   printf("Exit function #1 called\n");
   system("pause");
}

void exit_fn2(void)
{
   printf("Exit function #2 called\n");
   system("pause");
}
void exit_fn3(void)
{
   printf("Exit function #3 called\n");
   system("pause");
}

int main(void)
{
	atexit(exit_fn1); //最晚调用
	atexit(exit_fn2);
	atexit(exit_fn3); //最先调用
	return 0;
}

```





