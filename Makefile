OBJS = mainlb.cpp

CC = g++

COMPILER_FLAG = -w

INCLUDE_FLAG  = -Ilib/json/single_include

LINKER_FLAG =

OBJ_NAME = mongolb

all: ${OBJS}
	${CC} ${OBJS} ${COMPILER_FLAG} ${INCLUDE_FLAG} ${LINKER_FLAG} -o ${OBJ_NAME}
