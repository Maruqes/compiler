package parser

import (
	"fmt"

	backend "github.com/Maruqes/compiler/swig"
)

var SCOPE = "global"

type Variable struct {
	Name     string
	Type     string // "dq" for 64-bit, "db" for 8-bit, etc.
	Position uint64 // relative to RBP
	Scope    string
}

type VarsList struct {
	vars    []Variable
	lastPos uint64 // last position used in the stack
}

var VarList VarsList

func (vl *VarsList) AddVariable(name, varType string) {
	vl.vars = append(vl.vars, Variable{
		Name:     name,
		Type:     varType,
		Position: vl.lastPos,
		Scope:    SCOPE,
	})
	switch varType {
	case "dq":
		vl.lastPos += 8 // 64-bit variable takes 8 bytes
	case "dd":
		vl.lastPos += 4 // 32-bit variable takes 4 bytes
	case "dw":
		vl.lastPos += 2 // 16-bit variable takes 2 bytes
	case "db":
		vl.lastPos += 1 // 8-bit variable takes 1 byte
	default:
		panic("Unknown variable type: " + varType)
	}

}

func (vl *VarsList) GetVariable(name string, reg byte) error {
	for _, v := range vl.vars {
		if v.Name == name && v.Scope == SCOPE {
			// Load the variable's value into the specified register
			backend.Mov64_r_mi(reg, byte(backend.REG_BP), int(v.Position))
			return nil
		}
	}
	return fmt.Errorf("Variable %s not found in scope %s", name, SCOPE)
}

// push whats after the equal and save position in the stack compared to BASE POINTER (BP)
func createVar64(parser *Parser) error {
	//create var is get space on heap and get the after equal
	name, err := parser.NextToken()
	if err != nil {
		return err
	}

	// get the value after the equal sign in RAX
	if err := getAfterEqual(parser); err != nil {
		return err
	}

	VarList.AddVariable(name, "dq")
	backend.Push64(byte(backend.REG_RAX))
	return nil
}
