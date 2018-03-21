CC = gcc
FLAGS = -Wall -Wextra -pedantic -g -std=c99
SPECIAL_HEADER =
LIBS = -lm
INCLUDES =
OBJ_DIR = obj
C_FILES = $(wildcard *.c)
H_FILES = $(wildcard *.h)
O_FILES = $(patsubst %.c,$(OBJ_DIR)/%.o,$(C_FILES))
TARGET = fft

all: $(OBJ_DIR) $(TARGET)

$(TARGET): $(O_FILES)
	$(CC) $(FLAGS) $(LIBS) $^ -o $@

$(OBJ_DIR)/%.o: %.c $(H_FILES)
	$(CC) $(FLAGS) $(SPECIAL_HEADER) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $@

clean:
	rm -rf $(TARGET) $(OBJ_DIR)
