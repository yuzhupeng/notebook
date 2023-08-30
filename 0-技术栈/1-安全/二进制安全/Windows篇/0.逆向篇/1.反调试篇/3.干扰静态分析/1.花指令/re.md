

# 垃圾代码

向程序添加大量无意义的代码来增加代码调试的难度，

这就是“垃圾代码”反调试技术。

尤其是，这些垃圾代码中还含有真正有用的代码或者应用其他反调试技术时，

调试程序会变得更加困难。

但是实际的垃圾代码往往具有精巧又复杂的形态，含有大量条件分支语句和无尽的函数调用，想要跳过它们并非易



花指令的样子:

1), 短小而有精悍,一般10-20字节,

2),可移植性好,任意放在一个地方,对不会影响执行

3), 垃圾代码比较好,而不是简单的call





nop的倍数最好是1/2/4



# 待学习链接



**一些简单的花指令的解析(含Intel指令集)** 

https://www.52pojie.cn/thread-1068444-1-1.html

花指令 :

如何构造: [https://www.52pojie.cn/thread-326995-1-1.html](https://www.52pojie.cn/thread-326995-1-1.html)

如何用IDC解决: [https://www.cnblogs.com/LyShark/p/13100048.html](https://www.cnblogs.com/LyShark/p/13100048.html)

我们不应该垃圾代码不仅仅起着一个混淆的作用

因为一旦nop后,垃圾代码就不再起作用.我们应该

1). 让垃圾代码参与加密和运算,你的垃圾代码甚至可以是一堆字符串,用于加密

2). 让垃圾代码和有效代码加密挨着,穿插运行(不知道如何实现)

3). 



# 跳转指令





## 8组跳转指令



我说的是短跳转指令,跳转范围只有-128,+128

0), JO/JNO: 0x70/0x71

1), JC/JNC: 0x72/0x73

> ps: 机器码,jc=jb=jnae,jnc=jnb=jae

2), JZ/JNZ: 0x74/0x75   

> ps:机器码,JE=JZ,JNE=JNZ

3), JBE/JA:0x76/0x77

4), JS/JNS: 0x78/0x79

5), JP/JNP: 0x7A/0x7B

6), JL/JGE: 0x7C/0x7D

7), JLE/JG: 0x7E/0x7F,

> ps:JLE=JNG



具体到条件的分类:

ZF = 1 或 SF = 1: JLE/JNG

ZF = 0 且 SF = 0: JA/JNBE

SF^OF=0 :JGE/JL

SF ^OF=1: JL/JNGE

OF: OF=1就JO ,OF=0就JNO

SF: SF=1就JS,SF=0就JNS

CF: CF=1就JC/JB/JNAE,CF=0就JAE/JNC

ZF: ZF=1就JZ/JE,ZF=0就JNZ/JNE





## 其它类型

短call :0xE8 + 偏移

短jmp: 0xEB

长jmp: 0xE9



# 花指令类型



一些垃圾代码应该去随机生成,而不是自己去手写

比如利用时间的随机性去生成,然后写入

主要就用1/2/4的宽度,因为对程序执行毫无影响,所以就长度任意

 

## 强制跳转

> 强制跳转: 通常短jmp会比较好



```nasm
    __asm {
        jmp tag1
        nop
        nop
        nop
        nop
        tag1:
    }
};
```





## 基于互补条件的跳



> 基于互补条件的跳

不一定是jz/jnz,甚至可以是js/jns, ja/jbe以此类推

就常见的那8组互补指令



长度4

```assembly
    __asm {
    jz tag1
    jnz tag1
    nop
    nop
    nop
    nop
    tag1:
    }
    unsigned char data[10] = {
    0xF3, 0xAB, 0x74, 0x06, 0x75, 0x04, 0x90, 0x90, 0x90, 0x90
    };
```



长度2

```assembly
     __asm {
        jz tag1
        jnz tag1
        nop
        nop
        tag1:
    }
    unsigned char loc_41175F[6] = {
    0x74, 0x04, 0x75, 0x02, 0x90, 0x90
};
```



长度1

