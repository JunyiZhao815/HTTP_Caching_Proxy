#ifndef __POST_METHOD_TEST_H__
#define __POST_METHOD_TEST_H__
#include "../src/PostMethod.h"
#include "../src/test/UnitTest.h"

class PostMethodTest : public UnitTest {
public:
  void onRun() override;

private:
  void test_simple_post(std::string port = "4000");
};

#endif
