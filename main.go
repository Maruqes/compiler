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
)

func println(str string, strLen int) {
	backend.Mov64_r_i(byte(backend.REG_RAX), 1)
	backend.Mov64_r_i(byte(backend.REG_RDI), 1)
	backend.Create_variable_reference(str, byte(backend.REG_RSI))
	backend.Mov64_r_i(byte(backend.REG_RDX), uint64(strLen))
	backend.Syscall_instruction()
}

func exit(code int) {
	backend.Mov64_r_i(byte(backend.REG_RAX), 60) // syscall number for exit
	backend.Mov64_r_i(byte(backend.REG_RDI), uint64(code))
	backend.Syscall_instruction()
}

func main() {
	var par parser.Parser
	par.StartParse("test.lang")
	
	parser.StartParsing(&par)

	// cStr := "str1"
	// backend.Add_string_constant(cStr, "Hello, World!\n")

	// backend.Call("main")

	// backend.Create_label("print")
	// println("str1", 14)
	// backend.Ret()

	// backend.Create_label("main")
	// backend.Call("print")
	// backend.Jmp("main")
	// exit(0)

	backend.Write_elf()

	fmt.Println("All C backend functions called successfully!")
}
