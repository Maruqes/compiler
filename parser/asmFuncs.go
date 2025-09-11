package parser

import (
	"fmt"
	"reflect"
	"strconv"

	backend "github.com/Maruqes/compiler/swig"
)

//convert strings like "rax" "r8" "bl" etc to real registers byte

func convertStringToAsmReg(regString string) (byte, error) {
	switch regString {
	case "rax":
		return byte(backend.REG_RAX), nil
	case "rbx":
		return byte(backend.REG_RBX), nil
	case "rcx":
		return byte(backend.REG_RCX), nil
	case "rdx":
		return byte(backend.REG_RDX), nil
	case "rsi":
		return byte(backend.REG_RSI), nil
	case "rdi":
		return byte(backend.REG_RDI), nil
	case "rsp":
		return byte(backend.REG_RSP), nil
	case "rbp":
		return byte(backend.REG_RBP), nil
	case "r8":
		return byte(backend.REG_R8), nil
	case "r9":
		return byte(backend.REG_R9), nil
	case "r10":
		return byte(backend.REG_R10), nil
	case "r11":
		return byte(backend.REG_R11), nil
	case "r12":
		return byte(backend.REG_R12), nil
	case "r13":
		return byte(backend.REG_R13), nil
	case "r14":
		return byte(backend.REG_R14), nil
	case "r15":
		return byte(backend.REG_R15), nil

	// 32-bit registers
	case "eax":
		return byte(backend.REG_EAX), nil
	case "ebx":
		return byte(backend.REG_EBX), nil
	case "ecx":
		return byte(backend.REG_ECX), nil
	case "edx":
		return byte(backend.REG_EDX), nil
	case "esi":
		return byte(backend.REG_ESI), nil
	case "edi":
		return byte(backend.REG_EDI), nil
	case "esp":
		return byte(backend.REG_ESP), nil
	case "ebp":
		return byte(backend.REG_EBP), nil
	case "r8d":
		return byte(backend.REG_R8D), nil
	case "r9d":
		return byte(backend.REG_R9D), nil
	case "r10d":
		return byte(backend.REG_R10D), nil
	case "r11d":
		return byte(backend.REG_R11D), nil
	case "r12d":
		return byte(backend.REG_R12D), nil
	case "r13d":
		return byte(backend.REG_R13D), nil
	case "r14d":
		return byte(backend.REG_R14D), nil
	case "r15d":
		return byte(backend.REG_R15D), nil

	// 16-bit registers
	case "ax":
		return byte(backend.REG_AX), nil
	case "bx":
		return byte(backend.REG_BX), nil
	case "cx":
		return byte(backend.REG_CX), nil
	case "dx":
		return byte(backend.REG_DX), nil
	case "si":
		return byte(backend.REG_SI), nil
	case "di":
		return byte(backend.REG_DI), nil
	case "sp":
		return byte(backend.REG_SP), nil
	case "bp":
		return byte(backend.REG_BP), nil
	case "r8w":
		return byte(backend.REG_R8W), nil
	case "r9w":
		return byte(backend.REG_R9W), nil
	case "r10w":
		return byte(backend.REG_R10W), nil
	case "r11w":
		return byte(backend.REG_R11W), nil
	case "r12w":
		return byte(backend.REG_R12W), nil
	case "r13w":
		return byte(backend.REG_R13W), nil
	case "r14w":
		return byte(backend.REG_R14W), nil
	case "r15w":
		return byte(backend.REG_R15W), nil

	// 8-bit registers
	case "al":
		return byte(backend.REG_AL), nil
	case "bl":
		return byte(backend.REG_BL), nil
	case "cl":
		return byte(backend.REG_CL), nil
	case "dl":
		return byte(backend.REG_DL), nil
	case "sil":
		return byte(backend.REG_SIL), nil
	case "dil":
		return byte(backend.REG_DIL), nil
	case "r8b":
		return byte(backend.REG_R8B), nil
	case "r9b":
		return byte(backend.REG_R9B), nil
	case "r10b":
		return byte(backend.REG_R10B), nil
	case "r11b":
		return byte(backend.REG_R11B), nil
	case "r12b":
		return byte(backend.REG_R12B), nil
	case "r13b":
		return byte(backend.REG_R13B), nil
	case "r14b":
		return byte(backend.REG_R14B), nil
	case "r15b":
		return byte(backend.REG_R15B), nil
	}
	return byte(0), fmt.Errorf("unknown register: %s", regString)
}

