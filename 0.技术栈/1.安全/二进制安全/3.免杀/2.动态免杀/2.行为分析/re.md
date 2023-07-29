其实就是一个行为的分析,判断是不是一个恶意的行为

比如先调用OpenProcess,然后又调用了VirtualAlloc,然后又CreateRemoteThread

杀软就会根据这个特征匹配或者判断出,你将要干嘛,然后把你Kill掉





逃过机器学习:

通过异常的方式,比如SEH的try catch

