package parser

import (
	"fmt"
	"io"
	"os"

	backend "github.com/Maruqes/compiler/swig"
	"github.com/Maruqes/compiler/wrapper"
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

func parseFunctionCall(parser *Parser, token string) error {
	if parser.file == nil {
		return os.ErrInvalid
	}

	backend.Call(token)
	eatSemicolon(parser)
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
			wrapper.LeaveStack()
			backend.Ret()
			eatSemicolon(parser)
		default:
			return fmt.Errorf("Unknown token: '%s' found at line %d", token, parser.lineNumber)
		}
	}
}

// for now is -> func name {}
func createFunc(parser *Parser) error {
	//get name of the function
	name, err := parser.NextToken()
	if err != nil {
		return err
	}

	backend.Create_label(name)
	wrapper.CreateStack()
	VarList.lastPos -= 8
	err = parseCodeBlock(parser)
	if err != nil {
		return fmt.Errorf("Error parsing code block for function '%s': %w", name, err)
	}

	functions = append(functions, FunctionType{Name: name})
	return nil
}
