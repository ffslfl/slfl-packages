#include <dirent.h>
#include <zconf.h>

/* test that dir exists (1 success, -1 does not exist, -2 not dir) */
int xis_dir(const char *d) {
  DIR *dirptr;

  if (access ( d, F_OK ) != -1 ) {
    // file exists
    if ((dirptr = opendir (d)) != NULL) {
      closedir (dirptr);
    } else {
      return -2; /* d exists, but not dir */
    }
  } else {
    return -1;     /* d does not exist */
  }

  return 1;
}
