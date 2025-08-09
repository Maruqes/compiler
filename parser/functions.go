package parser

import (
	"fmt"
	"io"
	"os"

	backend "github.com/Maruqes/compiler/swig"
)

type ParamType struct {
	Name string
	Type int // DQ for 64-bit, DD for 32-bit, DW for 16-bit, DB for 8-bit
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
		return fmt.Errorf("Expected '(', got '%s'", token)
	}

	//falta checkar tipos e numero de parametros
	n_params := 0
	for {
		err, symbol := getUntilSymbol(parser, []string{")", ","}, byte(backend.REG_RAX))
		if err != nil {
			return fmt.Errorf("Error getting parameters for function '%s': %v", funcName, err)
		}
		PushStack64(byte(backend.REG_RAX))
		n_params++
		fmt.Println("Symbol:", string(*symbol))
		if *symbol == ")" {
			break
		}
	}

	functype := getFunction(funcName)
	if functype == nil {
		return fmt.Errorf("Function '%s' not found", funcName)
	}

	if functype.Params != nil && len(functype.Params) != n_params {
		return fmt.Errorf("Function '%s' expects %d parameters, got %d on line %d", funcName, len(functype.Params), n_params, parser.lineNumber)
	}

	eatSemicolon(parser)
	backend.Call(funcName)
	for i := 0; i < n_params; i++ {
		PopStack64(byte(backend.REG_RAX))
	}
	return nil
}

func temporaryPrintVar(parser *Parser) error {
	//parse "(", "var_name", ")"
	if parser.file == nil {
		return os.ErrInvalid
	}

	token, err := parser.NextToken()
	if err != nil {
		return err
	}

	if token != "(" {
		return fmt.Errorf("Expected '(', got '%s'", token)
	}

	varName, err := parser.NextToken()
	if err != nil {
		return err
	}

	token, err = parser.NextToken()
	if err != nil {
		return err
	}

	if token != ")" {
		return fmt.Errorf("Expected ')', got '%s'", token)
	}
	PrintVar(varName)
	eatSemicolon(parser)
	return nil
}

func parseCodeBlock(parser *Parser) error {
	if parser.file == nil {
		return os.ErrInvalid
	}

	token, err := parser.NextToken()
	if err != nil {
		if err == io.EOF {
			return nil
		}
		return err
	}

	if token != "{" {
		return fmt.Errorf("Expected '{' at the beginning of code block, got '%s'", token)
	}

	for {
		token, err := parser.NextToken()
		if err != nil {
			if err == io.EOF {
				return nil
			}
			return err
		}

		//check for function calls

		if isFunctionCall(token) {
			if err := parseFunctionCall(parser, token); err != nil {
				return err
			}
			continue
		}

		if token == "print" {
			if err := temporaryPrintVar(parser); err != nil {
				return err
			}
			continue
		}

		switch token {
		case "}":
			return nil
		case "dq":
			// create 64-bit variable
			if err := createVar(parser, DQ); err != nil {
				return err
			}
		case "dd":
			// create 32-bit variable
			if err := createVar(parser, DD); err != nil {
				return err
			}
		case "dw":
			// create 16-bit variable
			if err := createVar(parser, DW); err != nil {
				return err
			}
		case "db":
			// create 8-bit variable
			if err := createVar(parser, DB); err != nil {
				return err
			}
		case "return":
			LeaveStack()
			backend.Ret()
			eatSemicolon(parser)
			setScope(GLOBAL_SCOPE)
		default:
			return fmt.Errorf("Unknown token: '%s' found at line %d", token, parser.lineNumber)
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
		return nil, fmt.Errorf("Expected '(', got '%s'", token)
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

		if token == "dq" || token == "dd" || token == "dw" || token == "db" {
			paramType, err := getTypeFromToken(token)
			if err != nil {
				return nil, fmt.Errorf("Error getting type from token '%s': %v", token, err)
			}
			token, err = parser.NextToken()
			if err != nil {
				return nil, err
			}
			params = append(params, ParamType{Name: token, Type: paramType})
		} else {
			return nil, fmt.Errorf("Expected type (dq, dd, dw, db), got '%s'", token)
		}

	}

	return params, nil
}

// takes into account enw stack was created and params are in old stack
func createVariablesFromParams(parser *Parser, params []ParamType) error {

	paramLength := len(params) * 8
	for i, param := range params {
		backend.Mov64_r_mi(byte(backend.REG_RAX), byte(backend.REG_RBP), paramLength-((i-1)*8))
		createVarWithReg(parser, byte(backend.REG_RAX), param.Type, param.Name)
	}

	return nil
}

// for now is -> func name {}
func createFunc(parser *Parser) error {
	//get name of the function
	name, err := parser.NextToken()
	if err != nil {
		return err
	}

	params, err := getParams(parser)
	if err != nil {
		return fmt.Errorf("Error getting parameters for function '%s': %v", name, err)
	}

	if isFunctionCall(name) {
		return fmt.Errorf("Function '%s' already exists", name)
	}

	setScope(name)
	CreateVarList(name)

	backend.Create_label(name)
	CreateStack()

	err = createVariablesFromParams(parser, params)
	if err != nil {
		return fmt.Errorf("Error creating variables from parameters for function '%s': %v", name, err)
	}

	err = parseCodeBlock(parser)
	if err != nil {
		return fmt.Errorf("Error parsing code block for function '%s': %w", name, err)
	}

	functions = append(functions, FunctionType{Name: name, Params: params})
	return nil
}
