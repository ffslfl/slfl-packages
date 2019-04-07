#include "settings.h"

#include <uci.h>
#include <stdbool.h>
//#include <libgluonutil.h>
#include <stdio.h>
#include <string.h>

const char *get_fastd_secret(struct uci_context *ctx) {
    struct uci_package *p;
    if (!uci_load(ctx, "fastd", &p)) {
        const char *secret = get_first_option(ctx, p, "fastd", "secret");
        if (!secret || !*secret)
            return NULL;

        return secret;
    }
    return NULL;
}

void set_fastd_secret(struct uci_context *ctx, const char *secret) {
    struct uci_ptr uci_ptr;
    memset(&uci_ptr, 0, sizeof(uci_ptr));

    uci_ptr.package = "fastd";
    uci_ptr.value = secret;
    uci_set(ctx, &uci_ptr);
}

const char *get_fastd_pubkey(struct uci_context *ctx) {
    struct uci_package *p;
    if (!uci_load(ctx, "fastd", &p)) {
        const char *pubkey = get_first_option(ctx, p, "fastd.mesh_vpn", "pubkey");
        if (!pubkey || !*pubkey)
            return NULL;

        return pubkey;
    }
    return NULL;
}

const char *
get_first_option(struct uci_context *ctx, struct uci_package *p, const char *type, const char *option) {
    struct uci_section *s = get_first_section(p, type);
    if (s)
        return uci_lookup_option_string(ctx, s, option);
    else
        return NULL;
}

struct uci_section *get_first_section(struct uci_package *p, const char *type) {
    struct uci_element *e;
    uci_foreach_element(&p->sections, e)
    {
        struct uci_section *s = uci_to_section(e);
        if (!strcmp(s->type, type))
            return s;
    }

    return NULL;
}