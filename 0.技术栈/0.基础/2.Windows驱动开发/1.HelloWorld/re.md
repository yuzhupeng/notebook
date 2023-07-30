





环境配置 https://blog.csdn.net/hk_5788/article/details/46801647

我用的是vs2019+wdk10，安装过程既快有方便



```c
#include<ntifs.h>
void my_unload(PDRIVERT xx)
{
	DbgPrint(  "Good Bye\n" );

}
NTSTATUS  KmdfHelloWorldEvtDeviceAdd(
    _In_    WDFDRIVER       Driver, 
    _Inout_ PWDFDEVICE_INIT DeviceInit
)
{
     Driver->DriverUnload=my_unload;
    // Print "Hello World"
    DbgPrint(  "KmdfHelloWorld: KmdfHelloWorldEvtDeviceAdd\n" );

    return status;
}
```





加载驱动的3种方式

1), 通过服务

2), ntdll的API调用 ZwLoadDriver

3), inf文件

4), 百度一下





![image-20230729015911909](img/image-20230729015911909.png)