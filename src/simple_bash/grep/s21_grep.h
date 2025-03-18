#ifndef S21_GREP_H
#define S21_GREP_H

#include <getopt.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct {
  int e;
  int i;
  int v;
  int c;
  int l;
  int n;
  int h;
  int s;
  int f;
  int o;
  int uninitialized_flag;
} grep_opt;

typedef struct {
  char** data;
  int size;
  int capacity;
} dynamic_string_array;

void parse_flags(int argc, char** argv, grep_opt* grep_options,
                 dynamic_string_array* patterns_array,
                 dynamic_string_array* patterns_files);
void free_all_dynamic_string_arrays(dynamic_string_array* files_arr,
                                    dynamic_string_array* patterns_files,
                                    dynamic_string_array* patterns_array,
                                    dynamic_string_array* strings_flag_o);
void flags_e_and_f_workaraound(grep_opt* grep_options, int argc, char** argv,
                               dynamic_string_array* files_arr,
                               dynamic_string_array* patterns_array);
int flag_f_load_patterns(dynamic_string_array* patterns_files,
                         dynamic_string_array* patterns_array);
int check_filesize(char* filename);
char* read_file_to_buffer(FILE* file, int filesize);
void patterns_from_file(char* buffer, dynamic_string_array* patterns_array);
char* extract_line(const char* line_start, const char* line_end, char* line,
                   size_t line_size);
int initialize_regex_arr(regex_t* regex_arr,
                         dynamic_string_array* patterns_array,
                         grep_opt grep_options);
void free_dynamic_string_array(dynamic_string_array* arr);
void add_string(dynamic_string_array* arr, const char* str);
void free_compiled_regex(regex_t* regex_arr, int compiled_regex_count);
int fileExists(const char* filePath);
void process_text_grep(char* buffer, grep_opt grep_options, char* file_name,
                       regex_t* regex_arr, dynamic_string_array patterns_array,
                       dynamic_string_array strings_flag_o,
                       dynamic_string_array files_arr);
int process_line_grep(const char* line, grep_opt grep_options, char* file_name,
                      dynamic_string_array files_arr,
                      dynamic_string_array patterns_array,
                      dynamic_string_array strings_flag_o, regex_t* regex_arr,
                      int line_num);
int check_patterns_match(const char* line, dynamic_string_array patterns_array,
                         regex_t* regex_arr);
void grep_flag_c(dynamic_string_array files_arr, grep_opt grep_options,
                 char* file_name, int matched_line_counter);
void grep_flag_l(grep_opt grep_options, int matched_line_counter,
                 char* file_name);
void grep_flag_h(dynamic_string_array files_arr, grep_opt grep_options,
                 char* file_name);
void reset_dynamic_string_array(dynamic_string_array* arr);
void grep_flag_o(dynamic_string_array* patterns_array,
                 dynamic_string_array* strings_flag_o, regex_t* regex_arr,
                 const char* line);
void print_or_not_flag_o(grep_opt grep_options, dynamic_string_array files_arr,
                         dynamic_string_array strings_flag_o, char* file_name,
                         int line_num);
void grep_flag_n(grep_opt grep_options, int line_num);
void add_match(dynamic_string_array* strings_flag_o, const char* line,
               int offset, regmatch_t pmatch);
void find_matches_for_pattern(dynamic_string_array* strings_flag_o,
                              const char* line, regex_t* regex, int* offset);
void print_or_not_standart(grep_opt grep_options, const char* line,
                           dynamic_string_array files_arr, char* file_name,
                           int line_num);
#endif