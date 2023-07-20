# 任意Base&组合Base

# 任意

其实就是基于base58的原理,实现一个进制转化罢了

- 代码
    
    ```c
    #define _CRT_SECURE_NO_WARNINGS
    #include<stdio.h>
    #include<stdlib.h>
    #include<string.h>
    
    /*
    把长度为len的数组转化为二机制流,长度会多那么一位
    */
    char* num_to_bin(char* flag,int len,int max)
    {
    	char sz_tmp[8 + 1];
    	int i = 0, j = 0;
     
    	int round = 0;
    	int dw_tmp = 0;
    	char* bin_sz = 0;
    	sz_tmp[8] = 0;
    
    	bin_sz = malloc(len * max+max+1);
    	memset(bin_sz, '0', len * max+max);
    	bin_sz[len * max + max] = 0;
    	for (i = 0; i < len; i++)//这里是实际的转化为二进制字符串,而不是理论的
    	{
    		dw_tmp = flag[i];
    		for (j = max-1; j >= 0; j--)
    		{
    			sz_tmp[j] = dw_tmp % 2 + '0';
    			dw_tmp /= 2;
    		}
    		memcpy(&bin_sz[i * max], sz_tmp, max);
    	}
    	return bin_sz;
    }
    /*
    把二进制流按照间距max取出来,转化为数字,形成新的数组,数组长度为len
    */
    char* bin_to_num(char* bin_sz,int len,int max)//字符串长度/8,就是
    {
    	//每次读取4位
    	int i = 0, j = 0;
    	char* flag = 0;
    	flag = malloc(len);//
    	memset(flag, 0, len);
    
    	for (i = 0; i < len; i++)
    	{
    		for (j = 0; j < max; j++)
    		{
    			flag[i] = flag[i] * 2 + (bin_sz[max * i + j] - '0');
    		}
    	}
    	free(bin_sz);
    	return flag;
    }
    /*
    *根据索引值查表
    */
    char* Index_to_table(unsigned char* IndexArr,int len,char* t)
    {
    	int i = 0;
    	char* lp_ret = 0;
    
    	lp_ret = malloc(len+1);
    	memset(lp_ret, 0, len+1);
    	for (i = 0; i < len; i++) 
    	{
    		lp_ret[i] = t[IndexArr[i]];
    	}
    	free(IndexArr);
    	return lp_ret;
    }
    /*
    * 根据表查索引值
    */
    unsigned char* Table_to_index(char* flag,char* t)
    {
    	int len = 0;
    	int tlen = 0;
    	unsigned char* IndexArr = 0;
    	int i = 0;
    	int j = 0;
    	
    	len = strlen(flag);
    	tlen = strlen(t);
    	IndexArr = malloc(len);
    	memset(IndexArr, 0, len);
    	
    	for (i = 0; i < len; i++)
    	{
    		//用简单的查找,复杂的也行
    		for (j = 0; j < tlen; j++)
    		{
    			if (flag[i] == t[j])
    			{
    				IndexArr[i] = j;
    				break;
    			}
    		}
    	}
    	free(flag);
    	return IndexArr;
    	
    }
    
    char* BASE8_enc(char* flag)
    {
    	char t[] = "01234567";
    	int len2 = 0;
    	int len1 = strlen(flag);
    	if (len1 * 8 % 3 != 0)
    	{
    		len2 = len1 * 8 / 3 + 1;
    	}
    	else
    	{
    		len2 = len1 * 8 / 3;
    	}
    	return Index_to_table(bin_to_num(num_to_bin(flag, len1, 8), len2, 3), len2,t);
    }
    char* BASE8_dec(char* flag)
    {
    	char t[] = "01234567";
    	int len1 =0;//结果可能不一定是3的倍数,但是转化后,不一定是8的倍数
    	int len2 = 0;
    	len1 = strlen(flag);
    	len2 = len1 * 3 / 8;
    	
    	/*
    	1.把字符串3个3个的凑合
    	2.把字符串8个8个的取出,成为index
    	3.对于8个8个的取出,多余的不用管,那些是之前填充的
    	*/
    	return bin_to_num(num_to_bin(Table_to_index(flag,t),len1,3),len2,8);
    }
    char* BASE64_enc(char* flag)
    {
    	char t[]= "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    	int len2 = 0;
    	int len1 = strlen(flag);
    	if (len1 * 8 % 6 != 0)
    	{
    		len2 = len1 * 8 / 6 + 1;
    	}
    	else
    	{
    		len2 = len1 * 8 / 6;
    	}
    	return Index_to_table(bin_to_num(num_to_bin(flag, len1, 8), len2, 6), len2, t);
    }
    char* BASE64_dec(char* flag)
    {
    	char t[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    	int len1 = 0;//结果可能不一定是3的倍数,但是转化后,不一定是8的倍数
    	int len2 = 0;
    	len1 = strlen(flag);
    	len2 = len1 * 6 / 8;
    
    	/*
    	1.把字符串3个3个的凑合
    	2.把字符串8个8个的取出,成为index
    	3.对于8个8个的取出,多余的不用管,那些是之前填充的
    	*/
    	return bin_to_num(num_to_bin(Table_to_index(flag, t), len1, 6), len2, 8);
    }
    void test8()
    {
    	char flag[] = "D0g3{12345}";
    	char* enc = 0;
    	char* dec = 0;
    	enc = BASE8_enc(flag);
    	printf("%s\n", enc);
    	dec = BASE8_dec(enc);
    	printf("%s\n", dec);
    	free(dec);
    }
    void test64()
    {
    	char flag[] = "D0g3{awdhoischdbiurhqoemcas}";
    	char* enc = 0;
    	char* dec = 0;
    	enc = BASE64_enc(flag);
    	printf("%s\n", enc);
    	dec = BASE64_dec(enc);
    	printf("%s\n", dec);
    	free(dec);
    }
    int main()
    {
    	test64();
    	return 0;
    }
    ```
    

