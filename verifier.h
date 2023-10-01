#ifndef VERIFIER_H
#define VERIFIER_H

#include "mastercfg.h"
#include "subjectcfg.h"

typedef struct {
  int          count;
  int          nelems;
  apr_table_t* selected_env_cfg;

  // flag to indicate if there are errors in the verification
  // 0 means no errors, 1 means there are errors
  int           has_errors;
} iter_verify_ctx;

/**
* Returns 0 if configurations in subject_cfg match the configurations in master_cfg. Returns 1 otherwise.
 */
int
verify_subject_cfg(
subject_cfg *subject_cfg, /* The configuration to verify */
master_cfg *master_cfg    /* The master configuration to verify against */
);

#endif
