# ELF是如何运行的

# 执行流程

众所周知，一个Linux程序的执行流程应当如下图所示…

![Untitled](ELF%E6%98%AF%E5%A6%82%E4%BD%95%E8%BF%90%E8%A1%8C%E7%9A%84%207025521aa6c74304930204877796a1e2/Untitled.png)

在该函数中会调用__libc_start_main()函数，

这个函数会调用__libc_csu_init()函数进行一系列的初始化工作，

**之后才会调用main函数，来到用户代码空间**

同样的，无论用户代码中是否调用exit()函数，

在用户代码结束后**程序都会缺省调用exit函数**

```
public _start
_start proc near
; __unwind {
xor     ebp, ebp
mov     r9, rdx         ; rtld_fini
pop     rsi             ; argc
mov     rdx, rsp        ; ubp_av
and     rsp, 0FFFFFFFFFFFFFFF0h
push    rax
push    rsp             ; stack_end
xor     r8d, r8d        ; fini 不存在所以就xor清零
xor     ecx, ecx        ; init 不存在所以就xor清零
lea     rdi, main       ; main
call    cs:__libc_start_main_ptr
hlt
; } // starts at 1050
_start endp
```

# 编译器注册 init fini

然后我们可以手动的去注册一些

init函数,先于mian函数执行

fini函数,晚于main函数执行

注册的方法就是配合编译器

测试案例

文件名 w.c

运行环境: kali

```
#include <stdio.h>

void init()
{
    puts("init");
}
void fini()
{
    puts("fini");
}
int main()
{
    puts("main");
    return 0;
}
```

编译运行

```
┌──(kali㉿kali)-[~/code]
└─$ gcc -Wl,-init,init -Wl,-fini,fini w.c -o w.elf

┌──(kali㉿kali)-[~/code]
└─$ ./w.elf
init
main
fini
```

# 直接在源码写入init fini

当然也可以不用使用编译器那么麻烦

比如下面这个例子

```
#include <stdio.h>

__attribute((constructor)) void before_main()
{
    printf("%s\n", "I Can Before Main");
}

__attribute((destructor)) void after_main()
{
    printf("%s\n", "I Can Aftrer Main");
}

int main(int argc, char **argv)
{
    printf("%s\n", "I Am Main");
    return 0;
}
```

编译运行

```
gcc q.c -o q

输出

I Can Before Main
I Am Main
I Can Aftrer Main
```

# 写入多个init fini 指定执行顺序

当然,我们不是说只能写1个init和fini函数

我们还可以写入多个,然后给他们执行顺序去运行

```
#include <stdio.h>

void before_main_101(void) __attribute__((constructor(101)));
void before_main_102(void) __attribute__((constructor(102)));

// destructor priorities must be integers from 0 to 65535 inclusive
void after_main_65534(void) __attribute__((destructor(65534)));
void after_main_65535(void) __attribute__((destructor(65535)));

void before_main_102(void)
{
    puts("Before 102 ");
}
void before_main_101(void)
{
    puts("Before 101 ");
}
void after_main_65535(void)
{
    puts("After 65535");
}
void after_main_65534(void)
{
    puts("After 65534");
}
int main()
{
    puts("hello init fini");
    return 0;
}
```

直接编译就ok

gcc test.c -o test.elf

```
┌──(kali?kali)-[~/code]
└─$ ./w.elf
Before 101
Before 102
hello init fini
After 65535
After 65534
```

对于先于main函数执行的 **attribute**((constructor(num_xx)));

其中,num_xx 数值越小,越先执行来说

对于后于main函数执行的 **attribute**((destructor(num_xx)));

其中, num_xx数值越大,越先执行

# 关于init 和 fini 在何时何处执行 ？

我们以多个 **attribute**((constructor(num_xx))); 和 **attribute**((destructor(num_xx))); 为例

进行分析

## init何时

