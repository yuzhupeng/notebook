# CreateRemoteThread之dll注入

[入门1 创建LoadLibiary线程注入](CreateRemoteThread%E4%B9%8Bdll%E6%B3%A8%E5%85%A5%2038ffbc9ee1c148968fd9f87ef23e0967/%E5%85%A5%E9%97%A81%20%E5%88%9B%E5%BB%BALoadLibiary%E7%BA%BF%E7%A8%8B%E6%B3%A8%E5%85%A5%20ceda0f3bcc834c1aa72b31a4711205a6.md)

[入门1 创建FreeLibiary线程卸载](CreateRemoteThread%E4%B9%8Bdll%E6%B3%A8%E5%85%A5%2038ffbc9ee1c148968fd9f87ef23e0967/%E5%85%A5%E9%97%A81%20%E5%88%9B%E5%BB%BAFreeLibiary%E7%BA%BF%E7%A8%8B%E5%8D%B8%E8%BD%BD%2092aea4c6b8ec48b49d564c6b3111920a.md)

- 使用函数VirtualAllocEx在远程进程的地址空间中分配一块内存
- 使用函数WriteProcessMemory函数把DLL的路径名复制到第一步分配的内存中
- 使用函数GetProcAddress得到LoadLibrary函数的实际地址
- 使用函数CreateRemoteThread函数在远程进程中创建一个线程，让新线程调用正确的LoadLibrary函数并在参数中传入第一步分配的内存地址。现在远程进程中有一块内存，它是在第一步分配的，DLL也还在远程进程的地址空间中。为了对它进行清理，需要在远程线程退出之后执行后续步骤
- 使用函数VirtualFreeEx释放第一步分配的内存
- 使用函数GetProcAddress来得到FreeLibrary函数的实际地址
- 使用函数CreateRemoteThread在远程进程中创建一个线程，让该线程调用FreeLibrary函数并在参数中传入远程DLL的