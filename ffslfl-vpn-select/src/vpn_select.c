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
#define GWPORT_REGEX "ipv(4|6) \"([0-9.]+)\""
#define FASTDCONFIG_REGEX ".conf #(.*) ###"

struct keyserver_config {
    char *gw_name;
    char *fastd_config;
    int port;
};

struct recv_keyserver_ctx {
    struct keyserver_config k;
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

void parse_line(char *line, struct keyserver_config *k) {
    k->gw_name = getGWName(line);
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

            parse_line(line, &ctx->k);
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

    size_t needed =
            snprintf(NULL, 0, "http://keyserver.ffslfl.net/index.php?mac=%s&name=%s&port=%s&key=%s&lat=%s&long=%s", mac,
                     hostname, port, pubkey, lat, longitude) + 1;
    char *url = malloc(needed);
    sprintf(url, "http://keyserver.ffslfl.net/index.php?mac=%s&name=%s&port=%s&key=%s&lat=%s&long=%s", mac, hostname,
            port, pubkey, lat, longitude);

    struct recv_keyserver_ctx keyserver_ctx = {};
    keyserver_ctx.ptr = keyserver_ctx.buf;
    struct keyserver_config *k = &keyserver_ctx.k;

    int err_code = get_url(url, recv_keyserver_cb, &keyserver_ctx, -1);
    if (err_code != 0) {
        fprintf(stderr, "vpn-select: warning: error downloading keyserver config: %s\n", uclient_get_errmsg(err_code));
    }

    // TODO get name and ip and write config file for tunneldigger

    // TODO Check if tunneldigger is enabled

    // TODO build fastd config

    // TODO decide if fastd or tunneldigger gets enabled
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