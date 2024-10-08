run:
	gcc main.c functions.c strings.c uint32.c mov_reg_reg.c push_pop.c 16_code/functions_16.c 8high_code/functions_8high.c 8low_code/functions_8low.c
	./a.out
