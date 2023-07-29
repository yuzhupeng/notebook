# 入门级键盘hook (SetWindowsHookEx)

# 说几句

```c
HHOOK SetWindowsHookExA(
  [in] int       idHook, //勾子的行为属性
  [in] HOOKPROC  lpfn, //对应的处理函数
  [in] HINSTANCE hmod,// 钩子函数所在的模块句柄
  [in] DWORD     dwThreadId //要勾取的线程ID
);
```

如果参数dwThreadId =0,意味着 该钩子会被定义为一个全局的钩子

我以书的代码为例子

[](https://github.com/redqx/mycode/tree/5f7a977201c6b2aaf067e5a852ca6a5cc4e1a133/Re/API_HOOK/入门级键盘hook-SetWindowsHookExA)

其中dll导出的函数也就2个

挂钩的函数

卸钩的函数

钩子处理函数(未导出): 装载一个全局钩子, 起的作用就是一个拦截noepad.exe的键盘行为

main.c也没干什么,也就调用挂钩,然后根据用户的输入做一个卸载钩子

说一下系统会干什么?

dll挂的钩子是全局的,也就说

在之前main.c会加载 KeyHook.dll ,然后取出他们的函数并使用

一旦任何进程发生键盘事件,系统就会强制把KeyHook.dll 注入到该进程

甚至都不需要我么去给其它进程注入dll,,,系统会自动的注入

为什么要自动注入? 因为钩子是全局的,如果发生键盘事件,那么所有的进程都需要调用对应dll的处理函数

如果KeyHook.dll 的 是x86的,然后某某进程是x64的就会导致某某进程程序崩溃

如果位数是匹配的那还好

下面就是一个注入的例子

当我的谷歌浏览器接收到键盘输入的时候

浏览器就会强制加载dll,然后就弹窗了

![Untitled](%E5%85%A5%E9%97%A8%E7%BA%A7%E9%94%AE%E7%9B%98hook%20(SetWindowsHookEx)%20580b1d75b5bb47c183193f6f5e600d0e/Untitled.png)

vs2022在写代码的时候,也会被强制加载dll,然后就弹窗了

![Untitled](%E5%85%A5%E9%97%A8%E7%BA%A7%E9%94%AE%E7%9B%98hook%20(SetWindowsHookEx)%20580b1d75b5bb47c183193f6f5e600d0e/Untitled%201.png)

关于如何调试?

# x64dbg调试问题

得配置一下下x64dbg

![Untitled](%E5%85%A5%E9%97%A8%E7%BA%A7%E9%94%AE%E7%9B%98hook%20(SetWindowsHookEx)%20580b1d75b5bb47c183193f6f5e600d0e/Untitled%202.png)

用x64debg附加调试notepad

当我在notepad的键盘输入的时候

就会停下来

![Untitled](%E5%85%A5%E9%97%A8%E7%BA%A7%E9%94%AE%E7%9B%98hook%20(SetWindowsHookEx)%20580b1d75b5bb47c183193f6f5e600d0e/Untitled%203.png)

查看符号表

![Untitled](%E5%85%A5%E9%97%A8%E7%BA%A7%E9%94%AE%E7%9B%98hook%20(SetWindowsHookEx)%20580b1d75b5bb47c183193f6f5e600d0e/Untitled%204.png)

差导入函数,最右边就是导入的函数

![Untitled](%E5%85%A5%E9%97%A8%E7%BA%A7%E9%94%AE%E7%9B%98hook%20(SetWindowsHookEx)%20580b1d75b5bb47c183193f6f5e600d0e/Untitled%205.png)

双击那些函数,我么就可以去扎扎到关键的钩子处理处理函数

![Untitled](%E5%85%A5%E9%97%A8%E7%BA%A7%E9%94%AE%E7%9B%98hook%20(SetWindowsHookEx)%20580b1d75b5bb47c183193f6f5e600d0e/Untitled%206.png)

其中地址xxxx5E就是我么说的钩子函数

# IDA64调试问题

选择debgger,找到debugger option ,在最下面

![Untitled](%E5%85%A5%E9%97%A8%E7%BA%A7%E9%94%AE%E7%9B%98hook%20(SetWindowsHookEx)%20580b1d75b5bb47c183193f6f5e600d0e/Untitled%207.png)

,其实可以发现,就算我不对这notepad输入,,他也被加载进去了很奇怪

![Untitled](%E5%85%A5%E9%97%A8%E7%BA%A7%E9%94%AE%E7%9B%98hook%20(SetWindowsHookEx)%20580b1d75b5bb47c183193f6f5e600d0e/Untitled%208.png)

而且,,,IDA无法查看一些dll的符号表

![Untitled](%E5%85%A5%E9%97%A8%E7%BA%A7%E9%94%AE%E7%9B%98hook%20(SetWindowsHookEx)%20580b1d75b5bb47c183193f6f5e600d0e/Untitled%209.png)