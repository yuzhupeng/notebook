# 安装

可以参考链接 https://blog.csdn.net/weixin_45055269/article/details/105176185

pip安装建议使用豆瓣源，速度飞起

##### 安装virtualenv

```python
pip3 install -i http://pypi.douban.com/simple --trusted-host pypi.douban.com virtualenv
```

##### 创建目录

```
mkdir angr
cd angr
```

##### 创建环境

```
virtualenv   env1
```

##### 进入对应的环境

```
source venv/bin/activate
```

##### 安装angr

```
pip3 install -i http://pypi.douban.com/simple --trusted-host pypi.douban.com angr
```

如果后面缺少什么

就直接 

```
pip3 install -i http://pypi.douban.com/simple --trusted-host pypi.douban.com xxx
```

##### 退出当前venv环境

```python
deactivate
或者
source deactivate
```





# 去混淆?



defalt.py

https://github.com/Pure-T/deflat

defalt.py有很多新的版本,可能需要通过做题然后去发现吧





拿到后,可以发现是ollvm

然后利用angr模块分析和去除

```python
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

