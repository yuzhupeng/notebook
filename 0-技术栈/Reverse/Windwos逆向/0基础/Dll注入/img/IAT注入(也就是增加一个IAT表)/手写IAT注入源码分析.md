# 手写 IAT注入源码 分析

代码地址

[mycode/Re/Dll注入/IAT注入 at 623329ea1f9a02b75e9ebce2c3adff55206f732f · redqx/mycode](https://github.com/redqx/mycode/tree/623329ea1f9a02b75e9ebce2c3adff55206f732f/Re/Dll注入/IAT注入)

# 前言

不会去讲解每一个函数

而只是去点一下关键的函数

# 头文件和全局变量

```c
#include <windows.h>
#include <stdio.h>

#define BUFFER_SIZE 1024

IMAGE_DOS_HEADER*           lp_Dos;
IMAGE_FILE_HEADER*          lp_FileHeader;
IMAGE_OPTIONAL_HEADER64*    lp_option64;
IMAGE_OPTIONAL_HEADER32*    lp_option32;
IMAGE_IMPORT_DESCRIPTOR*    lp_ImportDescriptor;
IMAGE_SECTION_HEADER*       lp_SectionHeaders;

int isx86;
DWORD flen;
```

isx86 用于判断是不是x86或者x64的程序

flen 用于存储计文件长度

# main函数

```c
int main()
{
    TCHAR fpath[] = TEXT("F:/notepad.exe");
    TCHAR fpath_patch[] = TEXT("F:/notepad64_patch.exe");
    readPEinfo(fpath);
    inject_Dll(fpath);
    writeIntoFIle(fpath_patch);
    GlobalFree(lp_Dos);
    return 0;
}
```

读取一个文件的pe信息

把一个自己的dll信息注入到pe中

把新的pe信息写入文件

释放pe相关内存

# inject_Dll代码逻辑

```c
void inject_Dll()
{
    DWORD xx;
    DWORD fa_start;
    DWORD index;
    DWORD newImportSzie;
    DWORD newImportSziePadding=0;
    IMAGE_IMPORT_DESCRIPTOR* tlp_ImportDescriptor;
    xx=couldInsert(&index);
    newImportSzie = (index + 2) * 20 + 64;//64是导入表具体指向的4行
    if (xx==1) 
    {  
        
        fa_start = zeroPaddingData(16*4);

        //写入dll的名字
        memcpy(fa_start + (BYTE*)lp_Dos, "redqx.dll", 10); //第1行
        //写入函数的name结构体
        memcpy(fa_start + 16 + 2 + (BYTE*)lp_Dos, "funny", 6);//第2行
        //往INT写入数据(第2行)
        xx = fa2va(fa_start + 16);
        memcpy(fa_start + 32 + (BYTE*)lp_Dos, &xx, 4);

        lp_ImportDescriptor[index].Name = fa2va(fa_start);//指向第1行
        lp_ImportDescriptor[index].OriginalFirstThunk = fa2va(fa_start + 32);//指向第3行
        lp_ImportDescriptor[index].FirstThunk = fa2va(fa_start + 48);//指向第4行

        if (fa_start == 0)
        {
            //还是空间不足
            goto label1;
        }
    }
    else 
    {
    label1:
        
        fa_start = zeroPaddingData(newImportSzie);
        
        if (fa_start)
        {
            //可以迁移
            tlp_ImportDescriptor = (IMAGE_IMPORT_DESCRIPTOR*)(fa_start + (BYTE*)lp_Dos);
            memcpy(fa_start+(BYTE*)lp_Dos, lp_ImportDescriptor, index * 20);            
        }
        else
        {
            fa_start = flen;
            tlp_ImportDescriptor = (IMAGE_IMPORT_DESCRIPTOR*)(fa_start + (BYTE*)lp_Dos);
            newImportSziePadding = falien(newImportSzie);
            //尾部增加
            memcpy(fa_start + (BYTE*)lp_Dos, lp_ImportDescriptor, index * 20);
            if (isx86)
            {
                lp_option32->SizeOfImage += newImportSziePadding;
                //节区扩大
            }
            else
            {
                lp_option64->SizeOfImage += newImportSziePadding;
            }
            lp_SectionHeaders[lp_FileHeader->NumberOfSections - 1].SizeOfRawData += newImportSziePadding;
            lp_SectionHeaders[lp_FileHeader->NumberOfSections - 1].Misc.VirtualSize+= newImportSziePadding;
            flen += newImportSziePadding;
        }
        lp_option64->DataDirectory[1].VirtualAddress = fa2va(fa_start);
        lp_option64->DataDirectory[1].Size += 20;
        fa_start = fa_start + (index+2) * 20;

        //写入dll的名字
        memcpy(fa_start + (BYTE*)lp_Dos, "redqx.dll", 10); //第1行
        //写入函数的name结构体
        memcpy(fa_start + 16 + 2 + (BYTE*)lp_Dos, "funny", 6);//第2行
        
       
        xx = fa2va(fa_start + 16);
        //往INT写入数据(第2行)
        memcpy(fa_start + 32 + (BYTE*)lp_Dos, &xx, 4);
        //往IAT写入数据(第2行)
        memcpy(fa_start + 48 + (BYTE*)lp_Dos, &xx, 4);

        tlp_ImportDescriptor[index].Name = fa2va(fa_start);//指向第1行
        tlp_ImportDescriptor[index].OriginalFirstThunk = fa2va(fa_start + 32);//指向第3行
        tlp_ImportDescriptor[index].FirstThunk = fa2va(fa_start + 48);//指向第4行
    }
    return;
}
```

可不可以注入 我估么着有3种情况

1), 直接在原来的基础上增加一个新的导入表项

