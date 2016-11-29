all: MIPSSim

MIPSSim: MIPSSim.o readout.o dissembler.o BTB.o LinkedList.o SimTomasulo.o
	gcc MIPSSim.o SimTomasulo.o readout.o dissembler.o LinkedList.o BTB.o -o MIPSSim

MIPSSim.o: MIPSSim.c 
	gcc -c MIPSSim.c

readout.o: readout.c
	gcc -c readout.c

dissembler.o: dissembler.c
	gcc -c dissembler.c

BTB.o: BTB.c
	gcc -c BTB.c

SimTomasulo.o: SimTomasulo.c
	gcc -c SimTomasulo.c

LinkedList.o: LinkedList.c
	gcc -c LinkedList.c

clean: 
	rm -rf *.o MIPSSim

