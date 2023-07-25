# 播放音乐



```c
#include<Windows.h>
#include<mmsystem.h>//包含多媒体设备接口头文件
#pragma comment (lib,"winmm.lib")//加载静态库
int main()
{
	//alias   的作用是取别名
	mciSendString("open E:/0xC0de/C/2010/C1/C1/青花瓷.mp3 alias dqx ",0,0,0);//音乐在当前目录下也可以写"open ./2.mp3"./这里可以省略
	mciSendString("play dqx", 0, 0, 0);
	system("pause");//这里没有暂停程序会直接停止听不到音乐
	return 0;
}
/*
返回值：MCIERROR即DWORD类型，而DWORD类型就是unsigned int，执行成功返回0，出错返回非0值
参数  :
1，lpstrCommand，指向以null结尾的命令字符串：”命令 设备[参数]” open 打开音乐文件 play 播放音乐 repeat 重复播放 close 关闭音乐文件
2，lpstrReturnString，指向接收返回信息的缓冲区，为NULL时不返回信息
3，uReturnLength，上述缓冲区的大小
4，hwndCallback，在命令串中含notify时，它指定一个回调窗口的句柄，一般为NULL
*/
```