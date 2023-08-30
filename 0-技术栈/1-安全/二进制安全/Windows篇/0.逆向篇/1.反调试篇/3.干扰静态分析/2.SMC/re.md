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