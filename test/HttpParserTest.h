#ifndef __HTTP_PARSER_TEST_H__
#define __HTTP_PARSER_TEST_H__
#include "../src/HttpParser.h"
#include "../src/test/UnitTest.h"

class HttpParserTest : public UnitTest {
public:
  void onRun() override;
};
#endif
