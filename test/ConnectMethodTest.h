#ifndef __CONNECT_METHOD_TEST_H__
#define __CONNECT_METHOD_TEST_H__
#include "../src/ConnectMethod.h"
#include "../src/test/UnitTest.h"

class ConnectMethodTest : public UnitTest {
public:
  void onRun() override;

private:
  void test_simple_proxy(std::string port = "4000");
};

#endif
