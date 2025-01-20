package main

import (
	"bytes"
	"fmt"
	"os/exec"
	"strings"
)

func main() {
	cmd := exec.Command("./testRun")

	var out bytes.Buffer
	cmd.Stdout = &out

	err := cmd.Run()
	if err != nil {
		fmt.Println("Test failed: Execution error:", err)
		return
	}

	outputString := out.String()
	outputArr := strings.Split(outputString, "\n")
	for i := 0; i < len(outputArr); i++ {
		fmt.Println(outputArr[i])
	}
}
