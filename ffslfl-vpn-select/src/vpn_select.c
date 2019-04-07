#include <unistd.h>
#include <sys/types.h>
#include <grp.h>
#include <stdio.h>
#include <sys/stat.h>
#include <uci.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>

#include "util.h"
#include "settings.h"
#include "uclient.h"

#define MAX_LINE_LENGTH 512
#define STRINGIFY(str) #str
#define GWNAME_REGEX "(####)([a-zA-Z0-9.]+)"
#define GWIP_REGEX "ipv(4|6) \"([0-9.]+)\""
#define GWPORT_REGEX "port ([0-9]+)"
#define FASTDCONFIG_REGEX ".conf #(.*) ###"

struct keyserver_config {
    char *gw_name;
    char *fastd_config;
    char *gw_ip;
    int port;
};

struct recv_keyserver_ctx {
    struct keyserver_config k;
    char buf[MAX_LINE_LENGTH + 1];
    char *ptr;
};

struct gw_enabled_config {
    bool enabled;
};

struct recv_gw_enabled_ctx {
    struct gw_enabled_config k;
    char buf[MAX_LINE_LENGTH + 1];
    char *ptr;
};

void moveToCorrectProcess() {
    struct group *grp;

    grp = getgrnam("gluon-mesh-vpn");
    if (grp == NULL) {
        /// "Failed to get gid"
    }

    setpgid(0, grp->gr_gid);
}

char *getFastDConfig(char *line) {
    size_t maxMatches = 2;
    size_t maxGroups = 2;

    regex_t regexCompiled;
    regmatch_t groupArray[maxGroups];

    unsigned int m;
    char *cursor;

    if (regcomp(&regexCompiled, FASTDCONFIG_REGEX, REG_EXTENDED)) {
        printf("Could not compile regular expression.\n");
        return NULL;
    }

    m = 0;
    cursor = line;
    char *result;
    for (m = 0; m < maxMatches; m++) {
        if (regexec(&regexCompiled, cursor, maxGroups, groupArray, 0))
            break;  // No more matches

        unsigned int g = 0;
        unsigned int offset = 0;
        for (g = 0; g < maxGroups; g++) {
            if (groupArray[g].rm_so == (size_t) -1)
                break;  // No more groups

            if (g == 0)
                offset = groupArray[g].rm_eo;

            char cursorCopy[strlen(cursor) + 1];
            strcpy(cursorCopy, cursor);
            cursorCopy[groupArray[g].rm_eo] = 0;

            result = cursorCopy + groupArray[g].rm_so;
        }
        cursor += offset;
    }

    regfree(&regexCompiled);
    return result;
}

char *getGWIP(char *line) {
    size_t maxMatches = 2;
    size_t maxGroups = 2;

    regex_t regexCompiled;
    regmatch_t groupArray[maxGroups];

    unsigned int m;
    char *cursor;

    if (regcomp(&regexCompiled, GWIP_REGEX, REG_EXTENDED)) {
        printf("Could not compile regular expression.\n");
        return NULL;
    }

    m = 0;
    cursor = line;
    char *result;
    for (m = 0; m < maxMatches; m++) {
        if (regexec(&regexCompiled, cursor, maxGroups, groupArray, 0))
            break;  // No more matches

        unsigned int g = 0;
        unsigned int offset = 0;
        for (g = 0; g < maxGroups; g++) {
            if (groupArray[g].rm_so == (size_t) -1)
                break;  // No more groups

            if (g == 0)
                offset = groupArray[g].rm_eo;

            char cursorCopy[strlen(cursor) + 1];
            strcpy(cursorCopy, cursor);
            cursorCopy[groupArray[g].rm_eo] = 0;

            result = cursorCopy + groupArray[g].rm_so;
        }
        cursor += offset;
    }

    regfree(&regexCompiled);
    return result;
}

char *getPort(char *line) {
    size_t maxMatches = 2;
    size_t maxGroups = 2;

    regex_t regexCompiled;
    regmatch_t groupArray[maxGroups];

    unsigned int m;
    char *cursor;

    if (regcomp(&regexCompiled, GWPORT_REGEX, REG_EXTENDED)) {
        printf("Could not compile regular expression.\n");
        return NULL;
    }

    m = 0;
    cursor = line;
    char *result;
    for (m = 0; m < maxMatches; m++) {
        if (regexec(&regexCompiled, cursor, maxGroups, groupArray, 0))
            break;  // No more matches

        unsigned int g = 0;
        unsigned int offset = 0;
        for (g = 0; g < maxGroups; g++) {
            if (groupArray[g].rm_so == (size_t) -1)
                break;  // No more groups

            if (g == 0)
                offset = groupArray[g].rm_eo;

            char cursorCopy[strlen(cursor) + 1];
            strcpy(cursorCopy, cursor);
            cursorCopy[groupArray[g].rm_eo] = 0;

            result = cursorCopy + groupArray[g].rm_so;
        }
        cursor += offset;
    }

    regfree(&regexCompiled);
    return result;
}

