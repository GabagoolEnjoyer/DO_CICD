#ifndef S21_CAT_FUNC_H
#define S21_CAT_FUNC_H

#include <getopt.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Structure to store boolean variables indicating which flags have been used
typedef struct {
  int b;
  int e;
  int n;
  int s;
  int t;
  int v;
  int E;
  int T;
  int uninitialized_flag;
} cat_opt;

// Structure to store file handling information
typedef struct {
  int num_file;
  int line_number;
  int flag_exit;
} file_work;

// Function prototypes
void parse_flags(int argc, char **argv, cat_opt *cat_options);
void process_and_print_char(cat_opt *cat_options, unsigned char uc,
                            int *line_number_printed);
void process_file(const char *filename, cat_opt *cat_options,
                  file_work *file_options);

#endif