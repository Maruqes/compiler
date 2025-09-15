package main

/*
#cgo CFLAGS: -I${SRCDIR}/c_backend
#cgo LDFLAGS: -L${SRCDIR}/bin -lbackend
#include "backend.h"
*/
import "C"
import (
	"fmt"
	"os"

	"github.com/Maruqes/compiler/parser"
	backend "github.com/Maruqes/compiler/swig"
	"github.com/Maruqes/compiler/wrapper"
)

/*
known problems
-dd(b) = fffffffffffffffa (eu esperava 00000000fffffffa)

todo-version manager
github release automation
TESTAR FORTEMENTE O AFTER getUntilSymbol METER TODO OS TIPOS DA FUNCAO getValueFromToken EM VARIOS (), METER DENTRO DE ARRAYS FORA, VER ONDE A FUNCAO É USADA E TESTAR TUDO 1 A 1, POR AMOR DE DEUS
*/
func main() {
	var (
		par     parser.Parser
		outFile string
	)

	if len(os.Args) < 3 {
		fmt.Fprintln(os.Stderr, "usage: compiler <input> <output>")
		return
	}

	inFile := os.Args[1]
	outFile = os.Args[2]

	// Ensure input file exists and is a regular file before proceeding
	if fi, err := os.Stat(inFile); err != nil {
		if os.IsNotExist(err) {
			fmt.Fprintf(os.Stderr, "input file does not exist: %s\n", inFile)
		} else {
			fmt.Fprintf(os.Stderr, "cannot access input file %s: %v\n", inFile, err)
		}
		os.Exit(1)
	} else if fi.IsDir() {
		fmt.Fprintf(os.Stderr, "input path is a directory, expected file: %s\n", inFile)
		os.Exit(1)
	}

	par.StartParse(inFile)

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
	backend.Write_elf(outFile)

	fmt.Println("All C backend functions called successfully!")
}
