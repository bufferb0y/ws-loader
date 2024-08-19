CC := x86_64-w64-mingw32-gcc

CFLAGS := -ffunction-sections -fdata-sections -fmerge-all-constants
CFLAGS += -static-libgcc -Wno-write-strings -s

LFLAGS := -lwinhttp -lpowrprof

SRC := src
INC := inc
BIN := bin
OBJ := obj

SRC_FILES := $(wildcard $(SRC)/*.c)
OBJ_FILES := $(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SRC_FILES))

NAME := ws-loader

$(BIN)/$(NAME): $(OBJ_FILES)
	$(CC) $(CFLAGS) $^ $(LFLAGS) -o $@

$(OBJ)/%.o: $(SRC)/%.c $(OBJ) $(BIN)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ):
	mkdir $@

$(BIN):
	mkdir $@

clean:
	rm -rf $(BIN) $(OBJ)
