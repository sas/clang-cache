#include "waitdir.h"

#include <utils/cwd.h>

#include <pthread.h>
#include <string.h>
#include <unistd.h>

namespace clc { namespace utils {

/*
 * This is a hack to work around the fact that libclang does not support
 * specifying a base directory for all file accesses (which means we need to
 * chdir() to the right directory before parsing a file); and the fact that the
 * current working directory is a per-process attribute (as opposed to
 * per-thread).
 *
 * We assume that most translation units in a given project are going to be
 * compiled from the same working directory, or rather from a small number of
 * different working directories.
 *
 * This function then changes the process' working directory to the desired
 * target, and every thread that wants the same directory is just going to use
 * it without any wait.
 */
bool waitdir(const char* path)
{
  static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
  static pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
  static unsigned int count = 0;

  /*
   * This serializes accesses to the current working directory, and the `count`
   * variable.
   */
  pthread_mutex_lock(&mut);

  /* A null `path` means we are releasing the current working directory. */
  if (path != nullptr) {
    while (true) {
      /*
       * We have the lock so we can check the process state. If the current working
       * directory is the right one, increment the counter and return;
       */
      if (strcmp(path, cwd()) == 0) {
        count++;
        break;
      }

      if (count != 0) {
        /* Now we can wait until something happens to the variable. */
        pthread_cond_wait(&cond, &mut);
      }

      /*
       * Now that we are done waiting on the cond variable, we either
       * (1) have an opportunity to change the working directory
       * (2) or we've been just woken up by a broadcast because the working
       *     directory just changed; restart the check-loop.
       */
      if (count == 0) {
        if (chdir(path) == -1) {
          pthread_cond_signal(&cond);
          pthread_mutex_unlock(&mut);
          return false;
        }
        count = 1;
        pthread_cond_broadcast(&cond);
        break;
      }
    }
  } else {
    count--;
    if (count == 0) {
      pthread_cond_signal(&cond);
    }
  }

  pthread_mutex_unlock(&mut);

  return true;
}

}} // namespace clc::utils
