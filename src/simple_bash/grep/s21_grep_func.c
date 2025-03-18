#include "s21_grep.h"

void parse_flags(int argc, char** argv, grep_opt* grep_options,
                 dynamic_string_array* patterns_array,
                 dynamic_string_array* patterns_files) {
  int opt = 0;
  const char* short_options = "e:ivclnhsof:";  // short options

  while ((opt = getopt(argc, argv, short_options)) != -1) {
    switch (opt) {
      case 'e':
        grep_options->e = 1;
        add_string(patterns_array, argv[optind - 1]);
        break;
      case 'i':
        grep_options->i = 1;
        break;
      case 'v':
        grep_options->v = 1;
        break;
      case 'c':
        grep_options->c = 1;
        break;
      case 'l':
        grep_options->l = 1;
        break;
      case 'n':
        grep_options->n = 1;
        break;
      case 'h':
        grep_options->h = 1;
        break;
      case 's':
        grep_options->s = 1;
        break;
      case 'o':
        grep_options->o = 1;  // I offed opt o bcz too tired
        break;
      case 'f':
        grep_options->f = 1;
        add_string(patterns_files, argv[optind - 1]);
        break;
      case '?':
        grep_options->uninitialized_flag = 1;
        break;
    }
  }
}

void free_all_dynamic_string_arrays(dynamic_string_array* files_arr,
                                    dynamic_string_array* patterns_files,
                                    dynamic_string_array* patterns_array,
                                    dynamic_string_array* strings_flag_o) {
  dynamic_string_array* array_to_clean[] = {files_arr, patterns_files,
                                            patterns_array, strings_flag_o};
  for (int i = 0; i < 4; i++) {
    free_dynamic_string_array(array_to_clean[i]);
  }
}

void flags_e_and_f_workaraound(grep_opt* grep_options, int argc, char** argv,
                               dynamic_string_array* files_arr,
                               dynamic_string_array* patterns_array) {
  // if either of two is 1, then first file path == optind
  if (grep_options->e || grep_options->f) {
    for (int i = optind; i < argc; i++) {
      add_string(files_arr, argv[i]);
    }
  } else {
    add_string(patterns_array, argv[optind]);  // optind == first pattern
    for (int i = optind + 1; i < argc; i++) {  // optind + 1 == first file path
      add_string(files_arr, argv[i]);
    }
  }
}

int flag_f_load_patterns(dynamic_string_array* patterns_files,
                         dynamic_string_array* patterns_array) {
  int flag = 1;
  // read from all files per iteration
  for (int i = 0; i < patterns_files->size; i++) {
    FILE* patterns_file = fopen(patterns_files->data[i], "r");
    if (patterns_file == NULL) {
      flag = 0;
      break;
    }
    // check the size of file
    int filesize_patterns = check_filesize(patterns_files->data[i]);

    char* pattern_buffer = read_file_to_buffer(
        patterns_file, filesize_patterns);  // read file to buffer
    patterns_from_file(pattern_buffer, patterns_array);
    free(pattern_buffer);

    fclose(patterns_file);
  }

  return flag;
}

int check_filesize(char* filename) {
  FILE* pFile;
  int size = -1;  // Инициализация переменной
  pFile = fopen(filename, "rb");
  if (!pFile) {
    perror("Error opening file");
  } else {
    fseek(pFile, 0, SEEK_END);
    size = ftell(pFile);
    fclose(pFile);
  }
  return size;
}

char* read_file_to_buffer(FILE* file, int filesize) {  // read file to buffer
  int flag_success = 1;
  char* buffer = (char*)malloc(
      (filesize + 1) * sizeof(char));  // allocate memory equal to filesize

  if (buffer == NULL) {
    perror("Memory allocation failed\n");
    flag_success = 0;
  }

  if (flag_success != 0) {
    size_t bytes_read =
        fread(buffer, sizeof(char), filesize, file);  // read file to buffer
    if (!((int)bytes_read < filesize &&
          ferror(file))) {  // check if file is read successfully
      buffer[bytes_read] = '\0';
    } else {
      perror("Error reading file\n");
      flag_success = 0;
    }
  }

  if (flag_success == 0) {
    free(buffer);
    buffer = NULL;
  }

  return buffer;
}

void patterns_from_file(char* buffer,
                        dynamic_string_array* patterns_array) {  // для -f
  const char* line_start = buffer;  // pointer to start
  const char* line_end = NULL;      // init pointer
  int line_num = 1;

  while ((line_end = strchr(line_start, '\n')) !=
         NULL) {  // reading until no \ n symbol
    char line[1024];

    extract_line(line_start, line_end, line,
                 sizeof(line));        // extract a single line from text
    add_string(patterns_array, line);  // add a string into array

    line_start = line_end + 1;
    line_num++;
  }

  if (*line_start != '\0') {  // if it wasnt the last line previously
    char last_line[1024];
    line_end = strchr(line_start, '\0');

    extract_line(line_start, line_end, last_line, sizeof(last_line));
    add_string(patterns_array, last_line);
  }
}

