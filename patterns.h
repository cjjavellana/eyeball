#define PCRE2_CODE_UNIT_WIDTH 8
#include "third_party/pcre2/src/pcre2.h"

// Pattern to match domain names 
// Domain names may contain subdomains and may contain dashes
// Domain names may contain ports
#define DOMAIN_NAME_PATTERN "^([a-zA-Z0-9]+(-[a-zA-Z0-9]+)*\\.)+[a-zA-Z]{2,}$"

/**
* Removes all keys that do not match the patterns array in the table.
*/
pcre2_code *
compile_pattern(const char *filter_pattern);

/**
* Matches the pattern against the subject string.
*
* Returns Non-zerp if the pattern matches the subject string, 0 otherwise.
*/
int 
match_pattern(pcre2_code *re, const char *subject);