char *getGWName(char *line) {
    size_t maxMatches = 2;
    size_t maxGroups = 2;

    regex_t regexCompiled;
    regmatch_t groupArray[maxGroups];

    unsigned int m;
    char *cursor;

    if (regcomp(&regexCompiled, GWNAME_REGEX, REG_EXTENDED)) {
        printf("Could not compile regular expression.\n");
        return NULL;
    }

    m = 0;
    cursor = line;
    char *result;
    for (m = 0; m < maxMatches; m++) {
        if (regexec(&regexCompiled, cursor, maxGroups, groupArray, 0))
            break;  // No more matches

        unsigned int g = 0;
        unsigned int offset = 0;
        for (g = 0; g < maxGroups; g++) {
            if (groupArray[g].rm_so == (size_t) -1)
                break;  // No more groups

            if (g == 0)
                offset = groupArray[g].rm_eo;

            char cursorCopy[strlen(cursor) + 1];
            strcpy(cursorCopy, cursor);
            cursorCopy[groupArray[g].rm_eo] = 0;

            result = cursorCopy + groupArray[g].rm_so;
        }
        cursor += offset;
    }

    regfree(&regexCompiled);
    return result;
}

void parse_gw_enabled_line(char *line, struct gw_enabled_config *k) {
    int enabledI = strtol(line, &line, 10);
    k->enabled = (bool) enabledI;
}

/** Receives data from uclient, chops it to lines and hands it to \ref parse_line */
static void recv_gw_enabled_cb(struct uclient *cl) {
    struct recv_gw_enabled_ctx *ctx = uclient_get_custom(cl);
    char *newline;
    int len;

    while (true) {
        if (ctx->ptr - ctx->buf == MAX_LINE_LENGTH) {
            fputs("autoupdater: error: encountered manifest line exceeding limit of "
                  STRINGIFY(MAX_LINE_LENGTH)
                  " characters\n", stderr);
            break;
        }
        len = uclient_read_account(cl, ctx->ptr, MAX_LINE_LENGTH - (ctx->ptr - ctx->buf));
        if (len <= 0)
            break;
        ctx->ptr[len] = '\0';

        char *line = ctx->buf;
        while (true) {
            newline = strchr(line, '\n');
            if (newline == NULL)
                break;
            *newline = '\0';

            parse_gw_enabled_line(line, &ctx->k);
            line = newline + 1;
        }

        // Move the beginning of the next line to the beginning of the
        // buffer. We cannot use strcpy here because the memory areas
        // might overlap!
        int n = strlen(line);
        memmove(ctx->buf, line, n);
        ctx->ptr = ctx->buf + n;
    }
}

void parse_keyserver_line(char *line, struct keyserver_config *k) {
    k->gw_name = getGWName(line);
    k->gw_ip = getGWIP(line);
    char *portS = getPort(line);
    k->port = strtol(portS, &portS, 10);
    k->fastd_config = getFastDConfig(line);
}

/** Receives data from uclient, chops it to lines and hands it to \ref parse_line */
static void recv_keyserver_cb(struct uclient *cl) {
    struct recv_keyserver_ctx *ctx = uclient_get_custom(cl);
    char *newline;
    int len;

    while (true) {
        if (ctx->ptr - ctx->buf == MAX_LINE_LENGTH) {
            fputs("autoupdater: error: encountered manifest line exceeding limit of "
                  STRINGIFY(MAX_LINE_LENGTH)
                  " characters\n", stderr);
            break;
        }
        len = uclient_read_account(cl, ctx->ptr, MAX_LINE_LENGTH - (ctx->ptr - ctx->buf));
        if (len <= 0)
            break;
        ctx->ptr[len] = '\0';

        char *line = ctx->buf;
        while (true) {
            newline = strchr(line, '\n');
            if (newline == NULL)
                break;
            *newline = '\0';

            parse_keyserver_line(line, &ctx->k);
            line = newline + 1;
        }

        // Move the beginning of the next line to the beginning of the
        // buffer. We cannot use strcpy here because the memory areas
        // might overlap!
        int n = strlen(line);
        memmove(ctx->buf, line, n);
        ctx->ptr = ctx->buf + n;
    }
}

