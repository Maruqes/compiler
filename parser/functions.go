package parser

import (
	"fmt"
	"io"
	"os"

	backend "github.com/Maruqes/compiler/swig"
	"github.com/Maruqes/compiler/wrapper"
)

type ParamType struct {
	Name  string
	Type  int // DQ for 64-bit, DD for 32-bit, DW for 16-bit, DB for 8-bit
	Extra any
}

type FunctionType struct {
	Name   string
	Params []ParamType
}

var functions []FunctionType

func isFunctionCall(token string) bool {
	for _, function := range functions {
		if function.Name == token {
			return true
		}
	}
	return false
}

func getFunction(funcName string) *FunctionType {
	for _, function := range functions {
		if function.Name == funcName {
			return &function
		}
	}
	return nil
}

func CreateStack() {
	backend.Push64(byte(backend.REG_RBP))                           // Save the base pointer
	backend.Mov64_r_r(byte(backend.REG_RBP), byte(backend.REG_RSP)) // Set the base pointer to the current stack pointer
}

func LeaveStack() {
	backend.Mov64_r_r(byte(backend.REG_RSP), byte(backend.REG_RBP)) // Restore the stack pointer
	backend.Pop64(byte(backend.REG_RBP))                            // Restore the base pointer
	backend.Ret()                                                   // Return from the function
}

func parseFunctionCall(parser *Parser, funcName string) error {
	if parser.file == nil {
		return os.ErrInvalid
	}
	//get "(" params and ")" ;

	token, err := parser.NextToken()
	if err != nil {
		return err
	}

	if token != "(" {
		return fmt.Errorf("expected '(', got '%s'", token)
	}

	//falta checkar tipos e numero de parametros
	n_params := 0
	for {
		err, symbol, parsed := getUntilSymbol(parser, []string{")", ","}, byte(backend.REG_RAX))
		if err != nil {
			return fmt.Errorf("error getting parameters for function '%s': %v", funcName, err)
		}

		if parsed {
			PushStack64(byte(backend.REG_RAX))
			n_params++
		}

		if *symbol == ")" {
			break
		}
	}

	functype := getFunction(funcName)
	if functype == nil {
		return fmt.Errorf("function '%s' not found", funcName)
	}

	if len(functype.Params) != n_params {
		return fmt.Errorf("function '%s' expects %d parameters, got %d on line %d", funcName, len(functype.Params), n_params, parser.LineNumber)
	}

	backend.Call(funcName)
	SubStack(n_params * 8)
	return nil
}

func parseReturn(parser *Parser) error {
	err, _, _ := getUntilSymbol(parser, []string{";"}, byte(backend.REG_RAX))
	if err != nil {
		return err
	}

	LeaveStack()
	return nil
}

func temporaryPrintHexVar(parser *Parser) error {
	//parse "(", "var_name", ")"
	if parser.file == nil {
		return os.ErrInvalid
	}

	token, err := parser.NextToken()
	if err != nil {
		return err
	}

	if token != "(" {
		return fmt.Errorf("expected '(', got '%s'", token)
	}

	err, _, _ = getUntilSymbol(parser, []string{")"}, byte(backend.REG_RAX))
	if err != nil {
		return err
	}
	wrapper.PrintHex(byte(backend.REG_RAX))

	eatSymbol(parser, ";")
	return nil
}

/*
dq a = 20
a=30
*/
func parseVariableDeclaration(parser *Parser, token string) (bool, error) {
	varList := GetVarList(SCOPE)
	if varList == nil {
		return false, fmt.Errorf("Variable list for scope '%s' not found", SCOPE)
	}

	//if its a var redefenition
	if varList.DoesVarExist(token) {
		err := varList.setVarStruct(parser, token)
		return true, err
	}

	switch token {
	case "dq":
		// create 64-bit variable
		_, err := createVarStruct(parser, DQ, nil)
		return true, err
	case "dd":
		// create 32-bit variable
		_, err := createVarStruct(parser, DD, nil)
		return true, err
	case "dw":
		// c_eate 16-bit variable
		_, err := createVarStruct(parser, DW, nil)
		return true, err
	case "db":
		// create 8-bit variable
		_, err := createVarStruct(parser, DB, nil)
		return true, err
	case "ptr":
		// create pointer variable
		_, err := createPointerVar(parser)
		return true, err
	default:
		return true, fmt.Errorf("unknown variable declaration type: '%s'", token)
	}
}

