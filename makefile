assembler: assembler.o assemblerUtils.o pass.o firstPass.o secondPass.o symbolTable.o
	gcc -g -Wall -ansi -pedantic ./obj/assembler.o ./obj/assemblerUtils.o ./obj/pass.o ./obj/firstPass.o ./obj/secondPass.o ./obj/symbolTable.o -o assembler
assembler.o: ./src/assembler.c ./include/assemblerData.h
	gcc -c -g -Wall -ansi -pedantic ./src/assembler.c -o ./obj/assembler.o	
assemblerUtils.o: ./src/assemblerUtils.c ./include/assemblerData.h ./include/errorCodes.h
	gcc -c -g -Wall -ansi -pedantic ./src/assemblerUtils.c -o ./obj/assemblerUtils.o
pass.o: ./src/pass.c ./include/assemblerData.h ./include/pass.h ./include/errorCodes.h
	gcc -c -g -Wall -ansi -pedantic ./src/pass.c -o ./obj/pass.o
firstPass.o: ./src/firstPass.c ./include/assemblerData.h ./include/pass.h ./include/errorCodes.h
	gcc -c -g -Wall -ansi -pedantic ./src/firstPass.c -o ./obj/firstPass.o
secondPass.o: ./src/secondPass.c ./include/assemblerData.h ./include/pass.h ./include/errorCodes.h
	gcc -c -g -Wall -ansi -pedantic ./src/secondPass.c -o ./obj/secondPass.o
symbolTable.o: ./src/symbolTable.c ./include/assemblerData.h ./include/symbol.h ./include/errorCodes.h
	gcc -c -g -Wall -ansi -pedantic ./src/symbolTable.c -o ./obj/symbolTable.o
