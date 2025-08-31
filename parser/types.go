package parser

import (
	"fmt"
	"strconv"
	"strings"

	backend "github.com/Maruqes/compiler/swig"
)

// Global counter for unique label generation
var labelCounter int

const (
	DQ  = 8
	DD  = 4
	DW  = 2
	DB  = 1
	PTR = 8
)

func printType(size int) {
	switch size {
	case DQ:
		fmt.Println("dq/ptr")
	case DD:
		fmt.Println("dd")
	case DW:
		fmt.Println("dw")
	case DB:
		fmt.Println("db")
	default:
		fmt.Println("unknown")
	}
}

func printRegs(reg int) {
	switch reg {
	case backend.REG_RAX:
		fmt.Println("rax")
	case backend.REG_RBX:
		fmt.Println("rbx")
	case backend.REG_RCX:
		fmt.Println("rcx")
	case backend.REG_RDX:
		fmt.Println("rdx")
	case backend.REG_RSI:
		fmt.Println("rsi")
	case backend.REG_RDI:
		fmt.Println("rdi")
	case backend.REG_R8:
		fmt.Println("r8")
	case backend.REG_R9:
		fmt.Println("r9")
	case backend.REG_R10:
		fmt.Println("r10")
	case backend.REG_R11:
		fmt.Println("r11")
	case backend.REG_R12:
		fmt.Println("r12")
	case backend.REG_R13:
		fmt.Println("r13")
	case backend.REG_R14:
		fmt.Println("r14")
	case backend.REG_R15:
		fmt.Println("r15")
	default:
		fmt.Printf("unknown reg %d", reg)
	}
}

func getTypeFromToken(token string) (int, error) {
	switch token {
	case "dq":
		return DQ, nil // 64-bit variable
	case "ptr":
		return PTR, nil // pointer variable
	case "dd":
		return DD, nil // 32-bit variable
	case "dw":
		return DW, nil // 16-bit variable
	case "db":
		return DB, nil // 8-bit variable
	default:
		return -1, fmt.Errorf("unknown type: '%s'", token)
	}
}

func isTypeToken(token string) bool {
	switch token {
	case "dq", "dd", "dw", "db", "ptr":
		return true
	default:
		return false
	}
}

func isType(size int) bool {
	switch size {
	case DQ, DD, DW, DB:
		return true
	default:
		return false
	}
}

func parseRawNumbers(token string, reg byte) (bool, error) {
	if len(token) > 2 && token[:2] == "0x" {
		num, err := strconv.ParseUint(token[2:], 16, 64)
		if err != nil {
			return false, err
		}
		backend.Mov64_r_i(reg, num)
		return true, nil
	}

	num, err := strconv.Atoi(token)
	if err == nil {
		//is a number
		backend.Mov64_r_i(reg, uint64(num))
		return true, nil
	}

	return false, nil
}

func parseVariablesFuncCalls(parser *Parser, token string, reg byte) (bool, error) {
	varList := GetVarList(SCOPE)
	if varList == nil {
		return false, fmt.Errorf("Variable list for scope '%s' not found", SCOPE)
	}

	//variables
	err := varList.GetVariable(token, reg)
	if err == nil {
		return true, nil
	}

	//function calls
	if isFunctionCall(token) {
		err := parseFunctionCall(parser, token)
		if err != nil {
			return false, err
		}
		return true, nil
	}

	return false, nil
}

func parsePointer(parser *Parser, token string, reg byte) (bool, error) {
	varList := GetVarList(SCOPE)
	if varList == nil {
		return false, fmt.Errorf("Variable list for scope '%s' not found", SCOPE)
	}

	if len(token) >= 1 {
		switch token[0] {
		case '&':
			//create pointer
			token, err := parser.NextToken()
			if err != nil {
				return false, err
			}

			err = varList.GetVariableAddress(token, reg)
			if err != nil {
				return false, err
			}
			return true, nil
		case '*':

			token, err := parser.NextToken()
			if err != nil {
				return false, err
			}

			err = getValueFromToken(parser, token, reg)
			if err != nil {
				return false, err
			}
			backend.Mov64_r_m(reg, reg)
			return true, nil
		}
	}
	return false, nil
}

