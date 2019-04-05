#pragma once

static const char *get_fastd_secret(struct uci_context *ctx);

void set_fastd_secret(struct uci_context *ctx, const char *secret);

static const char *get_fastd_pubkey(struct uci_context *ctx);

static const char *
get_first_option(struct uci_context *ctx, struct uci_package *p, const char *type, const char *option);

static struct uci_section *get_first_section(struct uci_package *p, const char *type);