package parser

import (
	"fmt"

	backend "github.com/Maruqes/compiler/swig"
	"github.com/Maruqes/compiler/wrapper"
)

const GLOBAL_SCOPE = "global"

var SCOPE = GLOBAL_SCOPE

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

func setScope(scope string) {
	SCOPE = scope
}

type Variable struct {
	Name     string
	Type     int // DQ for 64-bit, DB for 8-bit, etc.
	Position int // relative to RBP
}

type VarsList struct {
	vars    []Variable
	lastPos int // last position used in the stack
	Scope   string
}

var VarList []VarsList

func CreateVarList(scope string) {
	VarList = append(VarList, VarsList{
		vars:    make([]Variable, 0),
		lastPos: 0,
		Scope:   scope,
	})
}

func GetVarList(scope string) *VarsList {
	for i := range VarList {
		if VarList[i].Scope == scope {
			return &VarList[i]
		}
	}
	return nil
}

func PushStack64(reg byte) {
	backend.Push64(reg)
	varL := GetVarList(SCOPE)
	if varL == nil {
		panic(fmt.Sprintf("Variable list for scope '%s' not found", SCOPE))
	}
	varL.lastPos -= 8
}

func PopStack64(reg byte) {
	backend.Pop64(reg)
	varL := GetVarList(SCOPE)
	if varL == nil {
		panic(fmt.Sprintf("Variable list for scope '%s' not found", SCOPE))
	}
	varL.lastPos += 8
}

// var should be in rax
func (vl *VarsList) AddVariable(name string, varType int) error {
	// PushStack64(byte(backend.REG_RAX))

	switch varType {
	case DB:
		backend.Sub64_r_i(byte(backend.REG_RSP), 1)
		backend.Mov8_m_r(byte(backend.REG_RSP), byte(backend.REG_RAX))
		vl.lastPos -= 1
	case DW:
		backend.Sub64_r_i(byte(backend.REG_RSP), 2)
		backend.Mov16_m_r(byte(backend.REG_RSP), byte(backend.REG_RAX))
		vl.lastPos -= 2
	case DD:
		backend.Sub64_r_i(byte(backend.REG_RSP), 4)
		backend.Mov32_m_r(byte(backend.REG_RSP), byte(backend.REG_RAX))
		vl.lastPos -= 4
	case DQ:
		backend.Sub64_r_i(byte(backend.REG_RSP), 8)
		backend.Mov64_m_r(byte(backend.REG_RSP), byte(backend.REG_RAX))
		vl.lastPos -= 8
	}

	vl.vars = append(vl.vars, Variable{
		Name:     name,
		Type:     varType,
		Position: vl.lastPos,
	})

	return nil
}

func (vl *VarsList) SetVar(variable *Variable) error {
	// PushStack64(byte(backend.REG_RAX))

	switch variable.Type {
	case DB:
		backend.Mov8_mi_r(byte(backend.REG_RBP), uint(variable.Position), byte(backend.REG_RAX))
	case DW:
		backend.Mov16_mi_r(byte(backend.REG_RBP), uint(variable.Position), byte(backend.REG_RAX))
	case DD:
		backend.Mov32_mi_r(byte(backend.REG_RBP), uint(variable.Position), byte(backend.REG_RAX))
	case DQ:
		backend.Mov64_mi_r(byte(backend.REG_RBP), uint(variable.Position), byte(backend.REG_RAX))
	}

	return nil
}
func (vl *VarsList) GetVariable(name string, reg byte) error {
	for _, v := range vl.vars {
		if v.Name == name {
			fmt.Println("Loading variable:", v.Name, "of type:", v.Type, "at position:", v.Position)
			backend.Mov64_r_mi(reg, byte(backend.REG_RBP), int(v.Position))
			return nil
		}
	}
	return fmt.Errorf("Variable %s not found in scope %s", name, SCOPE)
}

func (vl *VarsList) GetVariableStruct(name string, reg byte) (*Variable, error) {
	for _, v := range vl.vars {
		if v.Name == name {
			return &v, nil
		}
	}
	return nil, fmt.Errorf("Variable %s not found in scope %s", name, SCOPE)
}

func (vl *VarsList) DoesVarExist(name string) bool {
	for _, v := range vl.vars {
		if v.Name == name {
			return true
		}
	}
	return false
}

func (vl *VarsList) setVarStruct(parser *Parser, varName string) error {
	variable, err := vl.GetVariableStruct(varName, byte(backend.REG_RAX))
	if err != nil {
		return err
	}

	err = getAfterEqual(parser)
	if err != nil {
		return err
	}

	switch variable.Type {
	case DB:
		backend.And64_r_i(byte(backend.REG_RAX), 0x00000000000000ff)
	case DW:
		backend.And64_r_i(byte(backend.REG_RAX), 0x000000000000ffff)
	case DD:
		backend.And64_r_i(byte(backend.REG_RAX), 0x00000000ffffffff)
	case DQ:
		backend.And64_r_i(byte(backend.REG_RAX), 0xffffffffffffffff)
	}

	return nil
}

// push whats after the equal and save position in the stack compared to BASE POINTER (BP)
func createVarStruct(parser *Parser, varType int) error {
	//create var is get space on heap and get the after equal
	name, err := parser.NextToken()
	if err != nil {
		return err
	}

	varList := GetVarList(SCOPE)
	if varList == nil {
		return fmt.Errorf("Variable list for scope '%s' not found", SCOPE)
	}

	if varList.DoesVarExist(name) {
		return fmt.Errorf("Variable '%s' already exists in scope '%s' in line %d", name, SCOPE, parser.lineNumber)
	}

	// get the value after the equal sign in RAX
	if err := getAfterEqual(parser); err != nil {
		return err
	}

	switch varType {
	case DB:
		backend.And64_r_i(byte(backend.REG_RAX), 0x00000000000000ff)
	case DW:
		backend.And64_r_i(byte(backend.REG_RAX), 0x000000000000ffff)
	case DD:
		backend.And64_r_i(byte(backend.REG_RAX), 0x00000000ffffffff)
	case DQ:
		backend.And64_r_i(byte(backend.REG_RAX), 0xffffffffffffffff)
	}

	err = varList.AddVariable(name, varType)
	if err != nil {
		return err
	}
	return nil
}

func createVarWithReg(parser *Parser, reg byte, varType int, name string) error {
	varList := GetVarList(SCOPE)
	if varList == nil {
		return fmt.Errorf("Variable list for scope '%s' not found", SCOPE)
	}

	if varList.DoesVarExist(name) {
		return fmt.Errorf("Variable '%s' already exists in scope '%s' in line %d", name, SCOPE, parser.lineNumber)
	}

	if reg != byte(backend.REG_RAX) {
		backend.Mov64_r_r(byte(backend.REG_RAX), reg)
	}

	err := varList.AddVariable(name, varType)
	if err != nil {
		return err
	}

	return nil
}

// print a numeros de 48-122 ascii
func PrintVar(varName string) {
	varList := GetVarList(SCOPE)
	if varList == nil {
		panic(fmt.Sprintf("Variable list for scope '%s' not found", SCOPE))
	}

	varList.GetVariable(varName, byte(backend.REG_RAX))
	backend.Create_variable_reference("printSave", byte(backend.REG_RSI))
	backend.Mov8_m_r(byte(backend.REG_RSI), byte(backend.REG_RAX))
	wrapper.Println("printSave", 3)
}
