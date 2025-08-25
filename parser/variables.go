package parser

import (
	"fmt"
	"strings"

	backend "github.com/Maruqes/compiler/swig"
	"github.com/Maruqes/compiler/wrapper"
)

const GLOBAL_SCOPE = "global"

var SCOPE = GLOBAL_SCOPE

func setScope(scope string) {
	SCOPE = scope
}

// conjunto de tipos de origens, ficheiro, etc etc
type OriginType int

const (
	ORIGIN_RBP OriginType = iota
	ORIGIN_STRUCT
	ORIGIN_GLOBAL
)

type Variable struct {
	Name     string
	Type     int // DQ for 64-bit, DB for 8-bit, etc.
	Origin   OriginType
	Position int // relative to RBP
	Extra    any
}

type VarsList struct {
	vars    []Variable
	lastPos int // last position used in the stack
	Scope   string
}

var VarList []VarsList

func CreateVarList(scope string) {
	// If the scope already exists, do nothing.
	if GetVarList(scope) != nil {
		panic(fmt.Sprintf("Variable list for scope '%s' already exists", scope))
	}

	varListGlobal := GetVarList("global")
	if varListGlobal == nil {
		varListGlobal = &VarsList{
			vars:    make([]Variable, 0),
			lastPos: 0,
			Scope:   "global",
		}
	}

	// Create a fresh variable list for the given scope.
	varList := &VarsList{
		vars:    varListGlobal.vars,
		lastPos: 0,
		Scope:   scope,
	}

	VarList = append(VarList, *varList)
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

func SumStack(n int) {
	varL := GetVarList(SCOPE)
	if varL == nil {
		panic(fmt.Sprintf("Variable list for scope '%s' not found", SCOPE))
	}
	varL.lastPos += n
	backend.Sum64_r_i(byte(backend.REG_RSP), uint(n))
}

func SubStack(n int) {
	varL := GetVarList(SCOPE)
	if varL == nil {
		panic(fmt.Sprintf("Variable list for scope '%s' not found", SCOPE))
	}
	varL.lastPos -= n
	backend.Sub64_r_i(byte(backend.REG_RSP), uint64(n))
}

// var should be in rax
func (vl *VarsList) AddVariable(name string, varType int, extra any, origin OriginType) error {
	// PushStack64(byte(backend.REG_RAX))

	if origin == ORIGIN_RBP {
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
	}
	vl.vars = append(vl.vars, Variable{
		Name:     name,
		Type:     varType,
		Position: vl.lastPos,
		Extra:    extra,
		Origin:   origin,
	})

	fmt.Println(vl.vars)

	return nil
}

// sets the variable to the value in RAX
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
	//in case of global var return it
	if doesPublicVarExist(name) {
		getPublicVar(name, reg)
		return nil
	}
	for _, v := range vl.vars {
		if v.Name == name {
			switch v.Type {
			case DB:
				backend.Mov8_r_mi(reg, byte(backend.REG_RBP), int(v.Position))
			case DW:
				backend.Mov16_r_mi(reg, byte(backend.REG_RBP), int(v.Position))
			case DD:
				backend.Mov32_r_mi(reg, byte(backend.REG_RBP), int(v.Position))
			case DQ:
				backend.Mov64_r_mi(reg, byte(backend.REG_RBP), int(v.Position))
			}
			return nil
		}
	}
	return fmt.Errorf("Variable %s not found in scope1 %s in line", name, SCOPE)
}

func (vl *VarsList) GetVariableStruct(name string) (*Variable, error) {
	for _, v := range vl.vars {
		if v.Name == name {
			return &v, nil
		}
	}
	return nil, fmt.Errorf("Variable %s not found in scope2 %s", name, SCOPE)
}

