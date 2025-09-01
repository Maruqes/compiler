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
left shift right shift
*/

/*
known bugs->
	APAGAMOS STRUCTS/GLOBAL

	PROBLEMO PUBLIC VAR backend.REG_RSI in c
*/

func main() {

	var par parser.Parser
	// par.StartParse("features.lang")
	par.StartParse("test.lang")


	backend.Call("global")
	backend.Call("main")
	wrapper.Exit(0)

	err := parser.StartParsing(&par)
	if err != nil {
		panic(fmt.Sprintf("Error during parsing on line %d: %v", par.LineNumber, err))
	}

	// .data
	backend.Add_string_constant("printSave", "--\n")
	backend.Add_string_constant("printHex", "0123456789abcdef\n")
	backend.Write_elf()

	fmt.Println("All C backend functions called successfully!")
}
