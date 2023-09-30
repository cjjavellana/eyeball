#include "subjectcfg.h"
#include "third_party/yaml/include/yaml.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum yaml_value {
  KEY, VAL, SEQ
};

static void
print_values(
  char *key,        // the key parsed from the yaml element
  int *seq_count,   // we're mutating seq_count so we need a pointer
  char *value      // the value parsed from the yaml element
) {
  if(*seq_count) {
    printf("[Value] Key: %s.%d, Value: %s\n", key, *seq_count, value);
    *seq_count += 1;
  } else {
    printf("[Value] Key: %s, Value: %s\n", key, value);
  }
}

/**
* For top level elements, the key is null. Thus, return yaml_key as the top level key. 
* For nested elements, we need to concatenate the old key with the new key. Separated by a period.
*/
static char *
concat(
  const char *old_key,
  const char *yaml_key
) {
   char *k = NULL;
   if(old_key != NULL) {
     k = malloc(strlen(old_key) + strlen(yaml_key) + 2);
     strcpy(k, old_key);
     strcat(k, ".");
     strcat(k, yaml_key);
   } else {
     k = malloc(strlen(yaml_key) + 1);
     strcat(k, yaml_key);
   }

   return k;
}

static void
parse_internal(
  yaml_parser_t *parser, 
  char *key
) {

  yaml_event_t event;
  int yaml_value = KEY;   // variable used to track whether the event is a key or value
  int seq_count = 0;      // variable used to track the number of sequence elements
  char *old_key = key;    // remember the old key, we'll use this to concatenate the new key

  while(1) {
    yaml_parser_parse(parser, &event);
    char *value = (char *) event.data.scalar.value;

    if (event.type == YAML_SCALAR_EVENT) {
      if (yaml_value) {
        print_values(key, &seq_count, value);
      } else {
        // if the key is null, then we're at the top level
        key = (!key) ? strdup(value): concat(old_key, value);
      }
      yaml_value ^= VAL; 
    }
    else if (event.type == YAML_SEQUENCE_START_EVENT) {
      printf("Sequence Start \n");
      yaml_value = SEQ;
      seq_count = 1;
    } else if (event.type == YAML_SEQUENCE_END_EVENT) {
      printf("Sequence End \n");
      yaml_value = KEY;
      seq_count = 0;
    } else if (event.type == YAML_MAPPING_START_EVENT) {
      char *k = (key) ? strdup(key) : NULL;
      printf("Mapping Start: %s\n", k);
      parse_internal(parser, k);
      printf("After parse_internal. Key: %s, Old: %s\n", key, old_key);
      yaml_value ^= VAL; 
    } else if (event.type == YAML_MAPPING_END_EVENT) {
      printf("Mapping End \n");
      free(key);
      break;
    }else if (event.type == YAML_STREAM_END_EVENT) {
      printf("Stream End \n");
      free(key);
      break;
    }

    yaml_event_delete(&event);
  }
}

void
init_subject_cfg(subject_cfg *subject_cfg, char *cfgfile) {

  yaml_parser_t parser;
  if(!yaml_parser_initialize(&parser)) {
    fprintf(stderr, "Unable to initialize yaml_parser\n");
    exit(EXIT_FAILURE);
  }

  FILE *fh = fopen(cfgfile, "r");
  if(fh == NULL) {
    fprintf(stderr, "Unable to open file: %s\n", cfgfile);
    exit(EXIT_FAILURE);
  }

  yaml_parser_set_input_file(&parser, fh);
  parse_internal(&parser, NULL);

  // clean up
  yaml_parser_delete(&parser);
  fclose(fh);
}