// need to check for
// normal vars   "varName"
// struct vars   "structName.fieldName"
// global vars   "globalVarName"
func (vl *VarsList) GetVariableAddress(name string, reg byte) error {

	varName := name
	fullVarName := name
	//checking . usage for struct vars   struct.field1
	if strings.Contains(name, ".") {
		//divide and get first part to name
		parts := strings.Split(name, ".")
		if len(parts) != 2 {
			return fmt.Errorf("invalid struct variable name: %s", name)
		}
		varName = parts[0]
	}

	varStruct, err := vl.GetVariableStruct(varName)
	if err != nil {
		return err
	}

	switch varStruct.Origin {
	case ORIGIN_RBP:
		backend.Mov64_r_i(reg, uint64(varStruct.Position))
		backend.Sum64_r_r(reg, byte(backend.REG_RBP))
	case ORIGIN_STRUCT:
		parsed, err := getPointerOfStruct(fullVarName, reg)
		if err != nil && parsed {
			return err
		}
	case ORIGIN_GLOBAL:
		_, err := returnPointerFromPublicVar(name, reg)
		if err != nil {
			return err
		}
	}

	return nil
}

func (vl *VarsList) DoesVarExist(name string) bool {
	for _, v := range vl.vars {
		if v.Name == name {
			return true
		}
	}
	return false
}

func (vl *VarsList) incrementVar(parser *Parser, variable *Variable) error {
	//consume the token
	_, err := parser.NextToken()
	if err != nil {
		return err
	}

	switch variable.Type {
	case DB:
		backend.Inc8_mi(byte(backend.REG_RBP), uint(variable.Position))
	case DW:
		backend.Inc16_mi(byte(backend.REG_RBP), uint(variable.Position))
	case DD:
		backend.Inc32_mi(byte(backend.REG_RBP), uint(variable.Position))
	case DQ:
		backend.Inc64_mi(byte(backend.REG_RBP), uint(variable.Position))
	}

	eatSemicolon(parser)
	return nil
}

func (vl *VarsList) decrementVar(parser *Parser, variable *Variable) error {
	_, err := parser.NextToken()
	if err != nil {
		return err
	}
	switch variable.Type {
	case DB:
		backend.Dec8_mi(byte(backend.REG_RBP), uint(variable.Position))
	case DW:
		backend.Dec16_mi(byte(backend.REG_RBP), uint(variable.Position))
	case DD:
		backend.Dec32_mi(byte(backend.REG_RBP), uint(variable.Position))
	case DQ:
		backend.Dec64_mi(byte(backend.REG_RBP), uint(variable.Position))
	}
	eatSemicolon(parser)
	return nil
}

func (vl *VarsList) sumVar(parser *Parser, varName string, variable *Variable) error {
	_, err := parser.NextToken()
	if err != nil {
		return err
	}

	// First, load the current variable value into RAX
	err = vl.GetVariable(varName, byte(backend.REG_RAX))
	if err != nil {
		return fmt.Errorf("Eerror getting variable %s in line %d: %s", varName, parser.LineNumber, err.Error())
	}

	// Parse the expression to add and put result in RBX
	err, _, parsed := getUntilSymbol(parser, []string{";"}, byte(backend.REG_RBX))
	if !parsed {
		return fmt.Errorf("error parsing sum variable in line %d", parser.LineNumber)
	}

	// Add the values based on variable type
	switch variable.Type {
	case DB:
		backend.Sum8_r_r(byte(backend.REG_RAX), byte(backend.REG_RBX))
	case DW:
		backend.Sum16_r_r(byte(backend.REG_RAX), byte(backend.REG_RBX))
	case DD:
		backend.Sum32_r_r(byte(backend.REG_RAX), byte(backend.REG_RBX))
	case DQ:
		backend.Sum64_r_r(byte(backend.REG_RAX), byte(backend.REG_RBX))
	}

	// Clear the register based on variable type and store result
	clearReg(byte(backend.REG_RAX), variable.Type)
	vl.SetVar(variable)

	return nil
}

