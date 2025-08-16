package main

/*
#cgo CFLAGS: -I${SRCDIR}/c_backend
#cgo LDFLAGS: -L${SRCDIR}/bin -lbackend
#include "backend.h"
*/
import "C"
import (
	"fmt"

	"github.com/Maruqes/compiler/parser"
	backend "github.com/Maruqes/compiler/swig"
	"github.com/Maruqes/compiler/wrapper"
)

/*
multiple arr referece arr[0][0][0]

in a[x] x is a byte, we need to now if a is dq dd dw db and multiply x by 8, 4, 2, 1 respectively
loops tem problema de escopo devo criar uma nova stack e sair

func main(){
	dd c = 0;
	while 1 == 1 {
	 	//problema aqui
		dd ms = 2;
		if c == 6 {
			break;
		}
		c = c + 1;
	}

	dq a = 10;

	printHex(a);

	return;
}





func main(){
	dq arr = dq{1,2,3,dq{10,11,12,dq{0xaa, dq{0x2020, 0x2021, 0x2022, 0x023}, 0xcc, 0xdd}}};

	//com dd isto da seg fault com dq funciona
	for dd i = 0; i < 3; i = i + 1; {
		printHex(arr[i*8]);
	}

	return;
}


*/

func main() {
	var par parser.Parser
	par.StartParse("test.lang")

	backend.Call("main")
	wrapper.Exit(0)

	err := parser.StartParsing(&par)
	if err != nil {
		panic(fmt.Sprintf("Error during parsing: %v", err))
	}

	// .data
	backend.Add_string_constant("printSave", "--\n")
	backend.Add_string_constant("printHex", "0123456789abcdef\n")
	backend.Write_elf()

	fmt.Println("All C backend functions called successfully!")
}
