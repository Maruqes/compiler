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
todo->
asm funcs
inputs na main-> args    ./main <args>
*/
/*
known problems
-dd(b) = fffffffffffffffa (eu esperava 00000000fffffffa)

PARA FAZER
func kernelInfo(ptr structName<utsname>)
*/

func main() {

	var par parser.Parser
	// par.StartParse("features.lang")
	par.StartParse("test.lang")

	backend.Call("global_init")
	backend.Call("main")
	wrapper.Exit(0)

	err := parser.StartParsing(&par)
	if err != nil {
		panic(fmt.Sprintf("Error during parsing on line %d: %v", par.LineNumber, err))
	}

	backend.Create_label("global_init")
	for _, fn := range parser.FUNC_GLOBAL {
		backend.Call(fn)
	}
	backend.Ret()

	// .data
	backend.Add_string_constant("printSave", "--\n")
	backend.Add_string_constant("printHex", "0123456789abcdef\n")
	backend.Write_elf()

	fmt.Println("All C backend functions called successfully!")
}
