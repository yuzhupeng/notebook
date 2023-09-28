

# 调用门

Windows系统没有再使用调用门, 但是使用了中断门

学习调用门是为了更好地理解中断门 后续讲解中断门



`JMP FAR` 只能段间跳转? 也就是段之间的跳转,此刻是不提权的状态下

`CALL FAR` 也可以实现段间跳转,也是不提权的状态下

`CALL FAR` 也可以实现一个提权,,这就是跨段的跳转了

下面是代码跨段的跳转,涉及提权的

格式是 `CALL FAR xx:yyyy`

其中xx是一个调用门的描述符

然后yyyy是一个废弃的参数,随便填写什么都可以的







调用门的执行情况,现在看不懂没关系

分析1

![image-20230910085904021](./img/image-20230910085904021.png)

分析2

![](./img/image-20230910102855076.png)

另外`CALL FAR`不会把返回地址压入R3的堆栈

如果GDT表的成员不是段描述符的话,可能是调用门

怎么看他不是段描述符,而是调用门呢?

在描述符的S位,以及TYPE域中有说明的

S=0,说明是非段描述符符

进而..TYPE=0xC,说明是调用门



调用门的情况如下

![img](./img/image-20230911121622612.png)

关于调用门的TYPE域

![](./img/image-20230911154433755.png)

关于他的分布

![image-20230922172537620](./img/image-20230922172537620.png)



关于参数的个数,,其实卡得不是那么死

虽然最多15个,但是我们获取了三环的ESP/EBP,

我们就可以按照我们约定俗成的调用约定,来提取任意多个参数进来,

 

为什么调用门不稳定?

>在GDT的`0x80003F00~0x80003F90` 这段空间中,,,操作系统一直在修改他的地址
>
>所以如果你把调用门写在一个经常变化的内存中,就容易g
>
>别人说的: 能用GDT构建调用门,就不用IDT,,因为IDT容易被检测
>
>IDT前31个成员基本上是固定的,但是GDT表经常在修改,
>
>经常在修改的就不会被调用

 答案不正确,,酌情了解



## 无参调用门

Windows中并没有使用调用门，所以需要手动构造一个调用门。

然后调用门也在GDT表中,

然后调用们中的段选择子也位于GDT表中

所以,一般情况我们需要构造2段数据的



 

```c
---高位地址
Offset in Segment 31:16 = 0x0000		//暂定
					  P = 1	
					DPL = 二进制:11		//该段的权限,不太理解
					S	= 0
				   TYPE	= 二进制:1100
			Param.Count = 二进制:00000		//几个参数
	   Segment Selector = 0x0008		//段选择子
Offset in Segment 15:00 = 0x0000		//暂定
--- 地位地址
```



下面是一个通过调用门提权的操作



然后因为我没有环境,就写了一些自己的理解在代码注释里面

```c
#include <windows.h>
#include <stdio.h>
char Hello[]="Hello World\n";
void __declspec(naked) enHancePrivilege()
{
    __asm
    {
 /*
        push ebp
        mov ebp,esp
        sub esp,16
        lea eax,Hello
        push eax
        call eax
        add esp,4
        pop ebp
        mov esp,ebp
        pop ebp
 */
        retf        //注意返回, 不能是ret!
    }
}

int main()
{
    char buff[6];

    *(DWORD*)&buff[0] = 0x00000000; //可以随便写, 这个就是EIP, EIP是废弃的
    *(WORD*)&buff[4] = 0x48;        //不能随便写,涉及了索引值和RPL 通过它寻找调用门的描述符.
    //而调用门的段描述符涉及了最后前往的XX:EIP

    __asm
    {
        call fword ptr[buff]        //长调用
    }

    getchar();

    return 0;
}
```



```
0: kd> eq 8003f048 0040ec00`00081020
0: kd> dq 8003f000
8003f000  00000000`00000000 00cf9b00`0000ffff
8003f010  00cf9300`0000ffff 00cffb00`0000ffff
8003f020  00cff300`0000ffff 80008b04`200020ab
8003f030  ffc093df`f0000001 0040f300`00000fff
8003f040  0000f200`0400ffff 0040ec00`00081020
8003f050  80008955`27000068 80008955`27680068
8003f060  00009302`2f40ffff 0000920b`80003fff
8003f070  ff0092ff`700003ff 80009a40`0000ffff

