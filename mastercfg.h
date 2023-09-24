#include "third_party/apr/include/apr.h"
#include "third_party/apr/include/apr_hash.h"
#include "third_party/yaml/include/yaml.h"

#define MAX_CFG_KEYLEN 10

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
  char* env[MAX_CFG_KEYLEN];
  apr_hash_t* cfg[];
} master_cfg;

/**
* Parses the master configuration file.
*/
void
init_master_cfg(char* known_configuration);
