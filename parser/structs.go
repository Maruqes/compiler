package parser

import (
	"fmt"
	"strings"

	backend "github.com/Maruqes/compiler/swig"
)

type structField struct {
	Name string
	Type int
}

type StructType struct {
	Name   string
	Fields []structField
}

var structTypes []StructType

func AddStructType(structType StructType) {
	structTypes = append(structTypes, structType)
}

func GetStructByName(name string) *StructType {
	for i := range structTypes {
		if structTypes[i].Name == name {
			return &structTypes[i]
		}
	}
	return nil
}

var ErrNotStructParam = fmt.Errorf("not a struct param")

// parseStructParam parses "structVar.field" and returns:
// - the struct variable name,
// - a pointer to the StructType,
// - the cumulative size from the target field to the end of the struct (spacer),
// - the field index within the struct.
func parseStructParam(token string) (string, *StructType, int, int, error) {
	varList := GetVarList(SCOPE)
	if varList == nil {
		return "", nil, -1, -1, fmt.Errorf("Variable list for scope '%s' not found", SCOPE)
	}

	token = strings.TrimSpace(token)
	if token == "" {
		return "", nil, -1, -1, ErrNotStructParam
	}

	parts := strings.SplitN(token, ".", 2)
	if len(parts) != 2 {
		return "", nil, -1, -1, ErrNotStructParam
	}
	structName := strings.TrimSpace(parts[0])
	fieldName := strings.TrimSpace(parts[1])
	if structName == "" || fieldName == "" {
		return "", nil, -1, -1, ErrNotStructParam
	}

	variableS, err := varList.GetVariableStruct(structName)
	if err != nil {
		return "", nil, -1, -1, fmt.Errorf("error getting variable %s: %v", structName, err)
	}

	var structTypeName string
	switch v := variableS.Extra.(type) {
	case string:
		structTypeName = v
	case []byte:
		structTypeName = string(v)
	default:
		return "", nil, -1, -1, fmt.Errorf("expected struct name as string in variable '%s', got %T", structName, variableS.Extra)
	}

	st := GetStructByName(structTypeName)
	if st == nil {
		return "", nil, -1, -1, fmt.Errorf("struct '%s' not found1", structTypeName)
	}

	fieldIndex := -1
	for i := range st.Fields {
		if st.Fields[i].Name == fieldName {
			fieldIndex = i
			break
		}
	}
	if fieldIndex == -1 {
		return "", nil, -1, -1, fmt.Errorf("field '%s' not found in struct '%s'", fieldName, structName)
	}

	spacer := 0
	for i := len(st.Fields) - 1; i >= fieldIndex; i-- {
		spacer += st.Fields[i].Type
	}

	return structName, st, spacer, fieldIndex, nil
}

func createStructType(parser *Parser) error {

	//parse struct name
	structName, err := parser.NextToken()
	if err != nil {
		return err
	}

	//parse {
	eatFirstCurlBrace(parser)

	//parse struct fields
	var fields []structField
	for {
		fieldType, err := parser.NextToken()
		if err != nil {
			return err
		}
		if fieldType == "}" {
			break
		}

		//parse field type
		fieldName, err := parser.NextToken()
		if err != nil {
			return err
		}

		eatSemicolon(parser)

		fieldTypeInt, err := getTypeFromToken(fieldType)
		if err != nil {
			return err
		}

		fields = append(fields, structField{Name: fieldName, Type: fieldTypeInt})
	}

	//create struct type
	structType := StructType{Name: structName, Fields: fields}
	fmt.Println("Created struct:", structType)
	AddStructType(structType)

	return nil
}

// MyStruct structName = ...
func parseStructDeclaration(parser *Parser, token string) (bool, error) {
	structVar := GetStructByName(token)
	if structVar == nil {
		return false, fmt.Errorf("struct '%s' not found0", token)
	}

	//varname
	varname, err := parser.NextToken()
	if err != nil {
		return false, err
	}

	fmt.Println(varname)

	err = getAfterEqual(parser)
	if err != nil {
		return false, err
	}

	createVarWithReg(parser, byte(backend.REG_RAX), DQ, varname, token)

	return true, nil
}

