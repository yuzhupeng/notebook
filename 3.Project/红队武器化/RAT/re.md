# 用什么语言做远控最好



建议越底层越好,

因为越底层对环境要求越低

比如C#需要肉机得有.net环境

比如MFC,需要肉机得有xx环境

...

所以用C最好

同时vc6.0也是最好用的.因为他对环境没啥要求,编译出来的东西winxp.win7,win8.win10.甚至win11都可以运行



举一个实际的例子..

之前用vs2022写纯C语言,一个简单的MessageBOx,拖到虚拟机或者沙箱运行..

居然都需要2个dll环境,msvcruntime140d.dll和ucrtbased.dll

这是逼我用vs2019! 或者说逼我用vs2010? 还是说用vc6.0 ?



又说回来...环境问题的解决办法就一定只有一种吗? 难道我们不可以打包环境吗?比如附上msvcruntime140d.dll和ucrtbased.dll

但是.net那个环境确实不好附上

又说回来..有的时候..比如一些API..为了脱离环境...我们甚至需要手写这个API或者函数





# 什么是RAT,也就是远程控制的软件



[基于Gh0st的远控项目笔记](https://the-itach1.github.io/2022/08/10/基于Gh0st的远控项目笔记/)

[https://github.com/The-Itach1/MyGh0st](https://github.com/The-Itach1/MyGh0st)

葛军的灰鸽子

冰河的远控

教主哥的NRBUS

vkscvc





# 免杀项目

基于GHOST的,在百度云盘

基于新百家的,vc6.0 在阿里云盘



# Github远控

其它听说过的名字:

RedRAT

China RAT RedAttack

StarRAT

大灰狼

大蜘蛛

https://github.com/quasar/Quasar
