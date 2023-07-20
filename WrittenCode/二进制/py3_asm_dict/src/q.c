
#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<windows.h>
struct PY3AsmDict
{
	unsigned char ID;
	char* name;
};

struct PY3AsmDict* orgDict;
struct PY3AsmDict* sortDict;
char* FileContent;
void	Init_Sort(char*);
int		TellFileSize(char* );
void	bubble_sort(struct PY3AsmDict* , int );
void	JustOK();
void	FindID(int*);
void	FindName(char* );
int		Bin_Search(struct PY3AsmDict* , int , char* );
int		DealInput(char* input);
int main()
{
	char name[] = "PYAsm3089.dit";
	Init_Sort(name);
	JustOK();
	free(orgDict);
	free(sortDict);
	return 0;
}
void Init_Sort(char* name)
{
	char* Local_FileContent = 0;
	struct PY3AsmDict *Dict_Arr= 0;
	
	FILE* fp = 0;
	int  FileSize = 0;
	int i = 0;
	int len_Dict = 0;
	
	FileSize = TellFileSize(name);
	
	len_Dict = 260 * sizeof(struct PY3AsmDict);

	Dict_Arr = (struct PY3AsmDict*)malloc(len_Dict);
	sortDict = Dict_Arr;

	orgDict	  = (struct PY3AsmDict*)malloc(len_Dict);

	Local_FileContent = (char*)malloc(FileSize + 64);
	FileContent = Local_FileContent;

	 
	memset(Local_FileContent, 0, FileSize + 64);
	memset(orgDict, 0, len_Dict);
	memset(Dict_Arr, 0, len_Dict);

	if (Local_FileContent == 0)
		return;
	if (Dict_Arr == 0)
	{
		free(Local_FileContent);
		return;
	}
	if (orgDict == 0)
	{
		free(Local_FileContent);
		free(Dict_Arr);
		return;
	}
	fp = fopen(name, "rb");
	fread(Local_FileContent, FileSize, 1, fp);
	fclose(fp);
	for (i = 0; i < 256; i++)
	{
		Dict_Arr[i].ID = 0;
		Dict_Arr[i].ID = *Local_FileContent;
		Local_FileContent++;
		Dict_Arr[i].name = Local_FileContent;
		Local_FileContent = Local_FileContent+ strlen(Local_FileContent)+1;
	}
 
	memcpy((char*)orgDict, (char*)Dict_Arr, len_Dict);
	bubble_sort(Dict_Arr, 256);
	
	return ;
}
int TellFileSize(char* filename)
{
	int size = 0;
	FILE* fp = fopen(filename, "r");
	if (!fp)
		return -1;
	fseek(fp, 0L, SEEK_END);
	size = ftell(fp);
	fclose(fp);
	return size;
}
void swapt(struct PY3AsmDict* x, struct PY3AsmDict* y)
{
	struct PY3AsmDict tmp={0};
	memcpy(&tmp, x, sizeof(struct PY3AsmDict));
	memcpy(x, y, sizeof(struct PY3AsmDict));
	memcpy(y, &tmp, sizeof(struct PY3AsmDict));
}
void bubble_sort(struct PY3AsmDict* arr, int len)
{
	int i=0, j=0;
	int cp = 0;
	for (i = 0; i < len - 1; i++)
		for (j = 0; j < len - 1 - i; j++)
		{
			cp = strcmp(arr[j].name, arr[j + 1].name);
			if (cp==1)//da
			{
				swapt(&arr[j], &arr[j + 1]);
			}
		}
}
void JustOK()
{
	int choose=0;
	char Input[64] = { 0 };
label_2:
	puts("*******************************************");
	puts("       [In] x90        == 16进制ID         ");
	puts("       [In] 90         == 10进制ID         ");
	puts("       [In] LOAD_CONST == 字符串指令        ");
	puts("        w:清屏          q:退出              ");
	puts("*******************************************");
	while (1)
	{
		printf("[In]:  ");
		memset(Input, 0, 64);
		scanf("%s", Input);
		choose = DealInput(Input);
		switch (choose)
		{
		case 1:
			FindID((int*)Input);
			break;
		case 2:
			FindName(Input);
			break;
		case 3:
			system("cls");
			goto label_2;
		case 4:
			goto label_1;
		default:
			break;
		}
	}
	label_1:
	return;
}
void  FindID(int * ID)
{
	int tmp = *ID;
	if (tmp > 0xFF || tmp < 0)
	{
		printf("[Out]: 0x%08X %08d ID Error\n", tmp, tmp);
		return;
	}
		
	if (orgDict[tmp].name[0] == 0)
	{
		printf("[Out]: 0x%08X %08d ID Have No Name\n", tmp, tmp);
		return;
	}
	printf("[Out]: 0x%02X %02d %s\n", tmp, tmp,orgDict[tmp].name);
	return ;
}
void FindName(char *name)
{
	int result = 0;
	result=Bin_Search(sortDict, 256, name);
	if (result == -1)
		puts("[Out]: No Answer");
	else
		printf("[Out]: 0x%02X %02d %s \n", orgDict[result].ID, orgDict[result].ID,orgDict[result].name);
	return ;
}
int Bin_Search(struct PY3AsmDict* sodt_Dict, int cnt, char* OpcodeName)
{
	int i = 0;
	int first = 0;
	int last = cnt - 1;
	int mid = 0;
	int counter = 0;
	int cp = 0;
	while (first <= last)
	{
		mid = (first + last) / 2;
		cp = strcmp(sodt_Dict[mid].name, OpcodeName);
		if (cp == 1)//dayu
		{
			last = mid - 1;
		}
		else if (cp == -1)//xiaoyu
		{
			first = mid + 1;
		}
		else
		{
			return sodt_Dict[mid].ID;
		}
	}
	return -1;
}
int DealInput(char* input)
{
	int id = 0;
	if (input[0] == 'x')
	{
		sscanf(input + 1, "%x", &id);
		memset(input, 0, 64);
		memcpy(input, &id, 4);
		return 1;
	}
	else if (input[0] >= '0' && input[0] <= '9')
	{
		sscanf(input, "%d", &id);
		memset(input, 0, 64);
		memcpy(input, &id, 4);
		return 1;
	}
	else if (input[0] == 'q')
		return 4;
	else if (input[0] == 'w')
		return 3;
	else
		return 2;
}