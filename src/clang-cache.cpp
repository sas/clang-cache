#include <client/client.h>
#include <server/server.h>

#include <err.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char **argv)
{
  int separator;
  char** compile_argv;

  separator = 0;
  for (int i = 1; i < argc; ++i) {
    if (strcmp(argv[i], "--") == 0) {
      separator = i;
      break;
    } else if (strcmp(argv[i], "--start") == 0) {
      clc::server::start();
      return EXIT_SUCCESS;
    } else if (strcmp(argv[i], "--stop") == 0) {
      clc::server::stop();
      return EXIT_SUCCESS;
    }
  }

  compile_argv = argv + separator;
  compile_argv[0] = separator >= 2 ? argv[separator - 1] : (char*) "clang";

  /* Start the server and send the compile command. */
  if (!clc::server::is_running())
    clc::server::start();

  /* Run the compiler and print error message if execvp returns. */
  execvp(compile_argv[0], compile_argv);
  err(EXIT_FAILURE, "%s", compile_argv[0]);
}
