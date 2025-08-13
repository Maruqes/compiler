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
	ifCount++
	ifelse := fmt.Sprintf("if_else%d", ifCount)
	ifleave := fmt.Sprintf("if_leave%d", ifCount)
	err := parseCondition(parser, ifelse, "{", true)
	if err != nil {
		return err
	}

	err = parseCodeBlock(parser)
	if err != nil {
		return err
	}
	backend.Jmp(ifleave)
	backend.Create_label(ifelse)

	token, err := parser.NextToken()
	if err != nil {
		return err
	}

	switch token {
	case "else":
		err = parseCodeBlock(parser)
		if err != nil {
			return err
		}
	case "elif":
		parseIf(parser)
	default:
		parser.SeekBack(int64(len(token)))
	}
	backend.Create_label(ifleave)

	return nil
}


/*this exists to make breaks and continue possible so we can have fors inside fors*/
type LoopBreaks struct {
	breakLabel    string
	continueLabel string
}

// stack needs to be first in first out
var loopBreaks []LoopBreaks

func PushLoopBreaks(breakLabel string, continueLabel string) {
	loopBreaks = append(loopBreaks, LoopBreaks{
		breakLabel:    breakLabel,
		continueLabel: continueLabel,
	})
}

func PopLoopBreaks() LoopBreaks {
	if len(loopBreaks) == 0 {
		return LoopBreaks{"", ""}
	}
	breakLabel := loopBreaks[len(loopBreaks)-1].breakLabel
	continueLabel := loopBreaks[len(loopBreaks)-1].continueLabel
	loopBreaks = loopBreaks[:len(loopBreaks)-1]
	return LoopBreaks{breakLabel, continueLabel}
}

func PeekLoopBreaks() LoopBreaks {
	if len(loopBreaks) == 0 {
		return LoopBreaks{"", ""}
	}
	return loopBreaks[len(loopBreaks)-1]
}

func ParseNewBreak(parser *Parser) error {
	loopBreak := PeekLoopBreaks()
	if loopBreak.breakLabel == "" {
		return fmt.Errorf("No loop break label found you are not in a loop")
	}
	backend.Jmp(loopBreak.breakLabel)
	eatSemicolon(parser)
	return nil
}

func ParseNewContinue(parser *Parser) error {
	loopBreak := PeekLoopBreaks()
	if loopBreak.continueLabel == "" {
		return fmt.Errorf("No loop continue label found you are not in a loop")
	}
	backend.Jmp(loopBreak.continueLabel)
	eatSemicolon(parser)
	return nil
}

/*
for dq i = 0; i < 10; i++ {
}
*/
var forCount = 0

func parseFor(parser *Parser) error {
	forCount++
	//label creation
	forCond := fmt.Sprintf("for_cond%d", forCount)
	forSecond := fmt.Sprintf("for_second%d", forCount)
	forBody := fmt.Sprintf("for_body%d", forCount)
	forEnd := fmt.Sprintf("for_end%d", forCount)
	PushLoopBreaks(forEnd, forSecond)

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
	backend.Create_label(forCond)
	err = parseCondition(parser, forEnd, ";", false)
	if err != nil {
		return err
	}
	//condition is met we jump to body
	backend.Jmp(forBody)

	/*
		PARSE SECOND VAR DECLARATION
	*/
	token, err = parser.NextToken()
	if err != nil {
		return err
	}
	backend.Create_label(forSecond)
	parsed, err = parseVariableDeclaration(parser, token)
	if err != nil || !parsed {
		return fmt.Errorf("Error parsing variable declaration for 'for' loop: %v", err)
	}
	backend.Jmp(forCond) // we jump to condition

	/*
		PARSE body
	*/
	backend.Create_label(forBody)
	err = parseCodeBlock(parser)
	if err != nil {
		return err
	}
	backend.Jmp(forSecond) //we jump to second var declaration (i++)
	backend.Create_label(forEnd)
	PopLoopBreaks()
	return nil
}

var whileCount = 0

func parseWhile(parser *Parser) error {
	whileCount++
	//label creation
	whileCond := fmt.Sprintf("while_cond%d", whileCount)
	whileEnd := fmt.Sprintf("while_end%d", whileCount)
	PushLoopBreaks(whileEnd, whileCond)

	backend.Create_label(whileCond)
	err := parseCondition(parser, whileEnd, "{", true)
	if err != nil {
		return err
	}

	err = parseCodeBlock(parser)
	if err != nil {
		return err
	}
	backend.Jmp(whileCond)
	backend.Create_label(whileEnd)
	PopLoopBreaks()
	return nil
}
