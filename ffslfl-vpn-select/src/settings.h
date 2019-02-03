#pragma once

static const char * get_fastd_secret(struct uci_context *ctx);

static const char * get_first_option(struct uci_context *ctx, struct uci_package *p, const char *type, const char *option);