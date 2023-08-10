

# 垃圾代码

向程序添加大量无意义的代码来增加代码调试的难度，

这就是“垃圾代码”反调试技术。

尤其是，这些垃圾代码中还含有真正有用的代码或者应用其他反调试技术时，

调试程序会变得更加困难。

但是实际的垃圾代码往往具有精巧又复杂的形态，含有大量条件分支语句和无尽的函数调用，想要跳过它们并非易

# 待学习链接

**一些简单的花指令的解析(含Intel指令集)** https://www.52pojie.cn/thread-1068444-1-1.html

花指令 :

如何构造: [https://www.52pojie.cn/thread-326995-1-1.html](https://www.52pojie.cn/thread-326995-1-1.html)

如何用IDC解决: [https://www.cnblogs.com/LyShark/p/13100048.html](https://www.cnblogs.com/LyShark/p/13100048.html)

我们不应该垃圾代码不仅仅起着一个混淆的作用

因为一旦nop后,垃圾代码就不再起作用.我们应该

1). 让垃圾代码参与加密和运算,你的垃圾代码甚至可以是一堆字符串,用于加密

2). 让垃圾代码和有效代码加密挨着,穿插运行(不知道如何实现)

3). 

# 无条件直接儿跳

```nasm
_asm{
    jmp  LABEL1
      _emit 0xE8; //junk_code
    LABEL1:
}
```

# 基于互补条件的跳

```
_asm{
    jz LABEL1
    jnz LABEL1
    db junk_code
    LABEL1:
}
```

# 基于条件跳转

关于一些指令该这么跳

可以去参考<<加密解密>>.120

跳转又被分为断跳转和长跳转

这个种跳转都是短跳转机器码有

比如jmp的无条件跳转

```
EB XX :短跳转
E9 XX XX XX XX :长跳转
```

jz/jnz的短跳转

```
75 XX: 不等于
74 XX: 等于
```

jz/jnz的长跳转

```
0F 84 XX XX XX XX :jz
0F 85 XX XX XX XX :jnz
```

...其它的再问ChatGpt吧...

然后接着说:

jmp 无条件跳转 用的比较多,但是比较赤裸

主要用的还有有条件跳转

之前已经说开jz/jnz了,现在说说其它的跳转,已经如何初始化对应的flag标志位

| 助记符 | 说明 |  |  |
| --- | --- | --- | --- |
| JA=JNBE | > | CF|ZF=0 |  |
| JB=JNAE | < | CF=1 |  |
| JAE=JNB | >= | CF=0 |  |
| JNA=JBE | <= | CF|ZF=1 |  |

STC 让CF=1 适用于 JB 跳转

CLC 让CF=0 适用于 JNB 跳转

greate/little, above/ below

| 助记符 | 说明 |  |
| --- | --- | --- |
| JG=JNLE | > | (SF ^OF)|ZF=0 |
| JL=JNGE | < | SF^OF=1,意思SF = OF |
| JGE=JNL | >= | SF ^ OF=0,意思是SF! = OF |
| JLE=JNG | <= | (SF^OF)|ZF=1 |

## jns

![Untitled](x86%20%E8%8A%B1%E6%8C%87%E4%BB%A4(x64%20)%20%E9%98%B2%E5%AE%88%E7%AF%87%20516ffa14f0ed4b5986540474ac3fc2ef/Untitled.png)

可以看到中间的垃圾代码填充了很多,

填充得越多, 效果就越好

# 布尔永真

```
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

# 基于eip修改

没啥介绍的,,,,,,就是修改返回地址

call 可以修改eip

ret也可以修改eip

修改eip的方式

```
    call label1;
    _emit JunkCode;
label1:
    call label2;
    _emit JunkCode;
label2:
    call label3;
    _emit JunkCode;
label3:
	add esp,xxx; //恢复栈的正常
```

## 1)一般的修改栈的eip

```
#include<stdio.h>
int main()
{
    int a = 10;
    __asm
    {
        call LABEL9;
        _emit 0x83;  //从这里开始加8个字节就是a = 3                //占1个字节,
    LABEL9:
        add dword ptr ss : [esp] , 8;                           //占5个字节
        ret;                                                    //占1个字节
        __emit 0xF3;                                            //占1个字节
    }
    a = 3;
    printf("Dqx->%d", a);
}
```

## 2) 无限call不ret-x1

```
#include<stdio.h>
int main()
{
    int a = 10;
    __asm {
            push eax
            call LABEL1
            _emit 29h  //ip1
            _emit 5Ah
LABEL1 :
            POP eax
            imul eax, 3
            call LABEL2

            db junkCode
LABEL2 :
            //[esp]=ip2,[esp-4]=eax
            add esp, 4  //[esp]=eax
            pop eax
    }
    printf("Flower_Dqx->%d", a);
}
```

## 3) 无限call不ret-x2

```
    call LABEL1
    db 0E8h
LABEL2:
    jmp LABEL3
    db junk_code
LABEL1:
    :call LABEL2
LABEL3:
    add esp,8
```

# 基于堆栈的破坏

## 自毁形

### ExitProcess

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

### call后不管ebp

比如下面这个

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