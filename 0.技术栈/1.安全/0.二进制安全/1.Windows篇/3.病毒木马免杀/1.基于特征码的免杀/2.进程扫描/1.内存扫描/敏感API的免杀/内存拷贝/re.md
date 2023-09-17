比如一些内存拷贝函数 memcpy,CopyMemory

杀软可能会盯的比较紧张

所以





# 高级语言重写





# 汇编重写



也就C语言罢了



## x86



### 使用loop和movsb进行字节拷贝：

```assembly
;Copy Code    
	mov esi, source  ; 源内存地址
    mov edi, destination  ; 目标内存地址
    mov ecx, length  ; 拷贝的字节数

copy_loop:
    movsb  ; 按字节移动数据
    loop copy_loop  ; 循环拷贝，根据ECX寄存器的值判断循环次数
```



### 使用REP 和 movsb 进行连续拷贝：

只不过是对loop的一个更加简洁的表达,自我感觉没有什么区别

```assembly
;Copy Code    
	mov esi, source  ; 源内存地址
    mov edi, destination  ; 目标内存地址
    mov ecx, length  ; 拷贝的字节数

    cld  ; 设置方向标志位为正向
    rep movsb  ; 重复执行MOVSB指令，根据ECX寄存器的值判断循环次数
```

 

### 使用STOSW进行初始化：

换了一个更加复杂的形式

只不过只能用于初始化,不能用于数据拷贝

```assembly
;Copy Code    
	mov edi, destination  ; 目标内存地址
    mov ecx, length  ; 拷贝的字数
    mov ax, value  ; 要存储的字

    cld  ; 设置方向标志位为正向

copy_loop:
    stosw  ; 存储字
    loop copy_loop  ; 循环拷贝，根据ECX寄存器的值判断循环次数
```



### 使用REP STOS进行初始化：

只不过只能用于初始化,不能用于数据拷贝

```assembly
;Copy Code    
	mov edi, destination  ; 目标内存地址
    mov ecx, length  ; 存储的字节数
    mov eax, value  ; 要存储的双字

    cld  ; 设置方向标志位为正向
    rep stosd  ; 重复执行STOSD指令，根据ECX寄存器的值判断循环次数
```

这些示例演示了如何使用不同的汇编指令集在x86架构下进行内存拷贝。

具体的源内存地址、目标内存地址、拷贝的字节数以及要存储的值需要根据实际情况进行设置。





## x64

只不过是寄存器大小发生了变化

好的，下面是一些在x64架构下使用汇编指令进行内存拷贝的具体例子：



### 使用MOVSB进行字节拷贝：

```assembly
;Copy Code    
	mov rsi, source  ; 源内存地址
    mov rdi, destination  ; 目标内存地址
    mov rcx, length  ; 拷贝的字节数

copy_loop:
    movsb  ; 按字节移动数据
    loop copy_loop  ; 循环拷贝，根据RCX寄存器的值判断循环次数
```



### 使用REP MOVS进行连续拷贝：

```assembly
;Copy Code    
	mov rsi, source  ; 源内存地址
    mov rdi, destination  ; 目标内存地址
    mov rcx, length  ; 拷贝的字节数

    cld  ; 设置方向标志位为正向
    rep movsb  ; 重复执行MOVSB指令，根据RCX寄存器的值判断循环次数
```



### 使用STOSW进行初始化：

```assembly
;Copy Code    
	mov rdi, destination  ; 目标内存地址
    mov rcx, length  ; 拷贝的字数
    mov ax, value  ; 要存储的字

    cld  ; 设置方向标志位为正向

copy_loop:
    stosw  ; 存储字
    loop copy_loop  ; 循环拷贝，根据RCX寄存器的值判断循环次数
```



### 使用REP STOS进行初始化：

```assembly
;Copy Code    
	mov rdi, destination  ; 目标内存地址
    mov rcx, length  ; 存储的字节数
    mov eax, value  ; 要存储的双字

    cld  ; 设置方向标志位为正向
    rep stosd  ; 重复执行STOSD指令，根据RCX寄存器的值判断循环次数
```

需要注意的是，在x64架构下，通用寄存器的名称发生了变化。

原先的ESI、EDI、ECX对应的是RSI、RDI、RCX，其他指令和操作方式保持不变。

这些例子演示了如何在x64架构下使用汇编指令执行内存拷贝操作。

具体的源内存地址、目标内存地址、拷贝的字节数以及要存储的值需要根据实际情况进行设置。

