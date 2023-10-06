

参考链接

比较全面的： https://blog.csdn.net/weixin_38391755/article/details/80380786

感觉比较入门的： https://zhuanlan.zhihu.com/p/618350718



本是要学cmake的

因为cmake可以自动的生产makefile

所以为什么还要去学makefile对吧

当我去学cmake的时候,对于cmake自动生成的makefile看不懂,,,哈哈

所以又回来学makefile了



总体的规则模板

![image-20231002154442236](img/image-20231002154442236.png)

还有就是这目标的字符串名字可以任意指定,,但是最好是符合规范的

同时也有一些隐晦的规则用于目标名

此外，还有一些约定俗成的目标名：

- `all`：通常用于指定默认的目标，表示编译或构建所有的目标。
- `clean`：用于删除生成的文件，清理项目目录。
- `install`：用于将已编译的程序文件安装到指定的位置。
- `test`：用于运行测试脚本、执行测试用例等。
- `doc`：用于生成项目文档。



在看别人写的Makefile文件时，你可能会碰到以下三个变量：

``` 
$@，$^，$<
```

代表的意义分别是：

$@ --目标文件，

$^ --所有的依赖文件，

$< --第一个依赖文件。

实际的例子

```makefile
SUBDIR := .
SUBDIR += ./func

INCS := $(foreach dir,$(SUBDIR),-I$(dir))

main : ./entry.o ./func/bar.o
	gcc ./entry.o ./func/bar.o -o main

./entry.o : ./entry.c
	gcc -c $(INCS) ./entry.c -o ./entry.o

./func/bar.o : ./func/bar.c
	gcc -c $(INCS) ./func/bar.c -o ./func/bar.o
```

然后 

```makefile
SUBDIR := .
SUBDIR += ./func

INCS := $(foreach dir,$(SUBDIR),-I$(dir))
SRCS := $(foreach dir,$(SUBDIR),$(wildcard $(dir)/*.c))

main : ./entry.o ./func/bar.o
        gcc ./entry.o ./func/bar.o -o main

%.o : %.c
        gcc -c $(INCS) $< -o $@
```





注释符号 `#`



# Multi file operation



demo1

```makefile
# Makefile
SRCS := $(wildcard *.c)

main : $(SRCS)
	gcc $(SRCS) -o main
```



wildcard是一个关键字，所以没什么意义

然后*.C当然代表了所有的c文件

然后SRCS就相当于一个宏定义，包揽了所有的C文件

 

默认的情况下，make命令会在当前目录下按顺序找寻文件名为**“GNUmakefile”、“makefile”、“Makefile”**的

但是,,,我们一般都使用 `makefile`



一般来说，定义在Makefile中的目标可能会有很多，但是第一条规则中的目标将被确立为最终的目标。

如果第一条规则中的目标有很多个，那么，第一个目标会成为最终的目标。make所完成的也就是这个目标。





如果一个工程有3个头文件，和8个C文件，我们为了完成前面所述的那三个规则，

我们的Makefile应该是下面的这个样子的

```makefile
edit : main.o kbd.o command.o display.o insert.o search.o files.o utils.o

       cc -o edit main.o kbd.o command.o display.o insert.o search.o files.o utils.o
       
main.o : main.c defs.h

       cc -c main.c

kbd.o : kbd.c defs.h command.h

       cc -c kbd.c

command.o : command.c defs.h command.h

       cc -c command.c

display.o : display.c defs.h buffer.h

       cc -c display.c

insert.o : insert.c defs.h buffer.h

       cc -c insert.c

search.o : search.c defs.h buffer.h

       cc -c search.c

files.o : files.c defs.h buffer.h command.h

       cc -c files.c

utils.o : utils.c defs.h

       cc -c utils.c

clean :

       rm edit main.o kbd.o command.o display.o \

          insert.o search.o files.o utils.o
```



