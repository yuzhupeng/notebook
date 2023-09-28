*用于加载和操作 PE 文件的库*

libPEConv 的目标是创建一把用于自定义加载 PE 文件的“瑞士军刀”。

它不仅允许加载 PE 文件，还允许自定义一些步骤，即 IAT 挂钩（通过提供自定义 IAT 解析器）和函数重定向。

然而，它并不专注于内联挂钩，也不应该与 MS Detours 或 MinHook 等库混淆。

LibPeConv 可用于创建 PE 绑定器，因为它允许直接从资源加载 PE，并将其集成，就好像它是本地代码一样。

它还可以帮助您从内存中转储 PE，并重建其 IAT





# 用法



其实它给的是源代码,,可能更像是一个库,,,然后我们在源代码里面调用这些函数

然会一起编译

```c++
#include <Windows.h>
#include <iostream>

#include <peconv.h> // include libPeConv header

int main(int argc, char *argv[])
{
    if (argc < 2) {
        std::cout << "Args: <path to the exe>" << std::endl;
        return 0;
    }
    LPCSTR pe_path = argv[1];
    size_t v_size = 0;
#ifdef LOAD_FROM_PATH
    BYTE* my_pe = peconv::load_pe_executable(pe_path, v_size);
#else
    size_t bufsize = 0;
    BYTE *buffer = peconv::load_file(pe_path, bufsize);
    BYTE* my_pe = peconv::load_pe_executable(buffer, bufsize, v_size);
#endif
    if (!my_pe) {
        return -1;
    }
    peconv::set_main_module_in_peb((HMODULE)my_pe);
    const ULONGLONG load_base = (ULONGLONG)my_pe;
    peconv::load_delayed_imports(my_pe, load_base);
    peconv::run_tls_callbacks(my_pe, v_size);
    DWORD ep_rva = peconv::get_entry_point_rva(my_pe);
    if (!ep_rva) {
        return -2;
    }
    ULONG_PTR ep_va = ep_rva + (ULONG_PTR) my_pe;
    int (*new_main)() = (int(*)())ep_va;
    return new_main();
}

```

