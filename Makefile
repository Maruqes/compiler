BIN_DIR := bin
OBJ_DIR := $(BIN_DIR)/obj

# All C source files in the backend
SOURCES := c_backend/compiler.c \
		   c_backend/functions/functions.c \
		   c_backend/functions/bFunctions8/bFunctions8.c \
		   c_backend/functions/bFunctions8/utils8.c \
		   c_backend/functions/bFunctions8/bTestes8.c \
		   c_backend/functions/bFunctions8/arithmetics8.c \
		   c_backend/functions/bFunctions16/bFunctions16.c \
		   c_backend/functions/bFunctions16/utils16.c \
		   c_backend/functions/bFunctions16/arithmetics16.c \
		   c_backend/functions/bFunctions16/bTestes16.c \
		   c_backend/functions/bFunctions32/bFunctions32.c \
		   c_backend/functions/bFunctions32/utils32.c \
		   c_backend/functions/bFunctions32/arithmetics32.c \
		   c_backend/functions/bFunctions32/bTestes32.c \
		   c_backend/functions/bFunctions64/bFunctions64.c \
		   c_backend/functions/bFunctions64/utils64.c \
		   c_backend/functions/bFunctions64/bTestes64.c \
		   c_backend/functions/bFunctions64/arithmetics64.c \
		   c_backend/functions/bFunctions64/floats64.c \
		   c_backend/functions/jumps/jumps.c \
		   c_backend/functions/jumps/jumpTeste.c \
		   c_backend/functions/jumps/labels.c \
		   c_backend/raw_vars/raw_vars.c

# Convert source paths to object paths
OBJECTS := $(patsubst c_backend/%.c,$(OBJ_DIR)/%.o,$(SOURCES))

# Include directories for compilation
INCLUDE_DIRS := -Ic_backend/functions \
				-Ic_backend/functions/bFunctions8 \
				-Ic_backend/functions/bFunctions16 \
				-Ic_backend/functions/bFunctions32 \
				-Ic_backend/functions/bFunctions64 \
				-Ic_backend/functions/jumps \
				-Ic_backend/raw_vars

all: $(BIN_DIR)/libbackend.a

# Generic rule for compiling C files
$(OBJ_DIR)/%.o: c_backend/%.c
	@mkdir -p $(dir $@)
	gcc -c $< $(INCLUDE_DIRS) -o $@

# Create the static library
$(BIN_DIR)/libbackend.a: $(OBJECTS)
	@mkdir -p $(BIN_DIR)
	ar rcs $@ $^

swig:
	swig -go -cgo -intgosize 64 -outdir swig -o swig/backend_wrap.c swig/backend.i

clean:
	rm -rf $(BIN_DIR)


build:
	$(MAKE) clean
	$(MAKE) all
	$(MAKE) swig
	go build -ldflags="-s -w -X 'main.VERSION=$(VERSION)'" -trimpath -o compiler main.go
	cp compiler 512lang/compiler


run:
	$(MAKE) build
	cd 512lang && make tcp_conv_server


.PHONY: all clean swig

