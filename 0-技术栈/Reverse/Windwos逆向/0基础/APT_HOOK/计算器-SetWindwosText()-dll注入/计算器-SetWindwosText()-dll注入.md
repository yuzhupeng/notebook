# 计算器-SetWindwosText()-dll注入

# bb

```c
BOOL SetWindowText(
    HWND hWnd, // handle of window or control
    LPCTSTR lpString// address of string     
);
```

突然闪过一个想法, 我们常常去hook一些底层的API,而不是它的外层或者包装它的API

比如计算机用于窗口显示的API有SetDlgItemTextW和SetWindowTextW

但是SetDlgItemTextW又会去调用SetWindowTextW显示字符串

所以我们要hook的东西当然是SetWindowTextW了

然后就是再小小的提醒一句

x86的进程是插入x86的dll

x64的进程是插入x64的dll

再小小的插一句

![Untitled](%E8%AE%A1%E7%AE%97%E5%99%A8-SetWindwosText()-dll%E6%B3%A8%E5%85%A5%2088ed00214c154a99a9369a8f2aa232d0/Untitled.png)

对于一些函数到底是A版本还是W版本

或者有没有A,W版本 ,其中我们可以通过高亮去知道

其中A,W只不过是一个宏定义罢了

宏定义的颜色是紫色,所以getmoudulehandle有w和a版本的区别

然后getproaddress是黄色,说明没有A,W版本的区别

![Untitled](%E8%AE%A1%E7%AE%97%E5%99%A8-SetWindwosText()-dll%E6%B3%A8%E5%85%A5%2088ed00214c154a99a9369a8f2aa232d0/Untitled%201.png)

![Untitled](%E8%AE%A1%E7%AE%97%E5%99%A8-SetWindwosText()-dll%E6%B3%A8%E5%85%A5%2088ed00214c154a99a9369a8f2aa232d0/Untitled%202.png)

其中a1就是线程函数的地址,a2就是线程函数的参数

线程函数如下

![Untitled](%E8%AE%A1%E7%AE%97%E5%99%A8-SetWindwosText()-dll%E6%B3%A8%E5%85%A5%2088ed00214c154a99a9369a8f2aa232d0/Untitled%203.png)

线程函数参数如下

![Untitled](%E8%AE%A1%E7%AE%97%E5%99%A8-SetWindwosText()-dll%E6%B3%A8%E5%85%A5%2088ed00214c154a99a9369a8f2aa232d0/Untitled%204.png)

# dll说明

然后我们进入主题

注入器这方面,我就不多说,这是以前的知识点

然后说一下这个dll是干嘛的

## dllmain

```c
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    switch (fdwReason)
    {
        //动态的去修改iat 挂钩
    case DLL_PROCESS_ATTACH:
        lp_org_SetWindowTextW = GetProcAddress(GetModuleHandleA("user32.dll"),"SetWindowTextW");
        hook_IAT("user32.dll", lp_org_SetWindowTextW, (PROC)lp_new_SetWindowTextW);
        break;

        //动态的去修改iat 脱钩
    case DLL_PROCESS_DETACH:
        hook_IAT("user32.dll", (PROC)lp_new_SetWindowTextW, lp_org_SetWindowTextW);
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
        break;
    }

    return TRUE;
}
```

可以看到

进程加载的时候,它对SetWindowTextW 做了一个hook

在进程卸载的时候,],它对SetWindowTextW 做了以一个unohhk



## hook_IAT

其实就是对exe的IAT表做一个手脚

让计算器在调用SetWindowTextW的时候,直接去往我们的SetWindowTextW

然后我们对他的数据做一个处理

再执行SetWindowTextW

```c
/*
* 函数名字: hook_IAT_x86
* 函数参数:
* 参数1: 要hook的那个dll的a版字符串名字
* 参数2: 那个函数原来的地址(jmp地址)
* 参数3: 那个函数要被最终替换的地址(jmp地址)
* 
* 函数功能: 也就替换IAT[index]=我们的地址
*/

BOOL hook_IAT_x86(char* sz_user32Dll, PROC pfnOrg, PROC pfnNew)
{
    HMODULE hMod;
    LPCSTR szDllname;
    PIMAGE_IMPORT_DESCRIPTOR pImportDesc;
    PIMAGE_THUNK_DATA pThunk;
    DWORD dwOldProtect, dwRVA;
    BYTE* pAddr;

    hMod = GetModuleHandleA(NULL);//获取exe进程的ImageBase
    pAddr = (BYTE*)hMod;
    pAddr += *((DWORD*)&pAddr[0x3C]);
    dwRVA = *((DWORD*)&pAddr[0x80]);
    pImportDesc = (PIMAGE_IMPORT_DESCRIPTOR)((DWORD)hMod + dwRVA);//是导入表...exe才有导入表

    for (; pImportDesc->Name; pImportDesc++)
    {
        szDllname = (LPCSTR)((DWORD)hMod + pImportDesc->Name);
        if (!_stricmp(szDllname, sz_user32Dll))//对已有有的dll进行遍历,寻找目标dll
        {
            pThunk = (PIMAGE_THUNK_DATA)((DWORD)hMod +pImportDesc->FirstThunk);//寻找目标dll的IAT表
            for (; pThunk->u1.Function; pThunk++)//其实没必要这么写..别人是一个联合体..遍历IAT表
            {
                if (pThunk->u1.Function == (DWORD)pfnOrg)
                {
                    //匹配成功,修改IAT表
                    VirtualProtect((LPVOID)&pThunk->u1.Function,4,PAGE_EXECUTE_READWRITE,&dwOldProtect);
                    pThunk->u1.Function = (DWORD)pfnNew;
                    VirtualProtect((LPVOID)&pThunk->u1.Function,4,dwOldProtect,&dwOldProtect);
                    return TRUE;
                }
            }
        }
    }
    return FALSE;
}
```

这段代码..干的事情其实就是去遍历IAT表

找到了,就替换,

## lp_new_SetWindowTextW

这是我们自己的处理函数

```c
/*
* 函数名字: lp_new_SetWindowTextW
* 函数参数:
*   参数1: 一个控件的句柄
*   参数2: 一个字符串的指针
* 函数功能:
*   hook原始SetWindowTextW() 的内容,然后在再调用原始的API
*/

BOOL WINAPI lp_new_SetWindowTextW(HWND hWnd, WCHAR* lpString)
{
    WCHAR* pNum = L"零一二三四五六七八九";
    WCHAR temp[2] = { 0, };
    int i = 0, nLen = 0, nIndex = 0;

    nLen = wcslen(lpString);
    for (i = 0; i < nLen; i++)
    {
        // 阿拉伯数字转中文
        if (L'0' <= lpString[i] && lpString[i] <= L'9')
        {
            temp[0] = lpString[i];
            nIndex = _wtoi(temp);//宽字符转数字
            lpString[i] = pNum[nIndex];
        }
    }
    // 调用原来的user32!SetWindowTextW() API
    return ((PFSETWINDOWTEXTW)lp_org_SetWindowTextW)(hWnd, lpString);
}
```

干的事情也只不过是对拿到的数据做一个变化,再交付给别人