var asmFuncs = map[string]any{
	// 64-bit moves
	"mov64_r_i":   backend.Mov64_r_i,
	"mov64_r_m":   backend.Mov64_r_m,
	"mov64_r_mi":  backend.Mov64_r_mi,
	"mov64_r_mr":  backend.Mov64_r_mr,
	"mov64_r_r":   backend.Mov64_r_r,
	"mov64_m_i32": backend.Mov64_m_i32,
	"mov64_m_r":   backend.Mov64_m_r,
	"mov64_mi_i":  backend.Mov64_mi_i,
	"mov64_mi_r":  backend.Mov64_mi_r,
	"mov64_mr_i":  backend.Mov64_mr_i,
	"mov64_mr_r":  backend.Mov64_mr_r,

	"lshfit64":     backend.Lshfit64,
	"rshfit64":     backend.Rshfit64,
	"lshfit64_reg": backend.Lshfit64_reg,
	"rshfit64_reg": backend.Rshfit64_reg,
	"inc64_r":      backend.Inc64_r,
	"inc64_m":      backend.Inc64_m,
	"inc64_mi":     backend.Inc64_mi,
	"inc64_mr":     backend.Inc64_mr,
	"dec64_r":      backend.Dec64_r,
	"dec64_m":      backend.Dec64_m,
	"dec64_mi":     backend.Dec64_mi,
	"dec64_mr":     backend.Dec64_mr,
	"push64":       backend.Push64,
	"pop64":        backend.Pop64,
	"push64_m":     backend.Push64_m,
	"push64_mi":    backend.Push64_mi,
	"pop64_m":      backend.Pop64_m,
	"pop64_mi":     backend.Pop64_mi,

	// 32-bit moves
	"mov32_r_i":  backend.Mov32_r_i,
	"mov32_r_m":  backend.Mov32_r_m,
	"mov32_r_mi": backend.Mov32_r_mi,
	"mov32_r_mr": backend.Mov32_r_mr,
	"mov32_r_r":  backend.Mov32_r_r,
	"mov32_m_i":  backend.Mov32_m_i,
	"mov32_m_r":  backend.Mov32_m_r,
	"mov32_mi_i": backend.Mov32_mi_i,
	"mov32_mi_r": backend.Mov32_mi_r,
	"mov32_mr_i": backend.Mov32_mr_i,
	"mov32_mr_r": backend.Mov32_mr_r,

	"lshfit32":     backend.Lshfit32,
	"rshfit32":     backend.Rshfit32,
	"lshfit32_reg": backend.Lshfit32_reg,
	"rshfit32_reg": backend.Rshfit32_reg,
	"inc32_r":      backend.Inc32_r,
	"inc32_m":      backend.Inc32_m,
	"inc32_mi":     backend.Inc32_mi,
	"inc32_mr":     backend.Inc32_mr,
	"dec32_r":      backend.Dec32_r,
	"dec32_m":      backend.Dec32_m,
	"dec32_mi":     backend.Dec32_mi,
	"dec32_mr":     backend.Dec32_mr,

	// 16-bit moves
	"mov16_r_i":  backend.Mov16_r_i,
	"mov16_r_m":  backend.Mov16_r_m,
	"mov16_r_mi": backend.Mov16_r_mi,
	"mov16_r_mr": backend.Mov16_r_mr,
	"mov16_r_r":  backend.Mov16_r_r,
	"mov16_m_i":  backend.Mov16_m_i,
	"mov16_m_r":  backend.Mov16_m_r,
	"mov16_mi_i": backend.Mov16_mi_i,
	"mov16_mi_r": backend.Mov16_mi_r,
	"mov16_mr_i": backend.Mov16_mr_i,
	"mov16_mr_r": backend.Mov16_mr_r,

	"lshfit16":     backend.Lshfit16,
	"rshfit16":     backend.Rshfit16,
	"lshfit16_reg": backend.Lshfit16_reg,
	"rshfit16_reg": backend.Rshfit16_reg,
	"inc16_r":      backend.Inc16_r,
	"inc16_m":      backend.Inc16_m,
	"inc16_mi":     backend.Inc16_mi,
	"inc16_mr":     backend.Inc16_mr,
	"dec16_r":      backend.Dec16_r,
	"dec16_m":      backend.Dec16_m,
	"dec16_mi":     backend.Dec16_mi,
	"dec16_mr":     backend.Dec16_mr,

	// 8-bit moves
	"mov8_r_i":  backend.Mov8_r_i,
	"mov8_r_m":  backend.Mov8_r_m,
	"mov8_r_mi": backend.Mov8_r_mi,
	"mov8_r_mr": backend.Mov8_r_mr,
	"mov8_r_r":  backend.Mov8_r_r,
	"mov8_m_i":  backend.Mov8_m_i,
	"mov8_m_r":  backend.Mov8_m_r,
	"mov8_mi_i": backend.Mov8_mi_i,
	"mov8_mi_r": backend.Mov8_mi_r,
	"mov8_mr_i": backend.Mov8_mr_i,
	"mov8_mr_r": backend.Mov8_mr_r,

	"lshfit8":          backend.Lshfit8,
	"rshfit8":          backend.Rshfit8,
	"lshfit8_reg":      backend.Lshfit8_reg,
	"rshfit8_reg":      backend.Rshfit8_reg,
	"inc8_r":           backend.Inc8_r,
	"inc8_m":           backend.Inc8_m,
	"inc8_mi":          backend.Inc8_mi,
	"inc8_mr":          backend.Inc8_mr,
	"dec8_r":           backend.Dec8_r,
	"dec8_m":           backend.Dec8_m,
	"dec8_mi":          backend.Dec8_mi,
	"dec8_mr":          backend.Dec8_mr,
	"syscall":          backend.Syscall_instruction,
	"ret":              backend.Ret,
	"call":             backend.Call,
	"call_raw_address": backend.Call_raw_address,
	"jmp_reg":          backend.Jump_reg,
	"jump_reg":         backend.Jump_reg,
	"and64_r_i":        backend.And64_r_i,
}

