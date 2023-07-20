# Py工具

以下是一些用于解析 Python .pyc 文件格式的工具：

1. 010 Editor：010 Editor 是一种高级文本编辑器，可用于解析任何类型的二进制文件。它提供了模板语言和 API，可以自定义模板来解析 Python .pyc 文件。010 Editor 还提供了大量的模板示例，可用于快速入门。
2. PyMarshal：PyMarshal 是 Python 的内置模块，提供了将 Python 对象序列化为二进制格式的功能。它可以将 Python 对象转换为 .pyc 文件格式，并解析已编译的代码对象。PyMarshal 模块的文档提供了有关 .pyc 文件格式的详细信息。
3. uncompyle6：uncompyle6 是一个反编译器，用于将 Python 字节码反编译为 Python 代码。它支持 Python 2 和 3 的 .pyc 文件，可以将二进制文件转换为人类可读的 Python 代码。
4. xdis：xdis 是一个 Python 反汇编库，用于反汇编 Python 字节码。它支持 Python 2 和 3 的字节码，还可以反汇编混淆的字节码和 PEP 3131。

# py->pyc

命令行使用:生成单个pyc

```
python -m py_compile test.py
```

命令行生成多个pyc:

```
python -m compileall test
```

使用-O选项，将.pyc文件优化为.pyo文件（体积更小）

```
python3 -O -m compileall -b .
```

用py脚本生成pyc

```
import py_compile
py_compile.compile('test.py')
```

# pyc->asm(伪汇编)

py3:

```
import dis, marshal
f=open('f:/test.pyc', 'rb').read()
co=marshal.loads(f[16:]) # Python3.7中，PyCodeObject在pyc文件中的偏移为16
out=dis.dis(co)
print(out)
#print(co.co_name)
#print(co.co_names)
#print(co.co_consts)

```

```
oc = test_func.__code__

print(oc.co_argcount)
print(oc.co_posonlyargcount)
print(oc.co_kwonlyargcount)
print(oc.co_nlocals)
print(oc.co_stacksize)
print(oc.co_flags)
print(oc.co_code)
print(oc.co_consts)
print(oc.co_names)
print(oc.co_varnames)
print(oc.co_freevars)
print(oc.co_cellvars)
print(oc.co_filename)
print(oc.co_name)
print(oc.co_firstlineno)
print(oc.co_lnotab)
```

py2

```
import dis, marshal
f=open('t.pyc', 'rb').read()
co=marshal.loads(f[8:]) # Python2.7中，PyCodeObject在pyc文件中的偏移为8
out=dis.dis(co)
print(out)
```

# py->asm

单独对某一个函数

```
import dis

def add_numbers(a, b):
    sum = a + b
    return sum

print(dis.dis(add_numbers))

```

对整个个文件

```
import dis
path="E:/Code/normal/Python/normal/test.py"
with open(path, 'rb') as f:
    code = compile(f.read(), path, 'exec')
print(dis.disassemble(code))

```

# pyc->py

方法1: pyc在线逆向

方法2: uncompyle6  (基于python的下载 `pip install uncompyle6`) 最高支持到python3.10

方法3: pycdc 需要编译才可以用 项目地址[https://github.com/zrax/pycdc](https://github.com/zrax/pycdc)

关于方法2

使用

```
uncompyle6 input.pyc > output.py
```

如果你版本过高的话可以用python2的uncompyle6再通过python带的2to3命令转化为python3语法(可能反编译结果不会太准确)

关于方法3的编译(基于linux环境)

```
apt install git   #安装git
apt install cmake #安装cmake
git clone https://github.com/zrax/pycdc.git #下载pycdc到桌面

cd pycdc #进入到pycdc的目录
cmake .
make #安装pycdc工具
```

# exe->pyc

如何把exe还原为py文件?

准备工具

pyinstxtractor.py 				拆包（解压）工具，将exe文件解压成一个文件夹

uncompyle6 					pyc反编译工具

下载地址: pyinstxtractor [https://github.com/extremecoders-re/pyinstxtractor](https://github.com/extremecoders-re/pyinstxtractor)

使用

```bash
E:\re\PYC逆向\解包\pyinstxtractor-master>python pyinstxtractor.py attachment.exe
[+] Processing attachment.exe
[+] Pyinstaller version: 2.1+
[+] Python version: 3.6
[+] Length of package: 6021662 bytes
[+] Found 59 files in CArchive
[+] Beginning extraction...please standby
[+] Possible entry point: pyiboot01_bootstrap.pyc
[+] Possible entry point: login.pyc
[!] Warning: This script is running in a different Python version than the one used to build the executable.
[!] Please run this script in Python 3.6 to prevent extraction errors during unmarshalling
[!] Skipping pyz extraction
[+] Successfully extracted pyinstaller archive: attachment.exe

You can now use a python decompiler on the pyc files within the extracted directory
```

然后就生成了一个文件夹 your_filename__extracted

![Untitled](Py%E5%B7%A5%E5%85%B7%2079db713cb2c84f47aa8db03a8125a891/Untitled.png)

进入login.exe_extracted 在第一次目录

![Untitled](Py%E5%B7%A5%E5%85%B7%2079db713cb2c84f47aa8db03a8125a891/Untitled%201.png)

找到那个yourfilename.pyc 比如这里的login.exe

然后拿出来,用 uncompyle6 或者在线网站[https://tool.lu/pyc/](https://tool.lu/pyc/) 反编译一下

# Python多版本管理(pyenv)

参考[https://zhuanlan.zhihu.com/p/31194682](https://zhuanlan.zhihu.com/p/31194682)