**反斜杠（\）**是换行符的意思。这样比较便于Makefile的易读 

最后目标的生产需要很多.o

.o文件的生成又需要很多的.c



搜先最后要生成的东西叫 edit

但是生成的名字由`-o`指定



关于变量的引用, 理解为宏定义会更加的好

比如我们用 objects 去引用字符串 `main.o kbd.o command.o display.o insert.osearch.o files.o utils.o`



```makefile
objects = main.o kbd.o command.o display.o insert.osearch.o files.o utils.o 
edit : $(objects)
       cc -o edit $(objects)
main.o : main.c defs.h
       cc -c main.c
kbd.o : kbd.c defs.h command.h
       cc -c kbd.c
command.o : command.c defs.h command.h
       cc -c command.c
display.o : display.c defs.h buffer.h
       cc -c display.c
insert.o : insert.c defs.h buffer.h
       cc -c insert.c
search.o : search.c defs.h buffer.h
       cc -c search.c
files.o : files.c defs.h buffer.h command.h
       cc -c files.c
utils.o : utils.c defs.h
       cc -c utils.c
clean :
       rm edit $(objects)
```



但是,这个东西我们还可以简化的

```makefile
   objects = main.o kbd.o command.o display.o  insert.o search.o files.o utils.o
 
   edit : $(objects)
           cc -o edit $(objects)
 
   main.o : defs.h
   kbd.o : defs.h command.h
   command.o : defs.h command.h
   display.o : defs.h buffer.h
   insert.o : defs.h buffer.h
   search.o : defs.h buffer.h
   files.o : defs.h buffer.h command.h
   utils.o : defs.h
 
   .PHONY : clean
   clean :
           rm edit $(objects)
```



# VPATH

# demo



## demo1

其实demo看多了，就懂很多了

```
# Makefile
SRCS := $(wildcard *.c)

main : $(SRCS)
        gcc $(SRCS) -o main
```



SRCS可以说是变量，当然也有人说他是宏定义

