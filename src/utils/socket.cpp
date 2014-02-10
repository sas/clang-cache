#include "socket.h"

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

namespace clc { namespace utils {

int create_socket(const char* sock_path)
{
  int fd;
  struct sockaddr_un sun;

  fd = socket(AF_UNIX, SOCK_STREAM, 0);
  if (fd == -1)
    return fd;

  /* Remove old sockets laying around. */
  unlink(sock_path);

  sun.sun_family = AF_UNIX;
  strcpy(sun.sun_path, sock_path);

  if (bind(fd, (struct sockaddr*) &sun, sizeof sun) == -1)
    goto cleanup;

  if (listen(fd, SOMAXCONN) == -1)
    goto cleanup;

  return fd;

cleanup:
  close(fd);
  return -1;
}

}} // namespace clc::utils
