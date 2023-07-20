# Dll注入-入门篇

# Dll注入

[Dll注入-入门篇](Dll%E6%B3%A8%E5%85%A5%2013288fb50c49466d8fdeac0d647ccb37/Dll%E6%B3%A8%E5%85%A5-%E5%85%A5%E9%97%A8%E7%AF%87%20ac75aa92045748c799f462536a8ad8ea.md)

下面这个文章还没有看

[](https://www.52pojie.cn/thread-830796-1-1.html)

[https://github.com/xia0ji233/Xprocess](https://github.com/xia0ji233/Xprocess)





![Untitled](Dll%E6%B3%A8%E5%85%A5-%E5%85%A5%E9%97%A8%E7%AF%87%20ac75aa92045748c799f462536a8ad8ea/Untitled.png)

如上所见,DLL 注入共四个步骤：

1. 附加到目标/远程进程
2. 在目标/远程进程内分配内存
3. 将DLL文件路径，或者DLL文件，复制到目标/远程进程的内存空间
4. 控制进程运行DLL文件

下面这个链接比较猛..

[https://www.ired.team/offensive-security/code-injection-process-injection/dll-injection](https://www.ired.team/offensive-security/code-injection-process-injection/dll-injection)

相关的注入技术:

[CreateRemoteThread之dll注入](Dll%E6%B3%A8%E5%85%A5-%E5%85%A5%E9%97%A8%E7%AF%87%20ac75aa92045748c799f462536a8ad8ea/CreateRemoteThread%E4%B9%8Bdll%E6%B3%A8%E5%85%A5%2038ffbc9ee1c148968fd9f87ef23e0967.md)

[AppInit_Dlls(不太行)](Dll%E6%B3%A8%E5%85%A5-%E5%85%A5%E9%97%A8%E7%AF%87%20ac75aa92045748c799f462536a8ad8ea/AppInit_Dlls(%E4%B8%8D%E5%A4%AA%E8%A1%8C)%20c4a2bba7bdd845ca8388532c988efb8b.md)

[IAT注入(也就是增加一个IAT表)](Dll%E6%B3%A8%E5%85%A5-%E5%85%A5%E9%97%A8%E7%AF%87%20ac75aa92045748c799f462536a8ad8ea/IAT%E6%B3%A8%E5%85%A5(%E4%B9%9F%E5%B0%B1%E6%98%AF%E5%A2%9E%E5%8A%A0%E4%B8%80%E4%B8%AAIAT%E8%A1%A8)%20e0fe1de6953b45d4aecde2e59067b8b0.md)

[代码注入](Dll%E6%B3%A8%E5%85%A5-%E5%85%A5%E9%97%A8%E7%AF%87%20ac75aa92045748c799f462536a8ad8ea/%E4%BB%A3%E7%A0%81%E6%B3%A8%E5%85%A5%20f3e567e431414a60bbcf90228787e529.md)

[dll文件替换](Dll%E6%B3%A8%E5%85%A5-%E5%85%A5%E9%97%A8%E7%AF%87%20ac75aa92045748c799f462536a8ad8ea/dll%E6%96%87%E4%BB%B6%E6%9B%BF%E6%8D%A2%207e7e15fcacef41f78a83e4cd8c173d92.md)

[**APC注入 (未完成,还挺高级的)**](Dll%E6%B3%A8%E5%85%A5-%E5%85%A5%E9%97%A8%E7%AF%87%20ac75aa92045748c799f462536a8ad8ea/APC%E6%B3%A8%E5%85%A5%20(%E6%9C%AA%E5%AE%8C%E6%88%90,%E8%BF%98%E6%8C%BA%E9%AB%98%E7%BA%A7%E7%9A%84)%20778ac1a42e8b479db9e836b4d332aa09.md)

SetWindowsHookEx() 消息钩取

- NtCreateThreadEx()
- QueueUserAPC
- RtlCreateUserThread()
- 利用SetThreadContext()找到的代码区域 直接把线程结构体指向自己?
- 反射DLL