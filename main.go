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

func println(str *C.char, strLen int) {
	C.mov64_r_i(C.REG_RAX, C.uint64_t(1))
	C.mov64_r_i(C.REG_RDI, C.uint64_t(1))
	C.create_variable_reference(str, C.REG_RSI)
	C.mov64_r_i(C.REG_RDX, C.uint64_t(strLen))
	C.syscall_instruction()
}

func main() {
	var par parser.Parser
	par.StartParse("test.lang")

	parser.StartParsing(&par)

	backend.Write_elf()

	fmt.Println("All C backend functions called successfully!")
}
