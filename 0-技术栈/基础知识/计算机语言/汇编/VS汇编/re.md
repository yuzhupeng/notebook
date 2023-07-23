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



# 调用约定

MASM（Microsoft Macro Assembler）用于编写 x64 架构的程序时，其汇编语法和寄存器调用约定如下：

1. 参数传递：
    - 前四个整型参数使用寄存器传递，按顺序依次是：`RCX`、`RDX`、`R8`、`R9`。
    - 如果有超过四个的整型参数，额外的参数将通过栈传递，从右到左依次压入栈中。
    - 浮点型参数使用 XMM0 到 XMM7 寄存器传递。
2. 返回值：
    - 整型和指针类型的返回值存储在 `RAX` 寄存器中。
    - 浮点型返回值使用 XMM0 寄存器。
3. Callee-Saved 寄存器：
    - `RBX`、`RBP`、`RDI`、`RSI`、`R12`、`R13`、`R14`、`R15` 被称为 Callee-Saved 寄存器，即被调用者保存的寄存器。
    - 在函数内部使用这些寄存器时，需要在函数开头保存其值，在函数末尾恢复其值。
4. 栈帧：
    - 函数调用时，需要在栈上为局部变量和临时数据分配空间，称为栈帧。
    - `RSP`（堆栈指针）寄存器指向栈顶，栈向低地址方向生长。
    - 函数开头通过 `SUB` 指令减少 `RSP` 来为局部变量分配空间，并将旧的 `RBP`（基址指针）值保存在新的栈帧中。
    - 函数末尾通过 `ADD` 指令恢复 `RSP`，并将保存的 `RBP` 值恢复。
    

这和以前的不太一样

1. 参数传递：
    - 前六个整型参数使用寄存器传递，按顺序依次是：RDI、RSI、RDX、RCX、R8、R9。
    - 如果有超过六个的整型参数，额外的参数将通过栈传递，从左到右依次压入栈中。
    - 浮点型参数使用 XMM0 到 XMM7 寄存器传递。
2. 返回值：
    - 整型和指针类型的返回值存储在 RAX 寄存器中。
    - 浮点型返回值使用 XMM0 寄存器。
3. Callee-Saved 寄存器：
    - RBP、RBX、R12、R13、R14、R15 被称为 Callee-Saved 寄存器，即被调用者保存的寄存器。
    - 在函数内部使用这些寄存器时，需要在函数开头保存其值，在函数末尾恢复其值。
4. 栈帧：
    - 函数调用时，需要在栈上为局部变量和临时数据分配空间，称为栈帧。
    - RSP 寄存器指向栈顶。
    - 函数开头通过 SUB 指令减少 RSP 来为局部变量分配空间，并将旧的 RBP 值保存在新的栈帧中。
    - 函数末尾通过 ADD 指令恢复 RSP，并将保存的 RBP 值恢复。
    

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