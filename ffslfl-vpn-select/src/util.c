#include <dirent.h>
#include <zconf.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>

/* test that dir exists (1 success, -1 does not exist, -2 not dir) */
int xis_dir(const char *d) {
  DIR *dirptr;

  if (access(d, F_OK) != -1) {
    // file exists
    if ((dirptr = opendir(d)) != NULL) {
      closedir(dirptr);
    } else {
      return -2; /* d exists, but not dir */
    }
  } else {
    return -1;     /* d does not exist */
  }

  return 1;
}

char *run_command(char *buffer, const char *command) {
  FILE *file = popen(command, "r");
  fgets(buffer, 100, file);
  pclose(file);
  return buffer;
}

char *deblank(char *input) {
  int i, j;
  char *output = input;
  for (i = 0, j = 0; i < strlen(input); i++, j++) {
    if (input[i] != ' ')
      output[j] = input[i];
    else
      j--;
  }
  output[j] = 0;
  return output;
}

int remove_directory(const char *path) {
  DIR *d = opendir(path);
  size_t path_len = strlen(path);
  int r = -1;

  if (d) {
    struct dirent *p;

    r = 0;

    while (!r && (p = readdir(d))) {
      int r2 = -1;
      char *buf;
      size_t len;

      /* Skip the names "." and ".." as we don't want to recurse on them. */
      if (!strcmp(p->d_name, ".") || !strcmp(p->d_name, "..")) {
        continue;
      }

      len = path_len + strlen(p->d_name) + 2;
      buf = malloc(len);

      if (buf) {
        struct stat statbuf;

        snprintf(buf, len, "%s/%s", path, p->d_name);

        if (!stat(buf, &statbuf)) {
          if (S_ISDIR(statbuf.st_mode)) {
            r2 = remove_directory(buf);
          } else {
            r2 = unlink(buf);
          }
        }
        free(buf);
      }
      r = r2;
    }
    closedir(d);
  }

  if (!r) {
    r = rmdir(path);
  }

  return r;
}