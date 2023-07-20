# IDA-Python

# 寻找特定指令

```python
from idc import *

def main():
    print("|-----BEGIN-----|")

    FBin1 = "E8 87 FE FF"
    x = 0x10005FC3
    addresses = []
    while True:
        x = idc.find_binary(x, SEARCH_DOWN, FBin1)
        if x == BADADDR:
            break
        addresses.append(hex(x)[2:].zfill(8))
        x = x + 1 
    for address in addresses:
        print(address)
    
    print("|------END------|")

if __name__ == '__main__':
    main()
```

# patch90

```c
from idaapi import *
from idc import *

def main():
    x = get_inf_structure().start_ea
    FBin1 = "75 05 74 03"
    print("start")
    while x != BADADDR:
        x = idc.find_binary(x, SEARCH_DOWN, FBin1)
        if x != BADADDR:
            patch_dword(x, 0x90909090)
            x += 4
            patch_word(x, 0x9090)
            x += 2
            patch_byte(x, 0x90)
            x += 1

    FBin1 = "75 04 74 02"
    x = get_inf_structure().start_ea
    while x != BADADDR:
        x = idc.find_binary(x, SEARCH_DOWN, FBin1)
        if x != BADADDR:
            patch_dword(x, 0x90909090)
            x += 4
            patch_word(x, 0x9090)
            x += 2

    FBin1 = "75 03 74 01"
    x = get_inf_structure().start_ea
    while x != BADADDR:
        x = idc.find_binary(x, SEARCH_DOWN, FBin1)
        if x != BADADDR:
            patch_dword(x, 0x90909090)
            x += 4
            patch_byte(x, 0x90)
            x += 1

    print("end")

if __name__ == '__main__':
    main()
```

# 下断点

```python
import idaapi

addresses = [
    0x1002051E #要下地址的列表
]

# 下断点
for address in addresses:
    idaapi.add_bpt(address)

print("断点已设置.")
```

如果已经下了断点,是不应该再下了

```python
import idaapi

addresses = [
    0x1002051E  # 要下断点的地址列表
]

# 下断点
for address in addresses:
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
```

# 写注释

```python
import idc

def add_comment(address, comment):
    idc.set_cmt(address, comment, 0)

# 示例用法
address = 0x10020526  # 替换为你想要添加注释的地址
comment = "这是一个注释"  # 替换为你想要添加的注释内容

add_comment(address, comment)
```

# 返回指定地址上一条指令的地址

```python
import idaapi

def get_previous_instruction_address(address):
    function = idaapi.get_func(address)
    if function is not None:
        return idaapi.prev_head(address, function.start_ea)
    return None

# 使用示例
address = 0x10006069  # 替换为你要查询的地址
previous_address = get_previous_instruction_address(address)
if previous_address is not None:
    print("Previous instruction address:" +hex(previous_address))
else:
    print("No previous instruction found.")
```