```

首先代码运行到call far的时候,程序会终止到windbg的0环调试器

```
Single step exception - code 80000004 (first chance)
First chance exceptions are reported before any exception handling.
This exception may be expected and handled.
00401020 55              push    ebp

```



当我们进入调用前

![image-20230126233743685](./img/image-20230126233743685.png)

进入调用后,出现在windbg调试器

```
0: kd> r esp
esp=ee9badd0 //新的栈
0: kd> dd ee9badd0
ee9badd0  	0040de39 //返回的EIP
            0000001b //以前的CS
            0012ff28 //以前的ESP
            00000023 //以前的CS
ee9bade0  	805470de 
            f7353b85 
            85f187e0 
            00000000
ee9badf0  	...

```





然后是一个通过提权后,读取高2G内存搞事的代码 

```c
#include <windows.h>
 
BYTE GDT[6] = {0};
DWORD dwH2GValue;
 
void __declspec(naked) GetRegister()
{
    __asm
    {
        pushad
        pushfd
 
        mov eax,0x8003f00c    //读取高2G内存
        mov ebx,[eax]
        mov dwH2GValue,ebx
        sgdt GDT;             //读取GDT
 
        popfd
        popad
 
        retf                  //注意返回, 不能是ret
    }
}
 
void PrintRegister()
{
    DWORD GDT_ADDR = *(PDWORD)(&GDT[2]);
    WORD GDT_LIMIT = *(PWORD)(&GDT[0]);
 
    printf("%x %x %x\n", dwH2GValue,GDT_ADDR,GDT_LIMIT);
}
 
int main()
{
    char buff[6];
    __asm
    {
        mov ebx,ebx
        mov ebx,ebx
    }
 
    *(DWORD*)&buff[0] = 0;
    *(WORD*)&buff[4] = 0x48;//CS 去往:GDT 索引:9 然后请求:R0
 
    
    //调用门:0040ec00`00081020
    __asm
    {
        call fword ptr[buff]//进入调用门
    }
 
    PrintRegister();
 
    getchar();
 
    return 0;
}
```

运行结果

![image-20230126231814236](./img/image-20230126231814236.png)



后面,海东老师讲到

读取GDTR的指令`SGDT`在ring3也可以正常使用

于是我自己跑了一下

```c
#include <windows.h>
#include<stdio.h>
BYTE GDT[6] = { 0 };

void __declspec(naked) GetRegister()
{
    __asm
    {
        sgdt GDT;
        ret        
    }
}

void PrintRegister()
{
    DWORD GDT_ADDR = *(PDWORD)(&GDT[2]);
    WORD GDT_LIMIT = *(PWORD)(&GDT[0]);

    printf("%x %x\n",GDT_ADDR, GDT_LIMIT);
}

int main()
{
    GetRegister();
    PrintRegister();
    return 0;
}
```

输出

```
4b590000 7f
```

啊这...好像不太对

但是,这个指令有个神奇的效果(程序调试的时候,莫名其妙的跑飞)



## 有参

其实也就说设置一下参数个数,然后再push

调用门配置

```
---高位地址
Offset in Segment 31:16 = 0x0000		// 暂定
					  P = 1
					DPL = 二进制:11
					S	= 0
				TYPE	= 二进制:1100
			Param.Count = 二进制:00011	// 注意变化！
	   Segment Selector = 0x0008