// callAsmFunction invokes a function value of any type using reflection with the given arguments.
// It attempts to coerce each arg to the parameter type of the function (e.g., int -> int64).
func callAsmFunction(fn any, args []any) error {
	v := reflect.ValueOf(fn)
	if v.Kind() != reflect.Func {
		return fmt.Errorf("asm function is not callable")
	}
	t := v.Type()
	numIn := t.NumIn()
	isVariadic := t.IsVariadic()

	// Validate arity
	if !isVariadic && len(args) != numIn {
		return fmt.Errorf("asm function expects %d args, got %d", numIn, len(args))
	}
	if isVariadic && len(args) < numIn-1 {
		return fmt.Errorf("asm function expects at least %d args, got %d", numIn-1, len(args))
	}

	// Build input values with type coercion
	in := make([]reflect.Value, len(args))
	for i, a := range args {
		var dstType reflect.Type
		if isVariadic && i >= numIn-1 {
			// Variadic tail uses element type of the last parameter
			dstType = t.In(numIn - 1).Elem()
		} else {
			dstType = t.In(i)
		}

		rv, err := coerceToType(a, dstType)
		if err != nil {
			return fmt.Errorf("arg %d: %w", i+1, err)
		}
		in[i] = rv
	}

	v.Call(in)
	return nil
}

