#include <stdio.h>
#include "patterns.h"

pcre2_code *
compile_pattern(const char *filter_pattern) {

  pcre2_code *re;
  int error_code;
  PCRE2_SIZE error_offset;

  re = pcre2_compile(
    (PCRE2_UCHAR8 *) filter_pattern,
    PCRE2_ZERO_TERMINATED,                      // Pattern is zero-terminated
    0, 
    &error_code,
    &error_offset,
    NULL
  );

  if(re == NULL) {
    PCRE2_UCHAR8 buff[128];
    pcre2_get_error_message(error_code, buff, 120);
    printf("[ERROR] Unable to compile %s: %s\n", filter_pattern, buff);
    exit(EXIT_FAILURE);
  }

  return re;
}

