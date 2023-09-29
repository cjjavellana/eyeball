#include <stdio.h>
#include <stdlib.h>
#include <sys/unistd.h>
#include <unistd.h>
#include "mastercfg.h"
#include "third_party/apr/include/apr_general.h"
#include "third_party/apr/include/apr_pools.h"
#include "third_party/apr/include/apr.h"
#include "third_party/apr/include/apr_tables.h"

typedef struct {
  int count;
  int nelems;
} iter_ctx;

typedef struct {
  char* file_to_scan;
  char* known_configuration;
} cmd_options;

/**
* How to use this thing.
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
 * Parses command line arguments and populates the cmd_options struct.
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

/**
* verify_options verifies that: 
* 1. File to scan is not null and exists. 
* 2. The known configuration master list exists.
*/
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

/**
* Initializes Apache Portable Runtime
*/
static void
init_apr() {
  apr_initialize();
}

static int
print_key_value_pair(void *rec, const char *key, const char *value) {
  printf("[INFO] Key: %s, Value: %s\n", key, value);
  iter_ctx *ctx = (iter_ctx *) rec;
  ctx->count = ctx->count + 1;

  // While we still have element return non-zero.
  return (ctx->count < ctx->nelems) ? 1 : 0;
}

static void
dump_master_cfg(master_cfg *cfg) {
  for(int i = 0; i < cfg->element_count; i++) {
    printf("Env: %s\n", cfg->env[i]);
    apr_array_header_t *table_header = (apr_array_header_t *) apr_table_elts(cfg->cfg[i]);
    printf("Elements: %d\n", table_header->nelts);

    // iteration context, keeps track of the iteration process
    // used to decide whether we've reached the end of the table or not.
    iter_ctx ctx;
    ctx.count = 0;
    ctx.nelems = table_header->nelts;

    apr_table_do(
      print_key_value_pair, 
      &ctx, 
      cfg->cfg[i], 
      NULL
    );
  }
}

int 
main(int argc, char* argv[]) {
  cmd_options cmd_options;
  master_cfg master_cfg;

  init_apr();
  read_options(&cmd_options, argc, argv);
  verify_options(&cmd_options);
  init_master_cfg(&master_cfg, cmd_options.known_configuration);
  dump_master_cfg(&master_cfg);

  return 0; 
}
