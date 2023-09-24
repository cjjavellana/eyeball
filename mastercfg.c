#include <stdio.h>
#include <stdlib.h>
#include <sys/unistd.h>
#include <unistd.h>
#include "mastercfg.h"

#include "third_party/yaml/include/yaml.h"
#include "third_party/apr/include/apr.h"

static void
parse_internal(yaml_parser_t *parser, yaml_event_t *event) {

  while(1) {
    yaml_parser_parse(parser, event)  ;
    
    printf("[INFO] Event found: %x\n", event->type);

    if(event->type == YAML_SCALAR_EVENT) {
      
    } else if(event->type == YAML_MAPPING_END_EVENT ||
      event->type == YAML_STREAM_END_EVENT) {
      break;
    }

    yaml_event_delete(event);
  }
}

/**
* Loads the master configuration into the static global variable
*/
void 
init_master_cfg(char* known_configuration) {
  yaml_parser_t parser;
  if(!yaml_parser_initialize(&parser))
    fputs("Failed to initialize parser!\n", stderr);

  printf("[INFO] Opening File: %s\n", known_configuration);

  FILE *fh = fopen(known_configuration, "r");
  if(fh == NULL)
    fputs("Failed to open file!\n", stderr);
  
  yaml_parser_set_input_file(&parser, fh);
  yaml_event_t event;

  parse_internal(&parser, &event);
  
  // cleanup
  yaml_parser_delete(&parser);
  fclose(fh);
}

