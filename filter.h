#include "third_party/apr/include/apr_tables.h"

// Pattern to match domain names 
// Domain names may contain subdomains and may contain dashes
// Domain names may contain ports
#define DOMAIN_NAME_PATTERN "^([a-zA-Z0-9]+(-[a-zA-Z0-9]+)*\\.)+[a-zA-Z]{2,}$"

typedef struct {
  char **patterns;
  int patterns_count;
} filter_pattern;

/**
* Removes all keys that do not match the patterns array in the table.
*/
void
filter_unrelated_cfg(
  apr_table_t *table,     // The table to filter
  filter_pattern *patterns // The patterns to match
);
