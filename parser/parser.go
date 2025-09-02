package parser

import (
	"fmt"
	"io"
	"os"

	backend "github.com/Maruqes/compiler/swig"
)

type Parser struct {
	file       *os.File
	LineNumber int
}

var comparisonOperators = []string{"==", "!=", "<=", ">=", "<", ">"}

// contains solo char that compose predefined tokens
var composingTokens = append([]string{"{", "}", "(", ")", "[", "]", ";", ",", "+", "-", "*", "%", "=", "!", "&", "&&", "|", "||", "^", "?", "/",
	"++", "--", "+=", "-=", "*=", "/=", "%=", "&=", "|=", "^=", "//", ">>", "<<"}, comparisonOperators...)

func (p *Parser) StartParse(fileName string) error {
	//open file
	file, err := os.Open(fileName)
	if err != nil {
		return err
	}
	p.file = file
	return nil
}

func (p *Parser) readNextChar() (string, error) {
	buf := make([]byte, 1)
	_, err := p.file.Read(buf)
	if err != nil {
		if err == io.EOF {
			return "", io.EOF
		}
		return "", err
	}
	if buf[0] == '\n' {
		p.LineNumber++
	}
	return string(buf), nil
}

func (p *Parser) SeekBack(number int64) error {
	if p.file == nil {
		return os.ErrInvalid
	}

	// current position
	pos, err := p.file.Seek(0, io.SeekCurrent)
	if err != nil {
		return err
	}
	if number > pos { // clamp to BOF
		number = pos
	}

	// peek the bytes we’ll cross without moving the cursor
	buf := make([]byte, number)
	_, err = p.file.ReadAt(buf, pos-number)
	if err != nil {
		return err
	}

	for _, b := range buf {
		if b == '\n' {
			p.LineNumber--
		}
	}

	_, err = p.file.Seek(-number, io.SeekCurrent)
	return err
}

func (p *Parser) isComposingToken(char string) (string, error) {
	found := false
	for _, token := range composingTokens {
		if char == token {
			found = true
			break
		}
	}
	if !found {
		return "", nil
	}

	//if found
	nextChar, err := p.readNextChar()
	if err != nil {
		if err == io.EOF {
			return char, io.EOF
		}
		return "", err
	}

	nextChar = char + nextChar
	for _, token := range composingTokens {
		if nextChar == token {
			return nextChar, nil
		}
	}

	//if not found, seek back and return the single character
	if err := p.SeekBack(1); err != nil {
		return "", err
	}
	return char, nil
}

func startsWithComposingToken(s string) bool {
	for _, token := range composingTokens {
		if token[0] == s[0] {
			return true
		}
	}
	return false
}

func (p *Parser) ClearSpaces() error {
	// read until a non-space character is found
	for {
		char, err := p.readNextChar()
		if err != nil {
			return err
		}
		if char != " " && char != "\n" && char != "\t" {
			if err := p.SeekBack(1); err != nil {
				return err
			}
			break
		}
	}
	return nil
}

func (p *Parser) RemoveLine() error {
	// read until a newline character is found
	for {
		char, err := p.readNextChar()
		if err != nil {
			return err
		}
		if char == "\n" {
			break
		}
	}
	return nil
}

func (p *Parser) NextToken() (string, error) {
	var res string
	var char string

	//clear spaces before reading the next token
	p.ClearSpaces()
	if p.file == nil {
		return "", os.ErrInvalid
	}

	//reads first character to check if it is a composing token
	char, err := p.readNextChar()
	if err != nil {
		return "", err
	}
	if s, err := p.isComposingToken(char); s != "" && err == nil {
		if s == "//" {
			p.RemoveLine()
			return p.NextToken()
		}
		return s, nil
	}
	p.SeekBack(1)

	for {
		char, err := p.readNextChar()
		if err != nil {
			if err == io.EOF {
				return res, io.EOF
			}
			return "", err
		}

		//read strings
		if char == "\"" {
			for {
				nextChar, err := p.readNextChar()
				if err != nil {
					return "", err
				}
				if nextChar == "\"" {
					break
				}
				res += nextChar
			}
			return "\"" + res + "\"", nil
		}

		//if found a composing token, seek back and return the accumulated string "ola/" found the "/" return "ola"
		if startsWithComposingToken(char) {
			if res != "" {
				if err := p.SeekBack(1); err != nil {
					return "", err
				}
				return res, nil
			}
		}

		if char == " " || char == "\n" || char == "\t" {
			_ = p.SeekBack(1) // não comer o separador
			break
		}
		res += char
	}
	return res, nil
}

