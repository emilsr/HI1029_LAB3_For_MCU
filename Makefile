CC=gcc
CFLAGS=-Wall -Wpedantic

SRC_DIR=src
OUT_DIR=out

SRC_FILES := $(wildcard $(SRC_DIR)/*.c)

OBJ_FILES := $(patsubst $(SRC_DIR)/%.c,$(OUT_DIR)/%.o,$(SRC_FILES))

ifeq ($(OS), Windows_NT)
	# Windows specific
	BIN_EXT =.exe
	CFLAGS +=-lmingw32 -mwindows
	MKDIR = powershell -Command "if (!(Test-Path -Path $(OUT_DIR) -PathType Container)) { New-Item -ItemType Directory -Path $(OUT_DIR) }"
	RM = del /Q
else
	# Other OS
	BIN_EXT =
	CFLAGS +=-lm
	MKDIR = mkdir -p $(OUT_DIR)
	RM = rm -f
endif

TARGET=main

$(TARGET): $(OBJ_FILES)
	$(CC) -o $@ $^ $(CFLAGS)

$(OUT_DIR)/%.o: $(SRC_DIR)/%.c
	$(MKDIR)
	$(CC) -c -o $@ $< $(CFLAGS)

.PHONY: clean

clean:
	-rm -rf $(OUT_DIR) $(TARGET)