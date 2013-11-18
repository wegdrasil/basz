all: basz

basz: main.o Basz.o CommandHistory.o Pipeline.o
		gcc main.o Basz.o CommandHistory.o Pipeline.o -o basz

main.o: main.c
		gcc -c main.c

Basz.o: Basz.c
		gcc -c Basz.c

CommandHistory.o: CommandHistory.c
		gcc -c CommandHistory.c

Pipeline.o: Pipeline.c
		gcc -c Pipeline.c
		
clean:	
		rm -rf *.o basz




