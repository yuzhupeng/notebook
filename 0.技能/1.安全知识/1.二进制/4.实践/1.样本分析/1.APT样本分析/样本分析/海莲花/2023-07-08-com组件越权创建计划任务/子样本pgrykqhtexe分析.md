# 子样本pgrykqht.exe分析

一样是通过白文件加载黑dll

白文件还是以前那个文件,dll是自己释放资源得到的

一样是通过dll劫持

一样是有很多的花指令

![Untitled](%E5%AD%90%E6%A0%B7%E6%9C%ACpgrykqht%20exe%E5%88%86%E6%9E%90%206bd8ee5b0fd5404eb12aa0fe42591fa0/Untitled.png)

通过火绒剑分析可得

也就是遍历目录,并启动程序

找到并执行了 4z4sytf2.exe, xc1i5rgl.exe

下面是线程函数

![Untitled](%E5%AD%90%E6%A0%B7%E6%9C%ACpgrykqht%20exe%E5%88%86%E6%9E%90%206bd8ee5b0fd5404eb12aa0fe42591fa0/Untitled%201.png)

自我感觉,我只能猜测他是遍历目录,然后找到文件并启动

其中用来一个大循环了实现,找到就执行,没找到几句继续找

其中涉及的文件遍历链子

```c
sub_10003FE8(v15);
	--sub_10003D87(v5);
		-- sub_10003F4C((int)v3) )
			-- FindFirstFileExW(*(this + 1144), FindExInfoStandard, (this + 544), FindExSearchNameMatch, 0, 0);
```