// structName.field1 = ...
func parseStructParamRedeclaration(parser *Parser, token string) (bool, error) {
	// token = structName.field1

	varList := GetVarList(SCOPE)
	if varList == nil {
		return false, fmt.Errorf("Variable list for scope '%s' not found", SCOPE)
	}

	fmt.Println(token)
	fmt.Println(token)
	fmt.Println(token)
	structName, structType, currentSpacer, fieldIndex, err := parseStructParam(token)
	if err != nil {
		return false, err
	}

	peekString, err := parser.Peek()
	if err != nil {
		return false, err
	}

	varList.GetVariable(structName, byte(backend.REG_RBX))

	switch peekString {
	case "=":
		err = getAfterEqual(parser)
		if err != nil {
			return true, err
		}

		switch structType.Fields[fieldIndex].Type {
		case DQ:
			backend.Mov64_mi_r(byte(backend.REG_RBX), uint(currentSpacer-structType.Fields[fieldIndex].Type), byte(backend.REG_RAX))
		case DD:
			backend.Mov32_mi_r(byte(backend.REG_RBX), uint(currentSpacer-structType.Fields[fieldIndex].Type), byte(backend.REG_RAX))
		case DW:
			backend.Mov16_mi_r(byte(backend.REG_RBX), uint(currentSpacer-structType.Fields[fieldIndex].Type), byte(backend.REG_RAX))
		case DB:
			backend.Mov8_mi_r(byte(backend.REG_RBX), uint(currentSpacer-structType.Fields[fieldIndex].Type), byte(backend.REG_RAX))
		default:
			return true, fmt.Errorf("unknown struct field type: %d", structType.Fields[fieldIndex].Type)
		}
	case "++":
		_, err = parser.NextToken() //consume token
		if err != nil {
			return true, err
		}

		switch structType.Fields[fieldIndex].Type {
		case DB:
			backend.Inc8_mi(byte(backend.REG_RBX), uint(currentSpacer-structType.Fields[fieldIndex].Type))
		case DW:
			backend.Inc16_mi(byte(backend.REG_RBX), uint(currentSpacer-structType.Fields[fieldIndex].Type))
		case DD:
			backend.Inc32_mi(byte(backend.REG_RBX), uint(currentSpacer-structType.Fields[fieldIndex].Type))
		case DQ:
			backend.Inc64_mi(byte(backend.REG_RBX), uint(currentSpacer-structType.Fields[fieldIndex].Type))
		}
		eatSemicolon(parser)
	case "--":
		_, err = parser.NextToken() //consume token
		if err != nil {
			return true, err
		}

		switch structType.Fields[fieldIndex].Type {
		case DB:
			backend.Dec8_mi(byte(backend.REG_RBX), uint(currentSpacer-structType.Fields[fieldIndex].Type))
		case DW:
			backend.Dec16_mi(byte(backend.REG_RBX), uint(currentSpacer-structType.Fields[fieldIndex].Type))
		case DD:
			backend.Dec32_mi(byte(backend.REG_RBX), uint(currentSpacer-structType.Fields[fieldIndex].Type))
		case DQ:
			backend.Dec64_mi(byte(backend.REG_RBX), uint(currentSpacer-structType.Fields[fieldIndex].Type))
		}
		eatSemicolon(parser)

	case "+=":
		_, err = parser.NextToken() //consume token
		if err != nil {
			return true, err
		}

		err, _, parsed := getUntilSymbol(parser, []string{";"}, byte(backend.REG_RCX))
		if !parsed {
			return parsed, fmt.Errorf("error parsing sum variable in line %d", parser.LineNumber)
		}
		if err != nil {
			return parsed, fmt.Errorf("error parsing sum variable in line %d: %v", parser.LineNumber, err)
		}

		switch structType.Fields[fieldIndex].Type {
		case DQ:
			backend.Mov64_r_mi(byte(backend.REG_RAX), byte(backend.REG_RBX), int(currentSpacer-structType.Fields[fieldIndex].Type))
			backend.Sum64_r_r(byte(backend.REG_RAX), byte(backend.REG_RCX))
			backend.Mov64_mi_r(byte(backend.REG_RBX), uint(currentSpacer-structType.Fields[fieldIndex].Type), byte(backend.REG_RAX))
		case DD:
			backend.Mov32_r_mi(byte(backend.REG_RAX), byte(backend.REG_RBX), int(currentSpacer-structType.Fields[fieldIndex].Type))
			backend.Sum32_r_r(byte(backend.REG_RAX), byte(backend.REG_RCX))
			backend.Mov32_mi_r(byte(backend.REG_RBX), uint(currentSpacer-structType.Fields[fieldIndex].Type), byte(backend.REG_RAX))
		case DW:
			backend.Mov16_r_mi(byte(backend.REG_RAX), byte(backend.REG_RBX), int(currentSpacer-structType.Fields[fieldIndex].Type))
			backend.Sum16_r_r(byte(backend.REG_RAX), byte(backend.REG_RCX))
			backend.Mov16_mi_r(byte(backend.REG_RBX), uint(currentSpacer-structType.Fields[fieldIndex].Type), byte(backend.REG_RAX))
		case DB:
			backend.Mov8_r_mi(byte(backend.REG_RAX), byte(backend.REG_RBX), int(currentSpacer-structType.Fields[fieldIndex].Type))
			backend.Sum8_r_r(byte(backend.REG_RAX), byte(backend.REG_RCX))
			backend.Mov8_mi_r(byte(backend.REG_RBX), uint(currentSpacer-structType.Fields[fieldIndex].Type), byte(backend.REG_RAX))
		default:
			return true, fmt.Errorf("unknown struct field type: %d", structType.Fields[fieldIndex].Type)
		}
	default:
		return true, fmt.Errorf("unknown operator '%s' for struct field redeclaration at line %d", peekString, parser.LineNumber)
	}

	return true, nil
}

