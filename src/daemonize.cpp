
#include "HttpProxy.h"

void daemonize() {
  // fork(), parent exits
  pid_t pid = fork();
  assert(pid >= 0);
  if (pid > 0) {
    exit(EXIT_SUCCESS);
  }

  // dissociate from controlling tty
  assert(setsid() >= 0);

  // fork again, not be session leader, prevent opening tty again
  pid = fork();
  assert(pid >= 0);

  if (pid > 0) {
    exit(EXIT_SUCCESS);
  }

  // clear umask
  umask(0);

  chdir("/");

  // close stdin/stderr/stdout
  fclose(stdin);
  fclose(stderr);
  fclose(stdout);
}

int main() {
  daemonize();

  Cache cache(5, 0);
  HttpProxy httpProxy(cache);
  httpProxy.init("12345");
  httpProxy.multiThread();
}