```
.text:000055B47A3BC050 public _start
.text:000055B47A3BC050 _start proc near
.text:000055B47A3BC050 ; __unwind { // 55B47A3BB000
.text:000055B47A3BC050 xor     ebp, ebp
.text:000055B47A3BC052 mov     r9, rdx         ; rtld_fini
.text:000055B47A3BC055 pop     rsi             ; argc
.text:000055B47A3BC056 mov     rdx, rsp        ; ubp_av
.text:000055B47A3BC059 and     rsp, 0FFFFFFFFFFFFFFF0h
.text:000055B47A3BC05D push    rax
.text:000055B47A3BC05E push    rsp             ; stack_end
.text:000055B47A3BC05F xor     r8d, r8d        ; fini
.text:000055B47A3BC062 xor     ecx, ecx        ; init
.text:000055B47A3BC064 lea     rdi, main       ; main
.text:000055B47A3BC06B call    cs:__libc_start_main_ptr
.text:000055B47A3BC071 hlt
.text:000055B47A3BC071 ; } // starts at 55B47A3BC050
.text:000055B47A3BC071 _start endp

```

F8 进入 call cs:__libc_start_main_ptr

然后来到一个libc.so.6文件

然后对准刚刚进入的地址，利用IDA的快捷键 p 生成函数

不同时候,不同环境,IDA对 _libc_start_main 的翻译不同

- _libc_start_main 代码 点击查看
    
    ```
    void __fastcall __noreturn _libc_start_main(
            __int64 a1,
            unsigned int a2,
            const char **a3,
            void (__fastcall *a4)(_QWORD, const char **, _QWORD),
            __int64 a5,
            __int64 a6)
    {
      __int64 v9; // r13
      _QWORD *v10; // r14
      __int64 v11; // rcx
      __int64 v12; // rdi
      void (__fastcall **lp_func)(_QWORD, const char **, __int64, _QWORD, __int64); // rcx
      __int64 v14; // rsi
      void (__fastcall **lp_init_arr)(_QWORD, const char **, __int64, _QWORD, __int64); // r14
      __int64 last_func; // [rsp+8h] [rbp-40h]
    
      if ( a6 )
        (_cxa_atexit)(a6, 0LL, 0LL, a4, a5);
      if ( (rtld_global_ro & 2) != 0 )
        (*(&rtld_global_ro + 99))("\ninitialize program: %s\n\n", *a3);
      v9 = environ;
      if ( a4 )
      {
        (a4)(a2, a3, environ, a4, a5);
      }
      else
      {
        v10 = rtld_global;
        v11 = *(rtld_global + 160LL);
        if ( v11 )
          ((*rtld_global + *(v11 + 8)))(a2, a3, environ, *rtld_global + *(v11 + 8), a5);
        v12 = v10[33];
        if ( v12 )
        {
          lp_func = (*(v12 + 8) + *v10);
          v14 = *(v10[35] + 8LL) >> 3;
          if ( v14 )
          {
            lp_init_arr = lp_func + 1;
            last_func = &lp_func[(v14 - 1) + 1];
            while ( 1 )
            {
              (*lp_func)(a2, a3, v9, lp_func, a5);
              lp_func = lp_init_arr;
              if ( last_func == lp_init_arr )
                break;
              ++lp_init_arr;
            }
          }
        }
      }
      (unk_7F3FEEADC340)(rtld_global);
      if ( (rtld_global_ro & 2) != 0 )
        (*(&rtld_global_ro + 99))("\ntransferring control: %s\n\n", *a3);
      for_main_start(a1, a2, a3);
    }
    ```
    

在_libc_start_main 函数差不多末尾的地方有这样一个代码

![Untitled](ELF%E6%98%AF%E5%A6%82%E4%BD%95%E8%BF%90%E8%A1%8C%E7%9A%84%207025521aa6c74304930204877796a1e2/Untitled%201.png)

就会执行我们的init函数

也就是那个(*v13)(a2,a3,v9)

我我们看一下那循环

```
while ( 1 )
{
  (*lp_func)(a2, a3, v9, lp_func, a5);
  lp_func = lp_init;
  if ( last_init == lp_init )
    break;
  ++lp_init;
}
```

其实他就是在不断的遍历init_arr数组

然后看它的反汇编

