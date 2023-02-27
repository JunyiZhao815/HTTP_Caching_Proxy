#ifndef __GET_METHOD_TEST_H__
#define __GET_METHOD_TEST_H__
#include "../src/GetMethod.h"
#include "../src/test/UnitTest.h"
#include "../src/Cache.hpp"

class GetMethodTest : public UnitTest {
public:
  void onRun() override;

private:
  void test_simple_get(std::string port = "4000");
  void test_get_by_while(std::string port = "4000");
};

#endif
