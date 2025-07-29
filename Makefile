SOURCES = main.c functions/functions.c \
		  functions/bFunctions32/bFunctions32.c \
		  functions/bFunctions16/bFunctions16.c \
		  functions/bFunctions8/bFunctions8.c \
		  functions/bFunctions8/utils8.c \
		  functions/bFunctions64/bFunctions64.c \
		  functions/bFunctions64/bTestes64.c \
		  functions/bFunctions64/utils64.c \
		  functions/bFunctions64/arithmetics64.c \
		  functions/bFunctions32/bTestes32.c \
		  functions/bFunctions32/utils32.c \
		  functions/bFunctions16/bTestes16.c \
		  functions/bFunctions16/utils16.c \
		  functions/bFunctions8/bTestes8.c \
		  functions/jumps/jumps.c \
		  functions/jumps/labels.c \
		  functions/jumps/jumpTeste.c \
		  raw_vars/raw_vars.c \
		  
run:
	gcc $(SOURCES) -lm -Og -g -o a.out
	./a.out 
	./hello_elf_64






check:
	cppcheck --enable=all --suppress=constParameterPointer --suppress=redundantAssignment --suppress=unusedVariable --suppress=unusedFunction --inconclusive --template=gcc --language=c --platform=unix64 --suppress=missingIncludeSystem $(SOURCES)

check_mem_leaks:
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose --log-file=valgrind-out.txt ./a.out