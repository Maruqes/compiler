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
	if !strings.Contains(outputArr[0], "HELLOO_MUNDO") {
		fmt.Println("Test failed: Output not as expected1")
		fmt.Println(outputArr[0])
		return
	}
	if !strings.Contains(outputArr[1], "Hello World") {
		fmt.Println("Test failed: Output not as expected2")
		fmt.Println(outputArr[1])
		return
	}
	if !strings.Contains(outputArr[2], "Hello World2") {
		fmt.Println("Test failed: Output not as expected3")
		fmt.Println(outputArr[2])
		return
	}
	if !strings.Contains(outputArr[3], "FUNCIONA") {
		fmt.Println("Test failed: Output not as expected4")
		fmt.Println(outputArr[3])
		return
	}

	//print as green
	fmt.Println("\033[32mSmall Test passed\033[0m")
}
