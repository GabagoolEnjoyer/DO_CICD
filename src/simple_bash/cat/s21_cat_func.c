#include "s21_cat_func.h"

void parse_flags(int argc, char **argv, cat_opt *cat_options) {
  int opt = 0;
  // Define the long options for getopt_long
  static struct option long_options[] = {
      {"number-nonblank", no_argument, 0, 'b'},
      {"number", no_argument, 0, 'n'},
      {"squeeze-blank", no_argument, 0, 's'},
      {0, 0, 0, 0}};
  const char *short_options = "benstvET";  // Define the short options

  // Parse the command-line options using getopt_long
  while ((opt = getopt_long(argc, argv, short_options, long_options, NULL)) !=
         -1) {
    switch (opt) {
      case 'b':
        cat_options->b = 1;
        break;
      case 'e':
        cat_options->e = 1;
        cat_options->v = 1;
        break;
      case 'n':
        cat_options->n = 1;
        break;
      case 's':
        cat_options->s = 1;
        break;
      case 't':
        cat_options->t = 1;
        cat_options->v = 1;
        break;
      case 'v':
        cat_options->v = 1;
        break;
      case 'E':
        cat_options->e = 1;
        break;
      case 'T':
        cat_options->t = 1;
        break;
      case '?':
        cat_options->uninitialized_flag = 1;
        break;
    }
  }

  // Check if an uninitialized option was used
  if (cat_options->uninitialized_flag == 1) {
    printf("Uninitialised option used!\n");
    exit(1);
  }
}

void process_and_print_char(cat_opt *cat_options, unsigned char uc,
                            int *line_number_printed) {
  // Handle tab characters
  if (uc == '\t') {
    if (cat_options->t || cat_options->T) {
      printf("^I");
    } else {
      putchar(uc);
    }
  }
  // Handle newline characters
  else if (uc == '\n') {
    if (cat_options->e || cat_options->E) {
      printf("$\n");
      (*line_number_printed) = 0;
    } else {
      putchar(uc);
      (*line_number_printed) = 0;
    }
  }
  // Handle other characters with the 'v' option
  else if (cat_options->v) {
    if (uc < 32 && uc != '\t' && uc != '\n') {
      printf("^%c", uc + 64);
    } else if (uc == 127) {
      printf("^?");
    } else if (uc > 127 && uc < 160) {
      printf("M-^%c", uc - 64);
    } else if (uc >= 160 && uc < 255) {
      printf("M-%c", uc - 128);
    } else if (uc == 255) {
      printf("M-^?");
    } else {
      putchar(uc);
    }
  }
  // Print the character as is
  else {
    putchar(uc);
  }
}

void process_file(const char *filename, cat_opt *cat_options,
                  file_work *file_options) {
  // Open the file for reading
  FILE *file = fopen(filename, "r");
  if (!file) {
    perror(filename);
    file_options->flag_exit = 1;
  }

  // If the file was successfully opened, process its contents
  if (!file_options->flag_exit) {
    int empty_lines = 0;
    int is_empty = 1;
    int line_number_printed = 0;

    int c;

    // Read the file character by character
    while ((c = fgetc(file)) != EOF) {
      unsigned char uc = (unsigned char)c;

      // Check if the line is empty
      if (uc == '\n') {
        is_empty = 1;
      } else {
        is_empty = 0;
      }

      // Print the line number if the 'b' or 'n' option is set
      if (!line_number_printed) {
        if (cat_options->b && !is_empty) {
          printf("%6d\t", file_options->line_number++);
          line_number_printed = 1;
        } else if (cat_options->n) {
          printf("%6d\t", file_options->line_number++);
          line_number_printed = 1;
        }
      }

      // Handle the 's' option to squeeze multiple blank lines
      if (cat_options->s && uc == '\n') {
        if (empty_lines >= 2) {
          continue;
        }
        empty_lines++;
      } else {
        empty_lines = 0;
      }

      // Process and print the character
      process_and_print_char(cat_options, uc, &line_number_printed);
    }

    // Increment the file count and close the file
    file_options->num_file++;
    fclose(file);
  }
}