char *get_content(char *filepath) {
    FILE *f = fopen(filepath, "rb");
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);  /* same as rewind(f); */

    char *string = malloc(fsize + 1);
    fread(string, 1, fsize, f);
    fclose(f);

    string[fsize] = 0;

    return string;
}

// You must free the result if result is non-NULL.
char *str_replace(char *orig, char *rep, char *with) {
    char *result; // the return string
    char *ins;    // the next insert point
    char *tmp;    // varies
    int len_rep;  // length of rep (the string to remove)
    int len_with; // length of with (the string to replace rep with)
    int len_front; // distance between rep and end of last rep
    int count;    // number of replacements

    // sanity checks and initialization
    if (!orig || !rep)
        return NULL;
    len_rep = strlen(rep);
    if (len_rep == 0)
        return NULL; // empty rep causes infinite loop during count
    if (!with)
        with = "";
    len_with = strlen(with);

    // count the number of replacements needed
    ins = orig;
    for (count = 0; (tmp = strstr(ins, rep)); ++count) {
        ins = tmp + len_rep;
    }

    tmp = result = malloc(strlen(orig) + (len_with - len_rep) * count + 1);

    if (!result)
        return NULL;

    // first time through the loop, all the variable are set correctly
    // from here on,
    //    tmp points to the end of the result string
    //    ins points to the next occurrence of rep in orig
    //    orig points to the remainder of orig after "end of rep"
    while (count--) {
        ins = strstr(orig, rep);
        len_front = ins - orig;
        tmp = strncpy(tmp, orig, len_front) + len_front;
        tmp = strcpy(tmp, with) + len_with;
        orig += len_front + len_rep; // move to next "end of rep"
    }
    strcpy(tmp, orig);
    return result;
}

void make_config(struct uci_context *ctx) {
    const char *pubkey = get_fastd_pubkey(ctx);
    if (strcmp(pubkey, "") == 0) {
        const char *secret = get_fastd_secret(ctx);
        const char *part1 = "uci set fastd.mesh_vpn.pubkey=$(echo \"secret \\\\\"";
        const char *part2 = "\\\\\";\" | fastd -c - --show-key --machine-readable) && uci commit fastd && uci get fastd.mesh_vpn.pubkey";

        char *command;
        command = malloc(
                strlen(part1) + 1 + strlen(secret) + 1 +
                strlen(part2)); /* make space for the new string (should check the return value ...) */
        strcpy(command, part1); /* copy name into the new var */
        strcat(command, secret); /* add the extension */
        strcat(command, part2); /* add the extension */

        char buffer[100];
        pubkey = run_command(buffer, command);
    }

    // Cleanup tmp dirs
    remove_directory("/tmp/fastd_mesh_vpn_peers/");
    remove_directory("/etc/fastd_mesh_vpn_peers/");
    mkdir("/tmp/fastd_mesh_vpn_peers", 0700);
    mkdir("/etc/fastd/mesh_vpn/peers", 0700);

    const char *mac = get_content("/sys/class/net/br-wan/address");
    const char *hostname = get_content("/proc/sys/kernel/hostname");
    const char *port = "";
    const char *lat = get_latitude(ctx);
    const char *longitude = get_longitude(ctx);

    size_t url_needed =
            snprintf(NULL, 0, "http://keyserver.ffslfl.net/index.php?mac=%s&name=%s&port=%s&key=%s&lat=%s&long=%s", mac,
                     hostname, port, pubkey, lat, longitude) + 1;
    char *url = malloc(url_needed);
    sprintf(url, "http://keyserver.ffslfl.net/index.php?mac=%s&name=%s&port=%s&key=%s&lat=%s&long=%s", mac, hostname,
            port, pubkey, lat, longitude);

    struct recv_keyserver_ctx keyserver_ctx = {};
    keyserver_ctx.ptr = keyserver_ctx.buf;
    struct keyserver_config *k = &keyserver_ctx.k;

    int err_code = get_url(url, recv_keyserver_cb, &keyserver_ctx, -1);
    if (err_code != 0) {
        fprintf(stderr, "vpn-select: warning: error downloading keyserver config: %s\n", uclient_get_errmsg(err_code));
    }


    // Save fdroid peer
    char *fdroid_config_newlined = str_replace(k->fastd_config, " ", "\n");
    char *fdroid_config = str_replace(fdroid_config_newlined, "float;", "float yes;");

    size_t file_needed =
            snprintf(NULL, 0, "/etc/fastd/mesh_vpn/peers/%s", k->gw_name) + 1;
    char *file = malloc(file_needed);
    sprintf(file, "/etc/fastd/mesh_vpn/peers/%s", k->gw_name);

    FILE *f = fopen(file, "w");
    if (f == NULL) {
        printf("Error opening file!\n");
        exit(1);
    }

    fprintf(f, "%s", fdroid_config);
    fclose(f);

    const char *enabledS = get_tunneldigger_enabled(ctx);
    int enabledI = strtol(enabledS, &enabledS, 10);
    bool enabled = (bool) enabledI;

    size_t l2tp_check_url_needed =
            snprintf(NULL, 0, "http://%s/vpn.txt", k->gw_ip) + 1;
    char *l2tp_check_url = malloc(l2tp_check_url_needed);
    sprintf(url, "http://%s/vpn.txt", mac, hostname,
            port, pubkey, lat, longitude);

    struct recv_gw_enabled_ctx gw_enabled_ctx = {};
    gw_enabled_ctx.ptr = gw_enabled_ctx.buf;
    struct gw_enabled_config *g = &gw_enabled_ctx.k;

    int l2tp_check_err_code = get_url(l2tp_check_url, recv_gw_enabled_cb, &gw_enabled_ctx, -1);
    if (l2tp_check_err_code != 0) {
        fprintf(stderr, "vpn-select: warning: error downloading l2tp gw check: %s\n",
                uclient_get_errmsg(l2tp_check_err_code));
    }

    int l2port = k->port + 10000;

    if (enabled && g->enabled) {
        set_fastd_status(ctx, "0");

        // TODO build tunneldigger config

        commit_tunneldigger(ctx);
    } else {
        set_fastd_status(ctx, "1");
    }
}

