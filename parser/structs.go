package parser

import "fmt"

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

	eatSemicolon(parser)

	return true, nil
}