```
libc.so.6:00007F8FEA3452AC mov     rdx, r13
libc.so.6:00007F8FEA3452AF mov     rsi, rbx
libc.so.6:00007F8FEA3452B2 mov     edi, ebp
libc.so.6:00007F8FEA3452B4 call    qword ptr [rcx]
libc.so.6:00007F8FEA3452B6 mov     rcx, r14
libc.so.6:00007F8FEA3452B9 cmp     [rsp+48h+var_40], r14
libc.so.6:00007F8FEA3452BE jnz     short loc_7F8FEA3452A8
libc.so.6:00007F8FEA3452C0 jmp     loc_7F8FEA345226
```

进入rcx指向的位置,就是init_arr的成员

```
.init_array:000055D721F6DDB0 assume cs:_init_array
.init_array:000055D721F6DDB0 ;org 55D721F6DDB0h
.init_array:000055D721F6DDB0 dq offset before_main_101 ;这就是我们写入的函数
.init_array:000055D721F6DDB8 dq offset before_main_102 ;这就是我们写入的函数
.init_array:000055D721F6DDC0 __frame_dummy_init_array_entry dq offset frame_dummy
.init_array:000055D721F6DDC0 _init_array ends
```

于是我们就大概知道了何时运行init函数了

## main函数在哪里被调用

那个很大的函数    libc_start_main

```
void __fastcall __noreturn _libc_start_main(
        __int64 a1,
        unsigned int a2,
        const char **a3,
        void (__fastcall *a4)(_QWORD, const char **, _QWORD),
        __int64 a5,
        __int64 a6)
{
    xxxx;局部遍历赋值

    xxxx;//一些判断
    if ( a4 )
    {
        xxxx;
    }
    else
    {
        xxxx;
        if ( v12 )
        {
            xxxx;
            if ( v14 )
            {
                lp_init_arr = lp_func + 1;//第一个成员
                last_func = &lp_func[(v14 - 1) + 1];//取出最后一个成员的地址
                while ( 1 )
                {
                    (*lp_func)(a2, a3, v9, lp_func, a5);
                    lp_func = lp_init_arr;
                    if ( last_func == lp_init_arr )
                        break;
                    ++lp_init_arr;
                }
            }
        }
    }
    xxxx;//其它东西的判断和调用
    for_main_start(a1, a2, a3);
}
```

所以在该函数最后会进入一个for_main_start的函数 ps:名字是我自己修改的

```
void __fastcall __noreturn for_main_start(
        __int64 (__fastcall *main)(_QWORD, __int64, _QWORD),
        unsigned int a2,
        __int64 a3)
{
  unsigned int i; // eax
  signed __int64 v4; // rax
  char v6[72]; // [rsp+20h] [rbp-78h] BYREF
  unsigned __int64 v7; // [rsp+68h] [rbp-30h]
  unsigned __int64 v8; // [rsp+70h] [rbp-28h]
  unsigned __int64 v9; // [rsp+88h] [rbp-10h]

  v9 = __readfsqword(0x28u);
  if ( (setjmp)(v6) )
    goto LABEL_4;
  v7 = __readfsqword(0x300u);
  v8 = __readfsqword(0x2F8u);
  __writefsqword(0x300u, v6);
  for ( i = main(a2, a3, environ); ; i = 0 )
  {
    (exit)(i);
LABEL_4:
    sub_7F3FEEB3C250();
    if ( _InterlockedDecrement(_nptl_nthreads) )
    {
      while ( 1 )
        v4 = sys_exit(0);
    }
  }
}
```

在那个for循环会条件判断的时候就会进入main(),所以此刻我们已经进入main函数了

在main()返回值作为i的初始化

进入循环第一步干的事情就是

```
exit(i)
```

当调用了exit后,我们的程序一般就结束了

所以后面的代码就不会执行

后面的代码什么时候执行?

```
LABEL_4:
    sub_7F3FEEB3C250();
    if ( _InterlockedDecrement(_nptl_nthreads) )
    {
      while ( 1 )
        v4 = sys_exit(0);
    }
  }
```

取决于那个

```
  if ( (setjmp)(v6) )
    goto LABEL_4;
```

先不说 goto LABEL_4; 后面有时间再说

## fini何时运行?

这个肯定在main函数之后

因为main函数返回的时候是进入那个for循环,调用exit函数执行的呗

