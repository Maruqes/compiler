package parser

import (
	"fmt"

	backend "github.com/Maruqes/compiler/swig"
	"github.com/Maruqes/compiler/wrapper"
)

var SCOPE = "global"

const (
	DQ = 8
	DD = 4
	DW = 2
	DB = 1
)

func isTypeValid(varType int) bool {
	switch varType {
	case DQ, DD, DW, DB:
		return true
	default:
		return false
	}
}

type Variable struct {
	Name           string
	Type           int    // DQ for 64-bit, DB for 8-bit, etc.
	Position       int // relative to RBP
	OffsetPosition uint8  // offset relative to position (push only has 64 bit, if we save 8 bit, we add the 56 (7 bytes) offset to the position)
	Scope          string
}

type VarsList struct {
	vars    []Variable
	lastPos int // last position used in the stack
}

var VarList VarsList

func (vl *VarsList) AddVariable(name string, varType int) error {
	vl.vars = append(vl.vars, Variable{
		Name:           name,
		Type:           varType,
		Position:       vl.lastPos,
		Scope:          SCOPE,
		OffsetPosition: 8 - uint8(varType), // offset for 8-bit variables
	})
	if !isTypeValid(varType) {
		return fmt.Errorf("invalid variable type: %d", varType)
	}
	vl.lastPos -= varType // Decrease the last position by the size of the variable type
	return nil
}

func (vl *VarsList) GetVariable(name string, reg byte) error {
	for _, v := range vl.vars {
		if v.Name == name && v.Scope == SCOPE {
			// Load the variable's value into the specified register
			backend.Mov64_r_mi(reg, byte(backend.REG_RBP), int(v.Position))
			return nil
		}
	}
	return fmt.Errorf("Variable %s not found in scope %s", name, SCOPE)
}

// push whats after the equal and save position in the stack compared to BASE POINTER (BP)
func createVar(parser *Parser, varType int) error {
	//create var is get space on heap and get the after equal
	name, err := parser.NextToken()
	if err != nil {
		return err
	}

	// get the value after the equal sign in RAX
	if err := getAfterEqual(parser); err != nil {
		return err
	}

	err = VarList.AddVariable(name, varType)
	if err != nil {
		return err
	}
	backend.Push64(byte(backend.REG_RAX))
	return nil
}

// print a numeros de 48-122 ascii
func PrintVar(varName string) {
	VarList.GetVariable(varName, byte(backend.REG_RAX))
	backend.Create_variable_reference("printSave", byte(backend.REG_RSI))
	backend.Mov8_m_r(byte(backend.REG_RSI), byte(backend.REG_RAX))
	wrapper.Println("printSave", 3)
}
