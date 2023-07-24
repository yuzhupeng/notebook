

# main函数参数 A版本

```c
#include<stdio.h>
int main(int argc, char* argv[])
{
	printf("Hello World\n");
	return 0;
}
```

int main(int argc, char *argv[])

其中，argc 表示命令行参数个数，

argv 是一个字符的指针数组，它存储了这些命令行参数。

具体来说，argv 数组会保存从命令行输入的所有参数，包括执行程序的文件名称和其他任何我们在运行时添加的参数。

例如，我们可以使用以下命令启动一个程序，并传递三个参数

```c
./myprogram.exe arg1 arg2 arg3
```

那么在程序中，在 main 函数中就可以通过 argv 数组来得到这些参数：

- argv[0] 表示可执行程序的文件名，即 "myprogram.exe"
- argv[1] 表示第一个参数，即 "arg1"
- argv[2] 表示第二个参数，即 "arg2"
- argv[3] 表示第三个参数，即 "arg3"

因此，我们可以通过遍历 argv 数组，来获取程序接收到的所有参数的值，从而实现不同的功能