因为exit调用完毕后我们的程序已经死了

所以after_main在exit()函数里面被调用

```
void __fastcall __noreturn exit(__int64 a1)
{
  _QWORD *v1; // rdx
  void *v2; // r12
  int v3; // ebx
  _QWORD *v4; // rdi
  _DWORD *v5; // rax

  (unk_7F56682C5330)(a1, &unk_7F5668459820, 1LL, 1LL);//进入之后
    //下面的代码就不会再运行了
  v2 = off_7F5668459828;
  v3 = a1;
  if ( off_7F5668459828 )
  {
    v4 = off_7F5668459828;
    v1 = 0LL;
    while ( *v4 != v3 )
    {
      v1 = v4;
      if ( !v4[2] )
        goto LABEL_12;
      v4 = v4[2];
    }
    if ( &unk_7F5668459820 )
    {
      v4[1] = &unk_7F5668459820;
    }
    else
    {
      if ( v1 )
        v1[2] = v4[2];
      else
        off_7F5668459828 = v4[2];
      (unk_7F56682AD360)();
    }
  }
  else
  {
LABEL_12:
    if ( &unk_7F5668459820 )
    {
      v5 = (unk_7F56682AD368)(24LL, &unk_7F5668459820, v1);
      if ( v5 )
      {
        *v5 = v3;
        off_7F5668459828 = v5;
        *(v5 + 2) = _mm_unpacklo_epi64(&unk_7F5668459820, v2);
      }
    }
  }
}
```

进入 函数 (unk_7F56682C5330)(a1, &unk_7F5668459820, 1LL, 1LL);

```
void __fastcall __noreturn sub_7F56682C5330(unsigned int a1, unsigned __int64 a2, char a3, char a4)
{
  void (**v4)(void); // r12
  void (**v5)(void); // rbx
  void (**v6)(void); // r15
  void (*v7)(void); // rax
  __int64 v8; // rdx
  void (*v9)(void); // rcx
  void (**v10)(void); // rdx
  void (__fastcall *v11)(_QWORD, unsigned __int64); // rax
  void (**v12)(void); // rdx
  void (*v13)(void); // rax
  void (*v14)(void); // r13
  void (__fastcall *v15)(_QWORD, _QWORD); // rax
  void (*v16)(void); // rax
  void (*v17)(void); // rax
  void (*v18)(void); // [rsp+8h] [rbp-50h]
  void (__fastcall *v19)(_QWORD, _QWORD); // [rsp+8h] [rbp-50h]
  void (__fastcall *v20)(_QWORD, unsigned __int64); // [rsp+10h] [rbp-48h]

  v4 = a2;
  if ( a4 )
    (_call_tls_dtors)();
  if ( _InterlockedCompareExchange(dword_7F566845B2C8, 1, 0) )
    (_lll_lock_wait_private)(dword_7F566845B2C8);
  LODWORD(v5) = 0;
  while ( 1 )
  {
    v6 = *v4;
    if ( !*v4 )
    {
LABEL_25:
      byte_7F566845B708 = 1;
      if ( _InterlockedExchange(dword_7F566845B2C8, 0) > 1 )
        (_lll_lock_wake_private)(dword_7F566845B2C8);
      if ( a3 )
      {
        v5 = &unk_7F56684559D8;
        v4 = qword_7F56684559E0;
        if ( &unk_7F56684559D8 < qword_7F56684559E0 )
        {
          do
            (*v5++)();
          while ( v5 < qword_7F56684559E0 );
        }
      }
      v11 = (exit)(a1);
      goto LABEL_31;
    }
LABEL_7:
    v7 = v6[1];
    while ( v7 )
    {
      v7 = (v7 - 1);
      v6[1] = v7;
      v8 = 4LL * v7;
      v9 = v6[4 * v7 + 2];
      if ( v9 == 3 )
      {
        v16 = (__readfsqword(0x30u) ^ __ROR8__(v6[v8 + 3], 17));
        if ( _InterlockedExchange(dword_7F566845B2C8, v5) > 1 )
        {
          v18 = v16;
          (_lll_lock_wake_private)(dword_7F566845B2C8);
          v16 = v18;
        }
        v16();
        if ( v5 != _InterlockedCompareExchange(dword_7F566845B2C8, 1, v5) )
          goto LABEL_23;
        goto LABEL_7;
      }
      if ( v9 == 4 )
      {
        v12 = &v6[v8];
        v13 = v12[3];
        v14 = v12[4];
        v12[2] = 0LL;
        v15 = (__readfsqword(0x30u) ^ __ROR8__(v13, 17));
        if ( _InterlockedExchange(dword_7F566845B2C8, v5) > 1 )
        {
          v19 = v15;
          (_lll_lock_wake_private)(dword_7F566845B2C8);
          v15 = v19;
        }
        a2 = a1;
        v15(v14, a1);//这里会调用after_mian函数
        if ( v5 != _InterlockedCompareExchange(dword_7F566845B2C8, 1, v5) )
          goto LABEL_23;
        goto LABEL_7;
      }
      if ( v9 == 2 )
      {
        v10 = &v6[v8];
        a2 = v10[4];
        v11 = (__readfsqword(0x30u) ^ __ROR8__(v10[3], 17));
        if ( _InterlockedExchange(dword_7F566845B2C8, v5) <= 1 )
        {
          v11(a1, a2);
          goto LABEL_14;
        }
LABEL_31:
        v20 = v11;
        (_lll_lock_wake_private)(dword_7F566845B2C8);
        v20(a1, a2);
LABEL_14:
        if ( v5 != _InterlockedCompareExchange(dword_7F566845B2C8, 1, v5) )
LABEL_23:
          (_lll_lock_wait_private)(dword_7F566845B2C8);
        goto LABEL_7;
      }
    }
    v17 = *v6;
    *v4 = *v6;
    if ( !v17 )
      goto LABEL_25;
    (unk_7F56682AD360)(v6);
  }
}
```

