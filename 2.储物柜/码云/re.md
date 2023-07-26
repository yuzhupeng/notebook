上传的指令

```
git add .
git commit -m "笔记更新"
git push -u origin master


//git push -u -f origin master
//强制合并上传
```



以前解集上传问题

```
git config --global --unset http.proxy
git config --global --unset https.proxy
```



后来发现 设置这个效果棒极了

```
git config --global http.sslVerify "false"
```

