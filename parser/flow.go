package parser

import (
	"fmt"

	backend "github.com/Maruqes/compiler/swig"
)

func copyStackImage() (*VarsList, error) {
	varList := CopyVarListState()
	if varList == nil {
		return nil, fmt.Errorf("Variable list for scope '%s' not found", SCOPE)
	}
	return varList, nil
}

func setStackImage(varList *VarsList) {
	// Absolute restore: RSP = RBP + lastPos (lastPos <= 0 for locals)
	if varList == nil {
		return
	}
	backend.Mov64_r_r(byte(backend.REG_RSP), byte(backend.REG_RBP))
	backend.Sum64_r_i(byte(backend.REG_RSP), uint(varList.lastPos))

	SetVarListState(varList)
}

var ifCount int

/*
if condition {

}
*/
func parseIf(parser *Parser) error {

	vlImage, err := copyStackImage()
	if err != nil {
		return err
	}

	ifCount++

	ifleave := fmt.Sprintf("if_leave%d", ifCount)
	ifelse := fmt.Sprintf("if_else%d", ifCount)

	err, _, _ = getUntilSymbol(parser, []string{"{"}, byte(backend.REG_RAX))
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
	fmt.Println(token)
	switch token {
	case "else":
		tok, err := parser.NextToken()
		if err != nil {
			return err
		}

		if tok != "else" {
			return fmt.Errorf("expected 'else', got '%s'", tok)
		}

		err = parseCodeBlock(parser)
		if err != nil {
			return err
		}
	case "elif":
		tok, err := parser.NextToken()
		if err != nil {
			return err
		}
		if tok != "elif" {
			return fmt.Errorf("expected 'elif', got '%s'", tok)
		}

		setStackImage(vlImage)
		err = parseIf(parser)
		if err != nil {
			return err
		}
	}

	backend.Create_label(ifleave)

	setStackImage(vlImage)
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
	eatSymbol(parser, ";")

	return nil
}

func ParseNewContinue(parser *Parser) error {

	loopBreak := PeekLoopBreaks()

	if loopBreak.continueLabel == "" {
		return fmt.Errorf("no loop continue label found you are not in a loop")
	}

	backend.Jmp(loopBreak.continueLabel)
	eatSymbol(parser, ";")

	return nil
}

var whileCount int

func parseWhiles(parser *Parser) error {
	//save old
	vlImage, err := copyStackImage()
	if err != nil {
		return err
	}

	whileCount++

	//label creation
	whileCond := fmt.Sprintf("while_cond%d", whileCount)
	whileEnd := fmt.Sprintf("while_end%d", whileCount)
	whileContinue := fmt.Sprintf("while_continue%d", whileCount)
	// continue should jump to a handler that restores stack, then re-checks condition
	PushLoopBreaks(whileEnd, whileContinue)

	backend.Create_label(whileCond)
	err, _, _ = getUntilSymbol(parser, []string{"{"}, byte(backend.REG_RAX))
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

	// unify normal fall-through and `continue` path here
	backend.Create_label(whileContinue)
	setStackImage(vlImage) // Restore stack pointer for next iteration
	backend.Jmp(whileCond)
	backend.Create_label(whileEnd)
	PopLoopBreaks()

	setStackImage(vlImage)
	return nil
}

var forCount int

func parseFors(parser *Parser) error {
	// Snapshot before init
	preImage, err := copyStackImage()
	if err != nil {
		return err
	}

	// Snapshot after init
	var postImage *VarsList

	forCount++
	//label creation
	forCond := fmt.Sprintf("for_cond%d", forCount)
	forSecond := fmt.Sprintf("for_second%d", forCount)
	forBody := fmt.Sprintf("for_body%d", forCount)
	forEnd := fmt.Sprintf("for_end%d", forCount)
	forContinue := fmt.Sprintf("for_continue%d", forCount)

	PushLoopBreaks(forEnd, forContinue)

	/*
		PARSE FIRST VAR DECLARATION    dq i = 0;
	*/
	token, err := parser.NextToken()
	if err != nil {
		return err
	}

	parsed, _, err := parseVariableDeclaration(parser, token)
	if err != nil || !parsed {
		return fmt.Errorf("error parsing variable declaration for 'for' loop: %v", err)
	}
	// Capture stack/vars image after init so it includes loop variables
	postImage, err = copyStackImage()
	if err != nil {
		return err
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

	parsed, _, err = parseVariableDeclaration(parser, token)
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
	backend.Create_label(forContinue)
	setStackImage(postImage)
	backend.Jmp(forSecond)
	backend.Create_label(forEnd)

	PopLoopBreaks()
	// restore to pre-init image so 'i' (and other init vars) are removed
	setStackImage(preImage)
	return nil
}