```
__int64 sub_7F213646A6A0()
{
  __int64 result; // rax
  __int64 v1; // r12
  _DWORD *v2; // rbx
  unsigned int v3; // er15
  __int64 v4; // rsi
  void *v5; // rsp
  __int64 v6; // rax
  _QWORD *v7; // r14
  __int64 v8; // r15
  char v9; // al
  __int64 v10; // rax
  __int64 v11; // rdx
  void (**v12)(void); // rax
  __int64 v13; // rax
  _QWORD v14[2]; // [rsp+0h] [rbp-60h] BYREF
  _QWORD *v15; // [rsp+10h] [rbp-50h]
  int v16; // [rsp+18h] [rbp-48h]
  int v17; // [rsp+1Ch] [rbp-44h]
  void (**v18)(void); // [rsp+20h] [rbp-40h]
  void (**v19)(void); // [rsp+28h] [rbp-38h]

  v16 = 2;
  v17 = 0;
  result = qword_7F2136498A20;
  v1 = qword_7F2136498A20 - 1;
  if ( qword_7F2136498A20 - 1 < 0 )
  {
LABEL_31:
    if ( v16 != 1 )
    {
      result = 0LL;
      v17 = 1;
      v16 = 1;
    }
    return result;
  }
  v2 = (&rtld_global + 160 * qword_7F2136498A20 - 160);
  while ( 1 )
  {
    (pthread_mutex_lock)(&unk_7F2136498A28);
    v3 = v2[2];
    if ( !v3 || (*(*v2 + 822LL) & 1) != v17 )
    {
      result = (_pthread_mutex_unlock)(&unk_7F2136498A28);
      goto LABEL_4;
    }
    v14[1] = v14;
    (unk_7F213647C630)(v1, 2LL);
    v4 = 0LL;
    v5 = alloca(16 * ((8 * v3 + 15) >> 4));
    v6 = *v2;
    v7 = v14;
    if ( *v2 )
      break;
LABEL_13:
    if ( !v1 && v3 != v4 )
      goto LABEL_36;
    if ( v3 != v4 && v1 && v3 - 1 != v4 )
      goto LABEL_35;
    LODWORD(v19) = v4;
    (unk_7F2136476720)(v14, v4, v1 == 0, 1LL);
    (_pthread_mutex_unlock)(&unk_7F2136498A28);
    if ( v19 )
    {
      v15 = &v14[v19];
      while ( 1 )
      {
        v8 = *v7;
        v9 = *(*v7 + 820LL);
        if ( (v9 & 0x10) != 0 )
          break;
LABEL_27:
        --*(v8 + 816);
        if ( v15 == ++v7 )
          goto LABEL_28;
      }
      *(v8 + 820) = v9 & 0xEF;
      v10 = *(v8 + 272);
      if ( v10 )
      {
        v18 = (*v8 + *(v10 + 8));
        v11 = *(*(v8 + 288) + 8LL) >> 3;
        v12 = &v18[(v11 - 1)];
        if ( v11 )
        {
          do //在这会执行我们的after_main函数
          {
            v19 = v12;
            (*v12)();
            v12 = v19 - 1;
          }
          while ( v18 != v19 );
        }
        v13 = *(v8 + 168);
        if ( v13 )
LABEL_25:
          ((*v8 + *(v13 + 8)))();
      }
      else
      {
        v13 = *(v8 + 168);
        if ( v13 )
          goto LABEL_25;
      }
      (unk_7F213647C810)(v8);
      goto LABEL_27;
    }
LABEL_28:
    result = (unk_7F213647C630)(v1, 0LL);
LABEL_4:
    --v1;
    v2 -= 40;
    if ( v1 == -1 )
      goto LABEL_31;
  }
  while ( 1 )
  {
    while ( *(v6 + 40) != v6 )
    {
      v6 = *(v6 + 24);
      if ( !v6 )
        goto LABEL_13;
    }
    if ( v4 >= v3 )
      break;
    v14[v4] = v6;
    *(v6 + 1036) = v4;
    v4 = (v4 + 1);
    ++*(v6 + 816);
    v6 = *(v6 + 24);
    if ( !v6 )
      goto LABEL_13;
  }
  (unk_7F213647F140)("i < nloaded", "dl-fini.c", 82LL, "_dl_fini");
LABEL_35:
  (unk_7F213647F140)("ns == LM_ID_BASE || i == nloaded || i == nloaded - 1", "dl-fini.c", 93LL, "_dl_fini");
LABEL_36:
  (unk_7F213647F140)("ns != LM_ID_BASE || i == nloaded", "dl-fini.c", 92LL, "_dl_fini");
  return sub_7F213646AA00();
}

```

