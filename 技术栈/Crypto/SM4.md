# SM4

# 密钥生成算法

小总一下:

没有密钥的SM4算法,依然可以体现SM4的神奇之处

只不过是异或罢了

所以后面遇到SM4,没有密钥,也不要惊讶

密钥16字节,额外生成32个dword

比如`BYTE key[] = "Key[QkC5mKNFnzR]";`

我先列出整体的一个加密流程

```
DWORD* keyGen(DWORD* Key)
{
	int j = 0, i = 0;
	DWORD* keyArr32 = 0;
	DWORD trol[] = { 0,13,23 };
	DWORD tmp123;
	keyArr32 = malloc(36 * 4);
	/***************************开始生成子秘钥********************************************/
	for (i = 0; i < 4; i++)
	{
		endianSwapt(&Key[i]);
		keyArr32[i] = Key[i] ^ TBL_SYS_PARAMS[i];
	}

	for (i = 0; i < 32; i++)//32次循环迭代运算
	{
		//5-36为32个子秘钥
		tmp123 = keyArr32[i + 1] ^ keyArr32[i + 2] ^ keyArr32[i + 3] ^ TBL_FIX_PARAMS[i];
		keyArr32[i + 4] = keyArr32[i + 0] ^ F(tmp123,trol,3);
	}
	return keyArr32;
}
```

首先,我们传入的`DWORD Key[4]`做一个端序转化

也就是对每个dword做一个端序的转化

然后我们每次会连续取出前面4个dword来生成第5个dword

先取出keyArr32[i + 1] , keyArr32[i + 2] ,keyArr32[i + 3] ,和一个常量异或

`tmp123 = keyArr32[i + 1] ^ keyArr32[i + 2] ^ keyArr32[i + 3] ^ TBL_FIX_PARAMS[i];`

然后把tmp123 传入一个F函数,然会返回一个dword,然后和我们的 `keyArr32[i + 0]`来个异或

最后生成了第5个`keyArr32[i + 4]`

这样往复的迭代下去,会生成32个dword

其中描述一下F函数

传入的参数

`tmp123`

`DWORD trol[] = { 0,13,23 };`

`3`

其中F函数长这个样子

```
DWORD F(DWORD tmp123,DWORD* tRol,int rolLen)
{
	int i = 0;
	BYTE* lpA = &tmp123;
	DWORD dwRet = 0;
	//endianSwapt(lpA);
	for (i = 0; i < 4; i++)
	{
		lpA[i] = TBL_SBOX[lpA[i]];
	}
	//endianSwapt(lpA);
	for (i = 0; i < rolLen; i++)
	{
		dwRet ^= fROL(tmp123, tRol[i]);
	}
	return dwRet;
}
```

做一个类似于AES/DES的查表操作

最后来一个循环左移动,分别循环左移0,13,23位,每次移位都来个异或操作

然后这就是循环位移的操作

# 加密算法流程

小结:

加密和解密都涉及一些端序的转化

其实端序转化不是什么阻碍.反正是对称的,

也就是说加密有端序转化,那么解密也要有.

值得注意的是加密生成的arr[3+32]

arr[-1],,arr[-2],arr[-3],,arr[-4]才是最后的加密结果

在解密的时候,也是拿着arr[-1],,arr[-2],arr[-3],,arr[-4]去解密

而不是arr[-4],,arr[-3],arr[-2],,arr[-1]

首先,加密算法采用的和解密算法是一样的流程

加密算法采用的和解密算法只不过传入的参数不一样

同时他们 和密钥生成算法其实很相似

加密算法每次分组加密16字节

整体流程如下,

