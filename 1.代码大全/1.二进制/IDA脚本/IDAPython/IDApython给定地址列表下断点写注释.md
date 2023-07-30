# IDA python 给定地址列表 下断点写注释

写注释还是手写吧..不方便

因为一些地址会遇到重复写注释

写注释的脚本

```python
import idc

def add_comment(address, comment):
    idc.set_cmt(address, comment, 0)

# 示例用法
address = 0x10020526  # 替换为你想要添加注释的地址
comment = "这是一个注释"  # 替换为你想要添加的注释内容

add_comment(address, comment)
```

了解一下

关键是怎么从火绒剑的 进程堆栈日志中提取地址,然后下断点

然后说一下批量下断点的原理

代码见github https://github.com/redqx/House/blob/master/txtCode/IDA_%E6%8F%92%E4%BB%B6/IDAPython/%E6%89%B9%E9%87%8F%E4%B8%8B%E6%96%AD%E7%82%B9.py



断点地址来源于火绒剑的进程堆栈cv下来的

```python
0x69890000
0x10000000
FIEL_chm0d: Micr0s0ft 0ffice 365 0nline.exe
0x698963070x698915760x69895F040x69895FCB
FIEL_chm0d: MSVCR100.dll
0x698963070x698915760x69895F040x69895FCB
FILE_t0uch: D0cument.d0c
0x698B05240x698B08E10x698AB5200x698AB4750x698AB5530x698A23980x6989DDA20x6989DDEA0x6989664F0x698917D30x69895F040x69895FCB
```

第一个是进程装载地址

第二个是PE文件的ImageBase

然后是动作的名称 FIEL_chm0d: Micr0s0ft 0ffice 365 0nline.exe

然后是该动作的调用 0x698963070x698915760x69895F040x69895FCB

该调用堆栈是我用QQ的ctrl alt A 文字提取出来的,所以变为了一行

需要用python处理一下该0x698963070x698915760x69895F040x69895FCB

实现的效果就是

```python
0x69896307
0x69891576
0x69895F04
0x69895FCB
```

期间互文字提取不准确

比如0变为字母o

这个需要手动处理一下

继续说代码原理

首先是读取前2行

```python
line = file.readline().strip()
real_base=int(line, 16)
line = file.readline().strip()
img_base=int(line, 16)
```

分别作为基地址

然后读取后面的数据

如果读取的是动作相关的字符串名称

就不处理,只数输出一下

如果是16进制字符串,进一步处理一下

```python
for line in file:
    line = line.strip()  # 去除行首尾的空白字符
    if line[0]=='0':
        solve_hex_line(line,real_base,img_base)#进一步处理一下
    else:
        print(line)
```

处理代码如下

```python
def solve_hex_line(text,real_base,img_base):
    # 使用正则表达式提取目标内容
    pattern = r"0x[0-9A-F]{8}"
    matches = re.findall(pattern, text)
    # 打印提取的内容
    for match in matches:
        RVA_addr=int(match, 16)-real_base 
        VA_dyn=RVA_addr+real_base
        VA_sta=RVA_addr+img_base
        print("VA_sta: "+addr_hex(VA_sta)+" VA_dyn: "+addr_hex(VA_dyn))
        addMy_bp(VA_sta)
```

其中把0x698963070x698915760x69895F040x69895FCB变为

```python
0x69896307
0x69891576
0x69895F04
0x69895FCB
```

用的是

```python
def solve_hex_line(text,real_base,img_base):
    # 使用正则表达式提取目标内容
    pattern = r"0x[0-9A-F]{8}"
    matches = re.findall(pattern, text)
```

下断点的具体代码

```python
    for match in matches:
        RVA_addr=int(match, 16)-real_base 
        VA_dyn=RVA_addr+real_base
        VA_sta=RVA_addr+img_base
        print("VA_sta: "+addr_hex(VA_sta)+" VA_dyn: "+addr_hex(VA_dyn))
        addMy_bp(VA_sta)
```

其中要获取RVA

下断点对于静态分析用VA_sta —> `addMy_bp(VA_sta)`

下断点对于动态分析用VA_dyn  —> `addMy_bp(VA_dyn)`

说一下下断点的代码

```python
def addMy_bp(address):
# 下断点
#for address in addresses:
    # 检查地址是否已设置断点
    is_breakpoint_set = False
    for i in range(idaapi.get_bpt_qty()):
        bpt = idaapi.bpt_t()
        if idaapi.getn_bpt(i, bpt) and bpt.ea == address:
            is_breakpoint_set = True
            break
    if not is_breakpoint_set:
        idaapi.add_bpt(address)
        print("断点已设置.")
    else:
        print("断点重复.")
 
```

自我感觉,大概的原理就是看一下传递进来的地址送福在bp列表中

如果是就不下断点

如果不是,就新增加一个断点