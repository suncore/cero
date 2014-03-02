
CFLAGS=-O3 -g -Wall -std=c++11
CPPFLAGS = ${CFLAGS}
OBJS=cpp2cero.o cero2cpp.o cero.o build.o platform.o
LFLAGS=

all: cero

cero: $(OBJS)
	g++ -o cero $(OBJS) $(LFLAGS)

clean:
	rm -f *.o


