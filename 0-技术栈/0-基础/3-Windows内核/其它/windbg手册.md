



该指令在windbg双机调试环境下有效 win10 -> xp

```
kd> r gdtr //产看gdtr数组在哪里
gdtr=8003f000
kd> r gdtl //查看gdtr表数组的大小
gdtl=000003ff
```





遍历所有进程

```c++
!process 0 0 
```

