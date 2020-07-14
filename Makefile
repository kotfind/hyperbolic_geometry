LIBS=-lGL -lSDL2
WARNS=-Wall
CFLAGS=$(WARNS) $(LIBS) -O3 -g0
all:
	g++ main.cpp $(CFLAGS)

run:
	./a.out
