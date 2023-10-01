#define PCRE2_CODE_UNIT_WIDTH 8
#include "third_party/pcre2/src/pcre2.h"

// Pattern for matching URLs, domain names, and IP addresses
// e.g. http://kubernetes.default.svc.cluster.local:8080, someserver.domain.com:8080,
// jdbc:mariadb://someserver.domain.com:3306, 
#define DOMAIN_PATTERN "^(jdbc:mariadb|https?)://(.*)(:\\d+)?(\\/.*)?$"

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

