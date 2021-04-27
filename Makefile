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
TEST_NSAMPLES := 8192

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(O_FILES)
	$(CC) $(FLAGS) $(LIBS) $^ -o $@

$(OBJ_DIR)/%.o: %.c $(H_FILES) $(OBJ_DIR)
	$(CC) $(FLAGS) $(SPECIAL_HEADER) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $@

clean:
	rm -rf $(TARGET) $(OBJ_DIR)

test: $(TARGET)
	bats ./test/test.bats

graph: $(TARGET)
	./$(TARGET) -n $(TEST_NSAMPLES) -t BRUTE sample/foo.wav
	gnuplot --persist sample/spectrum.gnuplot
	./$(TARGET) -n $(TEST_NSAMPLES) -t FFT sample/foo.wav
	gnuplot --persist sample/spectrum.gnuplot
