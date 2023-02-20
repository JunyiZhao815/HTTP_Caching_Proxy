#ifndef __HTTP_PARSER_TEST_H__
#define __HTTP_PARSER_TEST_H__
#include "../src/HttpParser.h"
#include "../src/test/UnitTest.h"

class HttpParserTest : public UnitTest {
private:
  void test_parse_request();
  void test_parse_response();
  void test_equal_operator_request();

public:
  void onRun() override;
};
#endif
