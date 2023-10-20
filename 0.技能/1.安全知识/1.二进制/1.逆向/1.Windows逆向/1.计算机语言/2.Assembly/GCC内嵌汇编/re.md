



```
asm(".intel_syntax noprefix");
```





```
gcc -masm=intel ./exmaple
```



```
"__asm__" 表示后面的代码为内嵌汇编，“asm”是“__asm__”的别名。
“__volatile__” 表示编译器不要优化代码，后面的指令保留原样，“volatile”是它的别名。 括号里面是汇编指令。

```

