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

/*
	struct MyStruct {
	    dq field1;
	    dq field2;
	    db field3;
	}

ptr structTest<MyStruct> = MyStruct{1,2,3};

createVarsFromStruct should create variables for each field in the struct
structTest.field1   should be structTest + 8
structTest.field2   should be structTest + 8 + sizeof(structTest.field1)
structTest.field3   should be structTest + 8 + sizeof(structTest.field1) + sizeof(structTest.field2)
*/
func createVarsFromStruct(structType *StructType) error {

	
	return nil
}
