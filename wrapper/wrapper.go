package wrapper

import backend "github.com/Maruqes/compiler/swig"

func CreateStack() {
	backend.Push64(byte(backend.REG_RBP))                           // Save the base pointer
	backend.Mov64_r_r(byte(backend.REG_RBP), byte(backend.REG_RSP)) // Set the base pointer to the current stack pointer
}

func LeaveStack() {
	backend.Mov64_r_r(byte(backend.REG_RSP), byte(backend.REG_RBP)) // Restore the stack pointer
	backend.Pop64(byte(backend.REG_RBP))                            // Restore the base pointer
	backend.Ret()                                                   // Return from the function
}

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
