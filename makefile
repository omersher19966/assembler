assembler: assembler.o assemblerUtils.o pass.o firstPass.o secondPass.o symbolTable.o
	gcc -g -Wall -ansi -pedantic ./bin/assembler.o ./bin/assemblerUtils.o ./bin/pass.o ./bin/firstPass.o ./bin/secondPass.o ./bin/symbolTable.o -o ./bin/assembler
assembler.o: ./src/assembler.c ./include/assemblerData.h
	gcc -c -g -Wall -ansi -pedantic ./src/assembler.c -o ./bin/assembler.o	
assemblerUtils.o: ./src/assemblerUtils.c ./include/assemblerData.h ./include/errorCodes.h
	gcc -c -g -Wall -ansi -pedantic ./src/assemblerUtils.c -o ./bin/assemblerUtils.o
pass.o: ./src/pass.c ./include/assemblerData.h ./include/pass.h ./include/errorCodes.h
	gcc -c -g -Wall -ansi -pedantic ./src/pass.c -o ./bin/pass.o
firstPass.o: ./src/firstPass.c ./include/assemblerData.h ./include/pass.h ./include/errorCodes.h
	gcc -c -g -Wall -ansi -pedantic ./src/firstPass.c -o ./bin/firstPass.o
secondPass.o: ./src/secondPass.c ./include/assemblerData.h ./include/pass.h ./include/errorCodes.h
	gcc -c -g -Wall -ansi -pedantic ./src/secondPass.c -o ./bin/secondPass.o
symbolTable.o: ./src/symbolTable.c ./include/assemblerData.h ./include/symbol.h ./include/errorCodes.h
	gcc -c -g -Wall -ansi -pedantic ./src/symbolTable.c -o ./bin/symbolTable.o