// parse ... = StructName{1,2,3}
func parseStructsCreation(parser *Parser, token string, reg byte) (bool, error) {
	structType := GetStructByName(token)
	if structType == nil {
		return false, fmt.Errorf("struct '%s' not found2", token)
	}

	structFieldCount := len(structType.Fields)

	eatFirstCurlBrace(parser)

	//falta checkar tipos e numero de parametros
	n_params := 0
	for {
		err, symbol, parsed := getUntilSymbol(parser, []string{"}", ","}, byte(backend.REG_RAX))
		if err != nil {
			return true, fmt.Errorf("error getting parameters for struct '%s': %v", structType.Name, err)
		}

		if parsed {
			if n_params >= structFieldCount {
				return true, fmt.Errorf("struct '%s' expects %d fields, got more", structType.Name, structFieldCount)
			}

			// PushStack64(byte(backend.REG_RAX))
			SubStack(structType.Fields[n_params].Type)
			switch structType.Fields[n_params].Type {
			case DQ:
				backend.Mov64_m_r(byte(backend.REG_RSP), byte(backend.REG_RAX))
			case DD:
				backend.Mov32_m_r(byte(backend.REG_RSP), byte(backend.REG_RAX))
			case DW:
				backend.Mov16_m_r(byte(backend.REG_RSP), byte(backend.REG_RAX))
			case DB:
				backend.Mov8_m_r(byte(backend.REG_RSP), byte(backend.REG_RAX))
			default:
				return true, fmt.Errorf("unknown struct field type: %d", structType.Fields[n_params].Type)
			}
			n_params++
		}

		if *symbol == "}" {
			break
		}
	}

	if n_params != structFieldCount {
		return true, fmt.Errorf("struct '%s' expects %d fields, got %d", structType.Name, structFieldCount, n_params)
	}

	backend.Mov64_r_r(reg, byte(backend.REG_RSP))

	return true, nil
}

// token should be something like this 'structName.field1'
func parseStructsGetParam(token string, reg byte) (bool, error) {

	varList := GetVarList(SCOPE)
	if varList == nil {
		return false, fmt.Errorf("Variable list for scope '%s' not found", SCOPE)
	}

	structName, structType, currentSpacer, fieldIndex, err := parseStructParam(token)
	if err != nil {
		return false, err
	}

	varList.GetVariable(structName, reg)
	switch structType.Fields[fieldIndex].Type {
	case DQ:
		backend.Mov64_r_mi(reg, reg, int(currentSpacer-structType.Fields[fieldIndex].Type))
	case DD:
		backend.Mov32_r_mi(reg, reg, int(currentSpacer-structType.Fields[fieldIndex].Type))
	case DW:
		backend.Mov16_r_mi(reg, reg, int(currentSpacer-structType.Fields[fieldIndex].Type))
	case DB:
		backend.Mov8_r_mi(reg, reg, int(currentSpacer-structType.Fields[fieldIndex].Type))
	default:
		return true, fmt.Errorf("unknown struct field type: %d", structType.Fields[fieldIndex].Type)
	}

	clearReg(reg, structType.Fields[fieldIndex].Type)

	return true, nil
}

// token = <structName.fieldName>
func getPointerOfStruct(token string, reg byte) (bool, error) {

	varList := GetVarList(SCOPE)
	if varList == nil {
		return false, fmt.Errorf("Variable list for scope '%s' not found", SCOPE)
	}

	structName, structType, currentSpacer, fieldIndex, err := parseStructParam(token)
	if err != nil {
		return false, err
	}

	varList.GetVariable(structName, reg)
	backend.Sum64_r_i(reg, uint(currentSpacer-structType.Fields[fieldIndex].Type))

	return true, nil
}

// if "structName.field2" exists
func doesStructVarFieldExist(varName string) (bool, error) {

	varList := GetVarList(SCOPE)
	if varList == nil {
		return false, fmt.Errorf("Variable list for scope '%s' not found", SCOPE)
	}

	_, _, _, fieldIndex, err := parseStructParam(varName)
	if err != nil {
		return false, err
	}

	return fieldIndex >= 0, nil
}