Offset in Segment 15:00 = 0x0000		// 暂定
--- 地位地址
```



假如有参数

他会在R3,把参数压入栈中

同时,在调用门返回的时候

也是在R3把那几个数据给pop出栈

虽然真正的ret在R0的

但是堆栈的平衡却在R3





# 门的权限检查



首先在调用call的时候

权限角度,跳转前

CPL<=DPL : 也就是要提权的意思

门的段选择子的RPL==门的段选择子的DPL : 也就是权限要同级的意思,,,RPL是未来的DPL

ps:我们没有去管门的DPL,和访问门的RPL



其实这一个仍然不是很清楚的

因为段选择子有RPL,门有DPL,门里面的段选择子也有DPL和RPL

所以哪一个和哪一个之间比较呢???

所以我们有2个DPL,2个RPL,一个CPL







比入实验1无参用的调用门

用的门是0x48, 

0x48对应的段选择子是0xB0或者0x08

当然CPL=R3

0xB0中,RPL=R0,对应的DPL=R0,所以权限检查通过

0x08中,RPL=R0,对应的DPL=R0,所以权限检查通过



# 实验





关于调用门的选择子都不是我们去构造的,

而是写的一些内置的以及存在的TSS描述符

后面再来慢慢了解



## int 8逆向



cli 指令让EFLAG的if位置零,

可屏蔽置空...然后别人就不可以打断它



## 实验1 无参调用,读取内存0x8003f034;



测试代码如下



```c++
#include<stdio.h>
#include <stdlib.h>

int xx;
void _declspec(naked) test()
{
	__asm{
		int 3;//方便后面的调试,去查看寄存器和堆栈
		push eax;
		push ecx;
		mov eax,0x8003f034;
		mov eax,DWORD PTR DS:[eax];
		lea ecx,xx;
		mov DWORD PTR DS:[ecx],eax;
		pop ecx;
		pop eax;
		retf;
	}
}
int main()
{
	char buf[]={0,0,0,0,0x48,0};
	*(int*)buf=(int)test;
	printf("%X\n",test);
	__asm{
		push fs;
		call fword ptr ds:[buf];
		pop fs;
	}
	printf("hi you get here %x\n",xx);
	getchar();
	return 0;
}
```





先断下来,然后查看函数地址

![image-20230923170103367](./img/image-20230923170103367.png)

地址如下

![image-20230923171807951](./img/image-20230923171807951.png)

然后构造调用门

于是windbg break;

```c++
0: kd> r gdtr
gdtr=8003f000
0: kd> dq 8003f000
8003f000  00000000`00000000 00cf9b00`0000ffff
8003f010  00cf9300`0000ffff 00cffb00`0000ffff
8003f020  00cff300`0000ffff 80008b04`200020ab
8003f030  ffc093df`f0000001 0040f300`00000fff
8003f040  0000f200`0400ffff 00000000`00000000
8003f050  80008955`87000068 80008955`87680068
8003f060  00009302`2f40ffff 0000920b`80003fff
8003f070  ff0092ff`700003ff 80009a40`0000ffff
0: kd> eq 8003f048 0040ec00`0008100a
0: kd> g

```

看一下寄存器

![image-20230923172529954](./img/image-20230923172529954.png)

发现CS=0x08,然后bin(0x08)='0b1000'

可以发现是CPL=R0了..所以R0就可以干很多事情了



然后说一下调用门吧 0040ec00`0008100a

```c++
|---------------------------------------|
| TYPE | S | DPL | P | AVL | - | DB | G |
| 0011 | 0 | 11  | 1 | 0   | 0 | 1  | 0 |
|---------------------------------------|
```

先不说调用门的选择子指向了什么东西,

调用门本身的DPL=R3,,,然后CPL=R3

最后去往内核后是CPL=R0的



如果换成 00408c00`0008100a

```c++
|---------------------------------------|
| TYPE | S | DPL | P | AVL | - | DB | G |
| 0011 | 0 | 00  | 1 | 0   | 0 | 1  | 0 |
|---------------------------------------|
```

CPL=R3,DPL=R0,,发现根本无法去往内核





然后继续运行vc6

windbg 停下来了

```c++
Break instruction exception - code 80000003 (first chance)
004010f0 cc              int     3
0: kd> p
004010f1 50              push    eax
0: kd> p
004010f2 51              push    ecx
1: kd> p
004010f3 b834f00380      mov     eax,8003F034h
3: kd> p
004010f8 3e8b00          mov     eax,dword ptr ds:[eax]
3: kd> p
004010fb 8d0d288e4200    lea     ecx,ds:[428E28h]
3: kd> p
00401101 3e8901          mov     dword ptr ds:[ecx],eax

```

然后再次回到vc6

成功运行

![image-20230923174244289](./img/image-20230923174244289.png)

且

![image-20230923174310470](./img/image-20230923174310470.png)