func (vl *VarsList) subVar(parser *Parser, varName string, variable *Variable) error {
	_, err := parser.NextToken()
	if err != nil {
		return err
	}

	// First, load the current variable value into RAX
	err = vl.GetVariable(varName, byte(backend.REG_RAX))
	if err != nil {
		return fmt.Errorf("error getting variable %s in line %d: %s", varName, parser.LineNumber, err.Error())
	}

	// Parse the expression to subtract and put result in RBX
	err, _, parsed := getUntilSymbol(parser, []string{";"}, byte(backend.REG_RBX))
	if !parsed {
		return fmt.Errorf("error parsing sub variable in line %d", parser.LineNumber)
	}

	// Subtract the values based on variable type
	switch variable.Type {
	case DB:
		backend.Sub8_r_r(byte(backend.REG_RAX), byte(backend.REG_RBX))
	case DW:
		backend.Sub16_r_r(byte(backend.REG_RAX), byte(backend.REG_RBX))
	case DD:
		backend.Sub32_r_r(byte(backend.REG_RAX), byte(backend.REG_RBX))
	case DQ:
		backend.Sub64_r_r(byte(backend.REG_RAX), byte(backend.REG_RBX))
	}

	// Clear the register based on variable type and store result
	clearReg(byte(backend.REG_RAX), variable.Type)
	vl.SetVar(variable)

	return nil
}

func (vl *VarsList) setVarStruct(parser *Parser, varName string) error {

	fmt.Println("Setting variable:", varName)
	variable, err := vl.GetVariableStruct(varName)
	if err != nil {
		return err
	}
	peekToken, err := parser.Peek()
	if err != nil {
		return err
	}
	switch peekToken {
	case "=":
		err = getAfterEqual(parser)
		if err != nil {
			return err
		}

		vl.SetVar(variable)

		clearReg(byte(backend.REG_RAX), variable.Type)
	case "++":
		return vl.incrementVar(parser, variable)
	case "--":
		return vl.decrementVar(parser, variable)
	case "+=":
		return vl.sumVar(parser, varName, variable)
	case "-=":
		return vl.subVar(parser, varName, variable)
	default:
		return fmt.Errorf("unexpected token '%s' after '--' in line %d", peekToken, parser.LineNumber)
	}

	return nil
}

func createPointerVar(parser *Parser) error {
	name, err := parser.NextToken()
	if err != nil {
		return err
	}

	varList := GetVarList(SCOPE)
	if varList == nil {
		return fmt.Errorf("Variable list for scope '%s' not found", SCOPE)
	}

	if varList.DoesVarExist(name) {
		return fmt.Errorf("Variable '%s' already exists in scope '%s' in line %d", name, SCOPE, parser.LineNumber)
	}

	// get the value after the equal sign in RAX
	if err := getAfterEqual(parser); err != nil {
		return err
	}

	err = varList.AddVariable(name, DQ, nil, ORIGIN_RBP)
	if err != nil {
		return err
	}

	return nil
}

// push whats after the equal and save position in the stack compared to BASE POINTER (BP)
func createVarStruct(parser *Parser, varType int, extra any) error {
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
		return fmt.Errorf("Variable '%s' already exists in scope '%s' in line %d", name, SCOPE, parser.LineNumber)
	}

	// get the value after the equal sign in RAX
	if err := getAfterEqual(parser); err != nil {
		return err
	}

	clearReg(byte(backend.REG_RAX), varType)

	err = varList.AddVariable(name, varType, extra, ORIGIN_RBP)
	if err != nil {
		return err
	}
	return nil
}

func createVarWithReg(parser *Parser, reg byte, varType int, name string, extra any, origin OriginType) error {
	varList := GetVarList(SCOPE)
	if varList == nil {
		return fmt.Errorf("Variable list for scope '%s' not found", SCOPE)
	}

	if varList.DoesVarExist(name) {
		return fmt.Errorf("Variable '%s' already exists in scope '%s' in line %d", name, SCOPE, parser.LineNumber)
	}

	if reg != byte(backend.REG_RAX) {
		backend.Mov64_r_r(byte(backend.REG_RAX), reg)
	}

	err := varList.AddVariable(name, varType, extra, origin)
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

func clearReg(reg byte, typeToken int) {
	switch typeToken {
	case DB:
		backend.And64_r_i(reg, 0x00000000000000ff)
	case DW:
		backend.And64_r_i(reg, 0x000000000000ffff)
	case DD:
		backend.Mov32_r_r(reg, reg)
	case DQ:
		return
	default:
		panic(fmt.Sprintf("Unknown type token %d in clearReg", typeToken))
	}
}
