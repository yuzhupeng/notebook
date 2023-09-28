

# Old



其实就是自己对自己的代码解密,然后再执行

先解密再执行

加密的数据就是自己的字节码



个人认为SMC分为2类,大型和小型

大型: 大型是对整块大区域代码进行一个解密执行,一般只会执行一次

小型: 小型是对小块区域进行一个解密执行, 并且小型的可以类似花指令, 数量可以多,位置可以任意,可重复执行

ps: 对于小型smc的描述过于牛逼,其实也可以做到,我只是不能完全做到罢了



smc是在源码上操作

编译成功后

我们用一个脚本,对编译好的二进制代码的作用区域加密

在运行的时候的操作是解密并执行,所以我们要提前加密



在源代码中加上编译宏定义

```
#pragma comment(linker, "/SECTION:.text,ERW"
```

意思是,`.text` 节区是可读可写可执行的



smc的局限性:

- 只能在固定基地址上操作,为什么? 因为我们的加密就是在ImageBase的基础上操作的,如果别人重定位了,有些和地址相关的字节码发生了变化,我们再去解密执行,就会出现问题



大型SMC demo

```assembly
//x86 example
__asm{
	pushad ;保存环境
	call tag1
tag1:
	pop esi
	add esi,16 ;定位的要解密的数据的开始区域,不一定是0x20哈
	mov ecx,0x1000;作用区域的长度
tag2:
	xor byte ptr[esi],0x77;用于异或的key
	inc esi;推动遍历
	loop tag2 ;ecx循环
	popad ;恢复环境
}
unsigned char data[22] = {
    0x60, 0xE8, 0x00, 0x00, 0x00, 0x00, 0x5E, 0x83, 0xC6, 0x10, 0xB9, 0x00, 0x10, 0x00, 0x00, 0x80, 
    0x36, 0x77, 0x46, 0xE2, 0xFA, 0x61
};
```



小型smc

特点: 

- 长度尽量小
- 作用范围尽量小
- smc和smc之间,距离尽量隔开
- 重复执行不影响



关于重复执行机制

我只是加了一个判断的机制,难免会增加汇编指令的长度

如果0xE8存在,说明是第一次执行

如果不存在,说明已经被执行过了,不再解密,只解密一次

加密过程中,0xE8会变为0x90,不影响执行nop



1字节解密执行

```assembly
    __asm {
        //15字节
        push eax
        call $ + 5
        pop eax
        cmp byte ptr[eax + 16], 0x90
        jz label1
        xor BYTE ptr[eax + 17],0x77
        xor byte ptr[eax + 16], 0x78
        label1:
        pop eax
        _emit 0xe8
    }
    unsigned char data[23] = {
    0x50, 0xE8, 0x00, 0x00, 0x00, 0x00, 0x58, 0x80, 0x78, 0x10, 0x90, 0x74, 0x08, 0x80, 0x70, 0x11, 
    0x77, 0x80, 0x70, 0x10, 0x78, 0x58, 0xE8
    };
```



2字节解密执行

```assembly
    __asm {
        //15字节
        push eax
        call $ + 5
        pop eax
        cmp byte ptr[eax + 18], 0x90
        jz label1
        xor WORD ptr[eax + 19],0x0927
        xor byte ptr[eax + 18], 0x78
        label1:
        pop eax
        _emit 0xe8
    }
    unsigned char data[25] = {
    0x50, 0xE8, 0x00, 0x00, 0x00, 0x00, 0x58, 0x80, 0x78, 0x12, 0x90, 0x74, 0x0A, 0x66, 0x81, 0x70, 
    0x13, 0x27, 0x09, 0x80, 0x70, 0x12, 0x78, 0x58, 0xE8
    };
```



4字节解密执行