遇到的无法解决的问题?



> 未知的报错1

windbg出现报错



打断点的情况如下

![image-20230910161250223](./img/image-20230910161250223.png)



windbg出现报错

```c++
Single step exception - code 80000004 (first chance)
First chance exceptions are reported before any exception handling.
This exception may be expected and handled.
nt_400000!_imp__VidSetTextColor+0x2:
00b0:0040100a e911000000      jmp     nt_400000!_imp__VidCleanUp (00401020)
0: kd> g
//然后继续正常运行
```

期间出现1次停止,估计是遇到异常

异常是Single step exception -然后g一下,完美运行





> 未知的报错2



就是会出现很多次异常,1次或者很多次

每次点击确定,就没事了,会在

![image-20230910161627420](./img/image-20230910161627420.png)

F9之后正常运行





## 实验2 有参调用,并正常返回



在`实验1 无参`的基础上,我们继续实验

```c++
eq 8003f048 0040ec04`00b0100a
```



参数4个

```c++
#include<stdio.h>
#include <stdlib.h>

void _declspec(naked) test()
{
	__asm{
		int 3;//方便后面的调试,去查看寄存器和堆栈

		retf;
	}
}
int main()
{
	char buf[]={0,0,0,0,0x48,0};
	*(int*)buf=(int)test;
	printf("%X\n",test);
	__asm{
		push fs;
		push 1;
		push 2;
		push 3;
		push 4;
		call fword ptr ds:[buf];
		pop fs;
	}
	printf("hi you get here\n");
	getchar();
	return 0;
}
```







![image-20230910163135952](./img/image-20230910163135952.png)



当windbg停下来

```c++
0: kd> dc esp
0040de06 ;返回的IP
0000001b ;返回的CS
00000004 
00000003   
00000002 
00000001 
0012ff14 ;返回的ESP
00000023 ;返回的SS
```



如果g一下的话,,,系统就Game Over了

为什么? 以为堆栈没有平衡

没平衡的是传递进去的那4个参数

其中

`0040de06 0000001b`是CS:IP

`0012ff14 00000023`是SS:SP

`00000004 00000003 00000002 00000001`是传递进来的参数





所以我们要去平衡堆栈

```c++
void _declspec(naked) test()
{
	__asm{
		int 3;//方便后面的调试,去查看寄存器和堆栈
		retf 4*4;
	}
}
```





于是断点运行

```c++
0: kd> eq 8003f048 0040ec04`0008100a
0: kd> g
watchdog!WdUpdateRecoveryState: Recovery enabled.
Single step exception - code 80000004 (first chance)
First chance exceptions are reported before any exception handling.
This exception may be expected and handled.
nt_400000!_imp__VidSetTextColor+0x2:
0040100a e921000000      jmp     nt_400000!_imp_KfLowerIrql (00401030)
0: kd> g
Break instruction exception - code 80000003 (first chance)
nt_400000!_imp_KfLowerIrql:
00401030 cc              int     3

```



![image-20230910194442997](./img/image-20230910194442997.png)



可以看到成功运行

![image-20230910194625701](./img/image-20230910194625701.png)







## 实验3 提权 调用内核 API



调用一个 RtlInitAnsiString

先去获取该函数的地址



```c++
0: kd> r idtr
idtr=8003f400
0: kd> uf RtlInitAnsiString
nt!RtlInitAnsiString:
8052f734 57              push    edi
8052f735 8b7c240c        mov     edi,dword ptr [esp+0Ch]
8052f739 8b542408        mov     edx,dword ptr [esp+8]
8052f73d c70200000000    mov     dword ptr [edx],0
8052f743 897a04          mov     dword ptr [edx+4],edi
8052f746 0bff            or      edi,edi
8052f748 741e            je      nt!RtlInitAnsiString+0x34 (8052f768)
```



vc 源码如下