# 组合

实现的原理是基于(任意BASE编码)
对二进制流任意取出随机位,取出的个数限于[2,6]
然后形成index,在table里面找

- encode
    
    ```c
    #define _CRT_SECURE_NO_WARNINGS
    #include<stdio.h>
    #include<stdlib.h>
    #include<string.h>
    #include<time.h>
    /*
    把长度为len的数组转化为二机制流,长度会多那么一位
    */
    char* num_to_bin(char* flag, int len, int max)
    {
    	char sz_tmp[8 + 1];
    	int i = 0, j = 0;
    
    	int round = 0;
    	int dw_tmp = 0;
    	char* bin_sz = 0;
    	sz_tmp[8] = 0;
    
    	bin_sz = malloc(len * max + max + 1);
    	memset(bin_sz, '0', len * max + max);
    	bin_sz[len * max + max] = 0;
    	for (i = 0; i < len; i++)//这里是实际的转化为二进制字符串,而不是理论的
    	{
    		dw_tmp = flag[i];
    		for (j = max - 1; j >= 0; j--)
    		{
    			sz_tmp[j] = dw_tmp % 2 + '0';
    			dw_tmp /= 2;
    		}
    		memcpy(&bin_sz[i * max], sz_tmp, max);
    	}
    	return bin_sz;
    }
    /*
    把二进制流按照间距max取出来,转化为数字,形成新的数组,数组长度为len
    */
    char* bin_to_num_enc(char* bin_sz, int len)//字符串长度/8,就是
    {
    	//每次读取4位
    	int i = 0, j = 0, k = 0;
    	char* flag = 0;
    	int max = 0;
    	flag = malloc(len * 6);//
    	memset(flag, 0, len * 6);
    	srand(time(NULL));
    	for (i = 0; i < len * 8; )
    	{
    		max = rand() % 4 + 3;//3,4,5,6
    		for (j = 0; j < max; j++)
    		{
    			flag[2 * k + 0] = flag[2 * k + 0] * 2 + (bin_sz[i + j] - '0');
    		}
    		flag[2 * k + 1] = max + 17;
    		i += max;
    		k++;
    	}
    	free(bin_sz);
    	flag[2 * k + 0] = 0xff;
    	return flag;
    }
    char* Index_to_table(unsigned char* IndexArr, int len, char* t)
    {
    	int i = 0;
    	char* lp_ret = 0;
    	lp_ret = malloc(len * 6);
    	memset(lp_ret, 0, len * 6);
    	for (i = 0; IndexArr[i] != 0xff; i++)
    	{
    		lp_ret[i] = t[IndexArr[i]];
    	}
    	free(IndexArr);
    	return lp_ret;
    }
    char* BASE_Rand_enc(char* flag)
    {
    	char t[] = "012ABCDEF345GHIJKLMNOPQRSTUVWXYZ9abcdefghijkl678mnopqrstuvwzyz#$";
    	int len = strlen(flag);
    	char* tmp = 0;
    	tmp = num_to_bin(flag, len, 8);
    	tmp = bin_to_num_enc(tmp, len);
    	tmp= Index_to_table(tmp, len, t);
    	return tmp;
    }
    int main()
    {
    	char flag[] = "D0g3{qwer5008}";
    	char* enc = 0;
    	enc = BASE_Rand_enc(flag);
    	printf("%s\n", enc);
    	return 0;
    }
    ```
    
