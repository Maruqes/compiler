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

/*
map "0123456789abcdef" to 0-15
mov rdi, 0x123456789abcdef0 // rdi has the value to

rdi has the value to print
mov rax, rdi
and rax, 0x000000000000000f //remove all but the last nibble, we have 16
mov rbx, [map + rax]

print char
*/
func PrintHex(reg byte) {
	//64 bits sao 16 nibbles ou 8 bytes
	//preparation
	backend.Sub64_r_i(byte(backend.REG_RSP), 32)                         // 8 bytes + \n  space on the stack
	backend.Mov64_r_r(byte(backend.REG_R8), reg)                         //save the value to print
	backend.Mov8_mi_i(byte(backend.REG_RSP), 16, 0x0a)                   // \n at the end of the string
	backend.Create_variable_reference("printHex", byte(backend.REG_RSI)) //getting the address of the string

	var andValue uint
	andValue = 0x000000000000000f
	loop := 16

	for i := 0; i < loop; i++ {
		//conversion part
		backend.Mov64_r_r(byte(backend.REG_RAX), byte(backend.REG_R8))
		backend.And64_r_i(byte(backend.REG_RAX), andValue)

		backend.Mov8_r_mr(byte(backend.REG_RBX), byte(backend.REG_RSI), byte(backend.REG_RAX))
		backend.Mov8_mi_r(byte(backend.REG_RSP), uint(loop-1-i), byte(backend.REG_RBX))

		//shift the value to the right by 4 bits to get the next nibble
		backend.Rshfit64(byte(backend.REG_R8), 4)
	}


	backend.Mov64_r_i(byte(backend.REG_RAX), 1)
	backend.Mov64_r_i(byte(backend.REG_RDI), 1)
	backend.Mov64_r_r(byte(backend.REG_RSI), byte(backend.REG_RSP))
	backend.Mov64_r_i(byte(backend.REG_RDX), uint64(loop+1)) // +1 for \n
	backend.Syscall_instruction()

	backend.Sum64_r_i(byte(backend.REG_RSP), 32)
}
