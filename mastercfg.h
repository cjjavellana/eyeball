#include "third_party/apr/include/apr.h"
#include "third_party/apr/include/apr_hash.h"
#include "third_party/apr/include/apr_tables.h"
#include "third_party/yaml/include/yaml.h"

#define MAX_ENVS_COUNT 15
#define CFG_KEYVALUEPAIR_PATTERN "^(.*?):\\s?(.*)$"

/**
 *  master_cfg structure is a hierarchal datastructure for storing
 *  the per environment configuration.
 *
 *  Format for visualization purposes is as follow:
 *  {
 *    "dev": apr_hash_t,
 *    "uat": apr_hash_t,
 *    "preprod": apr_hash_t,
 *    "prod": apr_hash_t
 *  }
 *
 */
typedef struct {
  char*           env[MAX_ENVS_COUNT];
  apr_table_t*     cfg[MAX_ENVS_COUNT];

  // The total number of environments in the master_cfg struct
  unsigned short  element_count;
} master_cfg;

typedef struct {
  char *key;
  char *value;
} key_value_pair;

/**
* Parses the master configuration file.
*/
void
init_master_cfg(master_cfg* master_cfg, char* known_configuration);

