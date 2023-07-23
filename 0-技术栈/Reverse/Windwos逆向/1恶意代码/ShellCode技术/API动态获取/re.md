# 动态函数的获取

# 基址获取

# 导出表遍历

码云\mycode\PE\遍历导出函数

# 特色

比如加载一个API

我们要提供dll名字和api函数的名字

如何隐藏?

经历1

以前遇到过一个编码的,懒得写了

经历2

![Untitled](6d58e620ecf848fab69a088309affba7Untitled.png)

传递进来的是一个4字节的数值,这个数值对于了某个dll的某个api

正常来说,dll和api的字符串总和会远远大于4字节

但是,如果我们用一个算法,其实可以压缩到只有字节

用4字节确定一个dll

![Untitled](6d58e620ecf848fab69a088309affba7Untitled1.png)

用4字节确定一个api

![Untitled](6d58e620ecf848fab69a088309affba7Untitled2.png)

然后用8字节确定一个api+dll

![Untitled](6d58e620ecf848fab69a088309affba7Untitled3.png)