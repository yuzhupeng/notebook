# VEH

# 导入

大名: Vectored	Exception	Handler

小名: VCH

是第一个抓取异常的函数

VEH自己有一个异常处理链表

```
void* __stdcall AddVectoredExceptionHandler(
    _In_ unsigned long First,                  //// 1-veh链头部，0-veh链尾部。
    _In_ PVECTORED_EXCEPTION_HANDLER Handler   // 异常处理函数指针
);

//处理函数类型
typedef long (__stdcall *PVECTORED_EXCEPTION_HANDLER)(struct _EXCEPTION_POINTERS *ExceptionInfo);
```

2023.01.11实践

注册VEH异常处理函数

```
AddVectoredExceptionHandler(1, VEH_1);//push 0 //往VEH链表头部插入
AddVectoredExceptionHandler(0, VEH_2);//push 0 //往VEH链表尾部插入
```

返回值 😥😥😥😥

```
__except 后的括号中会存在一个异常过滤表达式表达式的返回值必定是一下说明的几个之一

- 返回 0,1是等价的,会继续往后遍历
#define EXCEPTION_EXECUTE_HANDLER      1 	//把异常丢给下一位嘉宾
#define EXCEPTION_CONTINUE_SEARCH      0 	//把异常丢给下一位嘉宾

- 返回 -1会从异常那里继续执行
#define EXCEPTION_CONTINUE_EXECUTION  (-1)  //自己处理,EIP会根据CONTEXT做一个返回 ,EIP是异常发生的地方
```

本进程中任何线程出现异常都可以被VEH处理，

所以在有些时候是很有用处的。

VEH注册和移除的方法：

使用AddVectoredExceptionHandler函数添加。

使用RemoveVectoredExceptionHandler函数移除

```
#include <windows.h>
#include <stdio.h>
LONG WINAPI VectoredExceptionHandler(PEXCEPTION_POINTERS ExceptionInfo)
{
    printf("ExceptionCode: %X\n", ExceptionInfo->ExceptionRecord->ExceptionCode);

    if (ExceptionInfo->ExceptionRecord->ExceptionCode == EXCEPTION_INT_DIVIDE_BY_ZERO)
    {
        ExceptionInfo->ContextRecord->Eax = 1;
        ExceptionInfo->ContextRecord->Ecx = 1;

        return EXCEPTION_CONTINUE_EXECUTION;
    }

    return EXCEPTION_EXECUTE_HANDLER;
}

int main()
{
    int number = 0;

    // 通过一个API可以直接安装VEH
    // 参数一是布尔值，如果为 TRUE，就将当前的函数添加到全局 VEH 函数的链表头部
    // 否则则为尾部
    AddVectoredExceptionHandler(TRUE, VectoredExceptionHandler);

    __try
    {
        number /= 0;
    }
    // 异常首先被 VEH 接收到，如果无法处理才会传递给 SEH
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        printf("永远不会被执行\n");
    }

    printf("number = %d\n", number);

    system("pause");
    return 0;
}
```

# RtlAddVectoredExceptionHandler(别看)

进入函数AddVectoredExceptionHandler

此刻我们已经压入2个参数

```
push    offset Handler  ; Handler
push    1               ; First
call    ds:AddVectoredExceptionHandler
```

Asm

```
; int __stdcall ntdll_RtlAddVectoredExceptionHandler(int flag, int handler)
    ntdll_RtlAddVectoredExceptionHandler proc near
;flag= dword ptr  8
        ;handler= dword ptr  0Ch
mov     edi, edi
push    ebp
mov     ebp, esp
mov     edx, [ebp+handler] 	;[ebp+12] 刚才的参数2
mov     ecx, [ebp+flag] 	;[ebp+8] 刚才的参数1
push    0 					;于是我们压入一个新的参数 0,说明是VEH,如果是1,说明是VCH
call    sub_77CEB136
pop     ebp
retn    8
            ntdll_RtlAddVectoredExceptionHandler endp
```

C

```
int __stdcall ntdll_RtlAddVectoredExceptionHandler(int flag, int handler)
{
    return RtlpAddVectoredHandler(flag, handler, 0);
}
```

# RtlpAddVectoredHandler(别看)

```c
PVOID WINAPI RtlpAddVectoredHandler(
        _In_  ULONG FirstHandler,
        _In_  PVECTORED_EXCEPTION_HANDLER VectoredHandler,
        _In_  ULONG Key
        )
{
        PPEB pPeb=0;
        __asm
        {
                //取得PEB地址
                mov eax,fs:[0x18]
                mov eax,[eax+0x30]
                mov pPeb,eax
        }
        //申请内存
        PVECTORED_LIST vl=RtlAllocateHeap(pPeb->DefaultHeap,0,sizeof(VECTORED_LIST));

        if (vl==0)
        {
                return 0;
        }
        vl->dwMark=1;
        //加密指针，把参数VectoredHandler加密
        DWORD dwEncodePointer=RtlEncodePointer(VectoredHandler);

        //LdrpActiveThreadCount是全局变量
        PVECTORED_LIST_ENTR pVectordListEntr=LdrpActiveThreadCount;

        //根据key选择挂在哪一个链上，key==0的时候所挂载的链上的函数，是从RtlAddVectoredExceptionHandler来的，这一条链上是要在SEH执行前执行的
        pVectordListEntr=&pVectordListEntr[Key];

        //Slim读写锁
        RtlAcquireSRWLockExclusive(pVectordListEntr->Head);

        //看看链表是否为空,自己指向自己就说明链表为空
        if (pVectordListEntr->Head==(PVECTORED_LIST)&pVectordListEntr->Head)
        {
                //标志位，很重要,如果不设置这位，将不会检测VEH链
                pPeb->EnvironmentUpdateCount|=(0x1<<(Key+2));

        }
        if (FirstHandler==0)
        {
                //放在链表的后面
                vl->pNext=(PVECTORED_LIST)pVectordListEntr;
                vl->pPrevious=pVectordListEntr->Tail;
                pVectordListEntr->Tail->pNext=vl;
                pVectordListEntr->Tail=vl;

        }
        else
        {
                //放在链表前面
                vl->pNext=pVectordListEntr->Head;
                vl->pPrevious=(PVECTORED_LIST)pVectordListEntr;
                pVectordListEntr->Head->pPrevious=vl;
                pVectordListEntr->Head=vl;
        }

        RtlReleaseSRWLockExclusive(pVectordListEntr->Head);

        return vl;
}
```