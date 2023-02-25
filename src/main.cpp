#include "HttpProxy.h"

int main() {
  HttpProxy httpProxy;
  httpProxy.init("4000");
  // httpProxy.multiThread();
  httpProxy.noneMultiThread();
}
