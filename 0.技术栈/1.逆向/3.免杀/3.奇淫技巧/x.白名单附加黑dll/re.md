白名单文件: 一个文件的哈希值被信任

可以是带有Microsoft的二进制文件，例如office软件

也可以是Microsoft系统目录中二进制文件

也可以是第三方认证签名程序



常见的白名单程序

```
Rundll32.exe、Msiexec.exe、MSBuild.exe、InstallUtil.exe、
Mshta.exe、Regsvr32.exe、Cmstp.exe、CScript.exe、
WScript.exe、Forfiles.exe、te.exe、Odbcconf.exe、
InfDefaultInstall.exe、Diskshadow.exe、PsExec.exe、
Msdeploy.exe、Winword.exe、Regasm.exe、Regsvcs.exe
、Ftp.exe、pubprn.vbs、winrm.vbs、slmgr.vbs、
Xwizard.exe、Compiler.exe、IEExec.exe、
MavInject32、Presentationhost.exe、Wmic.exe、
Pcalua.exe、Url.dll、zipfldr.dll、Syncappvpublishingserver.vbs
```

具体更多白名单文件也可以去 https://lolbas-project.github.io/ 看看



ps: 为什么要用白名单? 白名单的对立面是什么(黑免单+未知程序),是不是白名单程序的dll就不会被扫描呢





如果说是用于攻击的DLL,采用的手法是基于文件的DLL替换

其中要注意的是,DLL是如何被加载的

1), 导入表加载,最具有漏洞的一种, 

2), exe模块LoadLibrary一个dll, 其中也可能会存在DLL的哈希校验, 也可能存在DLL加载路径是一个写死的路径

3), dll模块去LoadLibrary一个dll, 其中也可能会存在DLL的哈希校验, 也可能存在DLL加载路径是一个写死的路径





> 关于主动加载DLL的一个hash校验:

比如说是否在当前目录下的dll

比如路径是否在360文件夹下

比如文件的MD5值是否正确

比如文件大小是否正确

ps: 导入表加载不可能存在hash校验





如何判断LoadLIbrary的路径是不是写死的路径

> 方法1:直接运行

你把exe单独拖到任意路径,

如果看到exe仍然可以运行

那么大概说明该DLL的路径不是写死的

ps: 如我是开发着,在第一次安装的时候,我就会记录当前文件位置环境并写死到全局的环境变量

,那么exe在任意地方加载dll都可以对环境做一个检查,

要么直接导向全局变量的环境

要么判断当前环境是否异常





> 方法2:调试

对LoadLibraryA/W下断点,看写入的路径是不是一个绝对路径



>方法3:静态查看

其实就是拖进IDA看









案例:

https://mp.weixin.qq.com/s/1WEglPXm1Q5n6T-c4OhhXA

https://mp.weixin.qq.com/s/XPrBK1Yh5ggO-PeK85mqcg