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
else/else if
break/continue
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
