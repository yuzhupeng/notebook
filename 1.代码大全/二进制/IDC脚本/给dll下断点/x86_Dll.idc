//文件名：test.idc
#include <idc.idc>
static main()
{
    auto x,y,dll_start,dll_end;
    auto FBin,FBin2;
    dll_start=0x6bfb0000;
    dll_end=0x6bfb0000+0x00020000;
    FBin = "83 BD";
    FBin2 ="55 8B EC";
    Message("|-----BEGIN-----|\n");
    for (x = FindBinary(dll_start,SEARCH_DOWN,FBin);x < dll_end;x = FindBinary(x,0x03,FBin))
    {    
        x=x+6;
        if(Byte(x)<4)
        {         
            x=x+1;
            if(Byte(x)==0x74)
            {
                y = FindBinary(x,SEARCH_UP,FBin2);
                if(y!=0&&y!=-1)
                {
                    Message("%X\n",y);
                }         
               // AddBpt(地址); //添加一个断点
            }
        }
    }
    Message("|------END------|\n");
}