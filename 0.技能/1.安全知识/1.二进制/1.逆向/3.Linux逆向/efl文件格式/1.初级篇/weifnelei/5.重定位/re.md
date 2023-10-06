

# 重定位



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

对于重定位文件来说，此值是受重定位作用的存储单元在节中的字节偏移量；

对于可执行文件或共享目标文件来说，此值是受重定位作用的存储单元的虚拟地址。

ps: 听不懂



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





在重定位文件中，r_offset 成员含有一个节偏移量。也就是说，重定位节本身描述的是如何修改文件中的另一个节的内容，

重定位偏移量(r_offset)指向了另一个节中的一个存储单元地址。




在可执行文件或共享目标文件中，r_offset 含有的是符号定义在进程空间中的虚拟地址。

可执行文件和共享目标文件是用于运行程序而不是构建程序的，

所以对它们来说更有用的信息是运行期的内存虚拟地址，而不是某个符号定义在文件中的位置

ps: 看不懂,哈哈

尽管对于不同类型的目标文件，r_offset 的含义不同，但其重定位的作用是不变的。