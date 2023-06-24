#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<windows.h>

#define block 0 //障碍物品
#define empty 1	//空地

#define notvisted 0 //没有访问
#define visted 1 //访问了

#define map_src  3
#define map_dest 4


struct node {

	int x, y;//坐标,用于判断所处环境
	int cnt;//当前走了多少步
	char ascii;//走的痕迹dsaw
	//struct node* dsaw[4];//用于记录前驱后继

	struct node* next; //用于队列的存储,和路径没有太大关系
	struct node* back;
};
struct HT
{
	struct node* head;
	struct node* tail;
};
struct mmap {
	int* data;
	int hang;
	int lie;
};


int maze_org [5][4] = {
	3,1,0,1,
	1,1,1,1,
	1,1,0,1,
	1,0,4,1,
	1,1,1,0
};
int maze_visted[5][4];
struct mmap st_map = { NULL,5,4 };

void mpush(struct HT* qHT, int x, int y, int cnt, char ascii)
{

	struct node* newMem = malloc(sizeof(struct node));

	newMem->x = x;
	newMem->y = y;
	newMem->cnt = cnt;
	newMem->ascii = ascii;
	newMem->next = NULL;
	newMem->back = qHT->head;
	if (!qHT->head)//第一次|队首
	{
		qHT->head = newMem;
	}
	if (qHT->tail)//后面几次
	{
		qHT->tail->next = newMem;//数据插入到队尾
	}
	qHT->tail = newMem;//更新队尾的身份
	return;
}
void mpop(struct HT* qHT,int flag)
{
	struct node* lp_tmp = 0;
	if (qHT->head)//非空,大多数情况下都是非空
	{
		lp_tmp = qHT->head->next;//记住下一个节点
		if (flag==0)
		{
			free(qHT->head);//释放当前节点
		}
		qHT->head = lp_tmp;//更新头节点的身份
	}
}

//寻找起点
void findStart(struct mmap* maze, int* x, int* y)
{
	int i = 0, j = 0;
	int hang = maze->hang;
	int lie = maze->lie;
	for (i = 0; i < hang; i++)
	{
		for (j = 0; j < lie; j++)
		{
			if ((maze->data)[i * lie + j] == map_src)
			{
				*x = j;
				*y = i;
				return;
			}
		}
	}
}
void showrRoutine(struct node* tmp_tail)
{
	if (tmp_tail)
	{
		showrRoutine(tmp_tail->back);
		printf("%c", tmp_tail->ascii);
	}
	return;
}
void BFS(struct mmap* maze, int x, int y)//地图,起点
{
	struct HT lht;
	int i = 0;
	int nx = 0, ny = 0;
	int dx[] = { 1,0,-1,0 };
	int dy[] = { 0,1,0,-1 };
	char dxy[] = "dsaw";
	int hang = maze->hang, lie = maze->lie;
	int flag_free = 0;
	lht.head = 0;
	lht.tail = 0;


	mpush(&lht, x, y, 0, 0);
	maze_visted[y][x] = visted;


	while (lht.head)
	{
		flag_free = 0;
		x = lht.head->x;
		y = lht.head->y;
		if ((maze->data)[x + y * lie] == 4)//终点
		{
			printf("%d:", lht.tail->cnt);
			showrRoutine(lht.tail);
		}
		for (i = 0; i < 4; i++)
		{
			nx = x + dx[i];
			ny = y + dy[i];
			if (nx == -1 || ny == -1 || nx == lie || ny == hang)
			{
				continue;
			}
			if (
				((maze->data)[nx + ny * lie] == empty || (maze->data)[nx + ny * lie] == map_dest)
				&&
				maze_visted[ny][nx] == notvisted
				)
			{
				mpush(&lht, nx, ny, lht.head->cnt + 1, dxy[i]);
				flag_free++;
				maze_visted[ny][nx] = visted;
			}
		}
		mpop(&lht,flag_free);
	}
}
int main()
{
	int x = 0, y = 0;//起点
	int i = 0;

	/*for (i = 0; i < 3; i++)
	{*/
		st_map.data = maze_org;
		memset(maze_visted, 0, st_map.hang * st_map.lie * 4);
		findStart(&st_map, &x, &y);
		BFS(&st_map, x, y);
		printf("\n");
	//}

}
