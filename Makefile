OBJS = src/mainlb.cpp src/handler/handshake.cpp

CC = g++

COMPILER_FLAG = -w

INCLUDE_FLAG  = -Ilib/json/single_include -Isrc/handler

LINKER_FLAG =

OBJ_NAME = mongolb

all: ${OBJS}
	${CC} ${OBJS} ${COMPILER_FLAG} ${INCLUDE_FLAG} ${LINKER_FLAG} -o ${OBJ_NAME}
clean:
	rm -f ${OBJ_NAME}
