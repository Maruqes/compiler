SOURCES = main.c functions/functions.c \
		  16_code/functions_16.c \
		  8high_code/functions_8high.c 8low_code/functions_8low.c \
		  functions/bFunctions32/bFunctions32.c \
		  functions/bFunctions8/bFunctions8.c \
		  functions/bFunctions64/bFunctions64.c

run:
	gcc $(SOURCES) -lm -Og -g -o a.out
	./a.out 
	./hello_elf_64






check:
	cppcheck --enable=all --suppress=constParameterPointer --suppress=redundantAssignment --suppress=unusedVariable --suppress=unusedFunction --inconclusive --template=gcc --language=c --platform=unix64 --suppress=missingIncludeSystem $(SOURCES)

check_mem_leaks:
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose --log-file=valgrind-out.txt ./a.out