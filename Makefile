.PHONY = all clean

CC = gcc

OUTPUT_DIR = ./output/
SOURCE_DIR = ./src/
CFLAGS = -Wall
LIBS = -lm -lgmp

SRCS := $(wildcard ${SOURCE_DIR}*.c)
OBJS := $(patsubst %.c, %.o, $(SRCS))
BINS := $(SRCS:%.c=%)

EXES := ${OUTPUT_DIR}

.PHONY: all

all: build ${OBJS} ${EXES}

${EXES}: ${OBJ}
	@echo "Checking..."
	${CC} ${CFLAGS} ${OUTPUT_DIR} $< ${LIBS} -o $@

${OBJS}: ${SOURCE_DIR}%.o: ${SOURCE_DIR}%.c
	@echo "Creating object..."
	${CC} ${CFLAGS} -c $< ${OUTPUT_DIR} ${LIBS} -o $@

build:
	@echo "Building..."
	@mkdir -p ${OUTPUT_DIR}

clean:
	@echo "Cleaning up..."
	rm -rvf ${OUTPUT_DIR}*
	find . -name "*~" -exec rm {} \;
	find . -name "*.o" -exec rm {} \;
