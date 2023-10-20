SO_SRCS :=$(filter-out src/main.c,$(shell find src -name *.c))
SO_OBJS :=$(patsubst src/%.c,obj/%.o,$(SO_SRCS))

include_path := ./include #头文件路径
library_path := ./lib #链接库的路径
linking_libs := func #链接库的名字

INCS := $(include_path:%=-I%)
LIB := lib/libfunc.so
OUT := build/main.elf


LDFALG := -g -O3 -w -fPIC $(INCS)
CFLAG := $(linking_libs:%=-l%) $(library_path:%=-L%)  $(library_path:%=-Wl,-rpath=%)

#要生成的目标
elf_gen: $(OUT)
lib_gen: $(LIB)
# =================编译动态链接库=================生成对应的o文件
obj/%.o : src/%.c
	@mkdir -p $(dir $@)
	@gcc -c $< $(LDFALG) -o $@
# ===============各个.o开始整和为动态库===================生出.a文件或者.so文件
$(LIB): $(SO_OBJS)
	@mkdir -p $(dir $@)
	@gcc -shared  $^ -o $@

# ===============main开始链接动态库===================
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
	@echo $(SO_SRCS)
	@echo $(SO_OBJS)
	@echo $(INCS)
	@echo $(LDFALG)
	@echo $(CFLAG)
.PHONY : clean debug 