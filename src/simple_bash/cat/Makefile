# Define the root directory of the project
ROOT_DIR := ../

# Define the directories to search for .c and .h files
SRC_DIRS := cat grep

# Define the build directories
BUILD_DIR_CAT := $(ROOT_DIR)cat/

# Compiler and flags
COMPILER := gcc
CFLAGS := -Wall -Werror -Wextra -std=c11

all: s21_cat

test: s21_cat
	bash my_test.sh

s21_cat: s21_cat.o s21_cat_func.o
	$(COMPILER) $(CFLAGS) $(BUILD_DIR_CAT)s21_cat.o $(BUILD_DIR_CAT)s21_cat_func.o -o $(BUILD_DIR_CAT)s21_cat

s21_cat_func.o: $(BUILD_DIR_CAT)s21_cat_func.c
	$(COMPILER) $(CFLAGS) -c $(BUILD_DIR_CAT)s21_cat_func.c -o $(BUILD_DIR_CAT)s21_cat_func.o

s21_cat.o: $(BUILD_DIR_CAT)s21_cat.c
	$(COMPILER) $(CFLAGS) -c $(BUILD_DIR_CAT)s21_cat.c -o $(BUILD_DIR_CAT)s21_cat.o

clean:
	rm -rf $(BUILD_DIR_CAT)s21_cat $(BUILD_DIR_CAT)*.o

rebuild: clean all

cppcheck:
	cppcheck $(ROOT_DIR)grep/*.c $(ROOT_DIR)common/*.c $(ROOT_DIR)common/*.h $(ROOT_DIR)cat/*.c