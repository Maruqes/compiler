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

// contains solo char that compose predefined tokens
var composingTokens = []string{"{", "}", "(", ")", "[", "]", ";", ",", "+", "-", "*", "/", "%", "=", "!", "<", ">", "&", "|", "?", "/", "==", "!=", "<=", ">=", "&&", "||",
	"++", "--", "+=", "-=", "*=", "/=", "%=", "&=", "|=", "^=", "//"}

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
	return string(buf), nil
}

func (p *Parser) SeekBack(number int64) error {
	if p.file == nil {
		return os.ErrInvalid
	}
	_, err := p.file.Seek(-number, io.SeekCurrent)
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
			return char, nil
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
			p.lineNumber++
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
				return res, nil
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
			if char == "\n" {
				p.lineNumber++
			}
			break
		}
		res += char
	}
	return res, nil
}

// uses 64 bit registers to get the value of the token
func getValueFromToken(token string, reg byte) error {
	//detect number and variables

	//numbers
	//check if token is a number
	num, err := strconv.Atoi(token)
	if err == nil {
		//is a number
		backend.Mov64_r_i(reg, uint64(num))
		return nil
	}

	//check if token is a variable
	err = VarList.GetVariable(token, reg)
	if err == nil {
		return nil
	}

	return err
}

// returns in RAX or similar the value after the equal sign
func getAfterEqual(parser *Parser) error {
	//parse token, parse symbol (+-*/ etc), parse token.... til ;
	equal, err := parser.NextToken()
	if err != nil {
		panic("Error getting after equal: " + err.Error())
	}
	if equal != "=" {
		return fmt.Errorf("Expected '=', got '%s' on line %d", equal, parser.lineNumber)
	}

	token, err := parser.NextToken()
	if err != nil {
		return err
	}

	err = getValueFromToken(token, byte(backend.REG_RAX))
	if err != nil {
		panic("Error getting value from token: " + err.Error())
	}

	for {
		//can be symbol or ; to signal end of expression
		symbol, err := parser.NextToken()
		if err != nil {
			return err
		}

		if symbol == ";" {
			return nil
		}

		//if not ";" it must be (+-*/etc) so we get whats after the symbol
		token, err = parser.NextToken()
		if err != nil {
			return err
		}

		err = getValueFromToken(token, byte(backend.REG_RBX))
		if err != nil {
			panic("Error getting value from token: " + err.Error())
		}

		switch symbol {
		case "+":
			// handle addition
			backend.Sum64_r_r(byte(backend.REG_RAX), byte(backend.REG_RBX))
		case "-":
			// handle subtraction
			backend.Sub64_r_r(byte(backend.REG_RAX), byte(backend.REG_RBX))
		case "*":
			// handle multiplication
			backend.Mul64_r_r(byte(backend.REG_RAX), byte(backend.REG_RBX))
		case "/":
			// handle division
			backend.Xor64_r_r(byte(backend.REG_RDX), byte(backend.REG_RDX)) // clear RDX before division
			backend.Div64_r(byte(backend.REG_RBX))
		default:
			fmt.Printf("Current line is %d\n", parser.lineNumber)
			panic("Unknown symbol after equal: " + symbol)
		}
	}
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
			createFunc(parser)
		default:
			return fmt.Errorf("Unknown token: %s", token)
		}
	}
}
