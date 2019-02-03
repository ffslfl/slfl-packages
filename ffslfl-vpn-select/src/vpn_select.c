#include <unistd.h>
#include <sys/types.h>
#include <grp.h>

void moveToCorrectProcess() {
  struct group *grp;

  grp = getgrnam("gluon-mesh-vpn");
  if (grp == NULL) {
    /// "Failed to get gid"
  }

  setpgid(0, grp->gr_gid);
}

void run() {

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