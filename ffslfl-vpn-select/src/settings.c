#include "settings.h"
#include <uci.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char *get_fastd_secret(struct uci_context *ctx) {
    struct uci_package *p;
    if (!uci_load(ctx, "fastd", &p)) {
        const char *secret = get_first_option(ctx, p, ".", "secret");
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
        const char *pubkey = get_first_option(ctx, p, ".mesh_vpn", "pubkey");
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

static struct uci_package *find_package(struct uci_context *ctx, const char *str, bool al) {
    struct uci_package *p = NULL;
    struct uci_element *e;
    char *sep;
    char *name;

    sep = strchr(str, '.');
    if (sep) {
        name = malloc(1 + sep - str);
        if (!name) {
            return NULL;
        }
        strncpy(name, str, sep - str);
        name[sep - str] = 0;
    } else
        name = (char *) str;

    uci_foreach_element(&ctx->root, e)
    {
        if (strcmp(e->name, name) != 0)
            continue;

        p = uci_to_package(e);
        goto done;
    }

    if (al)
        uci_load(ctx, name, &p);

done:
    if (name != str)
        free(name);
    return p;
}


static const char *get_first(struct uci_context *ctx, const char *conf, const char *stype) {
    const char *value;

    struct uci_package *p;
    struct uci_element *e, *tmp;
    const char *package, *type;
    int i = 0;

    package = conf;
    type = stype;

    p = find_package(ctx, package, true);
    if (!p)
        goto done;

    uci_foreach_element_safe(&p->sections, tmp, e)
    {
        struct uci_section *s = uci_to_section(e);

        i++;

        if (type && (strcmp(s->type, type) != 0))
            continue;

        if (s->e.name != NULL) {
            value = s->e.name;
            break;
        }
    }

    return value;
done:
    return NULL;
}

const char *get_latitude(struct uci_context *ctx) {
    struct uci_package *p;
    if (!uci_load(ctx, "gluon-node-info", &p)) {
        const char *secret = get_first_option(ctx, p, get_first(ctx, "gluon-node-info", "location"), "latitude");
        if (!secret || !*secret)
            return NULL;

        return secret;
    }
    return NULL;
}

const char *get_longitude(struct uci_context *ctx) {
    struct uci_package *p;
    if (!uci_load(ctx, "gluon-node-info", &p)) {
        const char *secret = get_first_option(ctx, p, get_first(ctx, "gluon-node-info", "location"), "longitude");
        if (!secret || !*secret)
            return NULL;

        return secret;
    }
    return NULL;
}