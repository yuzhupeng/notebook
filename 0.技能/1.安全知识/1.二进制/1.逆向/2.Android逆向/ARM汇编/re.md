# 杂七杂八的入门



他的寄存器名字奇奇怪怪的

叫R0,R1,R2......



立即数的表达

```
#0X12
```



MRS/MSR指令 的功能和MOV类似,

LDR Rd, [Rn , #offset], 表示Rd=[ Rn+offset] 读

STR Rd, [Rn, #offset] 表示 [ Rn+offset] =Rd 写



入栈出栈

```
 STMFD 	SP!,{R0~R3, R12} 	@R0~R3,R12 入栈
 STMFD 	SP!,{LR} 			@LR 入栈

 LDMFD 	SP!, {LR} 			@先恢复 LR
 LDMFD	 SP!, {R0~R3, R12}  @再恢复 R0~R3, R12

```





 B、BL 或 BX，

