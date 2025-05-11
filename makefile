CC := clang
CFLAGS := -g -O0
LDFLAGS :=

BIN_NAME := aradiac

SRC_DIR := ./src
OBJ_DIR := ./obj
BIN_DIR := ./bin

EXAMPLE_DIR := ./examples

SRCS := $(shell find $(SRC_DIR) -name '*.c')
OBJS := $(SRCS:%.c=$(OBJ_DIR)/%.o)


all: $(BIN_DIR)/$(BIN_NAME)

$(BIN_DIR)/$(BIN_NAME): $(OBJS)
	@mkdir -p $(dir $@)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN_DIR)/concept: $(OBJ_DIR)/concept.o
	ld $(OBJ_DIR)/concept.o -o $(BIN_DIR)/concept
$(OBJ_DIR)/concept.o: $(OBJ_DIR)/concept.s
	nasm -felf64 $(OBJ_DIR)/concept.s -o $(OBJ_DIR)/concept.o
$(OBJ_DIR)/concept.s: $(BIN_DIR)/$(BIN_NAME) $(EXAMPLE_DIR)/concept.aa
	cat $(EXAMPLE_DIR)/concept.aa | $(BIN_DIR)/$(BIN_NAME) > $(OBJ_DIR)/concept.s

$(BIN_DIR)/emit: $(OBJ_DIR)/emit.o
	ld $(OBJ_DIR)/emit.o -o $(BIN_DIR)/emit
$(OBJ_DIR)/emit.o: $(OBJ_DIR)/emit.s
	nasm -felf64 $(OBJ_DIR)/emit.s -o $(OBJ_DIR)/emit.o
$(OBJ_DIR)/emit.s: $(BIN_DIR)/$(BIN_NAME) $(EXAMPLE_DIR)/emit.aa
	cat $(EXAMPLE_DIR)/emit.aa | $(BIN_DIR)/$(BIN_NAME) > $(OBJ_DIR)/emit.s

$(BIN_DIR)/hello: $(OBJ_DIR)/hello.o
	ld $(OBJ_DIR)/hello.o -o $(BIN_DIR)/hello
$(OBJ_DIR)/hello.o: $(OBJ_DIR)/hello.s
	nasm -felf64 $(OBJ_DIR)/hello.s -o $(OBJ_DIR)/hello.o
$(OBJ_DIR)/hello.s: $(BIN_DIR)/$(BIN_NAME) $(EXAMPLE_DIR)/hello_world.aa
	cat $(EXAMPLE_DIR)/hello_world.aa | $(BIN_DIR)/$(BIN_NAME) > $(OBJ_DIR)/hello.s

$(BIN_DIR)/simple: $(OBJ_DIR)/simple.o
	ld $(OBJ_DIR)/simple.o -o $(BIN_DIR)/simple
$(OBJ_DIR)/simple.o: $(OBJ_DIR)/simple.s
	nasm -felf64 $(OBJ_DIR)/simple.s -o $(OBJ_DIR)/simple.o
$(OBJ_DIR)/simple.s: $(BIN_DIR)/$(BIN_NAME) $(EXAMPLE_DIR)/simple.aa
	cat $(EXAMPLE_DIR)/simple.aa | $(BIN_DIR)/$(BIN_NAME) > $(OBJ_DIR)/simple.s

.PHONY: clean concept emit hello simple
emit: $(BIN_DIR)/emit
hello: $(BIN_DIR)/hello
simple: $(BIN_DIR)/simple
concept: $(BIN_DIR)/concept
clean:
	rm -rf $(BIN_DIR) $(OBJ_DIR)
