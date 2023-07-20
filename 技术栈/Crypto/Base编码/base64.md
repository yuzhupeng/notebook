# base64

对于二进制流,一次性拿走6位,这4位成为一个[0,63]的数值
我们的标准索引表就是

```c
ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/
```

- C语言大端序编码
    
    ```c
    #define _CRT_SECURE_NO_WARNINGS
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <Windows.h>
    
    void indexFind(BYTE* input, BYTE* t, int ilen, int tlen)
    {
        int i, j;
        for (i = 0; i < ilen; i++)
        {
            for (j = 0; j < tlen; j++)
            {
                if (input[i] == t[j])
                {
                    input[i] = j;
                    break;
                }
            }
            if (j == tlen)
            {
                input[i] = 0;
            }
        }
    }
    BYTE* b64_encode(BYTE* input, BYTE* t64, int ilen, int tlen)//变为4/3
    {
        int i, j;
        BYTE* output = 0;
        output = malloc(ilen * 3);
        memset(output, 0, ilen * 3);
        
        if (output)
        {
            for (i = 0, j = 0; i < ilen; i += 3, j += 4)
            {
                output[j + 0] = t64[input[i + 0] >> 2];//1
                output[j + 1] = t64[(((input[i + 0] & 3) << 4) | (input[i + 1] >> 4))];//1,2
                output[j + 2] = t64[((input[i + 1] & 0xf) << 2) | (input[i + 2] >> 6)];//2,3
                output[j + 3] = t64[input[i + 2] & 0x3f];//3
            }
            switch (ilen % 3)
            {
            case 1:
                output[j - 2] = '=';
                output[j - 1] = '=';
                break;
            case 2:
                output[j - 1] = '=';
                break;
            }
          
            return output;
        }
        else
        {
            exit(-1);
        }
    
    }
    BYTE* b64_decode(BYTE* input, BYTE* t64, int ilen, int tlen)//变为3/4
    {
        int i, j;
        BYTE* dec = malloc(ilen + 16);
        if (dec)
        {
            indexFind(input, t64, ilen, tlen);
            memset(dec, 0, ilen + 16);
            for (i = 0, j = 0; i < ilen; i += 4, j += 3)
            {
                dec[j + 0] = (input[i + 0] << 2) | (input[i + 1] >> 4);
                dec[j + 1] = (input[i + 1] << 4) | (input[i + 2] >> 2);
                dec[j + 2] = (input[i + 2] << 6) | (input[i + 3]);
            }
            return dec;
        }
        else
        {
            exit(-1);
        }
    }
    int main(int argc, BYTE* argv[])
    {
        BYTE t[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
        BYTE input[] = "redq";
        BYTE szEnc[] = "cmVkcQ==";
        BYTE* enc, * dec;
        enc = b64_encode(input, t, 4, 64);//CMvKCxGa
        printf("%s\n", enc);
        dec = b64_decode(szEnc, t, strlen(szEnc), 64);//CMvKCxGa
        printf("%s\n", dec);
       
        return 0;
    }
    ```
    

- C语言小端序编码
    
    ```c
    BYTE* b64_encode(BYTE* input, BYTE* t64, int ilen, int tlen)//变为4/3
    {
        int i, j;
        BYTE* output = 0;
        output = malloc(ilen * 3);
        memset(output, 0, ilen * 3);
    
        if (output)
        {
            for (i = 0, j = 0; i < ilen; i += 3, j += 4)
            {
                output[j + 0] = t64[input[i + 0] &0b00111111];//1
                output[j + 1] = t64[(((input[i + 0] & 0b11000000)) >> 6) | ((input[i + 1] & 0b00001111) << 2)];
                output[j + 2] = t64[((input[i + 1] & 0b11110000) >> 4) | ((input[i + 2] & 0b11) << 4)];
                output[j + 3] = t64[(input[i + 2] & 0b11111100) >> 2];//3
            }
            switch (ilen % 3)
            {
            case 1:
                output[j - 2] = '=';
                output[j - 1] = '=';
                break;
            case 2:
                output[j - 1] = '=';
                break;
            }
    
            return output;
        }
        else
        {
            exit(-1);
        }
    
    }
    ```
    

- Python实现
    
    ```c
    def b64_enc(inputs,b64Table):
    	# 将字符串转化为2进制
    	bin_str = []
    	for i in inputs:
    		x = str(bin(ord(i))).replace('0b', '')
    		bin_str.append('{:0>8}'.format(x))
    	#print(bin_str)
    	# 输出的字符串
    	outputs = ""
    	# 不够三倍数，需补齐的次数
    	nums = 0
    	while bin_str:
    		#每次取三个字符的二进制
    		temp_list = bin_str[:3]
    		if(len(temp_list) != 3):
    			nums = 3 - len(temp_list)
    			while len(temp_list) < 3:
    				temp_list += ['0' * 8]
    		temp_str = "".join(temp_list)
    		#print(temp_str)
    		# 将三个8字节的二进制转换为4个十进制
    		temp_str_list = []
    		for i in range(0,4):
    			temp_str_list.append(int(temp_str[i*6:(i+1)*6],2))
    		#print(temp_str_list)
    		if nums:
    			temp_str_list = temp_str_list[0:4 - nums]
    			
    		for i in temp_str_list:
    			outputs += b64Table[i]
    		bin_str = bin_str[3:]
    	outputs += nums * '='
    	return outputs
    	
    def b64_dec(inputs,b64Table):
    	# 将字符串转化为2进制
    	bin_str = []
    	for i in inputs:
    		if i != '=':
    			x = str(bin(b64Table.index(i))).replace('0b', '')
    			bin_str.append('{:0>6}'.format(x))
    	#print(bin_str)
    	# 输出的字符串
    	outputs = ""
    	nums = inputs.count('=')
    	while bin_str:
    		temp_list = bin_str[:4]
    		temp_str = "".join(temp_list)
    		#print(temp_str)
    		# 补足8位字节
    		if(len(temp_str) % 8 != 0):
    			temp_str = temp_str[0:-1 * nums * 2]
    		# 将四个6字节的二进制转换为三个字符
    		for i in range(0,int(len(temp_str) / 8)):
    			outputs += chr(int(temp_str[i*8:(i+1)*8],2))
    		bin_str = bin_str[4:]	
    	return outputs
    	
    input_str= "redq"
    b64Table =  "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"
    print(b64_enc(input_str,b64Table))
    
    # d=b64_dec(input_str,b64Table)
    # for i in d:
    # 	print(hex(ord(i)),end=',')
    ```
    

对于加密而言

```
        enc[j + 0] = t[src[i + 0] >> 2];
        enc[j + 1] = t[(((src[i + 0] & 0b11) << 4) | (src[i + 1] >> 4))];
        enc[j + 2] = t[((src[i + 1] & 0b1111) << 2) | (src[i + 2] >> 6)];
        enc[j + 3] = t[src[i + 2] & 0b111111];
```

那里的`|`,其实都可以修改为`+`,其实是等价的

如果不等价,那么BASE64就有问题了

这就像0b11110000 | 0b00001111 = 0b11110000 + 0b00001111 不会涉及进位

不涉及进位的二进制加法都可以用`|`来等价代替

解码方法,除了index2table换表以外

index2table是一种基于查询额方法

另外一种就是基于映射的

比如table64是`ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/`

然后编码的时候,映射表

map['A']=0

map['B']=1

map['C']=2

...

于是通过这种映射方式就可以解码,然后再实现bit位的拼接 4*6->3*8