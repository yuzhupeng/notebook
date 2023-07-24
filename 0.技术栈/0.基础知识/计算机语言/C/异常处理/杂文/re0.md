# 其它

想我告诉过你，当一个线程出现错误时，操作系统给你一个机会被告知这个错误。

说得更明白一些就是，**当一个线程出现错误时，操作系统调用用户定义的一个回调函数**。

这个回调函数可以做它想做的一切。例如它可以修复错误，或者它也可以播放一段音乐。

无论回调函数做什么，它最后都要返回一个值来告诉系统下一步做什么。（这不是十分准确，但就此刻来说非常接近。）

- - dvlinker from csdn

敲黑板:

SEH是局部的(栈中),是基于线程的

VEH,UEF,VCH是全局的(堆中),基于进程的

重要的事情说3编:
VEH/UEF/VCH的处理函数都是stdcall
所以不要拿着一个 __cdecl  去注册一个应该stdcall的函数
否则会导致堆栈不平衡,导致函数参数读取错误,或者函数返回有问题等等



# API触发异常



```c
void RaiseException(
[in] DWORD           dwExceptionCode,
[in] DWORD           dwExceptionFlags,
[in] DWORD           nNumberOfArguments,
[in] const ULONG_PTR *lpArguments
);
```



# jmp_buf

jmp_buf 是一个数据类型

setjmp(jmp_buf_var) 就是初始化一个变量jmp_buf_var,第一次初始化会返回0

jmp_buf_var 里面保存了在当前ip的运行环境

longjmp(jmp_buf_var,1);就是根据jmp_buf_var的环境,跳转到上一次保存的环境,那个1是任意的一个返回值

因为setjmp很好的保存了各个寄存器的值,所以就可以很好的跳转回去正常运行(除非变量的依赖性不是很强)



```c
#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>

//异常的特征码,无关紧要
typedef enum _ErrorCode_
{
    ERR_OK = 100,
    ERR_DIV_BY_ZERO = -1
    } ErrorCode;

jmp_buf gExcptBuf;
typedef int (*pf)(int, int);
int my_div(int a, int b)
{
    if (b==0)
    {
        printf("try excepton. error = %d \n", ERR_DIV_BY_ZERO);
        longjmp(gExcptBuf,1);//ERR_DIV_BY_ZERO 是携带的返回值
    }
    return a / b;
}
int my_try(pf func, int a, int b)
{
    int flag = setjmp(gExcptBuf);   //保存才开始的环境
    int sum=0;
    if (flag==0)
    {
        sum=func(a, b); //正常的调用
        return sum;
    }
    return 0;
}
int main()
{
    int result = my_try(my_div, 8, 0);
    //int result = my_try(my_div, 8, 2);
    printf("%d\n",result);
    puts("GoodBye");
    return 0;
}
```



