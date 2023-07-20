# 认知方面

OLLVM混淆采用了多种混淆技术，如控制流平坦化、变量重命名、常量替换、过程抽象、代码插桩等，将代码转换为难以理解或逆向的形式

[https://mp.weixin.qq.com/s/3ZiBPRK4NcOR_o4gnjO8GQ](https://mp.weixin.qq.com/s/3ZiBPRK4NcOR_o4gnjO8GQ)

[https://github.com/cq674350529/deflat/tree/master](https://github.com/cq674350529/deflat/tree/master)

deflat.py官方下载 [https://github.com/SnowGirls/deflat](https://github.com/SnowGirls/deflat) 但是不好用,他是基于py2的

参考链接

[https://security.tencent.com/index.php/blog/msg/112](https://security.tencent.com/index.php/blog/msg/112)

[https://ylcao.top/2022/01/16/%E7%AC%A6%E5%8F%B7%E6%89%A7%E8%A1%8C%E5%8E%BB%E6%8E%A7%E5%88%B6%E6%B5%81%E5%B9%B3%E5%9D%A6%E5%8C%96/](https://ylcao.top/2022/01/16/%E7%AC%A6%E5%8F%B7%E6%89%A7%E8%A1%8C%E5%8E%BB%E6%8E%A7%E5%88%B6%E6%B5%81%E5%B9%B3%E5%9D%A6%E5%8C%96/)

[https://zhuanlan.zhihu.com/p/99509681](https://zhuanlan.zhihu.com/p/99509681)

# 待学习的链接

[windwos环境下搭建obfuscator-llvm，vs环境下添加llvm工具集](https://blog.csdn.net/aw2822/article/details/108472494)

[HAPPYers](https://www.jianshu.com/p/8f96e8ab1d98)

[LLVM初探](https://xz.aliyun.com/t/4960)

[OLLVM代码混淆移植与使用](https://www.jianshu.com/p/e0637f3169a3)

[LLVM-obfuscate](https://shaomi.github.io/2017/08/10/ollvm/)

[👻思泉 | Jev0n OLLVM混淆学习（0）——环境搭建及混淆初体验](https://jev0n.com/2022/07/07/ollvm-0.html) 该作者还出了一些几篇相关的文章

下面是一些项目的链接

ps:我都不知道里面有些什么东西

```
 https://github.com/Lazzzaro/lazzzaro.github.io
 https://github.com/kirschju/demovfuscator
 https://github.com/angr/angr
 https://github.com/HikariObfuscator/Hikari
 https://github.com/xoreaxeaxeax/movfuscator
 https://github.com/obfuscator-llvm/obfuscator/wiki/Installation
 https://github.com/GoSSIP-SJTU/Armariris
 https://github.com/SnowGirls/deflat
 https://github.com/AllocAndInit/ollvm5.0.1
 https://github.com/obfuscator-llvm/obfuscator/tree/llvm-3.6.1
 
 目前较新的符号执行工具
 https://github.com/angr/angr
 https://github.com/JonathanSalwan/Triton
 
 Python:
 https://github.com/cea-sec/miasm
```

# 介绍

控制流扁平化 和 虚假控制流 和 指令替换

clang:

LLVM项目的一个子项目，基于LLVM架构的C/C++/Objective-C编译器前端。

编译速度快,占用内存小,模块化设计,诊断信息可读性强,设计清晰简单，容易理解，易于扩展增强

![Untitled](%E8%AE%A4%E7%9F%A5%E6%96%B9%E9%9D%A2%2011e7ead9724e4124aad8a90434c0c4b6/Untitled.png)

LLVM整体架构，前端用的是clang，

广义的LLVM是指整个LLVM架构，

一般狭义的LLVM指的是LLVM后端（包含代码优化和目标代码生成）。

源代码（c/c++）经过clang--> 中间代码(经过一系列的优化，优化用的是Pass) --> 机器码

![Untitled](%E8%AE%A4%E7%9F%A5%E6%96%B9%E9%9D%A2%2011e7ead9724e4124aad8a90434c0c4b6/Untitled%201.png)

# 混淆原理

## 有用|无用块

函数的开始地址为序言的地址

序言的后继为主分发器后继为主分发器的块为预处理器

后继为预处理器的块为真实块

无后继的块为retn块

剩下的为无用块

![Untitled](%E8%AE%A4%E7%9F%A5%E6%96%B9%E9%9D%A2%2011e7ead9724e4124aad8a90434c0c4b6/Untitled%202.png)

## 常量传播 | 到达定值分析

这个确定性如果用于远程的距离的跳转,那么就是到达定值分析

如果用于短距离跳转,就是一般的常量传播

利用了结果的确定性吧

比如

```
 c < -1; 是一个肯定性的结果，于是我们用最高肯定性去构造我们的判断
 
 c < -1
 b + c < 1
 cmp c,b
```

如果我们让b+c<1成立的话，那么c<1-b,所以1-b<=-1

临界点就是b=2，于是我们就浅浅的构造了b=2的事实

# 使用

1. 基于`SnowGirls`的[deflat](https://github.com/SnowGirls/deflat)，利用[angr](https://github.com/angr/angr)框架实现去除控制流平坦化。
    
    `python deflat.py -f check_passwd_x8664_flat --addr 0x400530`
    
2. LLVM涉及的文件多种格式
    
    xx.c （源代码）
    
    xx.ll （LLVM字节码的文本表示）
    
    xx.s （机器汇编码表示的汇编文件）
    
    xx.bc （LLVM字节码的二进制形式）
    
    xx.out （可执行的二进制文件）
    

常用转换

```
 -mllvm -perBCF=20           : 对所有函数都混淆的概率是20%，默认100%
 -mllvm -boguscf-loop=3      : 对函数做3次混淆，默认1次
 -mllvm -boguscf-prob=40     : 代码块被混淆的概率是40%，默认30%
 -mllvm -split               ：激活基本块分割。在一起使用时改善展平。
 -mllvm -split_num=3         ：如果激活了传递，则在每个基本块上应用3次。默认值：1
 -mllvm -fla                 ：激活控制流扁平化
 -mllvm -sub                 ：激活指令替换
 -mllvm -bcf                 ：激活虚假控制流程
 -mllvm -bcf_loop=3          ：如果激活了传递，则在函数上应用3次。默认值：1
 -mllvm -bcf_prob=40         ：如果激活了传递，基本块将以40％的概率进行模糊处理。默认值：30
 -mllvm -sobf                ：编译时候添加选项开启字符串加密
 -mllvm -seed=0x9e3779b9     ：指定0x9e3779b9随机数生成器种子流程
```

eg

```
 clang test.c -emit-llvm -S -mllvm -sub -mllvm -fla  -mllvm -sub_loop=3 -mllvm -split_num=3  -mllvm -bcf
 clang test.ll -o test.elf
```

去除

```
 python  deflat.py test.elf 0x00400000
```