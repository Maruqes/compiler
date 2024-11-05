SOURCES = main.c functions/functions.c types/strings.c types/uint32.c \
		  mov_reg_reg/mov_reg_reg.c push_pop/push_pop.c 16_code/functions_16.c \
		  8high_code/functions_8high.c 8low_code/functions_8low.c jumps/jumps.c \
		  arithmetic/arithmetic.c logic/logic.c variables/variables.c \
		  parser/parser.c parser/parser_help.c parser/int/parser_int.c


run:
	gcc $(SOURCES) -lm -Og -g -o a.out
	./a.out


check:
	cppcheck --enable=all --suppress=constParameterPointer --suppress=redundantAssignment --suppress=unusedVariable --suppress=unusedFunction --inconclusive --template=gcc --language=c --platform=unix64 --suppress=missingIncludeSystem $(SOURCES)

check_mem_leaks:
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose --log-file=valgrind-out.txt ./a.out