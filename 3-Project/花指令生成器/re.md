

花指令真的有很多,

所以对于长度1/2/4的随机变化,我们就忽略了,如果加上,会导致更多的随机化

而且为了方便自己的一些操作,垃圾代码统一为4字节



15

```assembly
E8 02 00 00 00                call    sub_411760
90                            nop
90                            nop
90                            nop
90                            nop
36 80 04 24 06                add     byte ptr [esp+0],10
C3                            retn
```



12

```assembly
E8 04 00 00 00                call    sub_411760
90                            nop
90                            nop
90                            nop
90                            nop
83 C4 0C                      add     esp, 4
```



SF=1

```
50                            push    eax
32 C0                         xor     al, al
04 80                         add     al, 80h ; '€'
78 04                         js      short loc_411762
90                            nop
90                            nop
90                            nop
90                            nop
58                            pop     eax
```



SF=0

```
50                            push    eax
B0 80                         mov     al, 80h ; '€'
32 C0                         xor     al, al
79 04                         jns     short loc_41176E
90                            nop
90                            nop
90                            nop
90                            nop 
58                            pop     eax
```



一些修改标志位的还有我们的cmp,test指令

拿着cmp/test和我们的全局变量做一个比较

```
cmp lp_dword, dword_value
cmp lp_word, word_value
cmp lp_byte, byte_value


test lp_dword, dword_value
test lp_word, word_value
test lp_byte, byte_value
```

