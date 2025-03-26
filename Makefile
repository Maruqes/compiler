SOURCES = main.c functions/functions.c types/strings.c \
		  mov_reg_reg/mov_reg_reg.c push_pop/push_pop.c 16_code/functions_16.c \
		  8high_code/functions_8high.c 8low_code/functions_8low.c jumps/jumps.c \
		  arithmetic/arithmetic.c logic/logic.c variables/variables.c \
		  parser/parser.c parser/parser_help.c parser/parse_float.c parser/parser_functions.c parser/int/parser_int.c parser/strings/strings.c \
		  asm_parser/asm_parser.c asm_parser/asm_parser16.c asm_parser/asm_parse_floats.c asm_parser/asm_parser8.c \
		  functions/bFunctions32/bFunc32Floats.c \
		  functions/bFunctions32/bFunctions32.c \
		  functions/bFunctions8/bFunctions8.c \


run:
	gcc $(SOURCES) -lm -Og -g -o a.out
	./a.out code/example_test6 hello_elf_32
	./code/hello_elf_32	






check:
	cppcheck --enable=all --suppress=constParameterPointer --suppress=redundantAssignment --suppress=unusedVariable --suppress=unusedFunction --inconclusive --template=gcc --language=c --platform=unix64 --suppress=missingIncludeSystem $(SOURCES)

check_mem_leaks:
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose --log-file=valgrind-out.txt ./a.out