2), 无法在原来的基础上增加,需要迁移整个导入表到新的位置,然后增加一个导入表项

3), 也没办法迁移,直接在把导入表追加到文件的末尾

couldInsert函数

![Untitled](%E6%89%8B%E5%86%99%20IAT%E6%B3%A8%E5%85%A5%E6%BA%90%E7%A0%81%20%E5%88%86%E6%9E%90%20ab3bd2c18bc1456c97779b39c219833a/Untitled.png)

首先通过couldInsert函数去判断是否可以在原来的基础上增加一个导入表项

xx可以返回是否可以增加

index可以返回已有的导入表个数

原理就是遍历导入表

## 情况1

![Untitled](%E6%89%8B%E5%86%99%20IAT%E6%B3%A8%E5%85%A5%E6%BA%90%E7%A0%81%20%E5%88%86%E6%9E%90%20ab3bd2c18bc1456c97779b39c219833a/Untitled%201.png)

zeroPaddingData()函数就是去寻找0区域的空间

寻找的长度是可以通过参数给传递进去

然后返回值做了一个地址对齐的处理

也就返回值是8的倍数 (我把x86和x64的统一处理为8的倍数,在做地址对齐上,我就做的不是很好,因为会导致一些问题)

比如我们需求长度是32,通过地址对齐后,长度可能是28,或者其它的

![Untitled](%E6%89%8B%E5%86%99%20IAT%E6%B3%A8%E5%85%A5%E6%BA%90%E7%A0%81%20%E5%88%86%E6%9E%90%20ab3bd2c18bc1456c97779b39c219833a/Untitled%202.png)

为什么需要寻找额外的区域?

增加一个导入表项,不仅仅是增加一个20字节的东东

```c
typedef struct _IMAGE_IMPORT_DESCRIPTOR {
    union {
        DWORD   Characteristics;            // 0 for terminating null import descriptor
        DWORD   OriginalFirstThunk;         // RVA to original unbound IAT (PIMAGE_THUNK_DATA)
    } DUMMYUNIONNAME;
    DWORD   TimeDateStamp;                  // 0 if not bound,
                                            // -1 if bound, and real date\time stamp
                                            //     in IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT (new BIND)
                                            // O.W. date/time stamp of DLL bound to (Old BIND)

    DWORD   ForwarderChain;                 // -1 if no forwarders
    DWORD   Name;
    DWORD   FirstThunk;                     // RVA to IAT (if bound this IAT has actual addresses)
} IMAGE_IMPORT_DESCRIPTOR;
```

