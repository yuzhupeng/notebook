# 什么特征码



计算机病毒的特征码需要能够把病毒程序与一般的程序区分开来，即能够唯一标识一个或者一类多态病毒

要能够成为计算机病毒程序的指纹

每个杀毒软件公司都有自己的特征码提取方法和提取工具，这也是特别需要技术的地方，弄不好就造成误判，

将好文件当成病毒给杀了



一般来说，病毒特征码可能需要满足以下几个要求：

1), 不能从数据区提取，因为数据区的内容很容易改变，一旦病毒程序变更版本，改变了数据内容，特征码就会失效。而其它的区块则相对来说保险一些。

2), 在保持特征码的唯一性的前提下，应当尽量使得特征码短小精悍，从而减少检测过程中的时间与空间的复杂度，提高检测效率。

3), 经过详细的逆向分析之后选取出来的特征码，才足以将该病毒与其它病毒或正常程序相区别。

4), 病毒程序的特征码一定不能匹配到普通程序，比如选取病毒入口点的二进制代码，就必然出现误报的情况。

5), 从特征函数提取特征码,比如一些高频出现的解码函数,主打一个高频出现,不管病毒如何更新,该字节码序列是不变的



杀软可能选取多处特征码,交叉混合的匹配样本,当同时包含了多出特征,就会报毒



 ![image.png](img/47f00ac8af794143a1b9859141d7d0ba.png)





# YARA扫描

你的特征码其实一般情况下都会被Yara分析并识别

YARA是一款旨在帮助恶意软件研究人员识别和分类恶意软件样本的开源工具，

使用YARA可以基于文本或二进制模式创建恶意软件家族描述信息，当然也可以是其他匹配信息



下面这个是一个Yara的匹配案例,可以看看特征码扫描是什么样子的

```c
rule PoisonIvy_Generic_3 {
    //规则介绍
    meta:
        description = "PoisonIvy RAT Generic Rule"
        license = "https://creativecommons.org/licenses/by-nc/4.0/"
        author = "Florian Roth"
        date = "2022-04-07"
        hash = "e1cbdf740785f97c93a0a7a01ef2614be792afcd"
    
    //要匹配的数据    
    strings:
    	//敏感的某个字符串
        $k1 = "Tiger324{" fullword ascii
		
		//敏感的模块信息
        $s2 = "WININET.dll" fullword ascii
        $s3 = "mscoree.dll" fullword wide
        $s4 = "WS2_32.dll" fullword
        $s5 = "Explorer.exe" fullword wide
        $s6 = "USER32.DLL"
        $s7 = "CONOUT$"
        $s8 = "login.asp"
		
		//敏感的网络信息
        $h1 = "HTTP/1.0"
        $h2 = "POST"
        $h3 = "login.asp"
        $h4 = "check.asp"
        $h5 = "result.asp"
        $h6 = "upload.asp"
    condition:
        	uint16(0) == 0x5a4d and filesize < 500KB and //判断是不是PE文件,并且文件大小<500kb
            ( 
                $k1 or all of ($s*) or all of ($h*)
                //满足上面3个大类的其中任意一种情况都被查杀返回TRUE,否则返回FALSE
            )
}
```





# 杀软之常见的特征码结构



先了解一下常见的数据结构



病毒库结构: 名称表，包括壳名、包名、特征名 

```c
structVirInfo
{
    DWORD dwID;
    BYTE btLength;
    char VirName[btLength];
};
```



病毒库常见的PE库结构

```c
struct VirpeHeader
{
    WORD wUnknown;
    DWORD dwHeadSize;
    DWORD dwSize;
    WORD wTable1Count;
    WORD wdMaskCount;//表示共有多少条PE特征
    WORD wdUnknown4;
    DWORD dwTable1Off;
    DWORD dwTable2Off;
    DWORD dwTable3Off;
    DWORD dwTable4Off;
    DWORD dwTable5Off;//传统特征
    DWORD dwTable6Off //多模式特征
};
```



特征结构

```c
struct Sig 
{
    DWORD dwRecordID;//特征ID，用于关联特征名
    DWORD dwOffset;//特征偏移，用于定位病毒代码具体位置
    BYTE btLength;//特征长度，用于扫描匹配病毒代码长度
    BYTE btOffsetBaseAddress;//特征基地址，用于定位特征起始位置
    BYTE btVirMask[btLength];//病毒特征具体内容
};
```



二进制全文匹配: 也就是一一匹配.通常取出一些字节,然后和样本进行一个匹配

多模匹配:将恶意程序的明显字符串信息提取出，记录在病毒库中，与待检测样本进行多模匹配。