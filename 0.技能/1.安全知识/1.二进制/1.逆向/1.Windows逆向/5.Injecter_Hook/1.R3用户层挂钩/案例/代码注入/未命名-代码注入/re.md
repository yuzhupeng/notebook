# 代码注入

[逆向过程核心原理 CodeInject.cpp分析](%E4%BB%A3%E7%A0%81%E6%B3%A8%E5%85%A5%20f3e567e431414a60bbcf90228787e529/%E9%80%86%E5%90%91%E8%BF%87%E7%A8%8B%E6%A0%B8%E5%BF%83%E5%8E%9F%E7%90%86%20CodeInject%20cpp%E5%88%86%E6%9E%90%207ca131bbe8c34186b0667fb3d4b40b4d.md)

其实就在在远程进程写入代码,写入参数,然后让他运行

和dll有点类似

使用代码注入的原因

1. 占用内存少
2. 难以查找痕迹（比如**恶意代码**中就喜欢使用,也不是不可以查)
3. **DLL注入技术主要用于代码量大且复杂**的时候，而**代码注入则适合用于代码量小且简单**