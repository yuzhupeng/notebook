

https://anhkgg.com/aanti-virus/

# 国内的杀软公司



360全家桶 一级棒

火绒其次

然后是电脑管家

其它的可能较次之(金山毒霸,江明,瑞星...),主要是没太听说他们



主动防御方面: 360和火绒 较强, 可能火绒做的更加优秀

查杀方面: 360和电脑管家 较强

反问: 360全家桶是指哪些?

![image-20230724213237229](img/image-20230724213237229.png)

就如图4个,就是360全家桶





> 360:

延迟查杀: 发现样本后,先不杀,哪怕你主动扫描它, 360会把它上传到云端分析,依据结果再对你进行查杀 

# 传统的查杀



## 查杀方式的基本框架 



**静态查杀**  : 一般根据特征码(涉及多处多组)，然后对文件进 **多维的** 交叉匹配 

**行为查杀**  : 关注软件执行时的行为模式。行为查杀会监控程序的运行，并分析其行为，如文件操作、系统修改、网络通信等。

如果一个程序表现出类似恶意软件的行为模式，即使其没有明确的病毒特征，行为查杀系统也可以标记它为潜在威胁或恶意软件

**云查杀**    :  它将样本上传到云端进行分析和检测。云端服务器使用先进的算法和机器学习技术来识别恶意代码。

一旦新的威胁被确认，其特征会被添加到云数据库中，以便全球用户受益于对新威胁的防护







>    细说行为查杀:



传统杀软检测原理:

通过对病毒多年年的观察、研究，有些行行行为是病毒的共同行行行为，而而且比比较特殊，

在正常程序中，这些行行行为比比较罕 见。

当程序运行行行时，监视其进程的各种行行行为，如果发现了了病毒行行行为，立立即报警



主动防御:

主动防御并不不需要病毒特征码支支持，只要杀毒软件能分析并扫描到目目标程序的行行行为，

并根据预先设定的规则，判定是否应该进行行行清除操作主动防御本来想领先于病毒，

让杀毒软件自自己己变成安全工工程师来分析病毒，从而而达到以不不变应万变的境界。

但是，计算机的智能总是在一一系列列的规则下诞生生，而而普通用用户的技术水水平达不不到专业分析病毒的水水平，

两者之间的博弈将主动防御推上一一个尴尬境地。





>   机器器学习识别技术

也是一种nb的查杀方式

可参考 https://bbs.pediy.com/thread-272298.htm

机器器学习识别技术既可以做静态样本的二二进制分析，又又可以运用用在沙箱动态行行行为分析当中，

是为内容/行行行为+算法模式。伴随着深度学习的急速发展，各家厂厂商也开始尝试运用用深度学习技术来识别病毒特征，

如瀚思科技的基于深度学习的二二进制恶意样本检测



## 监测技术



1、内存读取监测

监控所有进程，扫描内存。



2、写入文件监测

对于写入磁盘的文件进行扫描。

比如,我们创建一个文件,会被杀软检测到,确定安全才会被创建




3、网页安全监测

检测是否存在网络攻击和不安全下载



4、邮件附件监测

对于电子邮件的附件进行病毒扫描。

比如自动解压一些文件,zip或者7z什么的,如果你加了密码,就不会被杀软解压缩



5、进程行为监测

沙盒模式即为进程行为检测，监测进程各种行为，对于敏感操作进行警报。



通过深度学习技术来识别病毒特征，不仅可以做静态样本的二进制分析，也可以运用到沙盒动态行为分析中。

过的免杀,: 是可以让样本运行起来,拍起来之后是否被杀,是另外的事情



浅浅的提及一下 360对进程的检测:

当有新的进程创建的时候,360会Hook,并挂起新的进程,并对该进程进行一个扫描

新的进程的父进程一般是 explore.exe



下面是一个360挂的内核钩子,对进程创建做一个检测

![image-20230724095625828](img/image-20230724095625828.png)

比如360对explore.exe的创建进程挂钩

![image-20230728114510450](img/image-20230728114510450.png)



VUE: 可以实现杀软的自动脱壳

![image-20230724111834892](img/image-20230724111834892.png)

既然都可以实现脱壳了,那么对于压缩包解压缩当然也是手到擒来的事情









## 扫描技术



1 扫描压缩包技术：即是对压缩包案和封装文件作分析检查的技术。

2 程序窜改防护：即是避免恶意程序借由删除杀毒侦测程序而大肆破坏电脑。

3 修复技术：即是对恶意程序所损坏的文件进⾏还原

4 急救盘杀毒：利用空白U盘制作急救启动盘，来检测电脑病毒。类似于大白菜pe启动器

5 智能扫描：扫描最常用的磁盘，系统关键位置，耗时较短。

6 全盘扫描：扫描电脑全部磁盘，耗时较长。

7 勒索软件防护：保护电脑中的文件不被⿊客恶意加密。

8 开机扫描：当电脑开机时自动进行扫描，可以扫描压缩文档和可能不需要的程序







## 报毒原因



![image-20230729211906041](img/image-20230729211906041.png)

比如这个东西,如果一款恶意样本被识别,那么就会有一个报毒原因



在确定一款病毒是恶意样本后

杀软会返回一个报毒原因,

比如定义这款是加了壳的,

比如这是一款木马

比如这是一个被恶意修改ep的

所以他么都会返回一个类似于报毒原因的东西

那么这段东西对我们免杀其实做了一个很好的东西

因为这个相当于一个错误回显,,,,让你知道你为什么被杀

然后通过这个东西我们再进一步修改自己的代码

来达到绕过杀软

常见的杀软的报毒原因参考 破资料



但是这个东西,杀软肯定是不会公开的,

我们只能通过测试,去猜测





# 为什么学免杀

个人兴趣罢了



下面是红队的一些东东

![image-20230729141256028](img/image-20230729141256028.png)

 

![image-20230730211251360](img/image-20230730211251360.png)

# 关于过免杀

在前期我们能做的是过静态免杀,启发式查杀

对于一些动态行为检测什么的,那个东西需要0Day或者漏洞之类的才可以绕过杀软

所以道阻且长

同时过免杀,其实还有点像过反调试



做到杀软打我3下,我还杀软7下

和他们玩捉迷藏

反正在思维方面要活跃起来

关键字: bypass, Anti-Virus
