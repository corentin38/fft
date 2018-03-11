CC = gcc
FLAGS = -Wall -Wextra -pedantic -g
SPECIAL_HEADER =
LIBS = -lm
INCLUDES =
OBJ_DIR = obj
C_FILES = $(wildcard *.c)
H_FILES = $(wildcard *.h)
O_FILES = $(patsubst %.c,$(OBJ_DIR)/%.o,$(C_FILES))
TARGET = fft

all: $(TARGET)

$(TARGET): $(O_FILES)
	$(CC) $(FLAGS) $(LIBS) $^ -o $@

$(OBJ_DIR)/%.o: %.c $(H_FILES)
	$(CC) $(FLAGS) $(SPECIAL_HEADER) -c $< -o $@