void run() {
    struct uci_context *ctx = uci_alloc_context();
    ctx->flags &= ~UCI_FLAG_STRICT;

    /**
     * Check if /tmp/fastd_mesh_vpn_peers already exists
     */
    if (xis_dir("/tmp/fastd_mesh_vpn_peers")) {
        char sumold_buffer[100];
        char sumnew_buffer[100];
        char fastd_running_status_buffer[100];
        char *sumold = run_command(sumold_buffer, "sha256sum /etc/config/tunneldigger");
        make_config(ctx);
        char *sumnew = run_command(sumnew_buffer, "sha256sum /etc/config/tunneldigger");
        if (strcmp(sumold, sumnew) != 0) {
            execl("/etc/init.d/tunneldigger", "restart", NULL);
        }
        execl("/etc/init.d/fastd", "reload", NULL);


        char *fastd_running_status_raw = run_command(fastd_running_status_buffer, "netstat -tulpn | grep fastd");
        char *fastd_running_status = deblank(fastd_running_status_raw);
        if (xis_dir("/tmp/fastd_mesh_vpn_peers")) {
            if (fastd_running_status == NULL) {
                execl("/etc/init.d/fastd", "start", NULL);
            }
        } else {
            if (fastd_running_status != NULL) {
                execl("/etc/init.d/fastd", "stop", NULL);
            }
        }
    } else {
        mkdir("/tmp/fastd_mesh_vpn_peers", 0700);
        const char *secret = get_fastd_secret(ctx);
        if (!secret || !*secret) {
            char buffer[100];
            char *secret_raw = run_command(buffer, "fastd --generate-key 2>&1 |  awk '/[Ss]ecret/ { print $2 }'");
            char *secret = deblank(secret_raw);
            set_fastd_secret(ctx, secret);
            struct uci_package *p;
            uci_commit(ctx, &p, false);
        }
        make_config(ctx);
        execl("/etc/init.d/fastd", "start", NULL);
        execl("/etc/init.d/tunneldigger", "start", NULL);
    }

    uci_free_context(ctx);
}

int main(int argc, char *argv[]) {
    int f = fork();
    if (f < 0)
        perror("fork error:");
    else if (f == 0) /* child */
    {
        moveToCorrectProcess();

        // RUN Script
        run();
    }

}