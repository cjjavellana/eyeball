#ifndef SUBJECTCFG_H
#define SUBJECTCFG_H

#include "third_party/apr/include/apr_tables.h"

/**
 * Holds the configuration parsed from the subject file. A subject file is a file that is 
 * subject for verification.
 *
 * The subject_cfg struct can only hold a single environment's worth of configuration. 
 */
typedef struct {
  apr_table_t *cfg;                 // Where the parsed configuration will be written
  apr_pool_t  *pool;                // The memory pool to use for the cfg table
  char        **patterns_to_match;  // The patterns of the values to match
  int         pattern_count;       // The number of patterns to match
} subject_cfg;

/**
* Initializes the subject configuration.
*
* @param *subject_cfg where the parsed configuration will be written
* @param *cfgfile the subject file to parse. Takes either a java properties file or a yaml file.
*  if cfgfile is a yaml file, flattens the keys before writing it into the *cfg
*  field of the subject_cfg struct
*/
void
init_subject_cfg(subject_cfg *subject_cfg, char *cfgfile);

#endif
