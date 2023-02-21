#include "HttpProxyTest.h"

void HttpProxyTest::onRun() { test_single_thread_proxy(); }

void HttpProxyTest::test_single_thread_proxy(std::string port) {
  HttpProxy httpProxy;
  httpProxy.init(port);
  httpProxy.noneMultiThread();
}