CC=g++
CCOPTS=-g
SDLOPTS=-l SDL

all: langton

langton:	langton.o
	$(CC) $(CCOPTS) $(SDLOPTS) -o langton langton.o

langton.o: langton.cpp
	$(CC) $(CCOPTS) -c -o langton.o langton.cpp

clean:
	rm -f langton.o langton