```assembly
    __asm {
        //15字节
        push eax
        call $ + 5
        pop eax
        cmp byte ptr[eax + 19], 0x90
        jz label1
        xor DWORD ptr[eax + 20], 0x20010927
        xor byte ptr[eax + 19], 0x78
        label1:
        pop eax
        _emit 0xe8
    }
    unsigned char data[26] = {
    0x50, 0xE8, 0x00, 0x00, 0x00, 0x00, 0x58, 0x80, 0x78, 0x13, 0x90, 0x74, 0x0B, 0x81, 0x70, 0x14, 
    0x27, 0x09, 0x01, 0x20, 0x80, 0x70, 0x13, 0x78, 0x58, 0xE8
    };
```



然后这个3个demo, 不出意外的话,是可以插入在源码的任何地方编译的

ps: 60%出意外,哈哈,但是原理我想应该描述到位了





另外,插入的话,我一般是插入n个0x90

然后用脚本处理编译后的文件,覆盖0x90

然乎处理相关加密,另外那个异或的key尽量做到随机,哈哈





# 病毒样本的SMC



前面对后面解密执行

后面又再对后面继续解密执行,,哈哈

就这样很多的操作

比如下面这个已经解密的demo

```c++
.data:0041A000                               var_C= byte ptr -0Ch
.data:0041A000                               var_4= byte ptr -4
.data:0041A000
.data:0041A000 BD A9 B4 D0 92                mov     ebp, 92D0B4A9h
.data:0041A005 DA C9                         fcmove  st, st(1)
.data:0041A007 D9 74 24 F4                   fnstenv [esp+var_C]
.data:0041A00B 5F                            pop     edi
.data:0041A00C 2B C9                         sub     ecx, ecx
.data:0041A00E B1 A3                         mov     cl, 0A3h
.data:0041A010
.data:0041A010                               loc_41A010:                   ; CODE XREF: sub_41A000+19↓j
.data:0041A010 31 6F 15                      xor     [edi+15h], ebp
.data:0041A013 83 C7 04                      add     edi, 4
.data:0041A016 03 6F 11                      add     ebp, [edi+11h]
.data:0041A019 E2 F5                         loop    loc_41A010
.data:0041A01B DA D2                         fcmovbe st, st(2)
.data:0041A01D D9 74 24 F4                   fnstenv byte ptr [esp-12]
.data:0041A021 58                            pop     eax
.data:0041A022 BB C6 8C 47 E9                mov     ebx, 0E9478CC6h
.data:0041A027 33 C9                         xor     ecx, ecx
.data:0041A029 B1 9D                         mov     cl, 9Dh
.data:0041A02B
.data:0041A02B                               loc_41A02B:                   ; CODE XREF: sub_41A000+34↓j
.data:0041A02B 31 58 17                      xor     [eax+17h], ebx
.data:0041A02E 83 C0 04                      add     eax, 4
.data:0041A031 03 58 13                      add     ebx, [eax+13h]
.data:0041A034 E2 F5                         loop    loc_41A02B
.data:0041A036 D9 EB                         fldpi
.data:0041A038 BB 6B EA 75 F5                mov     ebx, 0F575EA6Bh
.data:0041A03D D9 74 24 F4                   fnstenv [esp-8+var_4]
.data:0041A041 5D                            pop     ebp
.data:0041A042 33 C9                         xor     ecx, ecx
.data:0041A044 B1 96                         mov     cl, 96h
.data:0041A046
.data:0041A046                               loc_41A046:                   ; CODE XREF: sub_41A000+4F↓j
.data:0041A046 31 5D 18                      xor     [ebp+18h], ebx
.data:0041A049 03 5D 18                      add     ebx, [ebp+18h]
.data:0041A04C 83 ED FC                      sub     ebp, 0FFFFFFFCh
.data:0041A04F E2 F5                         loop    loc_41A046
.data:0041A051 DB C3                         fcmovnb st, st(3)
.data:0041A053 D9 74 24 F4                   fnstenv byte ptr [esp-0Ch]
.data:0041A057 5E                            pop     esi
.data:0041A058 33 C9                         xor     ecx, ecx
.data:0041A05A B1 8F                         mov     cl, 8Fh
.data:0041A05C BF 08 79 71 35                mov     edi, 35717908h
.data:0041A061
.data:0041A061                               loc_41A061:                   ; CODE XREF: sub_41A000+6A↓j
.data:0041A061 31 7E 19                      xor     [esi+19h], edi
.data:0041A064 83 EE FC                      sub     esi, 0FFFFFFFCh
.data:0041A067 03 7E 15                      add     edi, [esi+15h]
.data:0041A06A E2 F5                         loop    loc_41A061
.data:0041A06C DB D3                         fcmovnbe st, st(3)
.data:0041A06E B8 9E B6 3E C7                mov     eax, 0C73EB69Eh
.data:0041A073 D9 74 24 F4                   fnstenv byte ptr [esp-0Ch]
.data:0041A077 5A                            pop     edx
.data:0041A077                               sub_41A000 endp ; sp-analysis failed
.data:0041A077
.data:0041A078 33 C9                         xor     ecx, ecx
.data:0041A07A B1 88                         mov     cl, 88h
.data:0041A07C
.data:0041A07C                               loc_41A07C:
.data:0041A07C 31 42 1A                      xor     [edx+1Ah], eax
.data:0041A07F 83 EA FC                      sub     edx, 0FFFFFFFCh
.data:0041A082 03 42 16                      add     eax, [edx+16h]
.data:0041A085 E2 F5                         loop    loc_41A07C
.data:0041A087 DD C6                         ffree   st(6)
.data:0041A089 D9 74 24 F4                   fnstenv byte ptr [esp-0Ch]
.data:0041A08D 58                            pop     eax
.data:0041A08E 31 C9                         xor     ecx, ecx
.data:0041A090 BB 89 2C 9C 44                mov     ebx, 449C2C89h
.data:0041A095 B1 82                         mov     cl, 82h
.data:0041A097
.data:0041A097                               loc_41A097:                   ; CODE XREF: .data:0041A0A0↓j
.data:0041A097 31 58 17                      xor     [eax+17h], ebx
.data:0041A09A 83 C0 04                      add     eax, 4
.data:0041A09D 03 58 13                      add     ebx, [eax+13h]
.data:0041A0A0 E2 F5                         loop    loc_41A097
.data:0041A0A2 BA 2A DC B7 60                mov     edx, 60B7DC2Ah
.data:0041A0A7 D9 C2                         fld     st(2)
.data:0041A0A9 D9 74 24 F4                   fnstenv byte ptr [esp-0Ch]
.data:0041A0AD 58                            pop     eax
.data:0041A0AE 33 C9                         xor     ecx, ecx
.data:0041A0B0 B1 7B                         mov     cl, 7Bh ; '{'
.data:0041A0B2
.data:0041A0B2                               loc_41A0B2:
.data:0041A0B2 31 50 13                      xor     [eax+13h], edx
.data:0041A0B5 03 50 13                      add     edx, [eax+13h]
.data:0041A0B8 83 C0 04                      add     eax, 4
.data:0041A0BB E2 F5                         loop    loc_41A0B2
.data:0041A0BD BE 52 A6 A4 7A                mov     esi, 7AA4A652h
.data:0041A0C2 DB D0                         fcmovnbe st, st
.data:0041A0C4 D9 74 24 F4                   fnstenv byte ptr [esp-0Ch]
.data:0041A0C8 5A                            pop     edx
.data:0041A0C9 31 C9                         xor     ecx, ecx
.data:0041A0CB B1 74                         mov     cl, 74h ; 't'
.data:0041A0CD
.data:0041A0CD                               loc_41A0CD:
.data:0041A0CD 31 72 14                      xor     [edx+14h], esi
.data:0041A0D0 03 72 14                      add     esi, [edx+14h]
.data:0041A0D3 83 EA FC                      sub     edx, 0FFFFFFFCh
.data:0041A0D6 E2 F5                         loop    loc_41A0CD
.data:0041A0D8 B8 7E 38 FF 9B                mov     eax, 9BFF387Eh
.data:0041A0DD DB CF                         fcmovne st, st(7)
.data:0041A0DF D9 74 24 F4                   fnstenv byte ptr [esp-0Ch]
.data:0041A0E3 5F                            pop     edi
.data:0041A0E4 31 C9                         xor     ecx, ecx
.data:0041A0E6 B1 6D                         mov     cl, 6Dh ; 'm'
.data:0041A0E8
.data:0041A0E8                               loc_41A0E8:                   ; CODE XREF: .data:0041A0F1↓j
.data:0041A0E8 31 47 15                      xor     [edi+15h], eax
.data:0041A0EB 83 C7 04                      add     edi, 4
.data:0041A0EE 03 47 11                      add     eax, [edi+11h]
.data:0041A0F1 E2 F5                         loop    loc_41A0E8            ; CODE XREF: .data:0041A085↑j
.data:0041A0F3 B8 95 51 97 49                mov     eax, 49975195h
.data:0041A0F8 D9 CE                         fxch    st(6)
.data:0041A0FA D9 74 24 F4                   fnstenv byte ptr [esp-0Ch]
.data:0041A0FE 5A                            pop     edx
.data:0041A0FF 33 C9                         xor     ecx, ecx
.data:0041A101 B1 67                         mov     cl, 67h ; 'g'
.data:0041A103
.data:0041A103                               loc_41A103:                   ; CODE XREF: .data:0041A10C↓j
.data:0041A103 31 42 12                      xor     [edx+12h], eax
.data:0041A106 83 EA FC                      sub     edx, 0FFFFFFFCh
.data:0041A109 03 42 0E                      add     eax, [edx+0Eh]
.data:0041A10C E2 F5                         loop    loc_41A103
.data:0041A10E BD CB AA 0C 98                mov     ebp, 980CAACBh
.data:0041A113 D9 E9                         fldl2t
.data:0041A115 D9 74 24 F4                   fnstenv byte ptr [esp-0Ch]
.data:0041A119 58                            pop     eax
.data:0041A11A 31 C9                         xor     ecx, ecx
.data:0041A11C B1 60                         mov     cl, 60h ; '`'
.data:0041A11E
.data:0041A11E                               loc_41A11E:                   ; CODE XREF: .data:0041A127↓j
.data:0041A11E 83 E8 FC                      sub     eax, 0FFFFFFFCh
.data:0041A121 31 68 0F                      xor     [eax+0Fh], ebp
.data:0041A124 03 68 0F                      add     ebp, [eax+0Fh]
.data:0041A127 E2 F5                         loop    loc_41A11E
.data:0041A129
.data:0041A129                               loc_41A129:                   ; CODE XREF: .data:0041A180↓j
.data:0041A129 DA D6                         fcmovbe st, st(6)
.data:0041A12B D9 74 24 F4                   fnstenv byte ptr [esp-0Ch]
.data:0041A12F BD 6B 4B 48 3E                mov     ebp, 3E484B6Bh
.data:0041A134 5F                            pop     edi
.data:0041A135 2B C9                         sub     ecx, ecx
.data:0041A137 B1 59                         mov     cl, 59h ; 'Y'
.data:0041A139
.data:0041A139                               loc_41A139:                   ; CODE XREF: .data:0041A142↓j
.data:0041A139 83 C7 04                      add     edi, 4
.data:0041A13C 31 6F 15                      xor     [edi+15h], ebp
.data:0041A13F 03 6F 15                      add     ebp, [edi+15h]
.data:0041A142 E2 F5                         loop    loc_41A139
```

其实可以看出,,他有12个SMC

一直在对自己后面的代码解密执行