// Выделение строки из текста
char* extract_line(const char* line_start, const char* line_end, char* line,
                   size_t line_size) {
  size_t line_length = line_end - line_start;  // size == end - start
  if (line_length >= line_size) {              // if not enough space
    strncpy(line, line_start, line_size - 1);
    line[line_size - 1] = '\0';
  } else {
    strncpy(line, line_start, line_length);  // if normal
    line[line_length] = '\0';
  }
  return line;
}

// init regex array
int initialize_regex_arr(regex_t* regex_arr,
                         dynamic_string_array* patterns_array,
                         grep_opt grep_options) {
  int compiled_count = 0;  // counter for compiled regex
  for (int i = 0; i < patterns_array->size; i++) {
    int result = 0;
    if (grep_options.i == 1) {  // if opt i
      result = regcomp(&regex_arr[i], patterns_array->data[i],
                       REG_EXTENDED | REG_ICASE);
    } else {
      result = regcomp(&regex_arr[i], patterns_array->data[i],
                       REG_EXTENDED);  // if usual
    }
    if (result != 0) {  // exception workaround
      for (int j = 0; j < compiled_count; j++) {
        regfree(&regex_arr[j]);
      }
      return compiled_count;
    }
    compiled_count++;
  }
  return compiled_count;
}

void free_dynamic_string_array(dynamic_string_array* arr) {
  for (int i = 0; i < arr->size; i++) {
    free(arr->data[i]);  // Free each string
  }
  free(arr->data);  // Free the array of pointers
  arr->size = 0;
  arr->capacity = 0;
}

void add_string(dynamic_string_array* arr, const char* str) {
  if (str == NULL) {
    str = "";  // or handle as appropriate
  }

  if (arr->size == arr->capacity) {
    // Double the capacity or set to 1 if capacity is 0
    arr->capacity = arr->capacity == 0 ? 1 : arr->capacity * 2;
    // Reallocate memory for the array of pointers
    arr->data = realloc(arr->data, arr->capacity * sizeof(char*));
    if (arr->data == NULL) {
      perror("Memory allocation failed");
      exit(1);
    }
  }
  // Allocate memory for the new string
  arr->data[arr->size] = malloc((strlen(str) + 1) * sizeof(char));
  if (arr->data[arr->size] == NULL) {
    perror("Memory allocation failed");
    exit(1);
  }
  // Copy the string into the allocated memory
  strcpy(arr->data[arr->size], str);
  arr->size += 1;
}

void free_compiled_regex(regex_t* regex_arr,
                         int compiled_regex_count) {  // free regexes
  for (int j = 0; j < compiled_regex_count; j++) {
    regfree(&regex_arr[j]);
  }
}

// if file doesnt open it doesnt exist
int fileExists(const char* filePath) {  // Для флага -s
  int flag = 1;
  FILE* file = fopen(filePath, "r");
  if (file != NULL) {
    fclose(file);
    flag = 1;  // Файл существует
  } else {
    flag = 0;  // Файл не существует
  }

  return flag;
}

void process_text_grep(char* buffer, grep_opt grep_options, char* file_name,
                       regex_t* regex_arr, dynamic_string_array patterns_array,
                       dynamic_string_array strings_flag_o,
                       dynamic_string_array files_arr) {
  const char* line_start = buffer;
  const char* line_end = NULL;
  int line_num = 1;              // the num of curr line
  int matched_line_counter = 0;  // if it matches with what I seek

  while ((line_end = strchr(line_start, '\n')) != NULL) {
    char line[4096];

    extract_line(line_start, line_end, line,
                 sizeof(line));  // extract line from text
    // process the line, change the counter if found
    matched_line_counter +=
        process_line_grep(line, grep_options, file_name, files_arr,
                          patterns_array, strings_flag_o, regex_arr, line_num);

    line_start = line_end + 1;
    line_num++;
  }

  if (*line_start != '\0') {
    char last_line[4096];
    line_end = strchr(line_start, '\0');

    extract_line(line_start, line_end, last_line, sizeof(last_line));
    matched_line_counter +=
        process_line_grep(last_line, grep_options, file_name, files_arr,
                          patterns_array, strings_flag_o, regex_arr, line_num);
  }
  grep_flag_c(files_arr, grep_options, file_name, matched_line_counter);
  grep_flag_l(grep_options, matched_line_counter, file_name);
}

