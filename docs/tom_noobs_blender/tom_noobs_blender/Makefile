# Makefile

CC=gcc
CFLAGS= `sdl-config --cflags` `pkg-config --cflags gtk+-3.0` -Wall -Wextra -std=c99 -O3
LDLIBS= `sdl-config --libs` `pkg-config --libs gtk+-3.0`

EXE= tomnoob

SRC= plain.c Internal_fun.c Layer.c NeuralNetwork.c
OBJ=${SRC:.c=.o}
all: ${EXE}

tomnoob: ${SRC} -lm
	${CC} ${CFLAGS} ${SRC} -lm -o tomnoob ${LDLIBS}

-include ${DEP}

$(foreach f, $(EXE), $(eval $(f):))

clean:
	rm -f ${OBJ} ${DEP} ${EXE} bestscore

# END
