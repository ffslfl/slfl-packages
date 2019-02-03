#include <unistd.h>
#include <sys/types.h>
#include <grp.h>
#include <stdio.h>
#include <sys/stat.h>

#include "util.h"

void moveToCorrectProcess() {
  struct group *grp;

  grp = getgrnam("gluon-mesh-vpn");
  if (grp == NULL) {
    /// "Failed to get gid"
  }

  setpgid(0, grp->gr_gid);
}

void make_config() {

}

void run() {
  struct uci_context *ctx = uci_alloc_context();
  ctx->flags &= ~UCI_FLAG_STRICT;

  /**
   * Check if /tmp/fastd_mesh_vpn_peers already exists
   */
  if (xis_dir("/tmp/fastd_mesh_vpn_peers")) {

  } else {
    mkdir("/tmp/fastd_mesh_vpn_peers", 0700);
    const char *secret = get_fastd_secret(ctx);
    if (!secret || !*secret) {

    }
    make_config();
    execl("/etc/init.d/fastd","start", NULL);
    execl("/etc/init.d/tunneldigger","start", NULL);
  }

  uci_free_context(ctx);
}

int main(int argc, char *argv[]) {
  int f = fork();
  if(f < 0)
    perror("fork error:");
  else if(f == 0) /* child */
  {
    moveToCorrectProcess();

    // RUN Script
    run();
  }

}