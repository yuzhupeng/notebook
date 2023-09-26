from capstone import Cs
from capstone import *
import struct
def decode(offset):
    data_bin = open(r'E:\QQsavefile\MobileFile\mazes.exe', 'rb').read()
    data = data_bin[offset: offset + 0x199b + 20]
    md = Cs(CS_ARCH_X86, CS_MODE_64)
    inscnt = 0
    inscnt2 = 0
    map1 = []
    map2 = []
    map3 = []
    for i in md.disasm(data, offset):
        ins = "0x%x:\t%s\t%s" % (i.address, i.mnemonic, i.op_str)
        if 'mov	byte ptr [rbp' in ins:
            if inscnt < 629:
                #print(ins)
                map1.append(int(i.op_str.split(', ')[1], 16))
            inscnt += 1

        if 'mov	dword ptr [rbp' in ins:
            if inscnt2 < 2:
                map3.append(int(i.op_str.split(', ')[1], 16))
            inscnt2 += 1

        if 'lea	rax, [rip +' in ins:
            off1 = i.op_str[12:-1]
            off1 = int(off1, 16) + i.address + 7
            map2_data = data_bin[off1: off1 + 4 * 625]
            for i in range(625):
                map2.append(struct.unpack("I", map2_data[i * 4: i * 4 + 4])[0])

    data = []
    for i in range(625):
        data.append(map1[i] ^ map2[i])
    return data, bytearray(map1[-4:]), map3


def checkValid(map, x, y):
    if x < 0 or y < 0 or x > 24 or y > 24:
        return False
    return map[y * 25 + x] == ord('.')


def solve(map, startX, startY, direct, path):
    map[startY * 25 + startX] = ord('*')
    if len(path) == 15:
        return True, path

    all_dir = []
    if checkValid(map, startX, startY - 1):
        all_dir.append((startX, startY - 1, direct[1]))
    if checkValid(map, startX, startY + 1):
        all_dir.append((startX, startY + 1, direct[2]))
    if checkValid(map, startX - 1, startY):
        all_dir.append((startX - 1, startY, direct[3]))
    if checkValid(map, startX + 1, startY):
        all_dir.append((startX + 1, startY, direct[4]))

    for dir in all_dir:
        result = solve(map, dir[0], dir[1], direct, path + dir[2])
        if result[0] == True:
            return result
    return False, ''

def printMap(map1):
    for i in range(25):
        line = ''
        for j in range(25):
            line += chr(map1[i * 25 + j])
        print(line)

funcs = """"""

total = ''
for ll in funcs.splitlines():
    ll = ll.split("	")
    addr = int(ll[2], 16)
    map1, dirs, target = decode(addr)
    print("process:" + hex(addr))
    rr = solve(map1, target[0], target[1], 'A' + dirs.decode('utf-8'), '')
    total += rr[1]


import hashlib
mm = hashlib.md5(total.encode('utf-8')).hexdigest()

print(total)
print(len(total))
d = mm
print("flag{%s-%s-%s-%s-%s}" % (d[0:8],d[8:12],d[12:16],d[16:20],d[20:32]))