# **Compiler**  

This is a fully working compiler with a unique syntax. **Not a complete project (yet).**  
It is **not** intended for production use—this is purely a **proof of concept** that demonstrates its functionality.  

## **Forget LLVM. Forget interpreters.**  
This is a **true** compiler—your code is **directly compiled into raw machine instructions** with no middle layers.  

## **3 Data Types**  
We have the following data types:  

```
dd v4 = 0;  // 4 bytes (double word)
dw v2 = 0;  // 2 bytes (word)
db v1 = 0;  // 1 byte (byte)
```
pointers do exist but they are just a wrap around "dd"

```
func test_deaddressing(){
    dd a = 10;
    dd*b = &a;
    dd*c = &b;
    dd*d = &c;
    return ***d;
}
```
There are bugs and DO NOT exists complex bug checks, for example in this example if we do this 

```
func test_deaddressing(){
    db a = 10;  //<-  dd to db
    dd*b = &a;
    dd*c = &b;
    dd*d = &c;
    return ***d;
}
```
Does not work, bugs that maybe fixed in the future

## **How is this done**

Using the x86 instruction set, we identify the opcodes for each instruction.
Instead of converting the code to assembly and relying on an assembler to produce the final executable, this compiler directly translates the code into opcodes and writes them to the file.

See code example in the project files