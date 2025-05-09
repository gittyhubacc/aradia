CC := clang
CFLAGS :=
LDFLAGS :=

BIN_NAME := flcl

SRC_DIR := ./src
OBJ_DIR := ./obj
BIN_DIR := ./bin

SRCS := $(shell find $(SRC_DIR) -name '*.c')
OBJS := $(SRCS:%.c=$(OBJ_DIR)/%.o)

all: $(BIN_DIR)/$(BIN_NAME)

$(BIN_DIR)/$(BIN_NAME): $(OBJS)
	@mkdir -p $(dir $@)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

test: test.o
	ld test.o
test.o: test.s
	nasm -felf64 test.s

.PHONY: clean
clean:
	rm -rf $(BIN_DIR) $(OBJ_DIR)
