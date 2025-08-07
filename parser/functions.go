package parser

func createFunc(parser *Parser) error {
	//get name of the function
	_, err := parser.NextToken()
	if err != nil {
		return err
	}
	return nil
}