int process_line_grep(const char* line, grep_opt grep_options, char* file_name,
                      dynamic_string_array files_arr,
                      dynamic_string_array patterns_array,
                      dynamic_string_array strings_flag_o, regex_t* regex_arr,
                      int line_num) {
  int flag_matched = check_patterns_match(line, patterns_array, regex_arr);
  int flag_reversed = flag_matched == 1 ? 0 : 1;
  int flag_result = 0;

  if (grep_options.o == 1 && grep_options.v != 1) {
    reset_dynamic_string_array(&strings_flag_o);
    grep_flag_o(&patterns_array, &strings_flag_o, regex_arr, line);
  }

  if ((flag_matched == 1 && grep_options.v == 0) ||
      (flag_matched != 1 && grep_options.v == 1)) {
    if (grep_options.o == 1 && grep_options.v != 1) {
      print_or_not_flag_o(grep_options, files_arr, strings_flag_o, file_name,
                          line_num);
      reset_dynamic_string_array(&strings_flag_o);  // Free allocated strings
    } else {
      print_or_not_standart(grep_options, line, files_arr, file_name, line_num);
    }
  }

  if (grep_options.v == 1) {
    flag_result = flag_reversed;
  } else {
    flag_result = flag_matched;
  }

  return flag_result;
}

// if the pattern is in the text
int check_patterns_match(const char* line, dynamic_string_array patterns_array,
                         regex_t* regex_arr) {
  int flag_matched = 0;                      // how many matched
  int patterns_count = patterns_array.size;  // how many patterns

  // check each pattern for matches
  for (int i = 0; i < patterns_count; i++) {
    if (regexec(&regex_arr[i], line, 0, NULL, 0) == 0) {
      flag_matched = 1;
    }
  }

  return flag_matched;
}

void grep_flag_c(dynamic_string_array files_arr, grep_opt grep_options,
                 char* file_name, int matched_line_counter) {
  if ((grep_options.c == 1) && (grep_options.l == 0)) {
    grep_flag_h(files_arr, grep_options, file_name);
    printf("%d\n", matched_line_counter);
  }
}

void grep_flag_l(grep_opt grep_options, int matched_line_counter,
                 char* file_name) {
  if ((grep_options.l == 1) && (matched_line_counter > 0)) {
    printf("%s\n", file_name);
  }
}

void grep_flag_h(dynamic_string_array files_arr, grep_opt grep_options,
                 char* file_name) {
  if (files_arr.size > 1 && grep_options.h == 0) {
    printf("%s:", file_name);
  }
}

void reset_dynamic_string_array(dynamic_string_array* arr) {
  for (int i = 0; i < arr->size; i++) {
    free(arr->data[i]);
  }
  free(arr->data);
  arr->data = NULL;
  arr->size = 0;
  arr->capacity = 0;
}

void grep_flag_o(dynamic_string_array* patterns_array,
                 dynamic_string_array* strings_flag_o, regex_t* regex_arr,
                 const char* line) {
  int patterns_count = patterns_array->size;
  for (int i = 0; i < patterns_count; i++) {
    int offset = 0;
    find_matches_for_pattern(strings_flag_o, line, &regex_arr[i], &offset);
  }
}

void print_or_not_flag_o(grep_opt grep_options, dynamic_string_array files_arr,
                         dynamic_string_array strings_flag_o, char* file_name,
                         int line_num) {
  if ((grep_options.c == 0) && (grep_options.l == 0)) {
    for (int i = 0; i < strings_flag_o.size; i++) {
      grep_flag_h(files_arr, grep_options, file_name);
      grep_flag_n(grep_options, line_num);
      printf("%s\n", strings_flag_o.data[i]);
    }
  }
}

void grep_flag_n(grep_opt grep_options, int line_num) {
  if (grep_options.n == 1) {
    printf("%d:", line_num);
  }
}

void find_matches_for_pattern(dynamic_string_array* strings_flag_o,
                              const char* line, regex_t* regex, int* offset) {
  regmatch_t match;
  int ret;
  const size_t buffer_size = 4096;
  char matched_str[buffer_size];

  while ((size_t)(*offset) < strlen(line)) {
    ret = regexec(regex, line + *offset, 1, &match, 0);
    if (ret != 0) {
      break;
    }
    size_t match_length = match.rm_eo - match.rm_so;

    // Check for buffer overflow
    if (match_length + 1 > buffer_size) {
      // Handle the error, e.g., skip this match or report an error
      perror("Match exceeds buffer size, skipping this match.\n");
      *offset += match.rm_eo;
      continue;
    }

    // Copy the matched string into the static buffer
    strncpy(matched_str, line + *offset + match.rm_so, match_length);
    matched_str[match_length] = '\0';

    // Add the string to strings_flag_o
    // Assuming add_string makes its own copy of the string
    add_string(strings_flag_o, matched_str);

    *offset += match.rm_eo;
  }
}

void print_or_not_standart(grep_opt grep_options, const char* line,
                           dynamic_string_array files_arr, char* file_name,
                           int line_num) {
  if ((grep_options.c == 0) && (grep_options.l == 0) && (grep_options.o == 0)) {
    grep_flag_h(files_arr, grep_options, file_name);
    grep_flag_n(grep_options, line_num);

    printf("%s\n", line);
  }
}