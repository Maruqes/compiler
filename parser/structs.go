package parser

import (
	"fmt"

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
	for _, structType := range structTypes {
		if structType.Name == name {
			return &structType
		}
	}
	return nil
}

func createStructType(parser *Parser) error {

	//parse struct name
	structName, err := parser.NextToken()
	if err != nil {
		return nil
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

func parseStructDeclaration(parser *Parser, token string) (bool, error) {
	structVar := GetStructByName(token)
	if structVar == nil {
		return false, fmt.Errorf("struct '%s' not found", token)
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

	createVarWithReg(parser, byte(backend.REG_RAX), DQ, varname)


	return true, nil
}

// parse StructName{1,2,3}
func parseStructsCreation(parser *Parser, token string, reg byte) (bool, error) {
	structType := GetStructByName(token)
	if structType == nil {
		return false, fmt.Errorf("struct '%s' not found", token)
	}

	structFieldCount := len(structType.Fields)

	eatFirstCurlBrace(parser)

	//falta checkar tipos e numero de parametros
	n_params := 0
	for {
		err, symbol, parsed := getUntilSymbol(parser, []string{"}", ","}, byte(backend.REG_RAX))
		if err != nil {
			return false, fmt.Errorf("error getting parameters for struct '%s': %v", structType.Name, err)
		}

		if parsed {
			PushStack64(byte(backend.REG_RAX))
			n_params++
		}

		if *symbol == "}" {
			break
		}
	}

	if n_params != structFieldCount {
		return false, fmt.Errorf("struct '%s' expects %d fields, got %d", structType.Name, structFieldCount, n_params)
	}

	backend.Mov64_r_r(reg, byte(backend.REG_RSP))

	return true, nil
}