func countArrayElements(p *Parser) (int, error) {
	mark, err := p.Mark()
	if err != nil {
		return 0, err
	}
	defer p.Restore(mark)

	count := 0
	inElem := false

	for {
		tok, err := p.NextToken()
		if err != nil {
			return 0, err
		}
		if tok == "" {
			return 0, fmt.Errorf("unexpected EOF while counting array elements")
		}

		switch tok {
		case "}":
			if inElem {
				count++
				inElem = false
			}
			return count, nil

		case ",":
			if inElem {
				count++
				inElem = false
			}
			// continue to next token

		default:
			if isTypeToken(tok) {
				peekMark, _ := p.Mark()
				nx, err := p.NextToken()
				if err != nil {
					return 0, err
				}
				//caso seja um array
				if nx == "{" {
					depth := 1
					for depth > 0 {
						z, err := p.NextToken()
						if err != nil {
							return 0, err
						}
						if z == "" {
							return 0, fmt.Errorf("unexpected EOF inside nested array")
						}
						switch z {
						case "{":
							depth++
						case "}":
							depth--
						}
					}
					inElem = true
					continue
				} else {
					p.Restore(peekMark)
				}
			}

			inElem = true
		}
	}
}

var r15Stack bool = false
var depth int = 0

const maxDepth = 8 // shadow stack de 8 níveis (8*8 bytes)

func parseArrays(parser *Parser, token string, reg byte) (bool, error) {

	arrType, err := getTypeFromToken(token)
	if err != nil {
		return false, nil
	}

	token, err = parser.NextToken()
	if err != nil {
		return false, err
	}

	if len(token) < 1 {
		return false, nil
	}

	if token[0] != '{' {
		return false, nil
	}

	numberOfElements, err := countArrayElements(parser)
	if err != nil {
		return false, err
	}

	createdShadow := false
	if !r15Stack {
		r15Stack = true
		createdShadow = true
		SubStack(8 * maxDepth)
		backend.Mov64_r_r(byte(backend.REG_R15), byte(backend.REG_RSP)) // r15 = base da shadow stack
		depth = 0
	}

	// 3) Limite de profundidade
	if depth >= maxDepth {
		if createdShadow {
			r15Stack = false
		}
		return false, fmt.Errorf("nesting de arrays excede maxDepth=%d", maxDepth)
	}


	SubStack(arrType * numberOfElements)
	our_depth := depth
	backend.Mov64_mi_r(byte(backend.REG_R15), uint(our_depth*8), byte(backend.REG_RSP))
	depth++

	err, symbol, parsed := getUntilSymbol(parser, []string{"}", ","}, reg)
	if err != nil {
		return false, err
	}
	count := 0

	for *symbol == "}" || *symbol == "," {
		backend.Mov64_r_mi(byte(backend.REG_RDI), byte(backend.REG_R15), int(our_depth*8))
		offset := uint(count * arrType)
		//
		if parsed {
			switch arrType {
			case DQ:
				backend.Mov64_mi_r(byte(backend.REG_RDI), offset, reg)
			case DD:
				backend.Mov32_mi_r(byte(backend.REG_RDI), offset, reg)
			case DW:
				backend.Mov16_mi_r(byte(backend.REG_RDI), offset, reg)
			case DB:
				backend.Mov8_mi_r(byte(backend.REG_RDI), offset, reg)
			default:
				return false, fmt.Errorf("unknown array type: %d on line %d", arrType, parser.LineNumber)
			}
			count++
		}
		if *symbol == "}" {
			backend.Mov64_r_r(reg, byte(backend.REG_RDI))
			break
		}

		err, symbol, parsed = getUntilSymbol(parser, []string{"}", ","}, reg)
		if err != nil {
			return false, err
		}
	}

	depth--

	if createdShadow {
		r15Stack = false
	}

	if count != numberOfElements {
		return false, fmt.Errorf("number of elements parsed (%d) does not match expected (%d)", count, numberOfElements)
	}

	return true, nil
}

