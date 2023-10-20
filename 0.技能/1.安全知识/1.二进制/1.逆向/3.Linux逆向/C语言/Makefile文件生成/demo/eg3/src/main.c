#include <stdio.h>
#include "../include/add.h"
#include "../include/sub.h"
int main()
{
    int x=70,y=40;
    printf("a+b=%d\n",fAdd(x,y));
    printf("a-b=%d\n",fSub(x,y));
    return 0;
}