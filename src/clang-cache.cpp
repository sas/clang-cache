#include "daemon.h"

#include <err.h>
#include <iostream>
#include <string.h>
#include <unistd.h>

int main(int argc, char **argv)
{
  int separator = 0;
  char** compile_argv;

  for (int i = 1; i < argc; ++i) {
    if (strcmp(argv[i], "--") == 0) {
      separator = i;
      break;
    }
  }

  compile_argv = argv + separator;
  compile_argv[0] = separator >= 2 ? argv[separator - 1] : strdup("clang");

  if (clc::daemon::start()) {
    /* Send compile message to server. */
  }

  /* Run the compiler and print error message if execvp returns. */
  execvp(compile_argv[0], compile_argv);
  err(EXIT_FAILURE, "%s", compile_argv[0]);
}
