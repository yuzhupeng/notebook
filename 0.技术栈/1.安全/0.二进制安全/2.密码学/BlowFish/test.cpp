#include <Windows.h>
#include <stdio.h>
#include "test.h"

#define unsigned char byte

//blow����Կ����Ϊ��̬��  64bit - 448bit
//blowfish�������� unsigned long pbox[18] �� unsigned long sbox[4][256] �ܼ���4168���ֽ� 

/*
�㷨��������
1.����key�� p_box �� s_box ���б任
	1.1 �æе�С���������p_box��s_box �������ļ�ͷ���Ѿ�����
	1.2 ��key ��ÿ64bitΪһ����λ ����������������Կ�����ʱ�����������Կ
	1.2 ��BF_Fn��������һ��ȫ0��64bit��Ϣ
	1.3 �����64λ��䵽pBox��sBox
2.��������ÿ64bit(8�ֽ�)
	2.1��64bit �������32bit
	2.2����BF_Fn()�任
*/

DWORD valueMap(DWORD leftPart)
{
	DWORD value;

	//������ ÿһ���ֽڲ�һ�α� Ȼ����мӺ�������
	value = s_box[0][leftPart >> 24] + s_box[1][(leftPart >> 16) & 0xff];
	value ^= s_box[2][(leftPart >> 8) & 0xff];
	value += s_box[3][leftPart & 0xff];

	return value;
}

//BlowFish�������ܺ��� �������ĵļ��ܺͶ���Կ�ı任����������� �����任16��
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

	//��󽻻�һ��
	temp = *L;
	*L = *R;
	*R = temp;

}

void BF_Fn_dec(DWORD* L, DWORD* R)
{
	int i;
	DWORD temp;

	//��󽻻�һ��
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

//��pBox��sBox���б任
void ExchangeBox(byte* key, int keyLen)
{
	int i, j;
	//int keyLen = strlen((const char*)key);//��ȡkey����
	DWORD* keyLong = NULL;
	DWORD leftPart = 0, rightPart = 0;//�õ�����64bit������������
	keyLong = (DWORD*)malloc(keyLen * sizeof(byte));//��ȡӦ�õĿռ�
	memset(keyLong, 0,keyLen * sizeof(byte));
	keyLen = keyLen / sizeof(DWORD);

	printf("KeyLen : %d\n", keyLen);
	printf("Key : ");
	for (i = 0; i < keyLen; i++) {
		keyLong[i] = _byteswap_ulong(*((DWORD*)key + i));
		printf("0x%x\t", keyLong[i]);
	}
	printf("\n");

	for (i = 0; i < 18; i++) {//�������
		p_box[i] ^= keyLong[i % keyLen];
	}

	leftPart = rightPart = 0;//����һ��64λȫ0����
	for (i = 0; i < 18; i += 2) {//�任pBox
		BF_Fn(&leftPart, &rightPart);
		p_box[i] = leftPart;
		p_box[i + 1] = rightPart;
	}

	for (i = 0; i < 4; i++) {//�任sBox
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

	byte* lp_flag = data;//����һ��data���� �������ָ��ƫ��
	DWORD leftPart, rightPart;

	ExchangeBox(key,keyLen);
	//dataLen = strlen((const char*)data) / (sizeof(DWORD) * 2);//��ȡdata����

	//���˾Ϳ��Լ��������� һ�μ���2 * 4�ֽڴ�С
	for (i = 0; i < dataLen; i++) {
		//һ���Լ���8�ֽ�
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
		lp_flag += sizeof(DWORD) * 2;//ָ����һ�����ݿ�
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
