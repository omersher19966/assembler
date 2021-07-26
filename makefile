assembler: assembler.o assemblerUtils.o pass.o firstPass.o secondPass.o symbolTable.o
	gcc -g -Wall -ansi -pedantic assembler.o assemblerUtils.o pass.o firstPass.o secondPass.o symbolTable.o -o assembler
assembler.o: assembler.c assemblerData.h pass.h
	gcc -c -g -Wall -ansi -pedantic assembler.c -o assembler.o	
assemblerUtils.o: assemblerUtils.c assemblerData.h
	gcc -c -g -Wall -ansi -pedantic assemblerUtils.c -o assemblerUtils.o
pass.o: pass.c assemblerData.h pass.h
	gcc -c -g -Wall -ansi -pedantic pass.c -o pass.o
firstPass.o: firstPass.c assemblerData.h pass.h
	gcc -c -g -Wall -ansi -pedantic firstPass.c -o firstPass.o
secondPass.o: secondPass.c assemblerData.h pass.h
	gcc -c -g -Wall -ansi -pedantic secondPass.c -o secondPass.o
symbolTable.o: symbolTable.c assemblerData.h symbol.h
	gcc -c -g -Wall -ansi -pedantic symbolTable.c -o symbolTable.o