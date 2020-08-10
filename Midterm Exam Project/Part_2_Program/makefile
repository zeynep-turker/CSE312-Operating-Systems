all: makeFileSystem
makeFileSystem: makeFileSystem.o
	gcc -Wall -Wextra -pedantic makeFileSystem.c -o makeFileSystem
	./makeFileSystem
makeFileSystem.o: makeFileSystem.c
	gcc -c makeFileSystem.c

clean: 
	rm -rf *.o makeFileSystem
