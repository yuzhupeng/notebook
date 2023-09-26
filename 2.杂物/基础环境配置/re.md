# 基础环境配置

# 博客

Fluid , Stellar , Wikitten, Docsify



## notion-next

[https://docs.tangly1024.com/about](https://docs.tangly1024.com/about)

[https://www.tangly1024.com/article/notion-next](https://www.tangly1024.com/article/notion-next)

[https://www.tangly1024.com/article/notion-next-guide](https://www.tangly1024.com/article/notion-next-guide)

[https://www.tangly1024.com/article/vercel-deploy-notion-next](https://www.tangly1024.com/article/vercel-deploy-notion-next)

[https://vercel.com/dashboard](https://vercel.com/dashboard)

[https://github.com/tangly1024/NotionNext](https://github.com/tangly1024/NotionNext)



## hexo-Fluid

[Hexo Theme Fluid](https://hexo.fluid-dev.com/)

# **windows terminal ssh 链接 kali**

参考链接 [https://www.jianshu.com/p/b7a105a67253](https://www.jianshu.com/p/b7a105a67253)

在windwos terminal 的 setting.json 文件添加一个配置

```
{
    "commandline": "ssh 登录用户名@虚拟机IP -p 连接的端口号",
    //"commandline": "ssh kali@192.168.71.129 -p 22",
    "guid": "{c2b7c779-0f4f-4476-bfa5-313b42c9851a}", //不管
    "hidden": false, //不管
    "name": "vm-Kali" //取个名字
}
```

然后就可以连接了

每次连接要输入密码

于是采用一下操作

用git生成公开密钥文件

git的指令: ssh-keygen

```
cd ~
vi tmp.pub //这里要手动输入文件,然后保存退出
mkdir .ssh && chmod 700 .ssh
touch .ssh/authorized_keys && chmod 600 .ssh/authorized_keys
cat tmp.pub >> .ssh/authorized_keys
rm tmp.pub

```

# Ping指令

[解决Windows 10 防火墙开启后无法ping通的问题_COCO56（徐可可）的博客-CSDN博客](https://blog.csdn.net/coco56/article/details/105800213)

# PIP镜像源（国内）

常见pip镜像源（国内源）

清华：[https://pypi.tuna.tsinghua.edu.cn/simple](https://pypi.tuna.tsinghua.edu.cn/simple)

阿里云：[http://mirrors.aliyun.com/pypi/simple/](http://mirrors.aliyun.com/pypi/simple/)

中国科技大学 [https://pypi.mirrors.ustc.edu.cn/simple/](https://pypi.mirrors.ustc.edu.cn/simple/)

华中理工大学：[http://pypi.hustunique.com/](http://pypi.hustunique.com/)

山东理工大学：[http://pypi.sdutlinux.org/](http://pypi.sdutlinux.org/)

豆瓣：[http://pypi.douban.com/simple/](http://pypi.douban.com/simple/)

为什么使用这些源? 让pip下载起飞呗

如何使用这些源?

```
pip install -i https://pypi.tuna.tsinghua.edu.cn/simple 要下载的模块
```

比如下载pyDes

```
pip install -i http://mirrors.aliyun.com/pypi/simple/ --trusted-host mirrors.aliyun.com pyDes
```

# angr模块安装

可以参考链接 [https://blog.csdn.net/weixin_45055269/article/details/105176185](https://blog.csdn.net/weixin_45055269/article/details/105176185)

pip安装建议使用豆瓣源，速度飞起

### 安装virtualenv

```
pip3 install -i http://pypi.douban.com/simple --trusted-host pypi.douban.com virtualenv
```

### 创建目录

mkdir angr

cd angr

### 创建环境

virtualenv   env1

### 进入对应的环境

source venv/bin/activate

### 安装angr

pip3 install -i http://pypi.douban.com/simple --trusted-host pypi.douban.com angr

如果后面缺少什么

就直接

pip3 install -i http://pypi.douban.com/simple --trusted-host pypi.douban.com xxx

### 退出当前venv环境

```
deactivate
或者
source deactivate
```

# 其它

dll下载 [https://cn.dll-files.com/](https://cn.dll-files.com/)

phpstorm破解版: [https://www.php.cn/blog/detail/1046985.html](https://www.php.cn/blog/detail/1046985.html) , [https://www.bilibili.com/read/cv18994018](https://www.bilibili.com/read/cv18994018)