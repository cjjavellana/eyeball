#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/unistd.h>
#include <unistd.h>
#include <string.h>
#include "mastercfg.h"

#include "third_party/apr/include/apr_errno.h"
#include "third_party/apr/include/apr_pools.h"
#include "third_party/apr/include/apr_tables.h"

#define PCRE2_CODE_UNIT_WIDTH 8
#include "third_party/pcre2/src/pcre2.h"
#include "third_party/yaml/include/yaml.h"

/**
 * Extracts key key/value pair from _token_to_parse_ and assigns the key token 
 * to kv->key and value token to kv->value
 */
static void
extract_key_value_pair(key_value_pair *kv, char *token_to_parse) {
  int error_code;
  PCRE2_SIZE error_offset;
  pcre2_code *re = pcre2_compile(
    (PCRE2_UCHAR8 *) CFG_KEYVALUEPAIR_PATTERN, 
    PCRE2_ZERO_TERMINATED,                      // Pattern is zero-terminated
    0, 
    &error_code,
    &error_offset,
    NULL
  );

  if(re == NULL) {
    PCRE2_UCHAR8 buff[128];
    pcre2_get_error_message(error_code, buff, 120);
    printf("[ERROR] Unable to compile CFG_KEYVALUEPAIR_PATTERN: %s\n", buff);
    exit(EXIT_FAILURE);
  }

  uint32_t ovecsize = 128;
  pcre2_match_data *match_data = pcre2_match_data_create(ovecsize, NULL);
  int rc = pcre2_match(
      re,
      (PCRE2_UCHAR8 *) token_to_parse,
      strlen(token_to_parse),
      0,
      0,
      match_data,
      NULL
  );

  if(rc == 0) {
    fprintf(stderr, "[ERROR] offset vector too small: %d\n", rc);
  } else if (rc > 0) {
    PCRE2_SIZE *ovector = pcre2_get_ovector_pointer(match_data);
    PCRE2_SIZE i;
    for(i = 1; i < rc; i++) {
      PCRE2_SPTR start = (PCRE2_UCHAR8 *) token_to_parse + ovector[2*i];
      PCRE2_SIZE slen = ovector[2*i+1] - ovector[2*i];

      char *found_data = malloc(slen);    // allocate space in the heap equal to the length of the matched string
      sprintf(found_data, "%.*s", (int) slen, (char *) start); // write the matched string into found_data buffer
      if(i == 1) {
        kv->key = found_data;
      } else if(i == 2) {
        kv->value = found_data;
      }
    }
  } else {
    printf("[INFO] No match found: %s\n", token_to_parse);
  }

  pcre2_match_data_free(match_data);
  pcre2_code_free(re);
}

/**
 * @param env_settings is a newline (\n) delimited text block from the master configuration file.
 * @param env_table is a table that will contain a key-value pair of settings parsed & tokenized from
 *  the env_settings
 */
static void
init_per_env_cfg_table(apr_table_t* env_table, char* env_settings) {
  char* token = strtok(env_settings, "\n");
  while(token != NULL) {
    // at this point, token is in the format of:
    // spring.data.jpa.url: jdbc:mariadb://localhost:3306/db1
    key_value_pair kv;
    extract_key_value_pair(&kv, token);

    // set the extracted key-value pair into the env table
    apr_table_set(env_table, kv.key, kv.value);
    
    // move to the next element to parse
    token = strtok(NULL, "\n");
   }
}

/**
* Loads the master configuration into the static global variable
*/
void 
init_master_cfg(master_cfg* master_cfg, char* known_configuration) {
  yaml_parser_t parser;
  yaml_token_t token;

  /* Initial libyaml parser */
  if(!yaml_parser_initialize(&parser))
    fputs("Failed to initialize parser!\n", stderr);

  printf("[INFO] Opening File: %s\n", known_configuration);

  FILE *fh = fopen(known_configuration, "r");
  if(fh == NULL)
    fputs("Failed to open file!\n", stderr);
  
  yaml_parser_set_input_file(&parser, fh);

  // variable used to switch whether we have read a key token or a value token
  int state = 0;
  // variable used to hold the token read by the parser
  yaml_char_t* tk;

  // variable used to track the number of environments read
  // the `environment` here refers to a deployment environment e.g. dev,uat,preprod,prod
  int env_index = 0;

  do {
    yaml_parser_scan(&parser, &token);
    switch (token.type) {
      case YAML_KEY_TOKEN: state = 0; break;
      case YAML_VALUE_TOKEN: state = 1; break;
      case YAML_SCALAR_TOKEN:
        tk = token.data.scalar.value;

        if(state == 0) {
          // YAML Key i.e. the environments
          master_cfg->env[env_index] = strdup((char *)tk);
        } else {
          // YAML Value
          
          apr_table_t* t = apr_table_make(master_cfg->pool, MAX_ENVS_COUNT);
          master_cfg->cfg[env_index] = t;

          init_per_env_cfg_table(t, (char *)tk);

          // increment the env index pointer
          env_index += 1;
        }

        break;
      default:
        break;
    }

    if(token.type != YAML_STREAM_END_TOKEN) {
      yaml_token_delete(&token);
    }
  } while(token.type != YAML_STREAM_END_TOKEN);

  master_cfg->element_count = env_index;

  yaml_token_delete(&token);
  yaml_parser_delete(&parser);
  fclose(fh);
}

