

# API



## CoCreateInstanc

CoCreateInstance函数是用于在COM（Component Object Model）中创建一个特定类的实例的函数。

它是通过调用特定类的构造函数来创建对象的。



```c
HRESULT CoCreateInstance(
  REFCLSID  rclsid, //要创建的COM类的标识符（CLSID）。它是一个唯一的值，用于标识COM组件。
  LPUNKNOWN pUnkOuter, //指向外部未知对象（IUnknown）的指针。在大多数情况下，使用NULL表示没有外部未知对象
  DWORD     dwClsContext, //定类的上下文。常见的值有：
                            //-   CLSCTX_INPROC_SERVER：在调用进程的上下文中创建对象。
                            //-   CLSCTX_LOCAL_SERVER：通过启动独立进程来创建对象。
                            //-   CLSCTX_REMOTE_SERVER：通过远程服务器来创建对象。
  REFIID    riid, //要获取的接口的标识符（IID）。它是用于标识接口的唯一值。
  LPVOID    *ppv, //用于接收创建的对象指针的指针。
);

```



# eg1



```c
#include <atlbase.h>
#include <comdef.h>
#include <iostream>
#include <Windows.h>
#include <shlobj_core.h>
#include <taskschd.h>
#pragma comment(lib, "taskschd.lib")

ITaskService* m_lpITS = NULL;
ITaskFolder* m_lpRootFolder = NULL;


//初始化COM组件
void Init() {
	//1.CoInitialize初始化COM组件
	HRESULT hr = CoInitialize(NULL);
	if (FAILED(hr)) {
		MessageBox(NULL, L"初始化COM组件失败", L"Failed", MB_OK);
	}

	//2.CoCreateInstance创建任务服务对象
	hr = CoCreateInstance(CLSID_TaskScheduler, NULL, CLSCTX_INPROC_SERVER, IID_ITaskService, (LPVOID*)&m_lpITS);
	if (FAILED(hr)) {
		MessageBox(NULL, L"创建任务服务失败", L"Failed", MB_OK);
	}
	//3.连接到任务服务
	hr = m_lpITS->Connect(_variant_t(), _variant_t(), _variant_t(), _variant_t());
	if (FAILED(hr)) {
		MessageBox(NULL, L"连接服务失败", L"Failed", MB_OK);
	}
	//4.从ITaskService对象中获取根任务Root Task Folder的指针对象ITaskFolder，这个指针指向新注册的任务
	hr = m_lpITS->GetFolder(_bstr_t("\\"), &m_lpRootFolder);
	if (FAILED(hr)) {
		MessageBox(NULL, L"获取指针失败", L"Failed", MB_OK);

	}
}

//卸载COM组件
void UnInit() {
	if (m_lpITS)
	{
		m_lpITS->Release();
	}
	if (m_lpRootFolder)
	{
		m_lpRootFolder->Release();
	}
	::CoUninitialize();
}

//创建计划任务
BOOL CreateTask(const char* lpszTaskName, const char* lpszProgramPath, const char* lpszParameters, const char* lpszAuthor) {
	// 创建任务定义对象来创建任务
	ITaskDefinition* pTaskDefinition = NULL;
	HRESULT hr = m_lpITS->NewTask(0, &pTaskDefinition);
	if (FAILED(hr))
	{
		return FALSE;
	}

	/* 设置注册信息 */
	IRegistrationInfo* pRegInfo = NULL;
	CComVariant variantAuthor(NULL);
	variantAuthor = lpszAuthor;
	hr = pTaskDefinition->get_RegistrationInfo(&pRegInfo);
	if (FAILED(hr))
	{
		return FALSE;
	}
	// 设置作者信息
	hr = pRegInfo->put_Author(variantAuthor.bstrVal);
	pRegInfo->Release();

	/* 设置登录类型和运行权限 */
	IPrincipal* pPrincipal = NULL;
	hr = pTaskDefinition->get_Principal(&pPrincipal);
	if (FAILED(hr))
	{
		return FALSE;
	}
	// 设置登录类型
	hr = pPrincipal->put_LogonType(TASK_LOGON_INTERACTIVE_TOKEN);
	// 设置运行权限
	// 最高权限
	hr = pPrincipal->put_RunLevel(TASK_RUNLEVEL_HIGHEST);
	pPrincipal->Release();

	/* 设置其他信息 */
	ITaskSettings* pSettting = NULL;
	hr = pTaskDefinition->get_Settings(&pSettting);
	if (FAILED(hr))
	{
		return FALSE;
	}
	// 设置其他信息
	hr = pSettting->put_StopIfGoingOnBatteries(VARIANT_FALSE);
	hr = pSettting->put_DisallowStartIfOnBatteries(VARIANT_FALSE);
	hr = pSettting->put_AllowDemandStart(VARIANT_TRUE);
	hr = pSettting->put_StartWhenAvailable(VARIANT_FALSE);
	hr = pSettting->put_MultipleInstances(TASK_INSTANCES_PARALLEL);
	pSettting->Release();

	/* 创建执行动作 */
	IActionCollection* pActionCollect = NULL;
	hr = pTaskDefinition->get_Actions(&pActionCollect);
	if (FAILED(hr))
	{
		return FALSE;
	}
	IAction* pAction = NULL;
	// 创建执行操作
	hr = pActionCollect->Create(TASK_ACTION_EXEC, &pAction);
	pActionCollect->Release();

	/* 设置执行程序路径和参数 */
	CComVariant variantProgramPath(NULL);
	CComVariant variantParameters(NULL);
	IExecAction* pExecAction = NULL;
	hr = pAction->QueryInterface(IID_IExecAction, (PVOID*)(&pExecAction));
	if (FAILED(hr))
	{
		pAction->Release();
		return FALSE;
	}
	pAction->Release();
	// 设置程序路径和参数
	variantProgramPath = lpszProgramPath;
	variantParameters = lpszParameters;
	pExecAction->put_Path(variantProgramPath.bstrVal);
	pExecAction->put_Arguments(variantParameters.bstrVal);
	pExecAction->Release();

	/* 创建触发器，实现用户登陆自启动 */
	ITriggerCollection* pTriggers = NULL;
	hr = pTaskDefinition->get_Triggers(&pTriggers);
	if (FAILED(hr))
	{
		return FALSE;
	}
	// 创建触发器,把触发器设置为
	ITrigger* pTrigger = NULL;
	hr = pTriggers->Create(TASK_TRIGGER_LOGON, &pTrigger);
	if (FAILED(hr))
	{
		return FALSE;
	}

	/* 注册任务计划  */
	IRegisteredTask* pRegisteredTask = NULL;
	CComVariant variantTaskName(NULL);
	variantTaskName = lpszTaskName;
	hr = m_lpRootFolder->RegisterTaskDefinition(variantTaskName.bstrVal,
		pTaskDefinition,
		TASK_CREATE_OR_UPDATE,
		_variant_t(),
		_variant_t(),
		TASK_LOGON_INTERACTIVE_TOKEN,
		_variant_t(""),
		&pRegisteredTask);
	if (FAILED(hr))
	{
		pTaskDefinition->Release();
		return FALSE;
	}
	pTaskDefinition->Release();
	pRegisteredTask->Release();
	return TRUE;
}


//删除计划任务
BOOL DeleteTask(char* lpszTaskName)
{
	if (NULL == m_lpRootFolder)
	{
		return FALSE;
	}
	CComVariant variantTaskName(NULL);
	variantTaskName = lpszTaskName;
	HRESULT hr = m_lpRootFolder->DeleteTask(variantTaskName.bstrVal, 0);
	if (FAILED(hr))
	{
		return FALSE;
	}

	return TRUE;
}
int main()
{
	const char* lpszTaskName = "hacker";   //任务名
	const char* lpszProgramPath = "f:/test.exe";  //要执行的程序路径
	const char* lpszParameters = " redqx ";     //程序参数
	const char* lpszAuthor = "";

	Init();
	BOOL bRet = CreateTask(lpszTaskName, lpszProgramPath, lpszParameters, lpszAuthor);
	if (!bRet) {
		printf("Create Task Failed");
		return -1;
	}
	UnInit();
	printf("Successd");

	return 0;
}


```

