

资源数据所在的节通常名字是.rsrc

该节是一个多层的二叉树

struct IMAGE_DATA_DIRECTORY

资源表是一个比较复杂的结构,然后也不太会用它,

除非你想要把恶意数据写入资源表...但是



我们为什么不可以把恶意数据写入图片,然后把图片加载到恶意成语当中呢

总之办法很多,不到最后,就不忙学习资源表的东西吧

API

更新PE文件资源的: BeginUpdateResource、UpdateResource、EndUpdateResource。

遍历PE文件的资源: EnumResourceTypes、EnumResourceNames、EnumResourceLanguages。



具体可以参考一下,如何从PE角度遍历PE资源 [1.代码大全\1.二进制\PE编程\1.dict.md]

下面这个代码 从API角度直接获取资源

