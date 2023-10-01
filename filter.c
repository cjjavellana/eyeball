#include <stdio.h>
#include "filter.h"

#define PCRE2_CODE_UNIT_WIDTH 8
#include "third_party/pcre2/src/pcre2.h"

void
filter_unrelated_cfg(
  apr_table_t *table,
  filter_pattern *patterns
) {
  // Store the compiled regexes in an array
  pcre2_code *re[patterns->patterns_count];

  for(int i = 0; i < patterns->patterns_count; i++) {
    printf("Filtering: %s\n", patterns->patterns[i]);

    int error_code;
    PCRE2_SIZE error_offset;

    re[i] = pcre2_compile(
      (PCRE2_UCHAR8 *) patterns->patterns[i], 
      PCRE2_ZERO_TERMINATED,                      // Pattern is zero-terminated
      0, 
      &error_code,
      &error_offset,
      NULL
    );

    if(re[i] == NULL) {
      PCRE2_UCHAR8 buff[128];
      pcre2_get_error_message(error_code, buff, 120);
      printf("[ERROR] Unable to compile %s: %s\n", patterns->patterns[i], buff);
      exit(EXIT_FAILURE);
    }

  }
}