还得增加FirstThunk,Name,OriginalFirstThunk关联的东西

为什么要增加这些东西?

<<逆向工程核心原理>.说是最好要保证一个dll的基本完整性, 得有导出函数

![Untitled](%E6%89%8B%E5%86%99%20IAT%E6%B3%A8%E5%85%A5%E6%BA%90%E7%A0%81%20%E5%88%86%E6%9E%90%20ab3bd2c18bc1456c97779b39c219833a/Untitled%203.png)

下面红色的部分就是写入这些东西

然后那对应的信息写入导入表项

![Untitled](%E6%89%8B%E5%86%99%20IAT%E6%B3%A8%E5%85%A5%E6%BA%90%E7%A0%81%20%E5%88%86%E6%9E%90%20ab3bd2c18bc1456c97779b39c219833a/Untitled%204.png)

## 情况2&3

无法在原有的表上做一个修改

需要整张迁移IAT表

并且新增的内容仍然是Name;FuncName;FirstThunk;OriginalFirstThunk

![Untitled](%E6%89%8B%E5%86%99%20IAT%E6%B3%A8%E5%85%A5%E6%BA%90%E7%A0%81%20%E5%88%86%E6%9E%90%20ab3bd2c18bc1456c97779b39c219833a/Untitled%205.png)

所以在之前我就计算了大小

需要拓展的大小是(index+2)*20

由于导入表项还需要关联一些信息(Name;FuncName;FirstThunk;OriginalFirstThunk)

又增加了64字节,每16字节关联一个信息

![Untitled](%E6%89%8B%E5%86%99%20IAT%E6%B3%A8%E5%85%A5%E6%BA%90%E7%A0%81%20%E5%88%86%E6%9E%90%20ab3bd2c18bc1456c97779b39c219833a/Untitled%206.png)

第1行是dll名字

第2行是函数名字

第3行是OriginalFirstThunk

第4行是FirstThunk

这就是为什么增加64字节的原因

![Untitled](%E6%89%8B%E5%86%99%20IAT%E6%B3%A8%E5%85%A5%E6%BA%90%E7%A0%81%20%E5%88%86%E6%9E%90%20ab3bd2c18bc1456c97779b39c219833a/Untitled%207.png)

因为迁移了导入表

同时我们还要往新的导入表写入信息,于是新建了指针 IMAGE_IMPORT_DESCRIPTOR* tlp_ImportDescriptor;

而不是往以前的导入表写入东西 IMAGE_IMPORT_DESCRIPTOR* lp_ImportDescriptor;

QWQ: 我就是往lp_ImportDescriptor写东西,发现patch的文件怎么没东西,原来应该往tlp_ImportDescriptor写

情况2的fstart指向了寻找到的文件地址

情况3的fstart指向了文件的末尾

同时他们都有复制导入表的操作,导入的目的地址就是fstart

对于情况3,我们还得考虑一些文件地址对齐的因素,因为不能随便的增加文件的大小

![Untitled](%E6%89%8B%E5%86%99%20IAT%E6%B3%A8%E5%85%A5%E6%BA%90%E7%A0%81%20%E5%88%86%E6%9E%90%20ab3bd2c18bc1456c97779b39c219833a/Untitled%208.png)

同时还要去修改一些pe结构的成员变量

比如

SizeOfImage+= newImportSziePadding;

lp_SectionHeaders.SizeOfRawData += newImportSziePadding;
lp_SectionHeaders.Misc.VirtualSize+= newImportSziePadding;

flen += newImportSziePadding; 文件长度也发生变化

后面就是情况2和情况3一样的操作

都是写入导入表关联数据,把相关信息写入导入表项

![Untitled](%E6%89%8B%E5%86%99%20IAT%E6%B3%A8%E5%85%A5%E6%BA%90%E7%A0%81%20%E5%88%86%E6%9E%90%20ab3bd2c18bc1456c97779b39c219833a/Untitled%209.png)

是大概就是这样

代码写了差不多2天

遇到了很多问题

细节的地方可能还需要理解