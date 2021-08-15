.PHONY = all clean

CC = gcc

OUTPUT_DIR = output
SOURCE_DIR = src
CFLAGS = -pedantic -Wall -Werror
LIBS = -lm -lgmp

SRCS := $(wildcard ${SOURCE_DIR}*.c)
OBJS := $(patsubst %.c, %.o, $(SRCS))
BINS := $(SRCS:%.c=%)

EXES := ${OUTPUT_DIR}

.PHONY: all

all: build ${OBJS} ${EXES}

${EXES}: ${OBJ}
	@echo "Creating executable..."
	${CC} ${CFLAGS} $< ${LIBS} -o ${OUTPUT_DIR}$@

${OBJS}: ${SOURCE_DIR}%.c
	@echo "Creating object..."
	${CC} ${CFLAGS} -c $< ${LIBS} -o $@

build:
	@echo "Building..."
	@mkdir -p ${OUTPUT_DIR}

clean:
	@echo "Cleaning up..."
	rm -rvf ${OUTPUT_DIR}*
	find . -name "*~" -exec rm {} \;
	find . -name "*.o" -exec rm {} \;
