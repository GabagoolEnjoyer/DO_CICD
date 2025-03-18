#include "s21_cat_func.h"

int main(int argc, char *argv[]) {
  // Check if the number of arguments is less than 2
  if (argc < 2) {
    printf("Utility usage: s21_cat [OPTION] [FILE]...\n");
    return 1;
  }

  // Initialize the structure for storing cat options with default values (all
  // flags set to 0)
  cat_opt cat_options = {0};

  // Initialize the structure for file handling with default values
  file_work file_options = {0, 1, 0};

  // Parse the command-line flags and store them in the cat_options structure
  parse_flags(argc, argv, &cat_options);

  // Check if no file is specified after parsing the flags
  if (argc - optind == 0) {
    printf("Incorrect utility usage! No filename given. \n");
    return 1;
  }

  // Process each file specified in the command line
  for (int i = optind; i < argc; i++) {
    process_file(argv[i], &cat_options, &file_options);
  }

  return 0;
}