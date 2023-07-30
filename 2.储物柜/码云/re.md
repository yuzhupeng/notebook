参考链接



https://scofieldwyq.github.io/2016/02/29/git%E4%BB%8E%E8%BF%9C%E7%A8%8B%E5%BA%93%E5%90%8C%E6%AD%A5%E5%88%B0%E6%9C%AC%E5%9C%B0%E4%BB%93%E5%BA%93/

https://blog.csdn.net/qq_29493173/article/details/113094143

https://zhuanlan.zhihu.com/p/636418854



# 第一次



```c
git config --global user.name  "redqx"//自定义用户名
git config --global user.email "935797872@qq.com"//用户邮箱
ssh-keygen -t rsa -C "935797872@qq.com"
ssh -T git@github.com
git remote add origin https://github.com/redqx/NoteBook.git
```



# 上传慢问题



以前解集上传问题

```
git config --global --unset http.proxy
git config --global --unset https.proxy
```



后来发现 设置这个效果棒极了

```
git config --global http.sslVerify "false"
```





貌似上面这些问题都无法解决

修改git端口才是最棒的

```
# 注意修改成自己的IP和端口号
git config --global http.proxy http://127.0.0.1:7890 
git config --global https.proxy http://127.0.0.1:7890
```





# 远程同步到本地



## 1完全同步



```
git fetch --all
git reset --hard origin/master
#git pull
```

第一句: 拉取所有更新，不同步；

第二句：本地代码同步线上最新版本(会覆盖本地所有与远程仓库上同名的文件)；

第三句：再更新一次（其实也可以不用，第二步命令做过了其实）



## 2有增加的同步



```
git fetch origin master
git log master.. origin/master
git merge origin/master #有差异才输入这个
```

这回有一个什么效果: 如果出现不同的他会新增加(这就导致,你更名一个文件,他不会知道)





首先，查看远程仓库 `git remote -v`

```
$ git remote -v
origin	git@github.com:{User}/Understanding_Unix-Linux_Programming.git (fetch)
origin	git@github.com:{User}/Understanding_Unix-Linux_Programming.git (push)
```

把远程库更新到本地 `git fetch origin master`

```
$ git fetch origin master
Warning: Permanently added the RSA host key for IP address '{IP address such as: 192.168.1.1 }' to the list of known hosts.
From github.com:{User}/Understanding_Unix-Linux_Programming
 * branch            master     -> FETCH_HEAD
```

比较远程更新和本地版本库的差异 `git log master.. origin/master`

```
$ git log master.. origin/master
commit ce39f8b3eeee898a2a038444f897f2aef3673493
Author: {User} <794870409@qq.com>
Date:   Fri Feb 26 14:14:39 2016 +0800

    {The context origin added ... }
```

合并远程库 `git merge origin/master`

- 有差异

  ```
  $ git merge origin/master
  Updating eb32b20..ce39f8b
  Fast-forward
   README.md | 2 +-
   1 file changed, 1 insertion(+), 1 deletion(-)
  ```

- 无差异

  ```
  $ git merge origin/master
  Already up-to-date
  ```



# 本地上传到远程



上传的指令(强制替换替代)

```
git add .
git commit -m "笔记更新"
git push -u origin master


//git push -u -f origin master
//强制合并上传
```

