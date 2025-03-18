#include "s21_grep.h"

int main(int argc, char *argv[]) {
  dynamic_string_array files_arr = {NULL, 0, 0};  // file paths in argv
  dynamic_string_array patterns_files = {NULL, 0,
                                         0};  // pattern file paths in argv
  dynamic_string_array patterns_array = {NULL, 0,
                                         0};  // final pattern destination
  dynamic_string_array strings_flag_o = {
      NULL, 0, 0};              // substrings for flag o (not implemented)
  grep_opt grep_options = {0};  // init options for grep
  regex_t *regex_arr = NULL;    // init regex arr

  parse_flags(argc, argv, &grep_options, &patterns_array, &patterns_files);

  if (grep_options.uninitialized_flag ==
      1) {  // if uninitialized option selected
    free_all_dynamic_string_arrays(&files_arr, &patterns_files, &patterns_array,
                                   &strings_flag_o);
    perror("Uninitialized option used!");
    return 1;
  }

  flags_e_and_f_workaraound(&grep_options, argc, argv, &files_arr,
                            &patterns_array);

  if (grep_options.f == 1) {
    if (!flag_f_load_patterns(&patterns_files, &patterns_array)) {
      free_all_dynamic_string_arrays(&files_arr, &patterns_files,
                                     &patterns_array, &strings_flag_o);
      perror("Error while loading file patterns");
      return 1;
    }
  }

  if (patterns_array.size == 0) {  // if no patterns
    perror("No patterns added!");
    free_all_dynamic_string_arrays(&files_arr, &patterns_files, &patterns_array,
                                   &strings_flag_o);
    return 1;
  }

  regex_arr = malloc(patterns_array.size *
                     sizeof(regex_t));  // allocate memory for enough regex
  if (regex_arr == NULL) {
    perror("Memory allocation failed");
    free_all_dynamic_string_arrays(&files_arr, &patterns_files, &patterns_array,
                                   &strings_flag_o);
    return 1;
  }

  int compiled_regex_count = initialize_regex_arr(
      regex_arr, &patterns_array, grep_options);  // count of regexes compiled
  if (compiled_regex_count !=
      patterns_array.size) {  // if not all patterns compiled succesfully
    free_compiled_regex(regex_arr, compiled_regex_count);
    free(regex_arr);
    free_all_dynamic_string_arrays(&files_arr, &patterns_files, &patterns_array,
                                   &strings_flag_o);
    perror("Regex initialization error!");
    return 1;
  }

  for (int i = 0; i < files_arr.size; i++) {
    char *file_name = files_arr.data[i];      // name of curr file
    int flag_exists = fileExists(file_name);  // checks if file exists

    if (flag_exists == 1) {                      // if exists then continue
      FILE *file = fopen(file_name, "r");        // open curr file
      int filesize = check_filesize(file_name);  // check its len
      char *buffer = read_file_to_buffer(file, filesize);

      process_text_grep(buffer, grep_options, file_name, regex_arr,
                        patterns_array, strings_flag_o,
                        files_arr);  // the function to process text

      free(buffer);
      fclose(file);
    } else {
      // dont print anything if -s
      if (grep_options.s == 0) {
        printf("file %s does not exist\n", file_name);
      }
    }
  }

  free_all_dynamic_string_arrays(&files_arr, &patterns_files, &patterns_array,
                                 &strings_flag_o);
  free_compiled_regex(regex_arr, compiled_regex_count);
  free(regex_arr);

  return 0;
}