```assembly
    __asm {
    jz tag1
    jnz tag1
    nop
    tag1:
    }
    unsigned char loc_411765[5] = {
    0x74, 0x03, 0x75, 0x01, 0x90
    };
```

ps: jmp那边也有6/4/3,这里是10/6/5



之后根据编写,随意替换那8组互补的指令

并随机初始化0x90



## 基于条件的跳转

> 基于条件的跳转

缺点:

需要构造条件,条件的构造可能需要其它汇编指令,于是就破坏了环境,于是就需要保存和恢复环境

保存环境的这种,就需要pushad/popad

如果要去patch这种花指令就特别简单

只需要把pushad和popad之间的字节码nop,然后再nop pushad和popad

所以对于需要pushad和popad的我们不考虑

但是话有说回来

我们为什么要用pushad来保存环境? 因为简单,暴力

比如我们用到eax,只保存eax可以使用push eax

可是为了多样话,我们可能用push ecx,

所以我们就用了pushad和popad

但是pushad和popad又容易被patch

总结一句话: 如果我们能偶不使用pushad和popad,又保存了环境那么就会让花指令去除变得简单了



这样的话呀,有引起了我们的一个思考

那就是如何把花指令嵌套在源码中

说一下我们之前对微型SMC的处理

- 先把n个__asm nop 植入源代码,编译为二进制文件
- 用脚本实现90的寻找,然后把微型SMC覆盖到90
- 用脚本实现key的随机化
- 用脚本实现key的读取,然后对字节码解密



那么对于此刻的花指令来说

该如何处理? 貌似还是采用相同的方式,哈哈

其实呢? SMC之所以处理得比较快,是因为我只想出了3种简单的smc

然乎编译为二进制,然后覆盖

对于花指令来说的话,类似超多种,估计有几十种,

编译就有点恼火,要手动编译





```assembly
50                            push    eax
51                            push    ecx
52                            push    edx
53                            push    ebx
;66 50                         push    ax
;66 51                         push    cx
;66 52                         push    dx
;66 53                         push    bx
58                            pop     eax
59                            pop     ecx
5A                            pop     edx
5B                            pop     ebx
;66 58                         pop     ax
;66 59                         pop     cx
;66 5A                         pop     dx
;66 5B                         pop     bx
```





### ZF

1),常见的jz和jnz就不说了,看zf标志位,zf的初始化也很简单

xor

```assembly
33 C0                         xor     eax, eax
33 C9                         xor     ecx, ecx
33 D2                         xor     edx, edx
33 DB                         xor     ebx, ebx
;33 F6                         xor     esi, esi
;33 FF                         xor     edi, edi
;66 33 C0                      xor     ax, ax
;66 33 DB                      xor     bx, bx
;66 33 C9                      xor     cx, cx
;66 33 D2                      xor     dx, dx
32 C0                         xor     al, al
32 C9                         xor     cl, cl
32 D2                         xor     dl, dl
32 DB                         xor     bl, bl
32 E4                         xor     ah, ah
32 ED                         xor     ch, ch
32 F6                         xor     dh, dh
32 FF                         xor     bh, bh
```



sub

```assembly
2B C0                         sub     eax, eax
2B C9                         sub     ecx, ecx
2B D2                         sub     edx, edx
2B DB                         sub     ebx, ebx
;sub     esi, esi
;sub     edi, edi
;66 2B C0                      sub     ax, ax
;66 2B DB                      sub     bx, bx
;66 2B C9                      sub     cx, cx
;66 2B D2                      sub     dx, dx
2A C0                         sub     al, al
2A C9                         sub     cl, cl
2A D2                         sub     dl, dl
2A DB                         sub     bl, bl
2A E4                         sub     ah, ah
2A ED                         sub     ch, ch
2A F6                         sub     dh, dh
2A FF                         sub     bh, bh
```



因为16位寄存器的opcode长度是3,其它都是2,导致不一致

所以呢,我们以后再考虑3的情况



```assembly
__asm{
	push eax
	xor eax,eax
	jz label1
	nop
	nop
	nop
	nop
	
	label1:
	pop eax
}
```