func (p *Parser) Peek() (string, error) {
	if p.file == nil {
		return "", os.ErrInvalid
	}

	// guarda posição e linha
	pos, err := p.file.Seek(0, io.SeekCurrent)
	if err != nil {
		return "", err
	}
	line := p.LineNumber

	token, err := p.NextToken()
	if err != nil && err != io.EOF {
		return "", err
	}

	// repõe posição e linha
	if _, sErr := p.file.Seek(pos, io.SeekStart); sErr != nil {
		return "", sErr
	}
	p.LineNumber = line

	return token, nil
}

func (p *Parser) Mark() (int64, error) {
	return p.file.Seek(0, io.SeekCurrent)
}

func (p *Parser) Restore(mark int64) error {
	_, err := p.file.Seek(mark, io.SeekStart)
	return err
}

func eatEqual(parser *Parser) {
	//parse = else panic
	equal, err := parser.NextToken()
	if err != nil {
		panic("Error eating equal: " + err.Error())
	}
	if equal != "=" {
		panic(fmt.Sprintf("Expected '=', got '%s' on line %d", equal, parser.LineNumber))
	}
}

// (
func eatFirstBrace(parser *Parser) {
	//parse ( else panic
	firstBrace, err := parser.NextToken()
	if err != nil {
		panic("Error eating first brace: " + err.Error())
	}
	if firstBrace != "(" {
		panic(fmt.Sprintf("Expected '(', got '%s' on line %d", firstBrace, parser.LineNumber))
	}
}

func eatLastBrace(parser *Parser) {
	//parse ) else panic
	lastBrace, err := parser.NextToken()
	if err != nil {
		panic("Error eating last brace: " + err.Error())
	}
	if lastBrace != ")" {
		panic(fmt.Sprintf("Expected ')', got '%s' on line %d", lastBrace, parser.LineNumber))
	}
}

func eatFirstCurlBrace(parser *Parser) {
	//parse { else panic
	firstCurlBrace, err := parser.NextToken()
	if err != nil {
		panic("Error eating first curl brace: " + err.Error())
	}
	if firstCurlBrace != "{" {
		panic(fmt.Sprintf("Expected '{', got '%s' on line %d", firstCurlBrace, parser.LineNumber))
	}
}

func eatLastCurlBrace(parser *Parser) {
	//parse } else panic
	lastCurlBrace, err := parser.NextToken()
	if err != nil {
		panic("Error eating last curl brace: " + err.Error())
	}
	if lastCurlBrace != "}" {
		panic(fmt.Sprintf("Expected '}', got '%s' on line %d", lastCurlBrace, parser.LineNumber))
	}
}

func eatSemicolon(parser *Parser) {
	//parse ; else panic
	semicolon, err := parser.NextToken()
	if err != nil {
		panic("Error eating semicolon: " + err.Error())
	}
	if semicolon != ";" {
		panic(fmt.Sprintf("Expected ';', got '%s' on line %d", semicolon, parser.LineNumber))
	}
}

func eatSymbol(parser *Parser, symbol string) {
	//parse symbol else panic
	foundSymbol, err := parser.NextToken()
	if err != nil {
		panic("Error eating symbol: " + err.Error())
	}
	if foundSymbol != symbol {
		panic(fmt.Sprintf("Expected '%s', got '%s' on line %d", symbol, foundSymbol, parser.LineNumber))
	}
}

// Reserved scratch registers to avoid clobber across recursive parses
var reservedTmpRegs []byte

func pushReserved(regs ...byte) {
	reservedTmpRegs = append(reservedTmpRegs, regs...)
}

