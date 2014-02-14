#include <client/client.h>
#include <server/server.h>
#include <utils/logger.h>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

struct options {
  char* compiler;
  bool foreground;
  enum {
    ACTION_NONE,
    ACTION_START,
    ACTION_KILL,
  } action;
} options;

static void usage_die(const char* argv0)
{
  fprintf(stderr, "usage: %s [ -c COMPILER | -s | -k | -f ] -- COMPILER_OPTIONS\n", argv0);
  exit(EXIT_FAILURE);
}

static void parse_options(int argc, char** argv)
{
  int opt;

  options.compiler = strdup("clang");
  options.foreground = false;
  options.action = options::ACTION_NONE;

  while ((opt = getopt(argc, argv, "c:skf")) != -1) {
    switch (opt) {
      case 'c':
        free(options.compiler);
        options.compiler = strdup(optarg);
        break;
      case 's':
        options.action = options::ACTION_START;
        break;
      case 'k':
        options.action = options::ACTION_KILL;
        break;
      case 'f':
        options.foreground = true;
        break;
      default:
        usage_die(argv[0]);
    }
  }
}

int main(int argc, char **argv)
{
  parse_options(argc, argv);

  switch (options.action) {
    case options::ACTION_START:
      clc::server::start(options.foreground);
      return EXIT_SUCCESS;
    case options::ACTION_KILL:
      clc::server::stop();
      return EXIT_SUCCESS;
    case options::ACTION_NONE:
      /* Start the server and send the compile command. */
      if (!clc::server::is_running())
        clc::server::start();

      /* We need a free slot to stash the compiler name. */
      assert(optind >= 1);
      argc -= optind - 1;
      argv += optind - 1;
      argv[0] = options.compiler;

      /* Run the compiler and print error message if execvp returns. */
      PLOG_FATAL(execvp(argv[0], argv) == -1) << argv[0];
  }
}
