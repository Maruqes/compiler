package parser

import (
	"fmt"

	backend "github.com/Maruqes/compiler/swig"
)

func parseCondition(parser *Parser, endLabel string, stopChar string, seekBackStopChar bool) error {
	err, symbol, _ := getUntilSymbol(parser, comparisonOperators, byte(backend.REG_RAX))
	if err != nil {
		return err
	}
	PushStack64(byte(backend.REG_RAX))

	err, _, _ = getUntilSymbol(parser, []string{stopChar}, byte(backend.REG_RAX))
	if err != nil {
		return err
	}
	if seekBackStopChar {
		parser.SeekBack(1)
	}
	PopStack64(byte(backend.REG_RBX))

	backend.Cmp64_r_r(byte(backend.REG_RAX), byte(backend.REG_RBX))

	switch *symbol {
	case "==":
		backend.Jcc(endLabel, byte(backend.JNE_OPCODE))
	case "!=":
		backend.Jcc(endLabel, byte(backend.JE_OPCODE))
	case "<=":
		backend.Jcc(endLabel, byte(backend.JG_OPCODE))
	case ">=":
		backend.Jcc(endLabel, byte(backend.JL_OPCODE))
	case "<":
		backend.Jcc(endLabel, byte(backend.JGE_OPCODE))
	case ">":
		backend.Jcc(endLabel, byte(backend.JLE_OPCODE))
	}
	return nil
}

/*
if a == b {
}
*/
var ifCount = 0

func parseIf(parser *Parser) error {
	ifcc := fmt.Sprintf("if_end%d", ifCount)
	err := parseCondition(parser, ifcc, "{", true)
	if err != nil {
		return err
	}

	err = parseCodeBlock(parser)
	if err != nil {
		return err
	}
	backend.Create_label(ifcc)
	return nil
}

/*
for dq i = 0; i < 10; i++ {
}
*/
var forCount = 0

func parseFor(parser *Parser) error {
	/*
		PARSE FIRST VAR DECLARATION
	*/
	token, err := parser.NextToken()
	if err != nil {
		return err
	}

	parsed, err := parseVariableDeclaration(parser, token)
	if err != nil || !parsed {
		return fmt.Errorf("Error parsing variable declaration for 'for' loop: %v", err)
	}

	/*
		PARSE CONDITION
	*/
	backend.Create_label("for_cond")
	err = parseCondition(parser, "for_end", ";", false)
	if err != nil {
		return err
	}
	//condition is met we jump to body
	backend.Jmp("for_body")

	/*
		PARSE SECOND VAR DECLARATION
	*/
	token, err = parser.NextToken()
	if err != nil {
		return err
	}
	backend.Create_label("for_second")
	parsed, err = parseVariableDeclaration(parser, token)
	if err != nil || !parsed {
		return fmt.Errorf("Error parsing variable declaration for 'for' loop: %v", err)
	}
	backend.Jmp("for_cond") // we jump to condition

	/*
		PARSE body
	*/
	backend.Create_label("for_body")
	err = parseCodeBlock(parser)
	if err != nil {
		return err
	}
	backend.Jmp("for_second") //we jump to second var declaration (i++)
	backend.Create_label("for_end")
	return nil
}
