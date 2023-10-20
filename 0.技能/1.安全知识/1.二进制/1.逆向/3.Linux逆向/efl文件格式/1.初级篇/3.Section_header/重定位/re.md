

# .rel(a).dyn | .rel(a).plt

不管是`.rel(a).dyn`还是`.rel(a).plt`他们的数据结构都是一样的

只是针对不同的功能类型进行的一个区分和新的名字







.rel.dyn 包含了动态链接的二进制文件中需要重定位的变量的信息，

而 .rel.plt 包含了需要重定位的函数的信息。这两类重定位节都使用如下的结构 

重定位项结构



```c++
//x86
typedef struct
{
  Elf32_Addr	r_offset;		/* Address */
  Elf32_Word	r_info;			/* Relocation type and symbol index */
} Elf32_Rel;
typedef struct
{
  Elf32_Addr	r_offset;		/* Address */
  Elf32_Word	r_info;			/* Relocation type and symbol index */
  Elf32_Sword	r_addend;		/* Addend */
} Elf32_Rela;


//x64
typedef struct
{
  Elf64_Addr	r_offset;		/* Address */
  Elf64_Xword	r_info;			/* Relocation type and symbol index */
} Elf64_Rel;
typedef struct
{
  Elf64_Addr	r_offset;		/* Address */
  Elf64_Xword	r_info;			/* Relocation type and symbol index */
  Elf64_Sxword	r_addend;		/* Addend */
} Elf64_Rela;
```



r_offset

本数据成员给出重定位所作用的位置。

 

> r_info:



本数据成员既给出了重定位所作用的符号表索引，也给出了重定位的类型。

比如，如果是一个函数的重定位，本数据成员将要持有被调用函数所对应的符号表索引。

如果索引值为 STN_UNDEF，即未定义索引，那么重定位过程中将使用 0 作为符号值。

重定位类型依处理器不同而不同，各种处理器将分别定义自己的类型。

如果一种处理器规定自己引用了一个重定位项的类型或者符号表索引，

表明这种处理器应用了 ELF32_R_TYPE 或 ELF32_R_SYM 到其重定位项的 r_info 成员

ps: 听不懂

以下是应用于 r_info 的宏定义。

```
#define ELF32_R_SYM(i) ((i)>>8)
#define ELF32_R_TYPE(i) ((unsigned char)(i))
#define ELF32_R_INFO(s,t) (((s)<<8)+(unsigned char)(t))
```



> r_addend



本成员指定了一个加数，这个加数用于计算需要重定位的域的值

Elf32_Rela 与 Elf32_Rel 在结构上只有一处不同，就是前者有 r_addend。

Elf32_Rela 中是用 r_addend 显式地指出加数；而对 Elf32_Rel 来说，加数是隐含在被修改的位置里的

Elf32_Rel 中加数的形式这里并不定义，它可以依处理器架构的不同而自行决定。

在特定处理器上如何实现，可以指定一种固定的格式，也可以不指定格式而依据上下文来解析



一个“重定位节(relocation section)”需要引用另外两个节：

一个是符号表节,一个是被修改节。

在重定位节中，节头的 sh_info 和 sh_link 成员分别指明了引用关系。

不同的目标文件中，重定位项的 r_offset 成员的含义略有不同。







> DT_PLTREL：

指明PLT重定位的类型，这一项的value就高级了，value的值是DT_REL（17）或DT_RELA（7）宏的值







> RALA

ELF RELA Relocation Table

```
LOAD:0000000000000470                               ; ELF RELA Relocation Table
LOAD:0000000000000470 Elf64_Rela <3DF8h, 8, 1120h>  ; R_X86_64_RELATIVE +1120h
LOAD:0000000000000488 Elf64_Rela <3E00h, 8, 10E0h>  ; R_X86_64_RELATIVE +10E0h
LOAD:00000000000004A0 Elf64_Rela <4030h, 8, 4030h>  ; R_X86_64_RELATIVE +4030h
LOAD:00000000000004B8 Elf64_Rela <3FD8h, 200000006h, 0> ; R_X86_64_GLOB_DAT _ITM_deregisterTMCloneTable
LOAD:00000000000004D0 Elf64_Rela <3FE0h, 900000006h, 0> ; R_X86_64_GLOB_DAT worker
LOAD:00000000000004E8 Elf64_Rela <3FE8h, 400000006h, 0> ; R_X86_64_GLOB_DAT __gmon_start__
```





> JMPREL

ELF JMPREL Relocation Table

```
LOAD:0000000000000530                               ; ELF JMPREL Relocation Table
LOAD:0000000000000530  Elf64_Rela <4018h, 100000007h, 0> ; R_X86_64_JUMP_SLOT pthread_create
LOAD:0000000000000548  Elf64_Rela <4020h, 300000007h, 0> ; R_X86_64_JUMP_SLOT printf
LOAD:0000000000000560  Elf64_Rela <4028h, 600000007h, 0> ; R_X86_64_JUMP_SLOT sleep
```





重定位的过程中,,其实就包含了一些API的导入

X86_64_GLOB_DAT,X86_64_JMP_SLOT, 这东西好像是API的地址,是要load的

X86_64_RELATIVE 这个东西就当前模块的一些重定位



SYM=0,说明是本地的?



