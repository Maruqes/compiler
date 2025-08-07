package parser

import (
	"fmt"
	"io"
	"os"

	backend "github.com/Maruqes/compiler/swig"
	"github.com/Maruqes/compiler/wrapper"
)

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

	fmt.Println("Creating function:", name)
	fmt.Println("Creating function:", name)
	fmt.Println("Creating function:", name)

	backend.Create_label(name)
	wrapper.CreateStack()
	err = parseCodeBlock(parser)
	if err != nil {
		return fmt.Errorf("Error parsing code block for function '%s': %w", name, err)
	}
	return nil
}

