#include <stdio.h>
#include <stdlib.h>
#include <sys/unistd.h>
#include <unistd.h>
#include "mastercfg.h"
#include "subjectcfg.h"
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

// dump the content of the subject_cfg
static void
dump_subject_cfg(subject_cfg *cfg) {
  apr_array_header_t *table_header = (apr_array_header_t *) apr_table_elts(cfg->cfg);
  printf("Elements: %d\n", table_header->nelts);
  // iteration context, keeps track of the iteration process
  // used to decide whether we've reached the end of the table or not.
  iter_ctx ctx;
  ctx.count = 0;
  ctx.nelems = table_header->nelts;
  apr_table_do(
    print_key_value_pair, 
    &ctx, 
    cfg->cfg, 
    NULL
  );
}

int 
main(int argc, char* argv[]) {
  cmd_options cmd_options;
  
  // initialize Apache Portable Runtime before doing anything else
  init_apr();
  
  apr_pool_t *pool;
  int apr_status = apr_pool_create(&pool, NULL);
  if(apr_status != APR_SUCCESS) {
    fprintf(stderr, "Error: Unable to create memory pool\n");
    exit(EXIT_FAILURE);
  }
  // the master configuration file. Considered to be the source of truth.
  // values in the master_cfg are considered to be correct. This is where the 
  // configuration of subject_cfg will be compared to
  master_cfg master_cfg;
  master_cfg.pool = pool;

  // the configuration to be verified will be written to
  subject_cfg subject_cfg;
  subject_cfg.pool = pool;
  subject_cfg.cfg = apr_table_make(subject_cfg.pool, 512);

  read_options(&cmd_options, argc, argv);
  verify_options(&cmd_options);
  init_master_cfg(&master_cfg, cmd_options.known_configuration);
  dump_master_cfg(&master_cfg);
  init_subject_cfg(&subject_cfg, cmd_options.file_to_scan);
  dump_subject_cfg(&subject_cfg);

  return 0; 
}
