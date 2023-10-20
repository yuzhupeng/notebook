LIB_SRCS :=$(filter-out src/main.c,$(shell find src -name *.c))
LIB_OBJS :=$(patsubst src/%.c,obj/%.o,$(LIB_SRCS))



include_path := ./include #头文件路径
library_path := ./lib #链接库的路径
linking_libs := func #链接库的名字

INCS := $(include_path:%=-I%)
LIB := lib/libfunc.a 
OUT := build/main.elf


LDFALG := -g -O3 -w $(INCS)
CFLAG := $(linking_libs:%=-l%) $(library_path:%=-L%) 

#要生成的目标
elf_gen: $(OUT)
lib_gen: $(LIB)
# =================编译静态链接库=================
obj/%.o : src/%.c
	@mkdir -p $(dir $@)
	@gcc -c $< $(LDFALG) -o $@

lib/libfunc.a : $(LIB_OBJS)
	@mkdir -p $(dir $@)
	@ar -r $@ $^
# ===============开始链接静态库===================
obj/main.o : src/main.c 
	@mkdir -p $(dir $@)
	@gcc -c $< $(LDFALG) -o $@


# =============== 开始整合 ======================
build/main.elf: obj/main.o
	@mkdir -p $(dir $@)
	@gcc $^ $(CFLAG) -o $@


clean:
	@rm -rf obj build lib

debug:
	@echo $(LIB_SRCS)
	@echo $(LIB_OBJS)
	@echo $(INCS)
	@echo $(LDFALG)
	@echo $(CFLAG)
.PHONY : clean debug 