因为SRCS的使用，用的是 $(），所以我更加愿意叫他是变量





`:=` 是c语言的=好的意思

类似的还有一些符号，我只能说大概了解一下意思，然后看不懂意思

等以后遇到了再去细细的了解

其他符号还有： =，:= ， +=，?=



另外，C语言编译器除了gcc，还有cc

cc比较老，所以目前用gcc



另外再说一些东西，那就是字符串连接

在make脚本里面，好像目前我还没有看见数值这个东西，都是字符串的

比如相加是字符串的连接，连体也是字符串的连接

dqx+dqx就是dqxdqx

dqx$(demo)就是dqxdqx （当demo=dqx的时候）

 

还有就是，字符串相加最后是以空格作为分隔，主要是从运行结果来看，我没发现他们是连在一起的





## demo2



```makefile
# Makefile
SUBDIR := .
SUBDIR += ./func

INCS := $(foreach dir,$(SUBDIR),-I$(dir))
SRCS := $(foreach dir,$(SUBDIR),$(wildcard $(dir)/*.c))

main : $(SRCS)
        gcc $(INCS) $(SRCS) -o main
```



我们一句一句的分析

`SUBDIR += ./func`的结果就是 =

`.(space)./func`

所以会有空格的出现

然后我们再说说函数

foreach是GNU Make中的一个函数，用于迭代一个列表，并对列表中的每个元素执行一系列操作。它的语法如下：

$(foreach var, list, text)

其中，var是一个临时变量名，list是一个以空格或逗号分隔的列表，text是要执行的操作。

然后我们再来看看上面的脚本

```
$(foreach dir,$(SUBDIR),-I$(dir))
```

 其中，他的操作是把字符-I和变量$(dir)连接

$(dir)是从列表$(SUBDIR)中取出的，列表以空格或者逗号分隔

然后结果会输出到临时变量 dir 中



然后我们再来看看脚本是什么意思

```
# Makefile
SUBDIR := .
SUBDIR += ./func

INCS := $(foreach dir,$(SUBDIR),-I$(dir))
SRCS := $(foreach dir,$(SUBDIR),$(wildcard $(dir)/*.c))

main : $(SRCS)
        gcc $(INCS) $(SRCS) -o main
```



首先SUBDIR的值是`.`和`./func`,然后以空格分开

INCS是`-I.`和`-I./func` 

SRCS是　./下所有的c文件和./func/下的所有c文件集合

最后参与编译

```
redqx@UbuntuYY:~/code/c$ make
gcc -I. -I./func ./entry.c ./func/bar.c -o main
```

其中-I是指定头文件的地方

首先我们看一下目录结构

```
── CMakeLists.txt
├── entry.c
├── func
│   ├── bar.c
│   └── bar.h
├── main
└── Makefile

1 directory, 6 files
```

可以看到是有头文件的，





## demo3

说一下` $(patsubst pattern,replacement,text)`的意思

把text中,符号表达式patsubst的替换为表达式replacement的



```makefile
SUBDIR := .
SUBDIR += ./func

INCS := $(foreach dir,$(SUBDIR),-I$(dir))
SRCS := $(foreach dir,$(SUBDIR),$(wildcard $(dir)/*.c))
OBJS := $(patsubst %.c,%.o,$(SRCS))

main : $(OBJS)
	gcc $(OBJS) -o main

%.o : %.c
	gcc -c $(INCS) $< -o $@
```



```
%.o : %.c
	gcc -c $(INCS) $< -o $@
```

%.o会匹配所有.o文件,然后%.c会根据前面的结果把.c替换为.o

但是每次只进行一个操作,虽然会匹配多个结果

然后此刻的目标名貌似不能任意了,要和%.c一起使用了



然后

```
OBJS := $(patsubst %.c,%.o,$(SRCS))
```

就是把所有SRCS中,所有.c的换位.o的样子





## demo4



gcc -c 不会进行链接操作,也就是会生出.o文件,而不是最后的elf



```makefile
SUBDIR := ./
SUBDIR += ./func

OUTPUT := ./output

INCS := $(foreach dir,$(SUBDIR),-I$(dir))
SRCS := $(foreach dir,$(SUBDIR),$(wildcard $(dir)/*.c))
OBJS := $(patsubst %.c,$(OUTPUT)/%.o,$(SRCS))

main : $(OBJS)
	gcc $(OBJS) -o main

$(OUTPUT)/%.o : %.c
	mkdir -p $(dir $@)
	gcc -c $(INCS) $< -o $@
```

 

首先本次demo相比较上一次而言,他在指定路径保留了.o文件



## demo5

其实就是在指令面前加@,然后指令就不会显示出来



```makefile
SUBDIR := ./
SUBDIR += ./func

OUTPUT := ./output

INCS := $(foreach dir,$(SUBDIR),-I$(dir))
SRCS := $(foreach dir,$(SUBDIR),$(wildcard $(dir)/*.c))
OBJS := $(patsubst %.c,$(OUTPUT)/%.o,$(SRCS))

main : $(OBJS)
	@gcc $(OBJS) -o main

$(OUTPUT)/%.o : %.c
	mkdir -p $(dir $@)
	@gcc -c $(INCS) $< -o $@
	
.PHONY : clean

OUTPUT := ./output
clean:
	@rm -r $(OUTPUT)
```





# 左耳朵耗子





```makefile
objects = entry.o bar.o

edit : $(objects)
	gcc -o main $(objects)

entry.o :bar.h
bar.o 	:bar.h

.PHONY : clean
clean :
	rm main $(objects)
```

这是一种比较简洁的写法,但是有个问题,,那就是他默认是cc

比如编译下面的东西,他是用的cc,而不是gcc,所以很离谱

```
entry.o :bar.h
bar.o 	:bar.h
```

