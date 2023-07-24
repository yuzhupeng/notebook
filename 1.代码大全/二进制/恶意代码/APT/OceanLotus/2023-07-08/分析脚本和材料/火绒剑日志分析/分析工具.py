from idc import *
import re
import idaapi

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
        pass

def addr_hex(text):
    return (hex(text)[2:]).zfill(8)

def solve_hex_line(text,real_base,img_base):
    # 使用正则表达式提取目标内容
    pattern = r"0x[0-9A-F]{8}"
    matches = re.findall(pattern, text)
    # 打印提取的内容
    for match in matches:
        RVA_addr=int(match, 16)-real_base 
        VA_dyn=RVA_addr+0x6B860000 #手动修改
        VA_sta=RVA_addr+img_base
        print("VA_sta: "+addr_hex(VA_sta)+" VA_dyn: "+addr_hex(VA_dyn))
        addMy_bp(VA_dyn)

# def is_hexadecimal(line):
#     try:
#         int(line, 16)
#         return True
#     except ValueError:
#         return False

def process_file(file_path):
    with open(file_path, 'r', encoding='utf-8') as file:
        line = file.readline().strip()
        real_base=int(line, 16)
        line = file.readline().strip()
        img_base=int(line, 16)
        for line in file:
            line = line.strip()  # 去除行首尾的空白字符
            if line[0]=='0':
                solve_hex_line(line,real_base,img_base)
            else:
                print(line)




def main():
    print("|-----BEGIN-----|")
 
    FBin1 = "E8 87 FE FF FF" # start
    FBin2 = "F7 05 D8 FA" # dllmain ep
    
    x =  0x6B860000 
    x = idc.find_binary(x, SEARCH_DOWN, FBin1)
    if x!=-1:
        print((hex(x)[2:]).zfill(8))
        #addMy_bp(x)

    x =  0x6B860000
    x = idc.find_binary(x, SEARCH_DOWN, FBin2)
    if x!=-1:
        print((hex(x)[2:]).zfill(8))
        #addMy_bp(x)


    file_path = 'C:/Users/virus/Desktop/bp.txt'
    process_file(file_path)

    print("|------END------|")

if __name__ == '__main__':
    main()