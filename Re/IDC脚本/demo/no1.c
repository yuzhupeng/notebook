//文件名：test.idc
#include <idc.idc>
static patch90(lp_start,plen)
{
    auto i;
    if(Byte(lp_start)==0x90)
    {
        return;
    }
    for(i=0;i<plen;i++)
    {
        PatchByte(lp_start+i,0x90);
    }
    return;
}

static dw_jmp(FBin,start_add,end_addr)//10
{
    auto x,asm_jmp,cnt_patch;

    for (x = FindBinary(start_add,SEARCH_DOWN,FBin);x != BADADDR && x<end_addr ;x = FindBinary(x,SEARCH_DOWN,FBin))
    {
        //同一个起点,,遍历几次
        Message("%x\n",x);
        x=x+10;     
        asm_jmp=Byte(x);
        if(asm_jmp>=0x70&&asm_jmp<=0x7f)
        {
            x=x+1;
            cnt_patch=Byte(x);
            x=x+1;
            Message("(%x)\n",x-12);
            patch90(x-12,cnt_patch+12);
            x=x+cnt_patch;
        }
        else if(asm_jmp==0x0F)
        {
            asm_jmp=Byte(x+1);
            if(asm_jmp>=0x80&&asm_jmp<=0x8f)
            {
                Message("(%x)\n",x-10);
                patch90(x-10,10+6);
            }
        }
        
    }
} 

static w_jmp(FBin,start_add,end_addr)//10
{
    auto x,asm_jmp,cnt_patch;

    for (x = FindBinary(start_add,SEARCH_DOWN,FBin);x != BADADDR && x<end_addr ;x = FindBinary(x,SEARCH_DOWN,FBin))
    {
        //同一个起点,,遍历几次
        Message("%x\n",x);
        x=x+9;
        
        asm_jmp=Byte(x);
        if(asm_jmp>0x70&&asm_jmp<0x7f)
        {
            x=x+1;
            cnt_patch=Byte(x);
            x=x+1;
            Message("(%x)\n",x-11);
            patch90(x-11,cnt_patch+11);
            x=x+cnt_patch;
        }
        else if(asm_jmp==0x0F)
        {
            asm_jmp=Byte(x+1);
            if(asm_jmp>=0x80&&asm_jmp<=0x8f)
            {
                Message("(%x)\n",x-9);
                patch90(x-9,9+6);
            }
        }
    }
}

static b_jmp(FBin,start_add,end_addr)//10
{
    auto x,asm_jmp,cnt_patch;

    for (x = FindBinary(start_add,SEARCH_DOWN,FBin);x != BADADDR && x<end_addr ;x = FindBinary(x,SEARCH_DOWN,FBin))
    {
        //同一个起点,,遍历几次
        Message("%x\n",x);
        x=x+7;
        
        asm_jmp=Byte(x);
        if(asm_jmp>0x70&&asm_jmp<0x7f)
        {
            x=x+1;
            cnt_patch=Byte(x);
            x=x+1;
            Message("(%x)\n",x-9);
            patch90(x-9,cnt_patch+9);
            x=x+cnt_patch;
        }
        else if(asm_jmp==0x0F)
        {
            asm_jmp=Byte(x+1);
            if(asm_jmp>=0x80&&asm_jmp<=0x8f)
            {
                Message("(%x)\n",x-7);
                patch90(x-7,7+6);
            }
        }
    }
} 
static main()
{
    
    Message("|---- start ---- |\n");
    auto start_addr= 0x10001000;
    auto end_addr  = 0x1001d000;

    b_jmp("F6 05",start_addr,end_addr);
    b_jmp("80 3D",start_addr,end_addr);
    w_jmp("66 F7 05",start_addr,end_addr);
    w_jmp("66 81 3D",start_addr,end_addr);
    dw_jmp("81 3D",start_addr,end_addr);
    dw_jmp("F7 05",start_addr,end_addr);
    
    Message("|----  end  ---- |\n");
}