### CF

2), 基于CF标志位的

STC 让CF=1 

CLC 让CF=0

CMC 让CF取反 

然后其他的汇编指令好像就没了

```
STD（设置方向标志 DF）：将方向标志 DF 设置为1，用于字符串操作。
CLD（清除方向标志 DF）：将方向标志 DF 设置为0，用于字符串操作。
STI（设置中断标志 IF）：将中断标志 IF 设置为1，允许中断。
CLI（清除中断标志 IF）：将中断标志 IF 设置为0，禁止中断
```



当然也可以通过运算,针对于无符号数运算

比如

```
mov al,0x20
add al,0xff //无符号数,加法进位

mov al,0x20
sub al,0xff //无符号数,减法借位
```

基于CF的跳转指令,记住jc/jnc就可以了

```
jc=jb=jnae,jnc=jnb=jae
```



为了让长度更加小,我们就用STC/CLC





CF1=

```assembly
__asm{
	STC
	jc label1 
	nop
	nop
	nop
	nop
	label1 :
}
unsigned char data[7] = {
    0xF9, 0x72, 0x04, 0x90, 0x90, 0x90, 0x90
};
```



CF=0

```assembly
__asm{
	CLC
	jnc label1 
	nop
	nop
	nop
	nop
	label1 :
}
unsigned char data[7] = {
    0xF8, 0x73, 0x04, 0x90, 0x90, 0x90, 0x90
};
```



分别对应了长度是7/5/4



### SF

3),  计算结果是负数,SF=1,正数SF=0

```
mov al,0x10 ;原来是正数
add al,0x7f ;变为负数
xor al,al   ; 变为正数
```

我们不需要关心负多少,正多少

也不需要关心以前的数据是正是负数

只需要关心是负的,是正的就可以了



SF=1,计算是负数

```assembly
xor al,al ;31C0
add al,0x80 ;0480

不一定是是al,还有很多
不一定是0x80,还有很多,满足的条件就是>0x7F
不一定是add,也可以是sub
```



SF=0,计算是正数

```assembly
只要结果是0,就是正数,会比较方便,当然你也可以写的更加复杂,通过计算来获取正数

mov al,0x10
add al,0x50
```

一些mov操作

```assembly
B0 80                         mov     al, 80h ; '€'
B1 80                         mov     cl, 80h ; '€'
B2 80                         mov     dl, 80h ; '€'
B3 80                         mov     bl, 80h ; '€'
B4 80                         mov     ah, 80h ; '€'
B5 80                         mov     ch, 80h ; '€'
B6 80                         mov     dh, 80h ; '€'
B7 80                         mov     bh, 80h ; '€'
```



一些add操作,很奇怪,哈哈

```assembly
;04 80                         add     al, 80h ; '€'
80 C1 80                      add     cl, 80h ; '€'
80 C2 80                      add     dl, 80h ; '€'
80 C3 80                      add     bl, 80h ; '€'
;80 C4 80                      add     ah, 80h ; '€'
80 C5 80                      add     ch, 80h ; '€'
80 C6 80                      add     dh, 80h ; '€'
80 C7 80                      add     bh, 80h ; '€'
```





SF=1

```assembly
__asm{
	push eax
	xor al,al 
	add al,0x80
    js tag1
    nop
    nop
    nop
    nop
    tag1:
    pop eax
}
```



SF=0,只要结果=0,那么的话,等同于ZF=1

```assembly
__asm{
	push eax
    xor al,al
    jns tag1
    nop
    nop
    nop
    nop
    tag1:
    pop eax
}
```

计算版本

```assembly
__asm{
	push eax
    mov al,0x10
    add al,0x30
    jns tag1
    nop
    nop
    nop
    nop
    tag1:
    pop eax
}
```







### OF

把一个寄存器看作有符号的,如果超过了他表示的有符号范围就溢出了

比如无法正数最大0x7F,+1的话,OF=1

比如负数最小是0x80,-1的话,OF=1

注意OF是有符号运算

OF改变只会出现在

正数+正数(正数-负数)

负数+负数(负数-正数)



