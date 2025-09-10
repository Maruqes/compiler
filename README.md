# 512lang — An Experimental Language That Writes Its Own ELF

512lang is a tiny, educational systems language that compiles directly to native ELF64 binaries. It skips libc and linkers entirely: the compiler emits the ELF header, program header, code, and data, and talks to the Linux kernel via raw syscalls. The goal is to serve as a proof‑of‑concept and a learning playground for low-level code generation, calling conventions, and OS interfaces.


## Highlights

- Experimental systems language that emits native ELF64 binaries
- Recursive‑descent parser in Go
- C backend that writes x86‑64 machine code directly
- Manual memory management (brk + mmap)
- Native threading via Linux clone()
- Synchronization with futex
- Minimal HTTP server using only syscalls
- TCP networking: socket/bind/listen/accept/send/read/close
- Labels + jumps for control flow (if/elif/else, loops)
- Zero dependencies on libc or external linkers — just syscalls

See:
- Parser: [parser/](parser/), e.g. [`parser/variables.go`](parser/variables.go)
- Backend: [`c_backend/compiler.c`](c_backend/compiler.c), [`c_backend/backend.h`](c_backend/backend.h)
- Instruction emission and writing code: [`c_backend/functions/functions.c`](c_backend/functions/functions.c) (e.g. `write_all_custom_code`)
- String/data pool: [`c_backend/raw_vars/STRING_CONSTANTS.md`](c_backend/raw_vars/STRING_CONSTANTS.md)
- Build system: [`Makefile`](Makefile)
 - Examples: [`lang/`](lang/) (e.g. `webserver.lang`, `threading.lang`, `socket_tcp.lang`)

## Quick Example (WIP syntax)

The language syntax is evolving. A minimal “hello” conceptually looks like this:

```512lang
// hello.lang
include("lang/liblang/strings.lang")

func main() {
    print("Hello, World!\n");
    return;
}
```

The compiler lowers this to raw syscalls and emits a complete ELF64 with aligned segments and a data pool (strings/globals). For how the data pool and relocation of addresses work, see [`c_backend/raw_vars/STRING_CONSTANTS.md`](c_backend/raw_vars/STRING_CONSTANTS.md).

## Build

Prerequisites (Linux x86-64):
- Go (1.21+ recommended)
- GCC and Make
- SWIG
- A recent Linux kernel (for liblang)

Build steps:
```bash
make clean
make all         # builds C backend → bin/libbackend.a
make swig        # generates SWIG bindings → swig/backend_wrap.c
go build -o compiler main.go
```

What happens:
- The Makefile compiles the C backend modules (ELF writer, instruction emitters, jumps, raw vars) into a static library at `bin/libbackend.a`. See [`Makefile`](Makefile).
- SWIG generates the Go bindings used by the parser/frontend to call into the C backend (`swig` target).
- `go build` links against `bin/libbackend.a` (see cgo flags in [`main.go`](main.go)).

## Use

CLI:
```bash
./compiler <input.lang> <output-elf>
```

Example (matches the repo’s workflow):
```bash
./compiler features.lang hello_elf_64
chmod +x hello_elf_64
./hello_elf_64
```

Alternatively, use the canned workflow:
```bash
make run
```

Notes:
- The compiler is intentionally minimal and targets Linux x86_64 only.
- Binaries are standalone (no dynamic loader, no libc).

## Examples

Practical examples live under `lang/`. These demonstrate direct syscalls, threading with `clone()`/`futex`, and raw TCP networking without libc.

### webserver.lang — syscall-only HTTP server
Minimal HTTP server implemented with raw Linux syscalls (socket/bind/listen/accept/read/write). Useful to see how request parsing and responses can be done without any runtime.

Build and run:
```bash
./compiler lang/webserver.lang webserver_elf
chmod +x webserver_elf
./webserver_elf
```

### threading.lang — clone() + futex synchronization
Shows native threads created with `clone()` and synchronization primitives using `futex`. This illustrates how 512lang maps directly onto kernel primitives for concurrency.

Build and run:
```bash
./compiler lang/threading.lang threading_elf
chmod +x threading_elf
./threading_elf
```

### socket_tcp.lang — TCP sockets with pure syscalls
Demonstrates TCP networking using `socket`, `bind`, `listen`, `accept`, `connect`, `read`, and `write`. Adjust addresses/ports in the source if needed.

Build and run:
```bash
./compiler lang/socket_tcp.lang socket_tcp_elf
chmod +x socket_tcp_elf
./socket_tcp_elf
```

Tip: Explore other samples like `features.lang` for a tour of language constructs.

## Contributing

Contributions are welcome. Please:
- Open an issue to discuss substantial changes first
- Keep PRs small and focused
- Include minimal examples and expected output
- Run `make clean && make all && make swig && go build` before submitting

## License

GPL-3.0-only. See [LICENSE](LICENSE).

—
No libc • No linker • Just syscalls