#include <windows.h>
#include <stdio.h>

int main()
{
	HMODULE hDll = LoadLibraryA("./bad.dll");
	while (1)
	{
		hDll = hDll;
	}
	FreeLibrary(hDll);
	return 0;
}