- decode
    
    ```c
    #define _CRT_SECURE_NO_WARNINGS
    #include<stdio.h>
    #include<stdlib.h>
    #include<string.h>
    #include<time.h>
    //
    /*
    把长度为len的数组转化为二机制流,长度会多那么一位
    */
    char* num_to_bin(char* flag, int len)
    {
    
    	char* bin_sz = 0;
    	int i = 0, j = 0, k = 0;
    	int max = 0;
    	unsigned int dw_tmp = 0;
    	char sz_tmp[6 + 1];
    	bin_sz = malloc(len * 6);//每一个最多6个1
    	memset(bin_sz, 0, len * 6);
    
    	for (i = 0; i < len; i++)//这里是实际的转化为二进制字符串,而不是理论的
    	{
    		dw_tmp = flag[2 * i + 0];
    		max = flag[2 * i + 1] - 17;
    		for (j = max - 1; j >= 0; j--)
    		{
    			sz_tmp[j] = dw_tmp % 2 + '0';
    			dw_tmp /= 2;
    		}
    		memcpy(&bin_sz[k], sz_tmp, max);
    		k += max;
    	}
    	return bin_sz;
    }
    /*
    把二进制流按照间距max取出来,转化为数字,形成新的数组,数组长度为len
    */
    char* bin_to_num(char* bin_sz, int len, int max)//字符串长度/8,就是
    {
    	//每次读取4位
    	int i = 0, j = 0;
    	char* flag = 0;
    	flag = malloc(len);//
    	memset(flag, 0, len);
    
    	for (i = 0; i < len; i++)
    	{
    		for (j = 0; j < max; j++)
    		{
    			flag[i] = flag[i] * 2 + (bin_sz[max * i + j] - '0');
    		}
    	}
    	flag[i] = 0;
    	return flag;
    }
    /*
    * 根据表查索引值
    */
    unsigned char* Table_to_index(char* flag, char* t)
    {
    	int len = 0;
    	int tlen = 0;
    	unsigned char* IndexArr = 0;
    	int i = 0;
    	int j = 0;
    
    	len = strlen(flag);
    	tlen = strlen(t);
    	IndexArr = malloc(len);
    	memset(IndexArr, 0, len);
    
    	for (i = 0; i < len; i++)
    	{
    		//用简单的查找,复杂的也行
    		for (j = 0; j < tlen; j++)
    		{
    			if (flag[i] == t[j])
    			{
    				IndexArr[i] = j;
    				break;
    			}
    		}
    	}
    	return IndexArr;
    
    }
    char* BASE_Rand_dec(char* flag)
    {
    	char	t[] = "012ABCDEF345GHIJKLMNOPQRSTUVWXYZ9abcdefghijkl678mnopqrstuvwzyz#$";
    	int		len = strlen(flag);
    	char* tmp1 = 0;
    	tmp1 = Table_to_index(flag, t);
    	tmp1 = num_to_bin(tmp1, len / 2);//一半是记录,一半是确切数据
    	len = strlen(tmp1);
    	tmp1 = bin_to_num(tmp1, len/8, 8);
    	return tmp1;
    }
    int main()
    {
    	char flag[] = "D0g3{qwer5008}";
    	char data[] = "LRAR1RBOIPGQtR6RDOCQHPsR2OCOTQDRCODQ1RBOARFPEPDOBO";
    	char* dec = 0;
    	dec = BASE_Rand_dec(data);
    	printf("%s\n", dec);
    	return 0;
    }
    
    /*
    * flag转index
    * 数字转二进制,二进制转化为几位,看具体
    * 二进制抓数字,8
    */
    ```