```
void SM4(int round, DWORD* KeyArr36, BYTE* input, int mod)
{
	int j,i;
	DWORD  dwArr36[36] = { 0 };
	DWORD* dw_input = input;
	DWORD tmp123;
	DWORD trol[] = { 0,2,10,18,24 };
	for (i = 0; i < round; i++)
	{
		dw_input = input + 16 * i;
		for (j = 0; j < 4; j++)
		{
			endianSwapt(&dw_input[j]);//端序转化
			dwArr36[j] = dw_input[j];//赋值
			//printf("%08X\n", dwArr36[j]);
		}
		//加密
		for (j = 0; j < 32; j++)//根据已经有的DWORD[4],去生成32个新的DWORD[4]
		{

			tmp123 = dwArr36[j + 1] ^ dwArr36[j + 2] ^ dwArr36[j + 3] ^ KeyArr36[j*mod];
			dwArr36[j + 4] = dwArr36[j + 0] ^ F(tmp123, trol, 5);
			//printf("%08X\n", dwArr36[j + 4]);
		}
		for (j = 0; j < 4; j++)
		{
			endianSwapt(&dwArr36[35 - j]);//端序转化
			dw_input[j] = dwArr36[35 - j];//逆序赋值
		}
	}
	return;
}
```

我们对加密算法传入的参数是

`SM4(round, &keyArr[4], new_input, 1);`

总之注意我们的传入的`&keyArr[4]`

然后说一下加密的流程

提前准备了一个`DWORD  dwArr36[36] = { 0 };`

然后把已有传入的 `dw_input[0,4)`做一个端序转化

然后赋值给`dwArr36[0,4)`

然后就是一个和密钥算法差不多的操作了,加密算法只是没有异或一些常量

经过不断的迭代后,最后的dword [4] 差不多就是加密结果

对最后的dword [4]既要反正取出,还得做一个端序转化

# 解密算法流程

先说一下解密算法和加密算法有什么不同

首先 采用的算法是一致

传递的参数不同

加密是: `SM4(round, &keyArr[4], new_input, 1);`

解密是: `SM4(round, &keyArr[35], new_input, -1);`

于是导致的现象就是在迭代算法那里出现了差异

```
//加密
for (j = 0; j < 32; j++)//根据已经有的DWORD[4],去生成32个新的DWORD[4]
{

    tmp123 = dwArr36[j + 1] ^ dwArr36[j + 2] ^ dwArr36[j + 3] ^ KeyArr36[j*mod];
    dwArr36[j + 4] = dwArr36[j + 0] ^ F(tmp123, trol, 5);
    //printf("%08X\n", dwArr36[j + 4]);
}
```

加密异或的是`KeyArr36[4],KeyArr36[5],...,KeyArr36[35],`

解密异或的是`KeyArr36[35],KeyArr36[34],...,KeyArr36[4],`

然后解密的原理是什么呢??? 虽然看上去很对称

```
666C6167
7B4B306B
61736E63
4D344D7D
4DFB7570
...
662A41F3
2B40A6A5
2B7479D0
E08C3D9A
4DF32282
```

```
4DF32282
E08C3D9A
2B7479D0
2B40A6A5
662A41F3
...
4DFB7570
4D344D7D
61736E63
7B4B306B
666C6167
```

可以看出,每次解密都是反向的求出上一位数据是什么,最后求出了我们的明文输入

就像是

x0,x1,x2,x3 -->x4

x4,x3,x2,x1 -->x0

首先对于`x0,x1,x2,x3 -->x4`正常不过了

但是`x4,x3,x2,x1 -->x0`是什么原理呢???

假如我么只有轮循环1次

加密数据是(A,B,C,D)然后生成了E

`E=F(B^C^D^key,xx,xx)^A=E`

我么把`F(B^C^D^key,xx,xx)=M`

于是E=M^A

所以如何解密? 当然是A=M^E

其中`M=F(B^C^D^key,xx,xx)`

且B^C^D^key,xx,xx参数都是已知的

所以就很好解密啦

然后轮循环次数不是32的话,仔细琢磨一下就知道了

所以,我们就知道了,其实F函数的具体内容并不重要

无论F函数多么的复杂,只要保证y=f(x)是一个一对一的关系即可

# 查表优化

参考链接:

[https://www.cnblogs.com/kentle/p/15510582.html](https://www.cnblogs.com/kentle/p/15510582.html)