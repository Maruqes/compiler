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
	if outputArr[0] != "HELLO" {
		fmt.Println("Test failed: Output not as expected HELLO")
		return
	}
	if outputArr[1] != "FUNCIONA" {
		fmt.Println("Test failed: Output not as expected FUNCIONA")
		return
	}

	//print as green
	fmt.Println("\033[32mSmall Test passed\033[0m")
}
