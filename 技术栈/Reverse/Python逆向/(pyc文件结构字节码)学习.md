# (pyc文件结构|字节码)学习

参考链接:

[https://zhuanlan.zhihu.com/p/145811103](https://zhuanlan.zhihu.com/p/145811103)

[https://taardisaa.github.io/2022/02/09/Pyc%20Reverse%201/](https://taardisaa.github.io/2022/02/09/Pyc%20Reverse%201/)

# 文件头

不同版本的文件头不同的

# 3.8.6

![Untitled]((pyc%E6%96%87%E4%BB%B6%E7%BB%93%E6%9E%84%20%E5%AD%97%E8%8A%82%E7%A0%81)%E5%AD%A6%E4%B9%A0%207b4bcaf873a14cbbb5622b6561afb431/Untitled.png)

第1个红色的框框: python的版本号相关,一个版本对应一个编号,[https://github.com/google/pytype/blob/main/pytype/pyc/magic.py](https://github.com/google/pytype/blob/main/pytype/pyc/magic.py)

第2个红色框框: pyc文件生成的上时间戳

第3个红色的框框: 源代码文本文件的字节大小

第4个红色的框框: 和优化有关的一个标志位

# PyObject

挣个pyc文件其实就是一个PyObject

可以和PE文件类比,但是pyc文件就是一个不断嵌套的PyObject

PyObject 的(结构化)序列化在 Python/marshal.c 内实现

## 基本变量类型

关于变量存储的一些发现: 重复的变量不会存储第2遍

比如x=12和y=12 在pyc文件中,12只会出现1次,反倒x和y各自出现1次

```
#define TYPE_NULL               '0' 30 B0
#define TYPE_NONE               'N' 4E CE
#define TYPE_FALSE              'F'	46 C6
#define TYPE_TRUE               'T'	54 D4
#define TYPE_STOPITER           'S'	53 D3
#define TYPE_ELLIPSIS           '.'	2E AE
#define TYPE_INT                'i'	69 E9
#define TYPE_INT64              'I'	49 C9
#define TYPE_FLOAT              'f'	66 E6
#define TYPE_BINARY_FLOAT       'g'	67 E7
#define TYPE_COMPLEX            'x'	78 F8
#define TYPE_BINARY_COMPLEX     'y'	79 F9
#define TYPE_LONG               'l'	6C EC
#define TYPE_STRING             's'	73 F3
#define TYPE_INTERNED           't'	74 F4
#define TYPE_REF                'r'	72 F2
#define TYPE_TUPLE              '('	28 A8
#define TYPE_LIST               '['	5B DB
#define TYPE_DICT               '{'	7B FB
#define TYPE_CODE               'c'	63 E3
#define TYPE_UNICODE            'u'	75 F5
#define TYPE_UNKNOWN            '?'	3F BF
#define TYPE_SET                '<'	3C BC
#define TYPE_FROZENSET          '>'	3E BE
#define FLAG_REF                '\x80'
#define TYPE_ASCII              'a'	61 E1
#define TYPE_ASCII_INTERNED     'A'	41 C1
#define TYPE_SMALL_TUPLE        ')'	29 A9
#define TYPE_SHORT_ASCII        'z'	7A FA
#define TYPE_SHORT_ASCII_INTERNED 'Z'	5A DA

```

## PyCodeObject

```
struct PyCodeObject
{
    PyObject_HEAD
    int co_argcount;            /* 当前函数的参数个数 */
    int co_posonlyargcount;     /* #positional only arguments */
    int co_kwonlyargcount;      /* #keyword only arguments */
    int co_nlocals;             /* 局部变量的个数 */
    int co_stacksize;           /* 和栈大小有关 */
    int co_flags;               /* CO_..., see below */
    int co_firstlineno;         /* 代码对象的第一行位于所在文件的行号。 */
    PyObject *co_code;          /* 代码结构体 */
    PyObject *co_consts;        /* 当前函数所有常量的组合*/
    PyObject *co_names;         /* 此 code 的名称,名字一般是<module>*/
    PyObject *co_varnames;      /* tuple of strings (local variable names) */
    PyObject *co_freevars;      /*  闭包用的的变量名集合 */
    PyObject *co_cellvars;      /* 内部嵌套函数引用的变量名集合 */
    Py_ssize_t *co_cell2arg;    /* Maps cell vars which are arguments. */
    PyObject *co_filename;      /* 当前文件的名字*/
    PyObject *co_name;          /* 模块名|函数名|类名 */
    PyObject *co_lnotab;        /* 字节码指令和行号的对应关系 */
    void *co_zombieframe;       /* for optimization only (see frameobject.c) */
    PyObject *co_weakreflist;   /* to support weakrefs to code objects */
    void *co_extra;
    unsigned char *co_opcache_map;
    _PyOpcache *co_opcache;
    int co_opcache_flag;  // used to determine when create a cache.
    unsigned char co_opcache_size;  // length of co_opcache.
} ;
```

用代码打印pyc文件信息

他会列出一些和基本变量类型有关的东西

```
import dis, marshal
f=open('f:/test.pyc', 'rb').read()
co=marshal.loads(f[16:]) # Python3.7中，PyCodeObject在pyc文件中的偏移为16
out=dis.dis(co)
print(out)

print(co.co_argcount)
print(co.co_posonlyargcount)
print(co.co_kwonlyargcount)
print(co.co_nlocals)
print(co.co_stacksize)
print(co.co_flags)
#
#print(co.co_code)
print(co.co_consts)
print(co.co_names)
print(co.co_varnames)
print(co.co_freevars)
print(co.co_cellvars)
#print(co.co_cell2arg)
print(co.co_filename)
print(co.co_name)
print(co.co_firstlineno)
print(co.co_lnotab)
#print(co.co_zombieframe)
#print(co.co_weakreflist)

```

文件分析

```
int32 co_argcount;
int32 co_posonlyargcount;
int32 co_kwonlyargcount;
int32 co_nlocals;
int32 co_stacksize;
int32 co_flags;
Code code;
struct r_object consts;
struct r_object names;
struct r_object varnames;
struct r_object freevars;
struct r_object cellvars;
struct r_object filename;
struct r_object name;
int32 firstlineno;
LnoTab lnotab;
```

# 字节码学习

大佬的博文,建议第一次看一下 [https://taardisaa.github.io/tags/pyc/](https://taardisaa.github.io/tags/pyc/)

pyc的汇编指令可在Include/opcode.h 找到

python的指令集属于是代代更新的。每次都会多几个新指令，同时把一些不常用的旧指令又给抛弃掉。

## 常见字节码学习

### 和栈相关的指令

LOAD_CONST

从常量池中加载一个常量，并将其压入栈顶。因此，可以说LOAD_CONST指令具有将常量压栈的操作。

py3: 64 XX , XX是一个常量的索引值 具体load的是consts[XX]

BUILD_LIST:

从栈顶依次弹出n个元素，将它们封装成一个列表并将列表压入栈顶

```
3           8 LOAD_CONST               2 (1)
            10 LOAD_CONST               3 (2)
            12 LOAD_CONST               4 (3)
            14 BUILD_LIST               3
            16 STORE_NAME               2 (c)
```

STORE_NAME:

将栈顶元素赋值给指定的变量名,然后把名字的指针入栈

```
1           0 LOAD_CONST               0 (10)
            2 STORE_NAME               0 (a)
```

### 和跳转相关的指令

### 其它指令

## 变量创建

比如

```
en = [3, 37, 72, 9, 6, 132]
```

汇编是

```
  1           0 LOAD_CONST               0 (3)
              2 LOAD_CONST               1 (37)
             12 BUILD_LIST               2
             14 STORE_NAME               0 (en)
```