s那里



```assembly
mov al,0x70
add al,0x70 ;OF溢出了

mov al,0x70
sub al,0x90

mov al,0x80
add al,0x80 ;OF溢出了

mov al,0x80
sub al,0x10

```

不一定是al,

也不一定是0x80,0x70

满足的条件是: 都是正数或者负数







### PF

无论寄存器多大,只看低8位,是否是偶数个1,是的话,PF=1, 

> ps: 是运算结果哟,不是说mov一个,然后低8位,而不是sub,add

奇数个1就PF=0

```
mov al,0x10
add al,0xe0 ;偶数个1

mov al,0x10
add al,0xe8 ;奇数一个1
```

PF跳转是比较麻烦,因为要保证偶数个bit位是1



PF的东西



## call 跳转



没啥介绍的,,,,,,就是修改返回地址

call 可以修改eip

ret也可以修改eip

修改eip的方式



```assembly
    __asm
    {
        call tag1;
        nop
        nop
        nop
        nop
    tag1:
        add byte ptr ss : [esp] , 6;本质上来说是6个,多一个nop就+1
        ret;                                    
        nop
        nop
        nop
        nop
    }
```





```assembly
__asm{
    call label1;
    nop
    nop
    nop
    nop
label1:
    call label2;
    nop
    nop
    nop
    nop
label2:
    call label3;
    nop
    nop
    nop
    nop
label3:
	add esp,12; //恢复栈的正常
}
```



```assembly
    call LABEL1
    nop
    nop
    nop
    nop
LABEL2:
    jmp LABEL3
    nop
    nop
    nop
    nop
LABEL1:
    call LABEL2
    nop
    nop
    nop
    nop
LABEL3:
    add esp,8
```



## 其它类型



### 布尔永真

```c
#include<stdio.h>
#include<Windows.h>
int main()
{
	int a = 10;
	LoadLibrary("kernel64.dll");//函数返回值存储于eax中
	//用确定性构造不确定性
	__asm
	{
		cmp eax, 0;
		jb  LABEL_CanNotBe_1;
		jnb LABEL_MustBe_1;
LABEL_CanNotBe_1:
		push eax;
		add esp,0x100;
		pop esp;
		mov dword ptr [ebp+ecx*4+2],ebx;
		_emit 0xE8;
LABEL_MustBe_1:
	}

	printf("__%d", a);
}
```

### 自毁形



#### ExitProcess



就是无形之间破坏了堆栈的平衡

也就是它无法在函数退出的时候,准确的判断堆栈平衡

比如退出函数长这个样子

```
mov     ecx, [ebp+uExitCode]
push    ecx             ; uExitCode
call    ds:ExitProcess
```

如果来一个这样的代码

```
push    0		; uExitCode
call    ds:ExitProcess
cmp     esi, esp
call    __chkesp
```

可以看到后面那个代码会有一个__chkesp

其实这个代码根本不会执行

然后影响了IDA的反编译

因为IDA不知道怎么翻译它...

因为它不会被执行,,,也不知道应该放在源代码的哪里



#### call后不管后续



比如下面这个]

![Untitled](x86%20%E8%8A%B1%E6%8C%87%E4%BB%A4(x64%20)%20%E9%98%B2%E5%AE%88%E7%AF%87%20516ffa14f0ed4b5986540474ac3fc2ef/Untitled%201.png)

本来最后应该是

```c
.text:0040115B 8B E5                         mov     esp, ebp
.text:0040115D 5D                            pop     ebp
.text:0040115E C3                            retn
```

但是它只有

```c
.text:10005FCD 5D                            pop     ebp
.text:10005FCE 90                            nop
.text:10005FCF 90                            nop
.text:10005FD0 C3                            retn
```

导致esp的东西丢失了

这样会导致栈丢失或者不平衡, 自我猜测

但是,,,,如果说他要干的所有事情都在

```c
.text:10005FC6 E8 87 FE FF FF                call    near ptr sub_10005E52
```

干万了,那么不管以后的异常怎么样,,,他都把坏事早早的结束了

所以后面的事情已经无所谓了





