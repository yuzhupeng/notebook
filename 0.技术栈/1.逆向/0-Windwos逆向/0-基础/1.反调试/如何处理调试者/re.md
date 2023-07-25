# 如何处理调试者

# 线程死锁

在计算机基础.编程语言.win32 那里

我提及了线程死锁问题

这里不讲原理,只讲调用

```
#include <windows.h>
#include <stdio.h>
#include <stdint.h>

HANDLE mutex_delta;
uint32_t delta = 0x9e3779b9;

DWORD WINAPI Check_Dbg(LPVOID lpParam)
{
    while (1)
    {
        if (IsDebuggerPresent())
        {
            WaitForSingleObject(mutex_delta, INFINITE);
            Sleep(1000);
            //把互斥体抢过来,然后不给了
        }
        else
        {
            break;
        }
    }

    return 0;
}
DWORD WINAPI enc_func(LPVOID lpParam)
{
    DWORD* arg = lpParam;

    uint32_t* v = arg[0];
    uint32_t* k = arg[1];

    uint32_t v0 = v[0], v1 = v[1], sum = 0, i;
    uint32_t k0 = k[0], k1 = k[1], k2 = k[2], k3 = k[3];
    for (i = 0; i < 32; i++)
    {
        WaitForSingleObject(mutex_delta, INFINITE);
        sum += delta;
        v0 += ((v1 << 4) + k0) ^ (v1 + sum) ^ ((v1 >> 5) + k1);
        v1 += ((v0 << 4) + k2) ^ (v0 + sum) ^ ((v0 >> 5) + k3);
        ReleaseMutex(mutex_delta);
    }
    v[0] = v0;
    v[1] = v1;
    printf("enc down\n");
    return 0;
}
int main()
{
    HANDLE threads[2];
    uint32_t v[2] = { 1,2 }, k[4] = { 2,2,3,4 };
    DWORD argArr[2] = { v,k };
    int i;

    // 创建互斥体
    mutex_delta = CreateMutex(NULL, FALSE, NULL);

    // 创建线程
    threads[0] = CreateThread(NULL, 0, Check_Dbg, 0, 0, NULL);
    threads[1] = CreateThread(NULL, 0, enc_func, argArr, 0, NULL);

    // 等待线程完成
    WaitForMultipleObjects(2, threads, TRUE, INFINITE);

    // 关闭线程句柄
    for (i = 0; i < 2; i++)
    {
        CloseHandle(threads[i]);
    }
    // 关闭互斥体句柄
    CloseHandle(mutex_delta);

    return 0;
}

```

基于互斥体的死锁原理:

如果发现在调试,

就不断的抢夺过一个资源

然后不松手

# 无限递归

就是进入一个无限递归的函数啦

然后耗掉主线程

# 无限异常

一直触发异常

异常也会触发新的异常

# 修改字节码

# 修改全局某个变量

# 忽略异常

# 模拟键盘和鼠标的移动