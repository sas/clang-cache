#include <client/client.h>
#include <server/server.h>
#include <utils/logger.h>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

struct options {
  enum {
    ACTION_NONE,
    ACTION_START,
    ACTION_KILL,
    ACTION_FIND_DEFINITION,
  } action;
  char* compiler;
  bool foreground;
  char* usr;
} options;

static void usage_die(const char* argv0)
{
  const char* usages[] = {
    "[ -c COMPILER ] -- COMPILER_OPTIONS",
    "-s [ -f ]",
    "-k",
    "-d DEFINITION"
  };

  for (unsigned int i = 0; i < sizeof usages / sizeof *usages; ++i)
    fprintf(stderr, "%s %s %s\n", (i == 0 ? "usage:" : "      "), argv0, usages[i]);

  exit(EXIT_FAILURE);
}

static void parse_options(int argc, char** argv)
{
  int opt;

  options.compiler = strdup("clang");
  options.foreground = false;
  options.action = options::ACTION_NONE;

  while ((opt = getopt(argc, argv, "c:sfkd:")) != -1) {
    switch (opt) {
      case 'c':
        free(options.compiler);
        options.compiler = strdup(optarg);
        break;
      case 's':
        options.action = options::ACTION_START;
        break;
      case 'f':
        options.foreground = true;
        break;
      case 'k':
        options.action = options::ACTION_KILL;
        break;
      case 'd':
        options.action = options::ACTION_FIND_DEFINITION;
        options.usr = strdup(optarg);
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
    case options::ACTION_NONE:
      /* Start the server and send the compile command. */
      if (!clc::server::is_running())
        clc::server::start();

      /* We need a free slot to stash the compiler name. */
      assert(optind >= 1);
      argc -= optind - 1;
      argv += optind - 1;
      argv[0] = options.compiler;

      clc::client::register_compilation(argc, argv);

      /* Run the compiler and print error message if execvp returns. */
      PLOG_FATAL(execvp(argv[0], argv) == -1) << argv[0];

      break;
    case options::ACTION_START:
      clc::server::start(options.foreground);
      break;
    case options::ACTION_KILL:
      clc::server::stop();
      break;

    case options::ACTION_FIND_DEFINITION:
      clc::client::find_definition(options.usr);
      break;
  }

  return EXIT_SUCCESS;
}
