package parser

import (
	"io"
	"os"
)

type Parser struct {
	file *os.File
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
			break
		}
		res += char
	}
	return res, nil
}
