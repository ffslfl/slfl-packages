#include <dirent.h>
#include <zconf.h>
#include <stdio.h>
#include <string.h>

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