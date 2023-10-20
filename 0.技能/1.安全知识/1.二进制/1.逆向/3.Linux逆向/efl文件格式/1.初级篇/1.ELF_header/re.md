# 前言

ELF 文件头的位置是固定的，其它内容的位置全都可变

ELF 文件头规定了其它内容的布局是怎么样的



```c++
typedef struct elf32_hdr{
  unsigned char	e_ident[EI_NIDENT];//EI_NIDENT=16;
  WORD	e_type;
  WORD	e_machine; 
  DWORD	e_version; //文件版本，目前常见的ELF 文件版本均为EV_CURRENT（1）。
  DWORD	e_entry;  /* Entry point */
  DWORD	e_phoff; //Program Header 文件偏移
  DWORD	e_shoff; //Section Header 文件偏移
  DWORD	e_flags; //处理器特定的标志，一般为0。
  WORD	e_ehsize; //Elf_Header的大小（字节）
  WORD	e_phentsize; // （Program Header）的大小（字节）。
  WORD	e_phnum;//Program的数量。
  WORD	e_shentsize;//（Section Header）的大小（字节）。
  WORD	e_shnum;//Section的数量。
  WORD	e_shstrndx;//
} Elf32_Ehdr;


typedef struct elf64_hdr {
  unsigned char	e_ident[EI_NIDENT];	/* ELF "magic number" */
  WORD e_type;
  WORD e_machine;
  DWORD e_version;
  ULONGLONG e_entry;
  ULONGLONG e_phoff;
  ULONGLONG e_shoff;
  DWORD e_flags;
  WORD e_ehsize;
  WORD e_phentsize;
  WORD e_phnum;
  WORD e_shentsize;
  WORD e_shnum;
  WORD e_shstrndx;
} Elf64_Ehdr;
```



从elf_hdr来看

x64的比x86的多12字节,成员名字是一样的

但是其中有3个成员的长度从4变为8,所以长度会增加3x4=12字节

```
//这3个成员大小发生改变
ULONGLONG e_entry;
ULONGLONG e_phoff;
ULONGLONG e_shoff;
```





# 结构体成员



## e_ident

它的作用是什么?

用于解码和解析文件内容的数据，

如果不知道编码格式，系统将无法知道怎么去读取目标文件

是不依赖于具体操作系统的执行什么的



关于   `unsigned char	e_ident[EI_NIDENT];`

```c++
#define EI_NIDENT (16)

typedef struct
{
  unsigned char	e_ident[EI_NIDENT]; /* Magic number and other info */       // 1 byte * 16
  Elf64_Half	e_type;		    /* Object file type */                  // 2 bytes
  Elf64_Half	e_machine;	    /* Architecture */
  Elf64_Word	e_version;	    /* Object file version */               // 4 bytes
  Elf64_Addr	e_entry;	    /* Entry point virtual address */       // 8 bytes
  Elf64_Off		e_phoff;	    /* Program header table file offset */  // 8 bytes
  Elf64_Off		e_shoff;	    /* Section header table file offset */      
  Elf64_Word	e_flags;	    /* Processor-specific flags */             
  Elf64_Half	e_ehsize;	    /* ELF header size in bytes */
  Elf64_Half	e_phentsize;	    /* Program header table entry size */
  Elf64_Half	e_phnum;	    /* Program header table entry count */
  Elf64_Half	e_shentsize;	    /* Section header table entry size */
  Elf64_Half	e_shnum;	    /* Section header table entry count */
  Elf64_Half	e_shstrndx;	    /* Section header string table index */
} Elf64_Ehdr;
```



下表介绍`e_ident`数组的结构。其中每个成员均占用 1 字节空间。

