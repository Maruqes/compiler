package parser

import (
	"fmt"

	backend "github.com/Maruqes/compiler/swig"
)

var ifCount int

/*
if condition {

}
*/
func parseIf(parser *Parser) error {
	ifCount++

	ifleave := fmt.Sprintf("if_leave%d", ifCount)
	ifelse := fmt.Sprintf("if_else%d", ifCount)

	err, _, _ := getUntilSymbol(parser, []string{"{"}, byte(backend.REG_RAX))
	if err != nil {
		return err
	}
	err = parser.SeekBack(1) //return { for parseCodeBlock
	if err != nil {
		return err
	}

	backend.Cmp64_r_i(byte(backend.REG_RAX), 0)
	backend.Jcc(ifelse, byte(backend.JE_OPCODE))

	err = parseCodeBlock(parser)
	if err != nil {
		return err
	}
	backend.Jmp(ifleave)
	backend.Create_label(ifelse)

	token, err := parser.Peek()
	if err != nil {
		return err
	}

	switch token {
	case "else":
		_, err := parser.NextToken()
		if err != nil {
			return err
		}

		err = parseCodeBlock(parser)
		if err != nil {
			return err
		}
	case "elif":
		_, err := parser.NextToken()
		if err != nil {
			return err
		}
		parseIf(parser)
	}

	backend.Create_label(ifleave)
	return nil
}

/*this exists to make breaks and continue possible so we can have fors inside fors*/

type LoopBreaks struct {
	breakLabel string

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
		return fmt.Errorf("no loop break label found you are not in a loop")
	}

	backend.Jmp(loopBreak.breakLabel)
	eatSemicolon(parser)

	return nil
}

func ParseNewContinue(parser *Parser) error {

	loopBreak := PeekLoopBreaks()

	if loopBreak.continueLabel == "" {
		return fmt.Errorf("no loop continue label found you are not in a loop")
	}

	backend.Jmp(loopBreak.continueLabel)
	eatSemicolon(parser)

	return nil
}

var whileCount int

func parseWhiles(parser *Parser) error {
	//save old
	varList := CopyVarListState()
	if varList == nil {
		return fmt.Errorf("Variable list for scope '%s' not found", SCOPE)
	}
	oldStackSize := varList.lastPos
	backend.Mov64_r_r(byte(backend.REG_R9), byte(backend.REG_RSP))

	whileCount++

	//label creation
	whileCond := fmt.Sprintf("while_cond%d", whileCount)
	whileEnd := fmt.Sprintf("while_end%d", whileCount)
	PushLoopBreaks(whileEnd, whileCond)

	backend.Create_label(whileCond)
	err, _, _ := getUntilSymbol(parser, []string{"{"}, byte(backend.REG_RAX))
	if err != nil {
		return err
	}
	err = parser.SeekBack(1) //return { for parseCodeBlock
	if err != nil {
		return err
	}

	backend.Cmp64_r_i(byte(backend.REG_RAX), 0)
	backend.Jcc(whileEnd, byte(backend.JE_OPCODE))

	err = parseCodeBlock(parser)
	if err != nil {
		return err
	}

	backend.Mov64_r_r(byte(backend.REG_RSP), byte(backend.REG_R9))
	backend.Jmp(whileCond)
	backend.Create_label(whileEnd)
	PopLoopBreaks()

	backend.Mov64_r_r(byte(backend.REG_RSP), byte(backend.REG_R9))
	varList.lastPos = oldStackSize

	SetVarListState(varList)
	return nil
}

var forCount int

func parseFors(parser *Parser) error {
	//save old
	varList := CopyVarListState()
	if varList == nil {
		return fmt.Errorf("Variable list for scope '%s' not found", SCOPE)
	}
	oldStackSize := varList.lastPos
	backend.Mov64_r_r(byte(backend.REG_R9), byte(backend.REG_RSP))

	forCount++
	//label creation
	forCond := fmt.Sprintf("for_cond%d", forCount)
	forSecond := fmt.Sprintf("for_second%d", forCount)
	forBody := fmt.Sprintf("for_body%d", forCount)
	forEnd := fmt.Sprintf("for_end%d", forCount)

	PushLoopBreaks(forEnd, forSecond)

	/*
		PARSE FIRST VAR DECLARATION    dq i = 0;
	*/
	token, err := parser.NextToken()
	if err != nil {
		return err
	}

	parsed, err := parseVariableDeclaration(parser, token)
	if err != nil || !parsed {
		return fmt.Errorf("error parsing variable declaration for 'for' loop: %v", err)
	}

	/*
		PARSE CONDITION    a == b;
	*/
	backend.Create_label(forCond)
	err, _, _ = getUntilSymbol(parser, []string{";"}, byte(backend.REG_RAX))
	if err != nil {
		return err
	}
	backend.Cmp64_r_i(byte(backend.REG_RAX), 0)
	backend.Jcc(forEnd, byte(backend.JE_OPCODE))
	backend.Jmp(forBody)

	/*
		PARSE SECOND VAR DECLARATION  i++;
	*/

	token, err = parser.NextToken()
	if err != nil {
		return err
	}
	backend.Create_label(forSecond)

	parsed, err = parseVariableDeclaration(parser, token)
	if err != nil || !parsed {
		return fmt.Errorf("error parsing variable declaration for 'for' loop: %v", err)
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
	backend.Mov64_r_r(byte(backend.REG_RSP), byte(backend.REG_R9)) // Restore stack pointer for next iteration
	backend.Jmp(forSecond)                                         //we jump to second var declaration (i++)
	backend.Create_label(forEnd)

	PopLoopBreaks()
	backend.Mov64_r_r(byte(backend.REG_RSP), byte(backend.REG_R9)) //restore stack pointer for continue program
	varList.lastPos = oldStackSize

	SetVarListState(varList)
	return nil
}