func popReserved(n int) {
	if n <= 0 {
		return
	}
	if n > len(reservedTmpRegs) {
		reservedTmpRegs = reservedTmpRegs[:0]
		return
	}
	reservedTmpRegs = reservedTmpRegs[:len(reservedTmpRegs)-n]
}

func isReserved(r byte) bool {
	for i := len(reservedTmpRegs) - 1; i >= 0; i-- { // check from top for locality
		if reservedTmpRegs[i] == r {
			return true
		}
	}
	// Never use special regs for temps
	switch r {
	case byte(backend.REG_RAX), byte(backend.REG_RDX), byte(backend.REG_RSP), byte(backend.REG_RBP):
		return true
	}
	return false
}

func pickTmpReg(dest byte) (byte, error) {
	candidates := []byte{
		byte(backend.REG_R8),
		byte(backend.REG_R9),
		byte(backend.REG_R10),
		byte(backend.REG_R11),
		byte(backend.REG_R12),
		byte(backend.REG_R13),
		byte(backend.REG_R14),
		byte(backend.REG_R15),
	}
	for _, c := range candidates {
		if c != dest && !isReserved(c) {
			return c, nil
		}
	}
	return 0, fmt.Errorf("no available temporary registers, dont use that much (...(...(...(...(...))))), the limit is 8 levels")
}

func parseConditionals(token string, reg byte, tmp byte) error {
	falseLabel := fmt.Sprintf("equal_false_%d", labelCounter)
	trueLabel := fmt.Sprintf("equal_true_%d", labelCounter)
	endLabel := fmt.Sprintf("equal_end_%d", labelCounter)

	backend.Cmp64_r_r(reg, tmp)
	switch token {
	case "==":
		backend.Jcc(trueLabel, byte(backend.JE_OPCODE))
	case "!=":
		backend.Jcc(trueLabel, byte(backend.JNE_OPCODE))
	case "<":
		backend.Jcc(trueLabel, byte(backend.JG_OPCODE))
	case "<=":
		backend.Jcc(trueLabel, byte(backend.JGE_OPCODE))
	case ">":
		backend.Jcc(trueLabel, byte(backend.JL_OPCODE))
	case ">=":
		backend.Jcc(trueLabel, byte(backend.JLE_OPCODE))
	default:
		return fmt.Errorf("unexpected token '%s' in conditional expression", token)
	}

	backend.Create_label(falseLabel)
	backend.Mov64_r_i(reg, 0)
	backend.Jmp(endLabel)

	backend.Create_label(trueLabel)
	backend.Mov64_r_i(reg, 1)
	backend.Create_label(endLabel)
	return nil
}

