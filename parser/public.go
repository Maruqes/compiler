package parser

import (
	"fmt"

	backend "github.com/Maruqes/compiler/swig"
)

type PublicVar struct {
	name string

	size int
}

type PublicVarList struct {
	vars []PublicVar
}

var publicVarList PublicVarList

func (p *PublicVarList) AddVar(name string, size int) {
	p.vars = append(p.vars, PublicVar{name: name, size: size})
}

func (p *PublicVarList) GetVars() []PublicVar {
	return p.vars
}

func (p *PublicVarList) GetVar(name string) (PublicVar, bool) {
	for _, v := range p.vars {
		if v.name == name {
			return v, true
		}
	}
	return PublicVar{}, false
}

func getPublicVar(token string, reg byte) (bool, error) {
	varPublic, exists := publicVarList.GetVar(token)
	if !exists {
		return false, fmt.Errorf("global variable '%s' not found", token)
	}

	backend.Create_variable_reference(token, reg)
	backend.Mov64_r_m(reg, reg)
	clearReg(reg, varPublic.size)

	return true, nil

}

func setPublicVar(token string, reg byte) (bool, error) {
	if reg == byte(backend.REG_RBX) {
		return false, fmt.Errorf("cannot set global variable with RBX register")
	}

	_, exists := publicVarList.GetVar(token)
	if !exists {
		return false, fmt.Errorf("global variable '%s' not found", token)
	}

	backend.Create_variable_reference(token, byte(backend.REG_RBX))
	backend.Mov64_m_r(byte(backend.REG_RBX), reg)
	return true, nil
}

func doesPublicVarExist(token string) bool {
	_, exists := publicVarList.GetVar(token)
	return exists
}

func returnPointerFromPublicVar(token string, reg byte) (bool, error) {
	_, exists := publicVarList.GetVar(token)
	if !exists {
		return false, fmt.Errorf("global variable '%s' not found", token)
	}
	backend.Create_variable_reference(token, reg)
	return true, nil
}

func createPublicVar(parser *Parser, size int) error {

	varList := GetVarList(SCOPE)

	if varList == nil {
		return fmt.Errorf("Variable list for scope '%s' not found", SCOPE)
	}

	if !isType(size) {
		return fmt.Errorf("invalid type token '%d' on line %d", size, parser.LineNumber)
	}

	name, err := parser.NextToken()
	if err != nil {
		return err
	}

	buf := make([]byte, size)
	for i := range buf {
		buf[i] = '0'
	}

	backend.Add_string_constant(name, string(buf))
	err = getAfterEqual(parser)
	if err != nil {
		return err
	}

	backend.Create_variable_reference(name, byte(backend.REG_RSI))
	switch size {
	case DQ:
		backend.Mov64_m_r(byte(backend.REG_RSI), byte(backend.REG_RAX))
	case DD:
		backend.Mov32_m_r(byte(backend.REG_RSI), byte(backend.REG_EAX))
	case DW:
		backend.Mov16_m_r(byte(backend.REG_RSI), byte(backend.REG_AX))
	case DB:
		backend.Mov8_m_r(byte(backend.REG_RSI), byte(backend.REG_AL))
	}

	publicVarList.AddVar(name, size)
	_, err = varList.AddVariable(name, size, nil, ORIGIN_GLOBAL)
	if err != nil {
		return err
	}
	return nil
}

func checkPublicVars(parser *Parser) (bool, error) {

	name := "global"
	setScope(name)
	vl := GetVarList(name)
	if vl == nil {
		CreateVarList(name)
	} else {
		return false, fmt.Errorf("variable list for scope '%s' already exists", name)
	}

	backend.Create_label(name)
	CreateStack()
	functions = append(functions, FunctionType{Name: name, Params: nil})
	eatFirstCurlBrace(parser)

	typeTokenStr, err := parser.NextToken()
	if err != nil {
		return true, err
	}

	for isTypeToken(typeTokenStr) {
		typeToken, err := getTypeFromToken(typeTokenStr)
		if err != nil {
			return false, err
		}

		err = createPublicVar(parser, typeToken)
		if err != nil {
			return false, err
		}

		typeTokenStr, err = parser.NextToken()
		if err != nil {
			return false, err
		}

	}

	if typeTokenStr != "}" {
		return false, fmt.Errorf("expected '}' after global variables, got '%s' at line %d", typeTokenStr, parser.LineNumber)
	}

	LeaveStack()
	return false, nil
}
