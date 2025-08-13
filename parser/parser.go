package parser

import (
	"fmt"
	"io"
	"os"
	"strconv"

	backend "github.com/Maruqes/compiler/swig"
)

type Parser struct {
	file       *os.File
	lineNumber int
}

var comparisonOperators = []string{"==", "!=", "<=", ">=", "<", ">"}

// contains solo char that compose predefined tokens
var composingTokens = append([]string{"{", "}", "(", ")", "[", "]", ";", ",", "+", "-", "*", "%", "=", "!", "&", "|", "?", "/",
	"++", "--", "+=", "-=", "*=", "/=", "%=", "&=", "|=", "^=", "//"}, comparisonOperators...)

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
		p.lineNumber++
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
			p.lineNumber--
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
			break
		}
		res += char
	}
	return res, nil
}

// uses 64 bit registers to get the value of the token
func getValueFromToken(parser *Parser, token string, reg byte) error {
	//detect number and variables
	varList := GetVarList(SCOPE)
	if varList == nil {
		return fmt.Errorf("Variable list for scope '%s' not found", SCOPE)
	}

	// xFFFFFFFF hex
	if len(token) > 2 {
		if token[0] == '0' && token[1] == 'x' {
			// parse hex number
			num, err := strconv.ParseUint(token[2:], 16, 64)
			if err == nil {
				backend.Mov64_r_i(reg, num)
				return nil
			}
		}
	}

	if len(token) >= 1 {
		switch token[0] {
		case '&':
			//create pointer
			token, err := parser.NextToken()
			if err != nil {
				return err
			}

			//check if token is a variable
			varStruct, err := varList.GetVariableStruct(token, reg)
			if err != nil {
				return err
			}

			backend.Mov64_r_i(reg, uint64(varStruct.Position))
			backend.Sum64_r_r(reg, byte(backend.REG_RBP))

			return nil
		case '*':
			numberOfDereferences := 1
			//create pointer
			token, err := parser.NextToken()
			if err != nil {
				return err
			}

			for token == "*" {
				numberOfDereferences++
				token, err = parser.NextToken()
				if err != nil {
					return err
				}
			}

			//check if token is a variable
			err = varList.GetVariable(token, reg)
			if err != nil {
				return err
			}

			for i := 0; i < numberOfDereferences; i++ {
				backend.Mov64_r_m(reg, reg)
			}
			return nil
		}
	}

	//numbers
	//check if token is a number
	num, err := strconv.Atoi(token)
	if err == nil {
		//is a number
		backend.Mov64_r_i(reg, uint64(num))
		return nil
	}

	//check if token is a variable
	err = varList.GetVariable(token, reg)
	if err == nil {
		return nil
	}

	if isFunctionCall(token) {
		err := parseFunctionCall(parser, token)
		if err != nil {
			return err
		}
		return nil
	}

	return err
}

func eatEqual(parser *Parser) {
	//parse = else panic
	equal, err := parser.NextToken()
	if err != nil {
		panic("Error eating equal: " + err.Error())
	}
	if equal != "=" {
		panic(fmt.Sprintf("Expected '=', got '%s' on line %d", equal, parser.lineNumber))
	}
}

func eatSemicolon(parser *Parser) {
	//parse ; else panic
	semicolon, err := parser.NextToken()
	if err != nil {
		panic("Error eating semicolon: " + err.Error())
	}
	if semicolon != ";" {
		panic(fmt.Sprintf("Expected ';', got '%s' on line %d", semicolon, parser.lineNumber))
	}
}

// uses RBX and RDX in division
func getUntilSymbol(parser *Parser, stopSymbol []string, reg byte) (error, *string, bool) {
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
		panic("Error getting value from token in line " + fmt.Sprintf("%d: %s", parser.lineNumber, err.Error()))
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

		err = getValueFromToken(parser, token, byte(backend.REG_RBX))
		if err != nil {
			panic("Error getting value from token in line " + fmt.Sprintf("%d: %s", parser.lineNumber, err.Error()))
		}

		switch symbol {
		case "+":
			// handle addition
			backend.Sum64_r_r(reg, byte(backend.REG_RBX))
		case "-":
			// handle subtraction
			backend.Sub64_r_r(reg, byte(backend.REG_RBX))
		case "*":
			// handle multiplication
			backend.Mul64_r_r(reg, byte(backend.REG_RBX))
		case "/":
			// handle division
			backend.Xor64_r_r(byte(backend.REG_RDX), byte(backend.REG_RDX)) // clear RDX before division
			backend.Div64_r(byte(backend.REG_RBX))
		default:
			fmt.Printf("Current line is %d\n", parser.lineNumber)
			return fmt.Errorf("Unknown symbol '%s' found at line %d", symbol, parser.lineNumber), nil, false
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
		default:
			return fmt.Errorf("Unknown token: '%s' found at line %d", token, parser.lineNumber)
		}
	}
}

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
