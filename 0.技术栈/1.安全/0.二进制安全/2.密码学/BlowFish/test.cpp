#include <Windows.h>
#include <stdio.h>
#include "test.h"

#define unsigned char byte

//blow的密钥长度为动态的  64bit - 448bit
//blowfish有两个盒 unsigned long pbox[18] 和 unsigned long sbox[4][256] 总计是4168个字节 

/*
算法流程如下
1.根据key对 p_box 和 s_box 进行变换
	1.1 用π的小数部分填充p_box和s_box 这里在文件头中已经给出
	1.2 用key 以每64bit为一个单位 进行异或操作，当密钥不足的时候可以轮用密钥
	1.2 用BF_Fn函数加密一个全0的64bit信息
	1.3 输出的64位填充到pBox和sBox
2.加密密文每64bit(8字节)
	2.1将64bit 拆成左右32bit
	2.2进行BF_Fn()变换
*/

DWORD valueMap(DWORD leftPart)
{
	DWORD value;

	//查表操作 每一个字节查一次表 然后进行加和异或操作
	value = s_box[0][leftPart >> 24] + s_box[1][(leftPart >> 16) & 0xff];
	value ^= s_box[2][(leftPart >> 8) & 0xff];
	value += s_box[3][leftPart & 0xff];

	return value;
}

//BlowFish的主加密函数 它对明文的加密和对密钥的变换都是利用这个 迭代变换16轮
//inline void BF_Fn(DWORD* leftPart, DWORD* rightPart)
void BF_Fn(DWORD* L, DWORD* R)
{
	int i;
	DWORD temp;

	for (i = 0; i <8; i ++) {
		*L ^= p_box[2*i + 0];
		*R ^= valueMap(*L);
		*R ^= p_box[2*i + 1];
		*L ^= valueMap(*R);
		//printf("%08X %08X\n", *L, *R);
	}

	*L ^= p_box[16];
	*R ^= p_box[17];

	//最后交换一下
	temp = *L;
	*L = *R;
	*R = temp;

}

void BF_Fn_dec(DWORD* L, DWORD* R)
{
	int i;
	DWORD temp;

	//最后交换一下
	temp = *L;
	*L = *R;
	*R = temp;

	*L ^= p_box[16];
	*R ^= p_box[17];

	for (i = 7; i >= 0; i --) {
		//printf("%08X %08X\n", *L, *R);
		temp = p_box[2 * i + 0];
		*L ^= valueMap(*R) ^ temp;
		*R ^= valueMap((*L)^ temp) ^ p_box[2 * i + 1];
		
	}
}

//对pBox和sBox进行变换
void ExchangeBox(byte* key, int keyLen)
{
	int i, j;
	//int keyLen = strlen((const char*)key);//获取key长度
	DWORD* keyLong = NULL;
	DWORD leftPart = 0, rightPart = 0;//得到共计64bit的左右两部分
	keyLong = (DWORD*)malloc(keyLen * sizeof(byte));//获取应得的空间
	memset(keyLong, 0,keyLen * sizeof(byte));
	keyLen = keyLen / sizeof(DWORD);

	printf("KeyLen : %d\n", keyLen);
	printf("Key : ");
	for (i = 0; i < keyLen; i++) {
		keyLong[i] = _byteswap_ulong(*((DWORD*)key + i));
		printf("0x%x\t", keyLong[i]);
	}
	printf("\n");

	for (i = 0; i < 18; i++) {//进行异或
		p_box[i] ^= keyLong[i % keyLen];
	}

	leftPart = rightPart = 0;//产生一个64位全0数据
	for (i = 0; i < 18; i += 2) {//变换pBox
		BF_Fn(&leftPart, &rightPart);
		p_box[i] = leftPart;
		p_box[i + 1] = rightPart;
	}

	for (i = 0; i < 4; i++) {//变换sBox
		for (j = 0; j < 256; j += 2) {//256 / 2 == 128
			BF_Fn(&leftPart, &rightPart);
			s_box[i][j] = leftPart;
			s_box[i][j + 1] = rightPart;
		}
	}

}

//p_box s_box
void BlowFish(byte* key, byte* data,int dataLen,int keyLen,int is_enc)
{
	int i;
	//int dataLen; 

	byte* lp_flag = data;//建立一个data副本 方便进行指针偏移
	DWORD leftPart, rightPart;

	ExchangeBox(key,keyLen);
	//dataLen = strlen((const char*)data) / (sizeof(DWORD) * 2);//获取data长度

	//至此就可以加密明文了 一次加密2 * 4字节大小
	for (i = 0; i < dataLen; i++) {
		//一次性加密8字节
		leftPart = _byteswap_ulong(*((DWORD*)lp_flag));
		rightPart = _byteswap_ulong(*((DWORD*)lp_flag + 1));

		if (!is_enc) {
			BF_Fn(&leftPart, &rightPart);
		}
		else {
			BF_Fn_dec(&leftPart, &rightPart);
		}	
		*((DWORD*)lp_flag) = _byteswap_ulong(leftPart);
		*((DWORD*)lp_flag + 1) = _byteswap_ulong(rightPart);
		lp_flag += sizeof(DWORD) * 2;//指向下一个数据块
	}

	for (i = 0; i < dataLen*8; i++) {
		printf("%c", data[i]);
	}
	return;
}
  
int main()
{
	//BYTE data[] = "12345678";
	BYTE data[] = { 0x14,0xbc,0xd3,0xca,0x57,0x27,0x5a,0x52, };
	BYTE key[] = "12345678";

	BlowFish(key, data, 8 / 8, 8, 1);
	return 0;
}