```c++
#include<stdio.h>
#include <stdlib.h>
#include <windows.h>

typedef struct _STRING {
    WORD Length;
    WORD MaximumLength;
    char* Buffer;
} STRING;

STRING sz_dest;
char sz_src[]="D0g3.21.Reverse.redqx";

typedef  int (__stdcall *lp_RtlInitAnsiString)( STRING* DestinationString, char* SourceString);
lp_RtlInitAnsiString lp_Nt_func =(lp_RtlInitAnsiString)0x8052f734;


void _declspec(naked) test()
{
	//视频将的是在这里保存一个环境,,,我是真没看明白的
	__asm{
		int 3;
		mov ax,0x30;
		mov fs,ax;
		lea eax,sz_src;
		push eax;
		lea eax, sz_dest;
		push eax;
		call lp_Nt_func;
		retf;
	}
}
int main()
{
	char buf[]={0,0,0,0,0x48,0};
	printf("%08X\n",test);
	__asm{
		push fs;
		call fword ptr buf;
		pop fs;
	}
	printf("%s\n",sz_dest.Buffer);
	getchar();
	return 0;
}
```





然后去简单的构造一个调用门

```c++
kd> eq 8003f048 0040ec00`0008100a
```



然后就正常的往下走

然后windbg断下来

```c++
Break instruction exception - code 80000003 (first chance)
00401020 cc              int     3
0: kd> p
00401021 66b83000        mov     ax,30h
0: kd> p
00401025 668ee0          mov     fs,ax
1: kd> p
00401028 8d05305a4200    lea     eax,ds:[425A30h]
1: kd> p
0040102e 50              push    eax
2: kd> p
0040102f 8d05588e4200    lea     eax,ds:[428E58h]
2: kd> p
00401035 50              push    eax
2: kd> p
00401036 ff15485a4200    call    dword ptr ds:[425A48h]
2: kd> p
0040103c cb              retf
2: kd> g

```

然后继续运行,如下

正常情况如下,完美运行

![image-20230911195506279](./img/image-20230911195506279.png)



像这种代码的话,我就会运行失败的,,,,可能是返回的时候失败

但是具体原因还不知道的

```c++
void _declspec(naked) test()
{
	//视频将的是在这里保存一个环境,,,我是真没看明白的
	__asm{
		int 3;
		pushad;
		pushfd;
		push fs;
		mov ax,0x30;
		mov fs,ax;
		lea eax,sz_src;
		push eax;
		lea eax, sz_dest;
		push eax;
		call lp_Nt_func;
		pop fs;
		popfd;
		popad;
		retf;
	}
}
int main()
{
	char buf[]={0,0,0,0,0x48,0};
	printf("%08X\n",test);
	__asm{
		//pushad;
		//pushfd;
		//push fs;
		call fword ptr buf;
		//pop fs;
		//popfd;
		//popad;
	}
	printf("%s\n",sz_dest.Buffer);
	getchar();
	return 0;
}
```





## 实验4 失败+未知



在对全局变量写入的时候

就会触发一个异常

然后就g了



```c
#include <windows.h>
#include<stdio.h>

DWORD x;
DWORD y;
DWORD z;

void __declspec(naked) CateProc()
{
	__asm
	{

		int 3;
		pushad;
		pushfd;

	 
	
		mov eax,[esp+4*11+0*4];
		lea ecx,x;
		mov [ecx],eax;

		mov eax,[esp+4*11+1*4];
		lea ecx,y;
		mov [ecx],eax;
		
		mov eax,[esp+4*11+2*4];
		lea ecx,z;
		mov [ecx],eax;

		popfd;
		popad;
		retf 0xC			// 注意堆栈平衡 写错蓝屏
	}
}

void PrintRegister()
{
	printf("%d %d %d\n", x, y, z);
	return ;
}

int main(int argc, char* argv[])
{
	char buff[]={0,0,0,0,0x48,0};
	printf("%p\n",CateProc);
	__asm
	{
		push fs
		push 1					// 参数1
		push 2					// 参数2
		push 3					// 参数3
 		call fword ptr[buff] 	// 压入SS ESP CS IP 这里依次压入16 32 16 32 所以
		pop fs;
	}
	PrintRegister();
	getchar();
	return 0;
}
```





# 复习



其实就讲了一个调用门的组成

如何去使用一个调用门

以及调用门相关描述符的组成

权限什么的,,,调用门的DPL好像都是R3吧



1), 调用门的一个调用过程

2), 相关TYPE域的说明







