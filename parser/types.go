package parser

import (
	"fmt"
	"io"
	"strconv"

	backend "github.com/Maruqes/compiler/swig"
)

const (
	DQ  = 8
	DD  = 4
	DW  = 2
	DB  = 1
	PTR = 8
)

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
		return -1, fmt.Errorf("Unknown type: '%s'", token)
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

			//check if token is a variable
			varStruct, err := varList.GetVariableStruct(token, reg)
			if err != nil {
				return false, err
			}

			backend.Mov64_r_i(reg, uint64(varStruct.Position))
			backend.Sum64_r_r(reg, byte(backend.REG_RBP))

			return true, nil
		case '*':
			numberOfDereferences := 1
			//create pointer
			token, err := parser.NextToken()
			if err != nil {
				return false, err
			}

			for token == "*" {
				numberOfDereferences++
				token, err = parser.NextToken()
				if err != nil {
					return false, err
				}
			}

			//check if token is a variable
			err = varList.GetVariable(token, reg)
			if err != nil {
				return false, err
			}

			for i := 0; i < numberOfDereferences; i++ {
				backend.Mov64_r_m(reg, reg)
			}
			return true, nil
		}
	}
	return false, nil
}

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

	//count the number of elements
	//so we agree peeking into the next tokens and seekback to parse it normally
	// 1) remember where we are
	pos, err := parser.file.Seek(0, io.SeekCurrent)
	if err != nil {
		return false, err
	}

	paramCount := 0
	err, symbol, parsed := getUntilSymbol(parser, []string{"}", ","}, reg)
	if err != nil {
		return false, err
	}
	for *symbol == "}" || *symbol == "," {
		if parsed {
			paramCount++
		}
		if *symbol == "}" {
			break
		}
		err, symbol, parsed = getUntilSymbol(parser, []string{"}", ","}, reg)
		if err != nil {
			return false, err
		}
	}

	//after counting the number of params we get seek back and parse them
	parser.file.Seek(pos, io.SeekStart) //seek back to the position we were

	SubStack(arrType * paramCount)
	err, symbol, parsed = getUntilSymbol(parser, []string{"}", ","}, reg)
	if err != nil {
		return false, err
	}
	count := 0
	for *symbol == "}" || *symbol == "," {
		if parsed {
			//place variable in correct array place
			offset := (count * arrType)
			switch arrType {
			case DQ:
				backend.Mov64_mi_r(byte(backend.REG_RSP), uint(offset), reg)
			case DD:
				backend.Mov32_mi_r(byte(backend.REG_RSP), uint(offset), reg)
			case DW:
				backend.Mov16_mi_r(byte(backend.REG_RSP), uint(offset), reg)
			case DB:
				backend.Mov8_mi_r(byte(backend.REG_RSP), uint(offset), reg)
			}
			count++
		}
		if *symbol == "}" {
			backend.Mov64_r_r(reg, byte(backend.REG_RSP))
			return true, nil
		}

		err, symbol, parsed = getUntilSymbol(parser, []string{"}", ","}, reg)
		if err != nil {
			return false, err
		}
	}

	return false, nil
}

// uses r8
func parseGetArrayIndex(parser *Parser, token string, reg byte) (bool, error) {

	varList := GetVarList(SCOPE)
	if varList == nil {
		return false, fmt.Errorf("Variable list for scope '%s' not found", SCOPE)
	}

	if !varList.DoesVarExist(token) {
		return false, nil
	}

	err := varList.GetVariable(token, reg)
	if err != nil {
		return false, err
	}

	parsedOne := false

	token, err = parser.NextToken()
	if err != nil {
		return false, err
	}

	if token != "[" {
		parser.SeekBack(int64(len(token)))
		return false, nil
	}

	err, _, parsed := getUntilSymbol(parser, []string{"]"}, byte(backend.REG_R8))
	if err != nil {
		return false, err
	}

	if !parsed {
		return false, fmt.Errorf("Array index not found for variable '%s' in scope '%s'", token, SCOPE)
	}

	backend.Mov64_r_mr(reg, reg, byte(backend.REG_R8))
	parsedOne = true

	return parsedOne, nil
}

// may uses r8
// uses 64 bit registers to get the value of the token
func getValueFromToken(parser *Parser, token string, reg byte) error {
	//detect number and variables
	varList := GetVarList(SCOPE)
	if varList == nil {
		return fmt.Errorf("Variable list for scope '%s' not found", SCOPE)
	}

	parsed, err := parseRawNumbers(token, reg)
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
	if err != nil {
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

	return fmt.Errorf("Unknown token '%s' at line %d", token, parser.lineNumber)
}