func parseCodeBlock(parser *Parser) error {
	if parser.file == nil {
		return os.ErrInvalid
	}

	varList := GetVarList(SCOPE)
	if varList == nil {
		return fmt.Errorf("Variable list for scope '%s' not found", SCOPE)
	}

	token, err := parser.NextToken()
	if err != nil {
		if err == io.EOF {
			return nil
		}
		return err
	}

	if token != "{" {
		return fmt.Errorf("expected '{' at the beginning of code block, got '%s'", token)
	}

	for {
		token, err := parser.NextToken()
		if err != nil {
			if err == io.EOF {
				return nil
			}
			return err
		}

		if token == "printHex" {
			if err := temporaryPrintHexVar(parser); err != nil {
				return err
			}
			continue
		}

		switch token {
		case "}":
			return nil
		case "return":
			if err := parseReturn(parser); err != nil {
				return err
			}

		case "if":
			if err := parseIf(parser); err != nil {
				return err
			}
		case "while":
			if err := parseWhiles(parser); err != nil {
				return err
			}
		case "for":
			if err := parseFors(parser); err != nil {
				return err
			}
		case "break":
			if err := ParseNewBreak(parser); err != nil {
				return err
			}
		case "continue":
			if err := ParseNewContinue(parser); err != nil {
				return err
			}
		case "asm":
			if err := ParseNewAsmFunc(parser); err != nil {
				return err
			}
		case "loadReg":
			if err := ParseNewLoadRegFunc(parser); err != nil {
				return err
			}
		case "loadVar":
			if err := ParseNewLoadVarFunc(parser); err != nil {
				return err
			}
		case "*":
			if err := ParsePointerDefinition(parser); err != nil {
				return err
			}
		default:

			if isFunctionCall(token) {
				if err := parseFunctionCall(parser, token); err != nil {
					return err
				}
				eatSymbol(parser, ";")
				continue
			}

			parsed, err := parseVariableDeclaration(parser, token)
			if err != nil && parsed {
				return err
			}
			if parsed {
				continue
			}

			return fmt.Errorf("parseCodeBlock unknown token: '%s' found at line %d", token, parser.LineNumber)
		}
	}
}

func getParams(parser *Parser) ([]ParamType, error) {
	var params []ParamType

	token, err := parser.NextToken()
	if err != nil {
		return nil, err
	}

	if token != "(" {
		return nil, fmt.Errorf("expected '(', got '%s'", token)
	}

	for {
		token, err := parser.NextToken()
		if err != nil {
			return nil, err
		}

		if token == ")" {
			break
		}

		if token == "," {
			continue
		}

		if token == "dq" || token == "dd" || token == "dw" || token == "db" || token == "ptr" {
			paramType, err := getTypeFromToken(token)
			if err != nil {
				return nil, fmt.Errorf("error getting type from token '%s': %v", token, err)
			}
			token, err = parser.NextToken()
			if err != nil {
				return nil, err
			}

			var extra any

			_, _, err = parseCoisoEstranho(parser, &extra)
			if err != nil {
				return nil, err
			}

			params = append(params, ParamType{Name: token, Type: paramType, Extra: extra})
		} else {
			return nil, fmt.Errorf("expected type (dq, dd, dw, db), got '%s'", token)
		}

	}

	return params, nil
}

// takes into account enw stack was created and params are in old stack
func createVariablesFromParams(parser *Parser, params []ParamType) error {

	paramLength := len(params) * 8
	for i, param := range params {
		// fmt.Println(param.Name + " -> " + fmt.Sprintf("%d", paramLength-((i-1)*8)))
		backend.Mov64_r_mi(byte(backend.REG_RAX), byte(backend.REG_RBP), paramLength-((i-1)*8))
		createVarWithReg(parser, byte(backend.REG_RAX), param.Type, param.Name, param.Extra, ORIGIN_RBP)
	}

	return nil
}

// func allignStack(align int) {
// 	if align <= 0 || align > 16 {
// 		align = 16
// 	}

// 	// Align stack to 16 bytes (RSP%16==0)
// 	backend.Mov64_r_r(byte(backend.REG_RAX), byte(backend.REG_RSP))
// 	backend.And64_r_i(byte(backend.REG_RAX), ^(uint(align) - 1))
// 	backend.Sub64_r_r(byte(backend.REG_RSP), byte(backend.REG_RAX))
// }

func createFunc(parser *Parser) error {
	//get name of the function
	name, err := parser.NextToken()
	if err != nil {
		return err
	}

	params, err := getParams(parser)
	if err != nil {
		return fmt.Errorf("error getting parameters for function '%s': %v", name, err)
	}

	if isFunctionCall(name) {
		return fmt.Errorf("function '%s' already exists", name)
	}

	setScope(name)
	CreateVarList(name)
	backend.Create_label(name)
	CreateStack()

	err = createVariablesFromParams(parser, params)
	if err != nil {
		return fmt.Errorf("error creating variables from parameters for function '%s': %v", name, err)
	}

	functions = append(functions, FunctionType{Name: name, Params: params})

	err = parseCodeBlock(parser)
	if err != nil {
		return fmt.Errorf("error parsing code block for function '%s': %w", name, err)
	}
	LeaveStack()

	return nil
}
