关于我如何让exe编译成功的

打七的那个x86 debug模式也是编译有问题的,至少在我的电脑上是这样的



关闭C++ -> SDL检查

然后 链接库 -> 输入- >  

```c++
..\..\common\zlib\zlib.lib;Wininet.lib;kernel32.lib;user32.lib;gdi32.lib;winspool.lib;comdlg32.lib;advapi32.lib;shell32.lib;ole32.lib;oleaut32.lib;uuid.lib;odbc32.lib;odbccp32.lib;%(AdditionalDependencies)
```