| 数据成员名称  | 数组下标的索引 | 意义                             |
| ------------- | -------------- | -------------------------------- |
| EI_MAG0       | 0              | '.'                              |
| EI_MAG1       | 1              | 'E'                              |
| EI_MAG2       | 2              | 'L'                              |
| EI_MAG3       | 3              | 'F'                              |
| EI_CLASS      | 4              | 文件类别,x86,x64,...             |
| EI_DATA       | 5              | 编码格式,大端还是小端            |
| EI_VERSION    | 6              | 文件版本,固定为EV_CURRENT（1）。 |
| EI_OSABI      | 7              | OS ABI 识别标志                  |
| EI_ABIVERSION | 8              | ABI 版本                         |
| EI_PAD        | 9              | 补充字节开始的地址               |



 





EI_MAG0 ~ EI_MAG3

类似于PE的PE00或者MZ

所以这个字段的内容是`.ELF`

用于标识这是一个 ELF 文件。



EI_CLASS



| 名字         | 值   | 意义          |
| ------------ | ---- | ------------- |
| ELFCLASSNONE | 0    | 非法目标文件  |
| ELFCLASS32   | 1    | 32 位目标文件 |
| ELFCLASS64   | 2    | 64 位目标文件 |



```
#define EI_CLASS	4		/* File class byte index */
#define ELFCLASSNONE	0		/* Invalid class */
#define ELFCLASS32	1		/* 32-bit objects */
#define ELFCLASS64	2		/* 64-bit objects */
#define ELFCLASSNUM	3
```





EI_DATA

数据编码格式

| 名字        | 值   | 意义                      |
| ----------- | ---- | ------------------------- |
| ELFDATANONE | 0    | 非法编码格式              |
| ELFDATA2LSB | 1    | LSB 编码(小头编码) 小端序 |
| ELFDATA2MSB | 2    | MSB 编码(大头编码) 大端序 |



```
#define EI_DATA		5		/* Data encoding byte index */
#define ELFDATANONE	0		/* Invalid data encoding */
#define ELFDATA2LSB	1		/* 2's complement, little endian */
#define ELFDATA2MSB	2		/* 2's complement, big endian */
#define ELFDATANUM	3
```





## e_type



ELF的文件类型，定义如下：

注：GCC使用编译选项 -pie 编译的可执行文件实际 也是DT_DYN类型

```text
#define  ET_REL    1 //可重定位文 件（如目标文件）
#define  ET_EXEC   2 // 可执行文件（可直接执行的文件）
#define  ET_DYN    3 // 共享目标文件（如SO库）
#define  ET_CORE   4 //Core文件（吐核文件）
```

对于 ET_REL 类型 的ELF文件，节是必须存在的

然后我们回忆一下PE文件的话, NtHeader.FileHeader.Characteristics就定义了一个pe文件的具体类型



可重定位文件: (relocatable file)就是常见的.obj文件或者.o文件

共享目标文件:  感觉类似于一个dll, 要么参与一个目标的生成,要么被动态链接到一个进程

可执行文件:  经过链接的，可以执行的程序文





该数据类型是uint16_t数据类型的，占两个字节。通过字段查看，可以看到这个值为`00 02`。表格定义如下：

| 名称      | 取值   | 含义                     |
| --------- | ------ | ------------------------ |
| ET_NONE   | 0x0000 | 未知目标文件格式         |
| ET_ERL    | 0x0001 | 可重定位文件             |
| ET_EXEC   | 0x0002 | 可执行文件               |
| ET_DYN    | 0x0003 | 共享目标文件             |
| ET_CORE   | 0x0004 | Core文件(转储格式)       |
| ET_LOPROC | 0xFF00 | 特定处理器文件扩展下边界 |
| ET_HIPROC | 0xFFFF | 特定处理器文件扩展上边界 |

对应表格内容，可以看到类型为`EXEC`即可执行文件类型。



##  e_machine 



关于这个字段的解析，基本上就是表示该elf文件是针对哪个处理器架构的。

下面只列出几个常见的架构的序号

