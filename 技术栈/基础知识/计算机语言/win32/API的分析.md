# API的分析

https://www.cnblogs.com/LyShark/p/12976302.html



API分为W版本和A版本

然后什么时候会出现非要用W版本? 如果永远都不会涉及W版本,那么干嘛用W版本呢

比如 进程的名字, Dll的名字 这些就会涉及奇怪的名字

但是函数的名字不会出现W版本

# API连招

[API 连招](API%E7%9A%84%E5%88%86%E6%9E%90%20c2de792b95814bf485877649cbdd9901/API%20%E8%BF%9E%E6%8B%9B%204eefa683144c4ae18122ad88cf10a97a.md)

# **GetCurrentProcessId**

在进程终止之前，进程标识符会在整个系统中唯一标识进程。

```c
DWORD GetCurrentProcessId();
```

# FreeLibiary

[Freelibiary](API%E7%9A%84%E5%88%86%E6%9E%90%20c2de792b95814bf485877649cbdd9901/Freelibiary%20e6658476955e42ab82ce827d1d0f3e2d.md)

# GetModuleFileName

获取当前进程已加载模块的文件的完整路径，该模块必须由当前进程加载。

如果想要获取另一个已加载模块的文件路径，可以使用GetModuleFileNameEx函数。

```c
GetModuleFileName(
	HMODULE hModule,
	LPTSTR lpFilename,
	DWORD nSize 
);
```

# NtQuerySystemInformation

[NtQuerySystemInformation](API%E7%9A%84%E5%88%86%E6%9E%90%20c2de792b95814bf485877649cbdd9901/NtQuerySystemInformation%20e4ebb3ff9d11417b98c68aa33a9316b3.md)