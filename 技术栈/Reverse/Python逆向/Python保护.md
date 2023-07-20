# Python保护

# 源码保护

## ollvm混淆

先留个文章在这里

[https://zhuanlan.zhihu.com/p/81436880](https://zhuanlan.zhihu.com/p/81436880)

然后我想说的是 github有几个项目，

它可以根据pyc字节码绘画出类似于IDA的流程图

然后这些项目很久没更新了

## 变量命混淆

就是给一些变量取一些奇奇怪怪毫无意义的名字

[http://pyob.oxyry.com/](https://link.zhihu.com/?target=http%3A//pyob.oxyry.com/) 该网站可以实现

pyobfuscate ： [https://github.com/astrand/pyobfuscate](https://github.com/astrand/pyobfuscate) 也可以实现给变量乱取名字

## 鸡肋的反调试

就是基于源码的反调试 很鸡肋

ps： 如果发现是反调试，直接删除那个源码不就🆗了

# PYC保护

## 花指令:一般都是基于跳转

Python3 的 opcode 中 JUMP 相关的有

```
'JUMP_FORWARD',
'JUMP_IF_FALSE_OR_POP',
'JUMP_IF_TRUE_OR_POP',
'JUMP_ABSOLUTE',
'POP_JUMP_IF_FALSE',
'POP_JUMP_IF_TRUE'
```

其实还是 JUMP_FORWARD 和 JUMP_ABSOLUTE 比较好用（字面理解，一个是相对跳转，一个是绝对跳转)

## 关于去除花指令的反编译问题

其实就算是nop，反编译或多或少还是会遇到问题

也就是一旦打起了花指令的注意，那就很难重新还原出py文件了

修改dis模块

```
 print opname[op].ljust(20),
 i = i+1
 if op >= HAVE_ARGUMENT:
     oparg = ord(code[i]) + ord(code[i+1])*256 + extended_arg
     extended_arg = 0
     i = i+2
     if op == EXTENDED_ARG:
         extended_arg = oparg*65536L
     print repr(oparg).rjust(5),
```

修改后加入try和except过滤掉异常

```
 print opname[op].ljust(20),
 i = i+1
 if op >= HAVE_ARGUMENT:
     try: # 添加的地方
         oparg = ord(code[i]) + ord(code[i+1])*256 + extended_arg
     except:  # 添加的地方
         pass  # 添加的地方
     extended_arg = 0
     i = i+2
     if op == EXTENDED_ARG:
         extended_arg = oparg*65536L
     print repr(oparg).rjust(5),
```

## 魔改pyc

花指令是影响反汇编

然后魔改pyc 更多是基于对pyc字节码和文件结构很熟悉

然后去破坏栈平衡(假装破坏,不是真的破话)

然后也会影响反汇编

## Cython

虽说 Cython 的主要目的是带来性能的提升，

但是基于它的原理：将 .py/.pyx 编译为 .c 文件，

再将 .c 文件编译为 .so(Unix) 或 .pyd(Windows)，其带来的另一个好处就是难以破解

hello.py

```
def hello():
    print('hello')
```

setup.py

```
from distutils.core import setup
from Cython.Build import cythonize

setup(name='Hello World app',
      ext_modules=cythonize('hello.pyx'))
```

然后执行

python setup.py build_ext --inplace

执行 python -c "from hello import hello;hello()" 即可直接引用生成的二进制文件中的 hello() 函数