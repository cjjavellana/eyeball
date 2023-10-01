#include "verifier.h"
#include "third_party/apr/include/apr_tables.h"

// a callback function for apr_table_do
static int
verify_in_known_configuration(void *rec, const char *key, const char *value) {
  iter_verify_ctx *ctx = (iter_verify_ctx *) rec;
  const char *truth = apr_table_get((apr_table_t *) ctx->selected_env_cfg, key);
  if(truth == NULL || strcmp(truth, value)) {
    ctx->has_errors = 1;
    printf("ERROR: %s is %s, but should be %s\n", key, value, truth);
  } else {
    printf("OK: %s is %s\n", key, value);
  }

  ctx->count++;

  // while we still have elements return non-zero
  return (ctx->count < ctx->nelems) ? 1 : 0;
}

int
verify_subject_cfg(subject_cfg *subject_cfg, master_cfg *master_cfg) {

  // find the index of the selected environment
  int idx = 0;
  for(; idx < master_cfg->element_count; idx++) {
    if(!strcmp(master_cfg->env[idx], master_cfg->selected_env)) {
      break;
    }
  }

  // initialize the iteration context
  iter_verify_ctx ctx;
  ctx.count = 0;
  ctx.nelems = apr_table_elts(subject_cfg->cfg)->nelts;
  ctx.selected_env_cfg = master_cfg->cfg[idx];
  ctx.has_errors = 0;

  apr_table_do(
    verify_in_known_configuration,
    &ctx,
    subject_cfg->cfg,
    NULL
  );

  return ctx.has_errors;
}
