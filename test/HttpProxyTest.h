#ifndef __HTTP_PROXY_TEST_H__
#define __HTTP_PROXY_TEST_H__
#include "../src/HttpProxy.h"
#include "../src/test/UnitTest.h"

class HttpProxyTest : public UnitTest {
private:
  void test_single_thread_proxy(std::string port = "4000");

public:
  void onRun() override;
};

#endif
