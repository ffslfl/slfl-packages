#pragma once

#include <uci.h>

const char *get_fastd_secret(struct uci_context *ctx);

void set_fastd_secret(struct uci_context *ctx, const char *secret);

const char *get_fastd_pubkey(struct uci_context *ctx);

const char *
get_first_option(struct uci_context *ctx, struct uci_package *p, const char *type, const char *option);

struct uci_section *get_first_section(struct uci_package *p, const char *type);