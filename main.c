#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/unistd.h>
#include <unistd.h>
#include "mastercfg.h"
#include "subjectcfg.h"
#include "patterns.h"
#include "verifier.h"

#include "third_party/apr/include/apr_general.h"
#include "third_party/apr/include/apr_pools.h"
#include "third_party/apr/include/apr.h"
#include "third_party/apr/include/apr_tables.h"

#define MAX_CONFIG_PER_FILE 512
#define MAX_PATTERN 64

typedef struct {
  int count;
  int nelems;
} iter_ctx;

typedef struct {
  char *file_to_scan;
  char *known_configuration;
  char *env;
  char **pattern;
  int pattern_count;
} cmd_options;

/**
* How to use this thing.
*/
static void
show_usage() {
  printf("\n");
  printf("Usage:\n");
  printf("$ eyeball -f <name of the file to scan> -c <configuration file> -e <environment e.g. dev/uat/prod> [-p <pattern1> -p <pattern2>]\n");
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

  int pattern_count = 0;
  int opt;
  while((opt = getopt(argc, argv, "f:c:p:e:")) != -1) {
    switch (opt) {
      case 'f':
        cmd_options->file_to_scan = optarg;
        break;
      case 'c':
        cmd_options->known_configuration = optarg;
        break;
      case 'e':
        cmd_options->env = optarg;
        break;
      case 'p':
        cmd_options->pattern[pattern_count] = optarg;
        pattern_count++;
        if(pattern_count >= MAX_PATTERN) {
          fprintf(stderr, "Error: Too many patterns\n");
          show_usage();
          exit(EXIT_FAILURE);
        }
        break;
    }
  }

  // If we do not have any patterns, then we'll use the default pattern
  if(pattern_count == 0) {
    cmd_options->pattern[0] = DOMAIN_PATTERN;
    pattern_count++;
  }

  // set selected environment defaults
  if(cmd_options->env == NULL) {
    cmd_options->env = malloc(4 /* prod */ + 1 /* terminal char */);
    strcpy(cmd_options->env, "prod");
  }

  cmd_options->pattern_count = pattern_count;
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
  cmd_options.env = NULL;
  
  // initialize Apache Portable Runtime before doing anything else
  init_apr();
  
  apr_pool_t *pool;
  int apr_status = apr_pool_create(&pool, NULL);
  if(apr_status != APR_SUCCESS) {
    fprintf(stderr, "Error: Unable to create memory pool\n");
    exit(EXIT_FAILURE);
  }
  read_options(&cmd_options, argc, argv);
  verify_options(&cmd_options);

  // the master configuration file. Considered to be the source of truth.
  // values in the master_cfg are considered to be correct. This is where the 
  // configuration of subject_cfg will be compared to
  master_cfg master_cfg;
  master_cfg.pool = pool;
  master_cfg.selected_env = cmd_options.env;

  init_master_cfg(&master_cfg, cmd_options.known_configuration);
  dump_master_cfg(&master_cfg);

  // the configuration to be verified will be written to
  subject_cfg subject_cfg;
  subject_cfg.pool = pool;
  subject_cfg.cfg = apr_table_make(subject_cfg.pool, MAX_CONFIG_PER_FILE);
  subject_cfg.patterns_to_match = cmd_options.pattern;
  subject_cfg.pattern_count = cmd_options.pattern_count;

  init_subject_cfg(&subject_cfg, cmd_options.file_to_scan);
  dump_subject_cfg(&subject_cfg);
  
  return verify_subject_cfg(&subject_cfg, &master_cfg); 

}
