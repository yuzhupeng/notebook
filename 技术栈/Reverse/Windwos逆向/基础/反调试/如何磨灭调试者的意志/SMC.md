# SMC

具体细节不再讲解

原理很简单,其实懒得写

在这里留下一个去除SMC的idc脚本

```c
#include <idc.idc>
static xor_setp2()
{
	auto arr=0x0401500;
	//auto end;
    auto len  = 187;
    auto i = 0;
    for(i=0;i<len;i++)   //循环结束的条件为字节串的结束地址
    {
        PatchByte(arr+i,Byte(arr+i)^0x41);   //异或的数字根据情况修改
    }
}
static main()
{
	xor_setp2();
}
```

# SMC加解密算法

有种SMC加密算法是异或就完事

当然我们可以对SMC算法做一个简单的复杂化

如果过于复杂就是去了意义

```c
debug043:00E0000C 31 C9                         xor     ecx, ecx
debug043:00E0000E B9 1A 6B 05 00                mov     ecx, 56B1Ah
debug043:00E0000E
debug043:00E00013
debug043:00E00013                               loc_E00013:                   ; CODE XREF: debug043:00E0001C↓j
debug043:00E00013 83 C5 04                      add     ebp, 4
debug043:00E00016 31 45 17                      xor     [ebp+17h], eax
debug043:00E00019 03 45 17                      add     eax, [ebp+17h]
debug043:00E0001C E2 F5                         loop    loc_E00013
```

比如这个算法,就不是简单的异或

把异或变得稍微复杂了一点点

以前的异或,是很对称了,

也就是加密解密采用相同的算法,这里就不一样

伪代码

```c
ebp=____;
eax=____;
for(ecx=56B1Ah;ecx>0;ecx--)
{
	[ebp+17h]^= eax;
	eax+=[ebp+17h];
	ebp+=4;
}
```