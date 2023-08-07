# 初始CDN

CDN: 主服务器的副本,方便全国各地快捷访问相同的资源



https://ping.chinaz.com/

![image-20230808000920195](img/image-20230808000920195.png)

结果

![image-20230808000934734](img/image-20230808000934734.png)



https://www.itdog.cn/ping/

![image-20230808000622042](img/image-20230808000622042.png)

可以看到多个地方出现了响应,他们就是对于服务器的子服务器,可能也叫CDN

![image-20230808000803169](img/image-20230808000803169.png)

那些不同的响应IP就是对应的CDN服务器

我们通常要寻找到主服务器,也就是那个真实的IP,

而不是CDN服务器的IP



# 如何绕过CDN



域名解析记录

```
https://viewdns.info/iphistory
https://site.ip138.com
https://x.threatbook.com/
```



通过一些域名解析记录,

然后看他的注册时间

我们可能去发现他的真实IP

![image-20230808002603502](img/image-20230808002603502.png)





# 备案查询



```
https://beian.miit.gov.cn/#/Integrated/recordQuery
https://beian.tianyancha.com/ie5c91690282143559179#!
https://icp.chinaz.com/
```

