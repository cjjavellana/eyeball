#include <stdio.h>
#include <stdlib.h>
#include <sys/unistd.h>
#include <unistd.h>

typedef struct {
  char* file_to_scan;
  char* known_configuration;
} cmd_options;

/**
*
*/
static void
show_usage() {
  printf("\n");
  printf("Usage:\n");
  printf("$ eyeball -f <name of the file to scan> -c <configuration file>\n");
  printf("\n");
  printf("Supports: yml and java properties file formats\n");
  printf("For more information visit: https://github.io/eyeball");
}

/**
* read_options verifies that: 
* 1. There is one and only 1 argument
* 2. argv[1] refers to a valid file
*/
static void 
read_options(
  cmd_options *cmd_options, const int argc, char* argv[]) {

  int opt;

  while((opt = getopt(argc, argv, "f:c:")) != -1) {
    switch (opt) {
      case 'f':
        cmd_options->file_to_scan = optarg;
        break;
      case 'c':
        cmd_options->known_configuration = optarg;
        break;
    }
  }
}

static void
verify_options(cmd_options *cmd_options) {
  if(cmd_options->file_to_scan == NULL ||
    access(cmd_options->file_to_scan, F_OK) != 0) {
    fprintf(stderr, "Error: File %s does not exist\n", cmd_options->file_to_scan);
    show_usage();
    exit(EXIT_FAILURE);
  }

  if(cmd_options->known_configuration == NULL ||
    access(cmd_options->known_configuration, F_OK) != 0) {
    fprintf(stderr, "Error: Configuration File %s does not exist\n", cmd_options->known_configuration);
    show_usage();
    exit(EXIT_FAILURE);
  }
}

int 
main(int argc, char* argv[]) {
  cmd_options cmd_options;

  read_options(&cmd_options, argc, argv);
  verify_options(&cmd_options);

  return 0; 
}
