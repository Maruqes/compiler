package main

/*
#cgo CFLAGS: -I${SRCDIR}/c_backend
#cgo LDFLAGS: -L${SRCDIR}/bin -lbackend
#include "backend.h"
*/
import "C"
import (
	"fmt"
	"unsafe"
)

func println(str *C.char, strLen int) {
	C.mov64_r_i(C.REG_RAX, C.uint64_t(1))
	C.mov64_r_i(C.REG_RDI, C.uint64_t(1))
	C.create_variable_reference(str, C.REG_RSI)
	C.mov64_r_i(C.REG_RDX, C.uint64_t(strLen))
	C.syscall_instruction()
}

func main() {

	cStr1 := C.CString("string1")
	defer C.free(unsafe.Pointer(cStr1))
	cStr2 := C.CString("string2")
	defer C.free(unsafe.Pointer(cStr2))
	cStr3 := C.CString("string3")
	defer C.free(unsafe.Pointer(cStr3))

	println(cStr1, 14)

	HelloWorld := C.CString("Hello, World!\n")
	defer C.free(unsafe.Pointer(HelloWorld))
	AII_MEUU := C.CString("AII, MEUU!\n")
	defer C.free(unsafe.Pointer(AII_MEUU))
	DEUSSS_JESUS := C.CString("DEUSSS, JESUS CHRIST!\n")
	defer C.free(unsafe.Pointer(DEUSSS_JESUS))

	C.add_string_constant(cStr1, HelloWorld)
	C.add_string_constant(cStr2, AII_MEUU)
	C.add_string_constant(cStr3, DEUSSS_JESUS)

	C.funcao_teste_mul8_r()
	C.funcao_teste_mul8_m()
	C.funcao_teste_mul8_mi()
	C.funcao_teste_mul8_mr()

	C.write_elf()

	fmt.Println("All C backend functions called successfully!")
}