// uses r8
func parseGetArrayIndex(parser *Parser, token string, reg byte) (bool, error) {

	arrType := DB

	varList := GetVarList(SCOPE)
	if varList == nil {
		return false, fmt.Errorf("Variable list for scope '%s' not found", SCOPE)
	}

	if varList.DoesVarExist(token) {
		err := varList.GetVariable(token, reg)
		if err != nil {
			return false, err
		}
	} else {
		return false, fmt.Errorf("Variable '%s' not found in scope0 '%s'", token, SCOPE)
	}

	varStruct, err := varList.GetVariableStruct(token)
	if err != nil {
		return false, err
	}

	if varStruct.Extra != nil {
		switch v := varStruct.Extra.(type) {
		case string:
			if isTypeToken(v) {
				t, err := getTypeFromToken(v)
				if err != nil {
					return false, err
				}
				arrType = t
			}
		case int:
			if isType(v) {
				arrType = v
			}
		}
	}

	parsedOne := false

	for {
		token, err := parser.NextToken()
		if err != nil {
			return false, err
		}

		if token != "[" {
			parser.SeekBack(int64(len(token)))
			break
		}

		err, _, parsed := getUntilSymbol(parser, []string{"]"}, byte(backend.REG_RCX))
		if err != nil {
			return false, err
		}

		if !parsed {
			return false, fmt.Errorf("array index not found for variable '%s' in scope '%s'", token, SCOPE)
		}

		switch arrType {
		case DQ:
			backend.Mul64_r_i(byte(backend.REG_RCX), 8)
			backend.Mov64_r_mr(reg, reg, byte(backend.REG_RCX))
		case DD:
			backend.Mul64_r_i(byte(backend.REG_RCX), 4)
			backend.Mov32_r_mr(reg, reg, byte(backend.REG_RCX))
		case DW:
			backend.Mul64_r_i(byte(backend.REG_RCX), 2)
			backend.Mov16_r_mr(reg, reg, byte(backend.REG_RCX))
		case DB:
			backend.Mov8_r_mr(reg, reg, byte(backend.REG_RCX))
		default:
			return false, fmt.Errorf("unknown array type: %d on line %d", arrType, parser.LineNumber)
		}
		parsedOne = true
	}

	clearReg(reg, arrType)

	return parsedOne, nil
}

func parseParentheses(parser *Parser, token string, reg byte) (bool, error) {
	if token != "(" {
		return false, nil
	}

	err, endToken, _ := getUntilSymbol(parser, []string{")"}, reg)
	if err != nil {
		return false, err
	}

	// Verify we got the closing parenthesis
	if endToken == nil || *endToken != ")" {
		return false, fmt.Errorf("expected closing parenthesis ')' at line %d", parser.LineNumber)
	}

	return true, nil
}

func parseLogicalNot(parser *Parser, token string, reg byte) (bool, error) {
	if token != "!" {
		return false, nil
	}

	nextToken, err := parser.NextToken()
	if err != nil {
		return false, err
	}

	err = getValueFromToken(parser, nextToken, reg)
	if err != nil {
		return false, err
	}

	backend.Cmp64_r_i(reg, 0)

	// Use global counter for unique labels
	labelCounter++
	trueLabel := fmt.Sprintf("logical_not_true_%d", labelCounter)
	endLabel := fmt.Sprintf("logical_not_end_%d", labelCounter)

	backend.Jcc(trueLabel, byte(backend.JE_OPCODE))

	backend.Mov64_r_i(reg, 0)
	backend.Jmp(endLabel)

	backend.Create_label(trueLabel)
	backend.Mov64_r_i(reg, 1)

	backend.Create_label(endLabel)

	return true, nil
}

