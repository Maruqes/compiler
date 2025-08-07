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