| 名称     | 取值 | 含义                       |
| -------- | ---- | -------------------------- |
| EM_NONE  | 0    | No machine                 |
| EM_SPARC | 2    | SPARC                      |
| EM_386   | 3    | Intel 80386                |
| EM_MIPS  | 8    | MIPS I Architecture        |
| EM_PPC   | 0x14 | PowerPC                    |
| EM_ARM   | 0x28 | Advanced RISC Machines ARM |

他是一个WORD,所以范围很广



## e_entry

此字段指明程序入口的虚拟地址。即当文件被加载到进程空间里后，入口程

序在进程地址空间里的地址。对于可执行程序文件来说，当 ELF 文件完成加载之

后，程序将从这里开始运行；而对于其它文件来说，这个值应该是 0。



## 节&&段  



这2个成员,分别告诉了,section header在哪,Program Header在哪,貌似都是文件偏移

```
  //x64
  ULONGLONG e_phoff;
  ULONGLONG e_shoff;
  
  //x86
  DWORD	e_phoff; //Program Header 基于文件的偏移
  DWORD	e_shoff; //Section Header 基于文件的偏移
```





下面这4个成员,分别告诉了

section header的结构体大小,是一个结构体的大小

section header的总数量

Program Header的结构体大小,是一个结构体的大小

Program Header的总数量

```
  WORD e_phentsize;
  WORD e_phnum;
  WORD e_shentsize;
  WORD e_shnum;
```





## e_shstrndx

这个成员和节的名字有关,

他是节表SectionHeader的一个index,

这个index对应的节就代表了`.shstrtab` ,存放section名，







在运行过程中是必须的，在连接过程中是可选的，因为它的作用是告诉系统如何创建进程的镜像



通过 ` readelf -l demo.elf` 指令,我们就明白

段是什么样子的,然后节又是什么样子的

```
Elf file type is DYN (Position-Independent Executable file)
Entry point 0x1080
There are 13 program headers, starting at offset 64

Program Headers:
  Type           Offset             VirtAddr           PhysAddr
                 FileSiz            MemSiz              Flags  Align
  PHDR           0x0000000000000040 0x0000000000000040 0x0000000000000040
                 0x00000000000002d8 0x00000000000002d8  R      0x8
  INTERP         0x0000000000000318 0x0000000000000318 0x0000000000000318
                 0x000000000000001c 0x000000000000001c  R      0x1
  LOAD           0x0000000000000000 0x0000000000000000 0x0000000000000000
                 0x0000000000000680 0x0000000000000680  R      0x1000
  LOAD           0x0000000000001000 0x0000000000001000 0x0000000000001000
                 0x00000000000001cd 0x00000000000001cd  R E    0x1000
  LOAD           0x0000000000002000 0x0000000000002000 0x0000000000002000
                 0x00000000000000e4 0x00000000000000e4  R      0x1000
  LOAD           0x0000000000002db0 0x0000000000003db0 0x0000000000003db0
  ...

 Section to Segment mapping:
  Segment Sections...
   00
   01     .interp
   02     .interp .note.gnu.property .note.gnu.build-id .note.ABI-tag .gnu.hash .dynsym .dynstr .gnu.version .gnu.version_r .rela.dyn .rela.plt
   03     .init .plt .plt.got .plt.sec .text .fini
   04     .rodata .eh_frame_hdr .eh_frame
   05     .init_array .fini_array .dynamic .got .data .bss
   06     .dynamic
   07     .note.gnu.property
   08     .note.gnu.build-id .note.ABI-tag
   09     .note.gnu.property
   10     .eh_frame_hdr
   11
   12     .init_array .fini_array .dynamic .got
```

通过导出信息我们可以看到

```
Elf file type is DYN (Position-Independent Executable file)
Entry point 0x1080
There are 13 program headers, starting at offset 64
```

然后有13个段,

在每个段中,又由很多的Section组成

比如我们可以看到 `program headers[03]={.init .plt .plt.got .plt.sec .text .fini}`