func parseNegValues(parser *Parser, token string, reg byte) (bool, error) {

	if token != "-" {
		return false, nil
	}

	nextToken, err := parser.NextToken()
	if err != nil {
		return false, err
	}

	err = getValueFromToken(parser, nextToken, reg)
	if err != nil {
		return false, err
	}

	backend.Mul64_r_i(reg, ^uint(0))

	return true, nil
}

var sNumber = 0

func parseStrings(token string, reg byte) (bool, error) {

	if len(token) > 0 && token[0] != '"' {
		return false, nil
	}
	token = token[1 : len(token)-1] // remove surrounding quotes

	// (Optional) handle simple escapes you expect in literals
	// If you need more, switch to strconv.Unquote.
	token = strings.ReplaceAll(token, `\n`, "\n")
	token = strings.ReplaceAll(token, `\t`, "\t")
	token = strings.ReplaceAll(token, `\"`, `"`)

	// Append exactly one NUL terminator (only if not already there)
	if len(token) == 0 || token[len(token)-1] != 0 {
		token += "\x00"
	}

	name := fmt.Sprintf("string_%d", sNumber)
	sNumber++

	backend.Add_string_constant(name, token)
	backend.Create_variable_reference(name, reg)
	return true, nil
}

func parseTypeFuncs(parser *Parser, token string, reg byte) (bool, error) {
	if !isTypeToken(token) {
		return false, nil
	}

	s, err := parser.Peek()
	if err != nil {
		return false, err
	}
	if s != "(" {
		return false, nil
	}

	eatFirstBrace(parser)

	typeToken, err := getTypeFromToken(token)
	if err != nil {
		return false, err
	}

	nextToken, err := parser.NextToken()
	if err != nil {
		return false, err
	}

	err = getValueFromToken(parser, nextToken, reg)
	if err != nil {
		return false, err
	}

	clearReg(reg, typeToken)

	eatLastBrace(parser)

	return true, nil
}

// may uses r8
// uses 64 bit registers to get the value of the token
func getValueFromToken(parser *Parser, token string, reg byte) error {
	//detect number and variables
	varList := GetVarList(SCOPE)
	if varList == nil {
		return fmt.Errorf("Variable list for scope '%s' not found", SCOPE)
	}

	parsed, err := parseParentheses(parser, token, reg)
	if err != nil {
		return err
	}
	if parsed {
		return nil
	}

	parsed, err = parseLogicalNot(parser, token, reg)
	if err != nil {
		return err
	}
	if parsed {
		return nil
	}

	parsed, err = parseNegValues(parser, token, reg)
	if err != nil {
		return err
	}
	if parsed {
		return nil
	}

	parsed, err = parseStrings(token, reg)
	if err != nil {
		return err
	}
	if parsed {
		return nil
	}

	parsed, err = parseTypeFuncs(parser, token, reg)
	if err != nil {
		return err
	}
	if parsed {
		return nil
	}

	parsed, err = parseRawNumbers(token, reg)
	if err != nil {
		return err
	}
	if parsed {
		return nil
	}

	parsed, err = parsePointer(parser, token, reg)
	if err != nil {
		return err
	}
	if parsed {
		return nil
	}

	parsed, err = parseArrays(parser, token, reg)
	if err != nil {
		return err
	}
	if parsed {
		return nil
	}

	parsed, err = parseGetArrayIndex(parser, token, reg)
	if err != nil && parsed {
		return err
	}
	if parsed {
		return nil
	}

	parsed, err = parseVariablesFuncCalls(parser, token, reg)
	if err != nil {
		return err
	}
	if parsed {
		return nil
	}

	parsed, err = parseStructsCreation(parser, token, reg)
	if err != nil && parsed {
		return err
	}
	if parsed {
		return nil
	}

	return fmt.Errorf("unknown token '%s' at line %d", token, parser.LineNumber)
}
