# 去除混淆

# 工具

defalt.py

[https://github.com/Pure-T/deflat](https://github.com/Pure-T/deflat)

# 使用?

拿到后,可以发现是ollvm

然后利用angr模块分析和去除

```
┌──(env1)─(kali㉿kali)-[~/code/angr]
└─$ python3 deflat.py attachment 0x400620
...
0x401121:  ['0x401198']
0x401e2d:  ['0x401e73', '0x4020c2']
0x401ed6:  ['0x401efa']
0x4020b3:  ['0x401d9a']
0x401117:  ['0x4015d4']
0x400620:  ['0x401121']
0x401f54:  []
************************patch*****************************
Successful! The recovered file: attachment_recovered
```

然后可能会有很多的waring,然后等待几分钟,

0x400620是混淆函数的起始地址,而不是main函数或者start函数的地址

比如我们的main函数被加了混淆,那就填写main的地址

比如其它函数加了混淆,就填入其它函数的地址

最后就可以生成一个文件attachment_recovered

然后我们就只需要分析文件 attachment_recovered 即可

有些时候,仍然不能正常取出混淆

就是在取出的过程中会报错,g,g,g