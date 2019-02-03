#include "settings.h"

#include <uci.h>
#include <stdbool.h>
#include <printf.h>
//#include <libgluonutil.h>

static const char * get_fastd_secret(struct uci_context *ctx) {
  struct uci_package *p;
  if (!uci_load(ctx, "fastd", &p)) {
    const char *secret = get_first_option(ctx, p, "secret");
    if (!secret || !*secret)
      return NULL;

    return secret;
  }
}

static const char * get_first_option(struct uci_context *ctx, struct uci_package *p, const char *type, const char *option) {
  struct uci_section *s = get_first_section(p, type);
  if (s)
    return uci_lookup_option_string(ctx, s, option);
  else
    return NULL;
}