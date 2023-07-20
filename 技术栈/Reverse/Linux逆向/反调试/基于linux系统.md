# 基于linux系统

# 调试端口监测

原理： 调试器远程调试时，会占用一些固定的端口号。

做法： 读取/proc/net/tcp，查找IDA远程调试所用的23946端口，若发现说明进程正在被IDA调试。

也可以运行netstat -apn结果中搜索23946端口

```
 void CheckPort23946ByTcp(){
     FILE* pfile=NULL;
     char buf[0x1000]={0};
// 执行命令
     char* strCatTcp= "cat /proc/net/tcp |grep :5D8A";
//char* strNetstat="netstat |grep :23946";
     pfile=popen(strCatTcp,"r");
     if(NULL==pfile) {
         LOGA("CheckPort23946ByTcp popen打开命令失败!\n");
         return;
     }
 
// 获取结果
     while(fgets(buf,sizeof(buf),pfile))  {
// 执行到这里，判定为调试状态
         LOGA("执行cat /proc/net/tcp |grep :5D8A的结果:\n");
         LOGB("%s",buf);
     }//while
     pclose(pfile);
 }
```

# 调试器进程名检测

原理：远程调试要在手机中运行android_server gdbserver gdb等进程。

做法： 遍历进程，查找固定的进程名，找到说明调试器在运

```
 void SearchObjProcess() {
     FILE* pfile=NULL;
     char buf[0x1000]={0};
// 执行命令
//pfile=popen("ps | awk '{print $9}'","r"); // 部分不支持awk命令
     pfile=popen("ps","r");
     if(NULL==pfile) {
         LOGA("SearchObjProcess popen打开命令失败!\n");
         return;
     }
 
// 获取结果
     LOGA("popen方案:\n");
     while(fgets(buf,sizeof(buf),pfile)) {
// 打印进程
         LOGB("遍历进程:%s\n",buf);
// 查找子串
         char* strA=NULL,strB=NULL,strC=NULL,strD=NULL;
         strA=strstr(buf,"android_server");
         strB=strstr(buf,"gdbserver");
         strC=strstr(buf,"gdb");
         strD=strstr(buf,"fuwu");
         if(strA || strB ||strC || strD)  {
// 执行到这里，判定为调试状态
             LOGB("发现目标进程:%s\n",buf);
         }//if
     }//while
     pclose(pfile);
 }
```

# 父进程名检测

原理： 有的时候不使用apk附加调试的方法进行逆向，

而是写一个.out可执行文件直接加载so进行 调试，

这样程序的父进程名和正常启动apk的父进程名是不一样的。

测试发现：

（1）正常启动的apk程序：父进程是zygote

（2）调试启动的apk程序：在AS中用LLDB调试发现父进程还是zygote

（3）附加调试的apk程序：父进程是zygote

（4）vs远程调试 用可执行文件加载so:父进程名为gdbserver 结论：父进程名非zygote的，判定为调试状态。

读取/proc/pid/cmdline，查看内容是否为zygote

```
 void CheckParents()
 {
///////////////////
// 设置buf
     char strPpidCmdline[0x100]={0};
     snprintf(strPpidCmdline, sizeof(strPpidCmdline), "/proc/%d/cmdl ine", getppid());
// 打开文件
     int file=open(strPpidCmdline,O_RDONLY);
     if(file<0) {
         LOGA("CheckParents open错误!\n");
         return;
     }
// 文件内容读入内存
     memset(strPpidCmdline,0,sizeof(strPpidCmdline));
     ssize_t ret=read(file,strPpidCmdline,sizeof(strPpidCmdline));
     if(-1==ret) {
         LOGA("CheckParents read错误!\n");
         return;
     }
 
// 没找到返回0
     char sRet=strstr(strPpidCmdline,"zygote");
     if(NULL==sRet)  {
// 执行到这里，判定为调试状态
         LOGA("父进程cmdline没有zygote子串!\n");
         return;
     }
     int i=0;
     return;
 }
```

# fd文件检测

原理： 根据/proc/pid/fd/路径下文件的个数差异，判断进程状态。 （apk启动的进程和非apk启动的进程fd数量不一样） （apk的debug启动和正常启动，进程fd数量也不一样）

代码： 略

# 检测打开的fd文件

如果被调试的进程是通过gdb 的方式启动，那么它便是由gdb进程fork得到的。而fork在调用时，

父进程所拥有的fd(file descriptor)会被子进程继承。由于gdb在往往会打开多个fd，因此如果进程拥有的fd较多，

则可能是继承自gdb的，即进程在被调试。具体地，进程拥有的fd会在/proc/self/fd/下列出

```
 int CheckDbg3()
 {
    struct dirent *dir;
    DIR *d = opendir("/proc/self/fd");
    while(dir=readdir(d))
     {
     puts(dir->d_name);
        if(!strcmp(dir->d_name, "4"))
        {
            printf("Check fd Debugger detected\n");
            return 1;
        }
        if(!strcmp(dir->d_name, "5"))
        {
            printf("Check fd Debugger detected\n");
            return 1;
        }
     }
    closedir(d);
    printf("Check fd All good\n");
    return 0;
 }
 
```

# 运行时间的检测

```
 #include<signal.h>
 void alarmHandler(int sig)
 {
    printf("Debugger detected");
    exit(1);
 }
 void __attribute__((constructor))setupSig(void)
 {
    signal(SIGALRM, alarmHandler);
    alarm(2);
 }
```

# 调试器名字的检测

```
 int  CheckDbg1()
 {
     char buf0[32], buf1[128];
     FILE* fin;
 
     snprintf(buf0, 24, "/proc/%d/cmdline", getppid());
     fin = fopen(buf0, "r");
     fgets(buf1, 128, fin);
     fclose(fin);
 
//puts(buf1);
     if(!strcmp(buf1, "gdb"))
     {
         printf("Find  gdb\n");
//abort();
     }
//puts(buf1);
     if(!strcmp(buf1, "./linux_server64"))
     {
         printf("Find Romote Server Dbg\n");
//abort();
     }
//puts(buf1);
     if(!strcmp(buf1, "./64"))
     {
         printf("Find Romote Server Dbg\n");
//abort();
     }
//
     printf("Check All good\n");
     return 0;
 }
```

# 检测运行tracerPID状态的检测

```
 int CheckDbg2()
 {
     int i;
     char buf1[512];
     FILE* fin;
     fin = fopen("/proc/self/status", "r");
     int tpid;
     const char *needle = "TracerPid:";
     size_t nl = strlen(needle);
     while(fgets(buf1, 512, fin))
     {
         if(!strncmp(buf1, needle, nl))
         {
             sscanf(buf1, "TracerPid: %d", &tpid);
             if(tpid != 0)
             {
                 printf("Check File Record You are Dbg me:%08X\n",tpid);
                 return 1;
             }
         }
     }
     fclose(fin);
     printf("Check All good\n");
     return 0;
 }
```