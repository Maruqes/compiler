package parser

import backend "github.com/Maruqes/compiler/swig"


//push whats after the equal and save position in the stack compared to BASE POINTER (BP)
func createVar64(parser *Parser) error {
	//create var is get space on heap and get the after equal
	_, err := parser.NextToken()
	if err != nil {
		return err
	}

	// get the value after the equal sign in RAX
	if err := getAfterEqual(parser); err != nil {
		return err
	}
	
	backend.Push64(byte(backend.REG_RAX))

	return nil
}
