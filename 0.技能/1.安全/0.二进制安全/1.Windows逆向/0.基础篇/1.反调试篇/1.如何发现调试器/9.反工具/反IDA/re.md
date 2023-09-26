# 反IDA



# 扰乱IDA编译

这是南邮CTF WxyVM2出现的一次IDA反编译故障,原因就是代码量太多,2万行

IDA直接放弃了

看wp,说需要修改IDA配置文件

```
xxx\IDA Pro 7.6\cfg\hexrays.cfg
```

把文本内容`MAX_FUNCSIZE= 64`修改`MAX_FUNCSIZE= 1024`

我觉得,这也算得上是一个反调试的方法,而且很少有人可以解除,虽然手段有点低级



