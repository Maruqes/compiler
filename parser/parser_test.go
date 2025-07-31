package parser

import (
	"fmt"
	"io"
	"testing"
)

func TestStartParse1(t *testing.T) {
	//open file test
	p := &Parser{}
	err := p.StartParse("test2.txt")
	if err != nil {
		t.Errorf("StartParse with existing file failed: %v", err)
	}

	s, err := p.NextToken()
	if err != nil {
		t.Errorf("NextToken failed: %v", err)
	}
	for s != "" {
		fmt.Println(s)
		s, err = p.NextToken()
		if err != nil {
			if err == io.EOF {
				break
			}
			t.Errorf("NextToken failed: %v", err)
		}
	}
}