// uses RDX and funnels dividend through RAX for division
func getUntilSymbol(parser *Parser, stopSymbol []string, reg byte) (error, *string, bool) {
	// Temporary register for RHS values in binary operations; must differ from reg
	tmp, err := pickTmpReg(reg)
	if err != nil {
		return err, nil, false
	}

	// Reserve our accumulator so nested calls won't use it as a temp
	pushReserved(reg)
	defer popReserved(1)

	token, err := parser.NextToken()
	if err != nil {
		return err, nil, false
	}

	for _, stop := range stopSymbol {
		if token == stop {
			return nil, &token, false
		}
	}

	err = getValueFromToken(parser, token, reg)
	if err != nil {
		panic("Error getting value from token in line " + fmt.Sprintf("%d: %s", parser.LineNumber, err.Error()))
	}

	for {
		//can be symbol or ; to signal end of expression
		symbol, err := parser.NextToken()
		if err != nil {
			return err, nil, false
		}

		for _, stop := range stopSymbol {
			if symbol == stop {
				return nil, &symbol, true
			}
		}

		token, err = parser.NextToken()
		if err != nil {
			return err, nil, false
		}

		// Compute RHS into our temporary register
		pushReserved(tmp)
		err = getValueFromToken(parser, token, tmp)
		popReserved(1)
		if err != nil {
			panic("Error getting value from token in line " + fmt.Sprintf("%d: %s", parser.LineNumber, err.Error()))
		}

		fmt.Println(symbol)
		switch symbol {
		case "+":
			backend.Sum64_r_r(reg, tmp)
		case "-":
			backend.Sub64_r_r(reg, tmp)
		case "*":
			backend.Mul64_r_r(reg, tmp)
		case "/":
			if reg != byte(backend.REG_RAX) {
				backend.Mov64_r_r(byte(backend.REG_RAX), reg)
			}
			backend.Xor64_r_r(byte(backend.REG_RDX), byte(backend.REG_RDX)) // clear RDX before division
			backend.Div64_r(tmp)
			if reg != byte(backend.REG_RAX) {
				backend.Mov64_r_r(reg, byte(backend.REG_RAX))
			}
		case "&":
			backend.And64_r_r(reg, tmp)
		case "|":
			backend.Or64_r_r(reg, tmp)
		case "^":
			// XOR operation
			backend.Xor64_r_r(reg, tmp)
		case "&&":
			// Use global counter for unique labels
			labelCounter++
			falseLabel := fmt.Sprintf("logical_and_false_%d", labelCounter)
			endLabel := fmt.Sprintf("logical_and_end_%d", labelCounter)

			backend.Cmp64_r_i(reg, 0)
			backend.Jcc(falseLabel, byte(backend.JE_OPCODE))

			backend.Cmp64_r_i(tmp, 0)
			backend.Jcc(falseLabel, byte(backend.JE_OPCODE))

			backend.Mov64_r_i(reg, 1)
			backend.Jmp(endLabel)

			backend.Create_label(falseLabel)
			backend.Mov64_r_i(reg, 0)

			backend.Create_label(endLabel)
		case "||":
			// Use global counter for unique labels
			labelCounter++
			trueLabel := fmt.Sprintf("logical_or_true_%d", labelCounter)
			endLabel := fmt.Sprintf("logical_or_end_%d", labelCounter)

			backend.Cmp64_r_i(reg, 0)
			backend.Jcc(trueLabel, byte(backend.JNE_OPCODE))

			backend.Cmp64_r_i(tmp, 0)
			backend.Jcc(trueLabel, byte(backend.JNE_OPCODE))

			backend.Mov64_r_i(reg, 0)
			backend.Jmp(endLabel)

			backend.Create_label(trueLabel)
			backend.Mov64_r_i(reg, 1)

			backend.Create_label(endLabel)
		case "==", "!=", "<", "<=", ">", ">=":
			labelCounter++
			if err := parseConditionals(symbol, reg, tmp); err != nil {
				return err, nil, false
			}
		case "<<":
			//move to cl the ammount to shift
			backend.Mov64_r_r(byte(backend.REG_RCX), tmp)
			backend.Lshfit64_reg(reg)
		case ">>":
			backend.Mov64_r_r(byte(backend.REG_RCX), tmp)
			backend.Rshfit64_reg(reg)
		default:
			fmt.Printf("Current line is %d\n", parser.LineNumber)
			return fmt.Errorf("unknown symbol '%s' found at line %d", symbol, parser.LineNumber), nil, false
		}
	}
}

// returns in RAX or similar the value after the equal sign
func getAfterEqual(parser *Parser) error {
	//parse token, parse symbol (+-*/ etc), parse token.... til ;
	eatEqual(parser)
	err, _, _ := getUntilSymbol(parser, []string{";"}, byte(backend.REG_RAX))
	return err
}

func includeFile(parser *Parser) error {
	eatFirstBrace(parser)
	filename, err := parser.NextToken()
	if err != nil {
		return err
	}
	eatLastBrace(parser)

	var par Parser
	err = par.StartParse(filename[1 : len(filename)-1])
	if err != nil {
		return err
	}

	return StartParsing(&par)
}

func StartParsing(parser *Parser) error {
	if parser.file == nil {
		return os.ErrInvalid
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
		case "func":
			err := createFunc(parser)
			if err != nil {
				return err
			}
		case "global":
			_, err := checkPublicVars(parser)
			if err != nil {
				return err
			}
		case "struct":
			err := createStructType(parser)
			if err != nil {
				return err
			}
		case "include":
			err := includeFile(parser)
			if err != nil {
				return err
			}

		default:

			return fmt.Errorf("unknown token: '%s' found at line %d", token, parser.LineNumber)
		}
	}
}
