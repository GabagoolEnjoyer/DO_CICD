# Define the root directory of the project
ROOT_DIR := ../

BUILD_DIR_GREP := $(ROOT_DIR)grep/

# Compiler and flags
COMPILER := gcc
CFLAGS := -Wall -Werror -Wextra -std=c11

all: s21_grep

s21_grep: s21_grep.o s21_grep_func.o
	$(COMPILER) $(CFLAGS) $(BUILD_DIR_GREP)s21_grep.o $(BUILD_DIR_GREP)s21_grep_func.o -o $(BUILD_DIR_GREP)s21_grep

test: s21_grep
	./my_test_grep.sh

s21_grep_func.o: $(BUILD_DIR_GREP)s21_grep_func.c
	$(COMPILER) $(CFLAGS) -c $(BUILD_DIR_GREP)s21_grep_func.c -o $(BUILD_DIR_GREP)s21_grep_func.o

s21_grep.o: $(BUILD_DIR_GREP)s21_grep.c
	$(COMPILER) $(CFLAGS) -c $(BUILD_DIR_GREP)s21_grep.c -o $(BUILD_DIR_GREP)s21_grep.o

clean:
	rm -rf $(BUILD_DIR_GREP)s21_grep $(BUILD_DIR_GREP)*.o

rebuild: clean all