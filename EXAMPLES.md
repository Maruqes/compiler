
**⚠️ LINUX ONLY** - This compiler and examples are designed exclusively for Linux x86-64 systems.


Download the `512lang.zip` file from `https://github.com/Maruqes/compiler/releases` or download directly from [**512lang.zip**](`https://github.com/Maruqes/compiler/releases/latest/download/512lang.zip`) and decompile it to obtain the `compiler` binary and example files.


# Examples

This section provides examples of how to compile and run various 512lang programs. Each example demonstrates different features of the language.

## Basic Test

A simple test program that you can program.
```bash
./compiler test.lang out
./out
```
or
```bash
make test
```

## Threading Example

Demonstrates multi-threading capabilities with two worker threads that print numbers with delays.

```bash
./compiler threading.lang out
./out
```
or 
```bash
make threading
```

## Person Database

A program that manages a simple database of people stored in a file. You can add new persons (with name, age, and phone) or list existing entries from `people.txt`.

```bash
./compiler persondb.lang out
./out
```
or
```bash
make persondb
```

## Web Server

A basic HTTP web server that serves an HTML page on port 8080. It reads `webserver.html` and serves it to clients.

```bash
./compiler webserver.lang out
./out
```
or
```bash
make webserver
```

Then open a browser and navigate to `http://localhost:8080` to see the served HTML page.

## Socket TCP Client

A TCP client that connects to a server on `http://localhost:8080`. It reads input from stdin and sends it to the server, while printing received data to stdout.

To test this, first start a server using netcat:

```bash
nc -lvnp 8080
```

Then run the client:

```bash
./compiler socket_tcp.lang out
./out
```
or
```bash
make socket_tcp
```

## Cleanup

Remove the compiled output file:

```bash
rm -f out
```