// coerceToType converts a into a reflect.Value of dst type when possible.
func coerceToType(a any, dst reflect.Type) (reflect.Value, error) {
	// nil handling
	if a == nil {
		switch dst.Kind() {
		case reflect.Interface, reflect.Pointer, reflect.Slice, reflect.Map, reflect.Func, reflect.Chan:
			return reflect.Zero(dst), nil
		default:
			return reflect.Value{}, fmt.Errorf("cannot assign nil to %s", dst)
		}
	}

	src := reflect.ValueOf(a)

	if src.Type().AssignableTo(dst) {
		return src, nil
	}
	if src.Type().ConvertibleTo(dst) {
		return src.Convert(dst), nil
	}

	if s, ok := a.(string); ok {
		switch dst.Kind() {
		case reflect.String:
			return reflect.ValueOf(s), nil
		case reflect.Bool:
			b, err := strconv.ParseBool(s)
			if err != nil {
				return reflect.Value{}, err
			}
			v := reflect.New(dst).Elem()
			v.SetBool(b)
			return v, nil
		case reflect.Int, reflect.Int8, reflect.Int16, reflect.Int32, reflect.Int64:
			i64, err := strconv.ParseInt(s, 10, dst.Bits())
			if err != nil {
				return reflect.Value{}, err
			}
			v := reflect.New(dst).Elem()
			v.SetInt(i64)
			return v, nil
		case reflect.Uint, reflect.Uint8, reflect.Uint16, reflect.Uint32, reflect.Uint64, reflect.Uintptr:
			u64, err := strconv.ParseUint(s, 10, dst.Bits())
			if err != nil {
				return reflect.Value{}, err
			}
			v := reflect.New(dst).Elem()
			v.SetUint(u64)
			return v, nil
		case reflect.Float32, reflect.Float64:
			f64, err := strconv.ParseFloat(s, 64)
			if err != nil {
				return reflect.Value{}, err
			}
			v := reflect.New(dst).Elem()
			v.SetFloat(f64)
			return v, nil
		}
	}

	return reflect.Value{}, fmt.Errorf("cannot convert %T to %s", a, dst)
}

/*
every asm func should be like this->

asm(func_name, param1,param2,param3...)

asm(mov64_r_i, rax, 10)
*/
func ParseNewAsmFunc(parser *Parser) error {

	eatSymbol(parser, "(")

	asmFuncName, err := parser.NextToken()
	if err != nil {
		return err
	}

	peekString, err := parser.Peek()
	if err != nil {
		return err
	}

	if peekString != ")" {
		eatSymbol(parser, ",")
	}

	asmParams := []any{}
	for {
		param, err := parser.NextToken()
		if err != nil {
			return err
		}
		if param == ")" {
			break
		}

		finalReg, err := convertStringToAsmReg(param)
		if err != nil {
			//imediatos
			asmParams = append(asmParams, param)
		} else {
			//regs
			asmParams = append(asmParams, finalReg)
		}

		peekString, err := parser.Peek()
		if err != nil {
			return err
		}

		if peekString != ")" {
			eatSymbol(parser, ",")
		}
	}

	// fmt.Println(asmFuncName, asmParams)

	eatSymbol(parser, ";")

	actualFunc, exists := asmFuncs[asmFuncName]
	if !exists {
		return fmt.Errorf("unknown asm function: %s", asmFuncName)
	}

	// Call the actual function with the parsed parameters
	if err := callAsmFunction(actualFunc, asmParams); err != nil {
		return err
	}

	return nil
}

// loadReg(rax, var_name)
func ParseNewLoadRegFunc(parser *Parser) error {
	eatSymbol(parser, "(")

	regString, err := parser.NextToken()
	if err != nil {
		return err
	}

	finalReg, err := convertStringToAsmReg(regString)
	if err != nil {
		return err
	}
	eatSymbol(parser, ",")

	err, _, _ = getUntilSymbol(parser, []string{")"}, finalReg)
	if err != nil {
		return err
	}

	eatSymbol(parser, ";")
	return nil
}

// loadVar(var_name, rax)
func ParseNewLoadVarFunc(parser *Parser) error {

	varList := GetVarList(SCOPE)
	if varList == nil {
		return fmt.Errorf("Variable list for scope '%s' not found", SCOPE)
	}

	//PARSER PART
	eatSymbol(parser, "(")
	varname, err := parser.NextToken()
	if err != nil {
		return err
	}
	eatSymbol(parser, ",")

	regString, err := parser.NextToken()
	if err != nil {
		return err
	}

	eatSymbol(parser, ")")
	eatSymbol(parser, ";")

	//ASM PART
	finalReg, err := convertStringToAsmReg(regString)
	if err != nil {
		return err
	}

	varStruct, err := varList.GetVariableStruct(varname)
	if err != nil {
		return err
	}

	backend.Mov64_r_r(byte(backend.REG_RAX), finalReg)

	varList.SetVar(varStruct)

	return nil
}