# 全局class的析构函数和构造函数

它也可以实现和 init , fini 的功能

测试案例 test.cpp

编译: g++ test.cpp -o test.elf

```
#include<stdio.h>

class mt{
	public:
	mt()
	{
		puts("befor main");
	}
	~mt()
	{
		puts("after main");

	}

};
 class mt mt1;
 int main()

 {

 	puts("i am main");
 	return 0;
 }
```

## 构造函数

之前写init和fini的时候

存在的init_arr是

```
.init_array:000055D721F6DDB0 assume cs:_init_array
.init_array:000055D721F6DDB0 ;org 55D721F6DDB0h
.init_array:000055D721F6DDB0 dq offset before_main_101 ;这就是我们写入的函数
.init_array:000055D721F6DDB8 dq offset before_main_102 ;这就是我们写入的函数
.init_array:000055D721F6DDC0 __frame_dummy_init_array_entry dq offset frame_dummy
.init_array:000055D721F6DDC0 _init_array ends
```

现在用全局的calss,init_arr就变为了这样

```
.init_array:0000565393E91DB8 __frame_dummy_init_array_entry dq offset frame_dummy
.init_array:0000565393E91DC0 dq offset _GLOBAL__sub_I_mt1
.init_array:0000565393E91DC0 _init_array ends
```

所以class的构造函数执行类似于init

## 析构函数

它类似于fini的执行,但是又不同于fini

```
        v15(v14, a1);//如果是析构函数,这里直接就进入析构
		//如果是fini函数,这里会进入一个函数,然后那个函数很大,其中有一部分就是fini的函数调用,用while循环实现的
        if ( v5 != _InterlockedCompareExchange(dword_7F566845B2C8, 1, v5) )
          goto LABEL_23;
        goto LABEL_7;
```