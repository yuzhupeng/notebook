#include<stdio.h>
#include<stdlib.h>
#include<string.h>


#define block 0 //障碍物品
#define empty 1	//空地


#define notvisted 0 //没有访问
#define visted 1 //访问了

#define map_dest 4
#define map_src 3

struct ndoe {
	char d;
	struct ndoe* next;
};
struct nstack {
	int cnt;//栈个数
	struct ndoe* nesp;//栈顶部
};
struct mmap {
	int* data;
	int hang;
	int lie;
};

int min = 0x7fffffff;
struct nstack mstack;
int maze_org[3][15][15] = {
	1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	1, 1, 1, 1, 1, 0, 3, 1, 1, 0, 0, 0, 0, 0, 0,
	1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0,
	1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0,
	1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0,
	1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0,
	1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0,
	1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0,
	1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0,
	1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	1, 1, 0, 3, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,
	1, 1, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0,
	1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0,
	1, 1, 0, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0,
	1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0,
	1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0,
	1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0,
	1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0,
	1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0,
	1, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 0,
	1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
	1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 3, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
	0, 1, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0,
};
int maze_visted[15][15];
int dx[] = { 1,0,-1,0 };
int dy[] = { 0,1,0,-1 };
char dxy[] = "dsaw";

/*
* 不同于以前的链表,是 头->节点-> 这样的方向建立,于是可以从头部访问到尾部
* 这个链表根据需求,存储方向是 头<- 节点<-节点<-节点
* 功能: 把成员压入栈
* 参数1: 栈结构体
* 参数2: 压入的元素
*/
void nodePush(struct nstack* mystack, char data)
{
	struct ndoe* tmp = malloc(sizeof(struct ndoe));
	tmp->d = data;
	tmp->next = mystack->nesp;
	mystack->nesp = tmp;
	mystack->cnt++;
}
/*
* 功能: 常规的出战操作
* 参数: 栈结构体
*/
void nodePop(struct nstack* mystack)
{
	struct ndoe* tmp = 0;
	if (mystack->cnt)
	{
		tmp = mystack->nesp->next;//记住下一个成员位置
		free(mystack->nesp);//释放当前
		mystack->nesp = tmp;//esp指向下一个
		mystack->cnt--;//栈成员个数--
	}
}
/*
* 功能: 实现栈成员的遍历
* 关键点是给出尾巴,然后递归到头部,然后去打印
* 参数:栈的esp
*/
void nodeShow(struct ndoe* tmp)
{
	if (tmp == NULL)
	{
		return;
	}
	nodeShow(tmp->next);
	printf("%c", tmp->d);;
}

//搜索路径
/*
功能: 所有可达搜索路径 也可以搜索最短路径
参数1: 地图结构体
参数2: 起点坐标,或者当前坐标
参数3: 走的步数
*/
void dfs(struct mmap* maze, int x, int y, int cnt)//bfs
{
	int hang = maze->hang;
	int lie = maze->lie;

	int next_x = 0;
	int next_y = 0;

	int i = 0;
	if ((maze->data)[x + y * lie] == map_dest)//如果当前这一点是终点
	{
		if (cnt < min)
		{
			nodeShow(mstack.nesp);//那就
			printf("\n");
			min = cnt;//跟新最小值
		}
		return;
	}

	for (i = 0; i < 4; i++)//dsaw
	{
		next_x = x + dx[i];
		next_y = y + dy[i];
		//右  下一步是 空地|终点 && 没被访问
		if (
			((maze->data)[next_x + next_y * lie] == empty || (maze->data)[next_x + next_y * lie] == map_dest)
			&&
			maze_visted[next_y][next_x] == notvisted
			)
		{
			nodePush(&mstack, dxy[i]);//下一步可以走
			maze_visted[next_y][next_x] = 1;//下一步标记被走过
			dfs(maze, next_x, next_y, cnt + 1);
			maze_visted[next_y][next_x] = notvisted;//回溯,看一看其它路径
			nodePop(&mstack);//回溯,看一看其它路径
		}
	}
	return;
}
/*
* 功能: 寻找一个地图的起点
* 参数1: 地图结构体
* 参数2,参数3: 存储起点的指针变量
*/
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

int main()
{
	//初始化地图 行 列 数据

	//初始话起点(y,x)
	struct mmap st_map;
	int x = 0, y = 0;
	int i = 0;

	mstack.cnt = 0;
	mstack.nesp = 0;

	st_map.data = &maze_org;
	st_map.hang = 15;
	st_map.lie = 15;
	for (i = 0; i < 3; i++)
	{
		min = 0x7fffffff;
		st_map.data = &maze_org[i];//给地图数据提供接口
		findStart(&st_map, &x, &y);//找到地图的起点
		dfs(&st_map, x, y, 0);//寻找路径
	}
}