总体来说,就是一次性加密8字节,8x8=64

然后对这8字节做一些运算

如果该值>0.那么value=value<<1;

如果该值<0.那么value=(value<<1)^key;

这个key可以是任意值,唯一的要求就是bit最后一位是1

否则的话,,解密将是未知数





具体代码残开./res

先看一下加密代码.还是很简单的

```c
    unsigned char flag[48] = { 0 };
    long long* lp;
    long long mem_dq;
    int i,j;

    memcpy(flag, "flag{6ff29390-6c20-4c56-ba70-a95758e3d1f8}", 42);

    for (i = 0; i < 6; i++)//
    {
        lp = &flag[i * 8];
        mem_dq = *lp;
        for (j = 0; j < 64; j++)
        {
            if (mem_dq >= 0)                      
            {
                mem_dq = mem_dq << 1;
            }
            else 
            {
                mem_dq = (mem_dq << 1) ^ dq_key;
            }
        }
        *lp = mem_dq;
    }
```

每次获取8字节,然后对这8字节做一个运算

其中只得注意的是

如果左移的话,那么bit位最右边就会是0

如果是正数左移,那么还原就直接右移

如果是负数左移然后异或? 那就异或再右移,并且把符号位给补上去.

问题来了??? 如何判断加密的结果它之前是正数还是负数呢??

这就是key的作用了

对于负数而言` mem_dq = (mem_dq << 1) ^ dq_key;`

最后一个bit位是0,然后异或上key的最后一个bit位1,结果就是1

对于正数而言.它的最后一个bit为一直是0,没做过比较

于是我们就可以通过最后一个bit位来判断,那个值以前是负数还是正数了

于是解密如下

```c
    BYTE flag[48] = { 
    0x5B,0xF6,0xEF,0x68,0x8A,0x8A,0xA9,0x38,
    0x62,0x53,0x86,0xB1,0x63,0xFF,0xEA,0x03,
    0x71,0x00,0x5D,0x69,0xC7,0x54,0x67,0x0B,
    0xB4,0x40,0xBA,0x27,0xBF,0x32,0xD5,0xC7,
    0xE1,0xBF,0x09,0xD0,0xD8,0xBF,0xF8,0xC9,
    0xF8,0x4F,0x4D,0xB5,0x95,0x01,0x00,0x00,
     
    };
    __int64 p;
    int j, i;
    for (i = 0; i < 6; i++) 
    {
        p = *((__int64*)&flag[i * 8]);
        for (j = 0; j < 64; j++) 
        {
            if (p & 1) //其实这就是根据某个特征来的
            {
                p = ((unsigned __int64)p ^ dq_key) >> 1;
                p |= 0x8000000000000000;//还原那个符号位1 

            }
            else
            {
                p = (unsigned __int64)p >>1;
            }
        }
        *((__int64*)&flag[i * 8])=p;
    }
```

