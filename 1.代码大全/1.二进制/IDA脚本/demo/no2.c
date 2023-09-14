//进入每个函数, 每个函数的间距是一样的
//读取指定范围, 每个范围是一样的
//读取指定数组, 异或打出

#include <idc.idc>
static functionDump(start_func_addr)//一开始
{

  	auto i=0,j=0;
  	auto tmp,tmp2=0;
  	auto dw_Arr_xor;
  	auto dw_cur_Arr;
  	auto wsad;
  	auto funcName = get_func_name(start_func_addr);
    auto fd = fopen("maze.txt", "a+");
    auto mapx,mapy;
    
  	funcName="BYTE "+funcName+"_[2+4+625]={\n";
  	writestr(fd,funcName);
  	Message("%s",funcName);
  	
    mapx= Dword(start_func_addr+10596);
    mapy= Dword(start_func_addr+10588);
    fprintf(fd, "%d,%d//起点(x,y)\n",mapx,mapy);
    
  	wsad=Dword(start_func_addr+10602)+(start_func_addr+10606);
    fprintf(fd, "0x%X,0x%X,0x%X,0x%X,//adws 对应4个方向\n",Byte(wsad),Byte(wsad+1),Byte(wsad+2),Byte(wsad+3));
    
    
  	dw_Arr_xor=Dword(start_func_addr+10880)+(start_func_addr+10884);//4字节偏移+x64地址
	dw_cur_Arr=start_func_addr+11;//跨度14
	
  	for(i=0;i<25;i=i+1)
  	{
  		for(j=0;j<25;j=j+1)
  		{
  			tmp=Dword(dw_cur_Arr)^Dword(dw_Arr_xor);
  			if(tmp2<15)//在15的时候,下一次就得+17
  			{
				dw_cur_Arr=dw_cur_Arr+14;
  			}
  			else
  			{
				dw_cur_Arr=dw_cur_Arr+17;
  			}
  			
  			dw_Arr_xor=dw_Arr_xor+4;
  			fprintf(fd, "0x%X,",tmp);
  			tmp2=tmp2+1;
  			Message("0x%X,",tmp);
  		}
  		fputc('\n',fd);
  		Message("\n");
  	}
  	writestr(fd,"};\n");
    fclose(fd);
}
static findMap(start_addr)
{
    auto i;
    for(i=0;i<2;i=i+1)
    {
        functionDump(start_addr);
        start_addr=start_addr+10944;
    }
}
static main()
{
    Message("|---- work ----|\n");
    findMap(0x0000000140001460);//未运行前的地址,第一个迷宫函数地址
    Message("|---- end  ----|\n");
}