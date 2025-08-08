package wrapper

import backend "github.com/Maruqes/compiler/swig"


func Println(str string, strLen int) {
	backend.Mov64_r_i(byte(backend.REG_RAX), 1)
	backend.Mov64_r_i(byte(backend.REG_RDI), 1)
	backend.Create_variable_reference(str, byte(backend.REG_RSI))
	backend.Mov64_r_i(byte(backend.REG_RDX), uint64(strLen))
	backend.Syscall_instruction()
}

func Exit(code int) {
	backend.Mov64_r_i(byte(backend.REG_RAX), 60) // syscall number for exit
	backend.Mov64_r_i(byte(backend.REG_RDI), uint64(code))
	backend.Syscall_instruction()
}
