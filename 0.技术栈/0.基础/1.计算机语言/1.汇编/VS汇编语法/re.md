# MASM x64 汇编语法

Microsoft Macro Assembler 的汇编语法

关于如何配置x64调用汇编,不在解释

x64貌似不能内联汇编,只能把汇编代码块封转为一个函数,然乎去调用

最基本的函数

```nasm
.code

_sum PROC
    mov rax, rcx ; 参数1 函数调用约定
    add rax, rdx ; 参数2 函数调用约定
    ret
_sum ENDP

END
```

调用它

```nasm
#define  _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <stdio.h>
#include "head.h"
int main()
{   
	printf("%d", _sum(1,2));
	return 0;
}
```



# 汇编文件引用C语言变量

```nasm
.data
extern gvar:QWORD

.code
_sum PROC
    jmp qword ptr [gvar];
_sum ENDP

END
```

大概就是这个样子

另外一个C语言文件的全局变量大概张这个样子

```nasm
#define  _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <stdio.h>
#include "head.h"

void msg1();
void msg2();
DWORD64 gvar;
BYTE* getReallCall(BYTE* lp)
{
	return  *(DWORD*)(lp + 1) + (BYTE*)lp + 5;
}
void msg1()
{
	MessageBoxA(0, "org", "(:", 0);
	return;
}
void msg2()
{
	MessageBoxA(0, "hook", "(:", 0);
	msg1();
	return;
}
int main()
{
	gvar = getReallCall(msg2);
	_sum();
	return 0;
}
```