
SRCS :=$(shell find src -name *.c)
OBJS :=$(patsubst src/%.c,obj/%.o,$(SRCS))
include_path := /mnt/hgfs/share/demo/include
INCS := $(include_path:%=-I%)
BIN := build/main.elf


LDFALG := -g -O3 -w $(INCS)

obj/%.o : src/%.c
	@mkdir -p $(dir $@)
	@gcc -c $< $(LDFALG) -o $@

$(BIN): $(OBJS)
	@mkdir -p $(dir $@)
	@gcc $^ -o $@

all: $(BIN)

clean:
	@rm -rf obj build

debug:
	@echo $(SRCS)
	@echo $(OBJS)
	@echo $(INCS)
	@echo $(LDFALG)
.PHONY : clean