# C++混淆

# 全局clas

如果是局部的话,没太大意思

```
#include<stdio.h>
class sayHello
{
public:
	sayHello()
	{
		printf("start\n");
	}

	~sayHello()
	{
		printf("end\n");
	}
};

sayHello gclass;

void test()
{
	sayHello lclass;
	return;
}
int main()
{
	test();
	printf("I Am Main\n");
	return 0;
}
```

所以全局的class的

构造函数和析构函数是先于main和后于main

而局部的构造函数和析构函数就不多说