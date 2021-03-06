CC = gcc
OBJCOPY = objcopy
OBJDUMP = objdump
SIZE = size
NM = gcc-nm

GIT_COMMITS := $(shell git rev-list --count HEAD)
GIT_COMMIT := $(shell git log -n 1 --format="%h-%f")

#Compiler options
CFLAGS		= -Og -g -c -std=gnu11 `pkg-config --cflags --libs gtk+-3.0`
CFLAGS		+= -Wall -Wextra -Werror -fms-extensions -Wno-unused-parameter -Wno-address-of-packed-member
CFLAGS		+= -pedantic
CFLAGS		+= -DGIT_VERSION=\"[$(GIT_COMMITS)]-$(GIT_COMMIT)\" -DGIT_SHA1=\"$(GIT_SHA1)\"

LINK_FLAGS =  -lgmp -lm `pkg-config --cflags --libs gtk+-3.0` -ggdb3
LINK_FLAGS += -Wl,--start-group -lc -lgcc -Wl,--end-group -Wl,--gc-sections

INCLUDE_PATHS += -Iinclude

SOURCES :=	src/main.c			\
			src/modelling.c		\
			src/data.c			\
			src/gui.c			\
			src/graph.c

BUILD_DIR := build

OBJECTS = $(SOURCES:%.c=$(BUILD_DIR)/%.o)
DEPS = $(SOURCES:%.c=$(BUILD_DIR)/%.d)


WHOLE_EXE := $(BUILD_DIR)/main

default: $(WHOLE_EXE)

$(BUILD_DIR)/.git.$(GIT_COMMIT):
	mkdir -p "$(@D)"
	rm -f $(BUILD_DIR)/.git.*
	touch $@


$(OBJECTS): $(BUILD_DIR)%.o: .%.c
	mkdir -p `dirname $@`
	$(CC) $(CFLAGS) $(INCLUDE_PATHS) $< -o $@


$(WHOLE_EXE): $(OBJECTS)
	$(CC) $(OBJECTS) $(LINK_FLAGS) -o $(WHOLE_EXE)

clean:
	rm -rf $(BUILD_DIR)
	rm -rf output

valgrind: $(WHOLE_EXE)
	valgrind $(WHOLE_EXE) --leak-check=full

gdb: $(WHOLE_EXE)
	gdb $(WHOLE_EXE)

cppcheck: $(WHOLE_EXE)
	cppcheck --enable=all --std=c99 *.[ch]


-include $(shell find "$(BUILD_DIR)" -name "*.d")
