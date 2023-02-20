#ifndef __HTTP_CONNECTOR_TEST_H__
#define __HTTP_CONNECTOR_TEST_H__
#include "../src/test/UnitTest.h"
#include "../src/HttpConnector.h"
#include "../src/HttpParser.h"

class HttpConnectorTest: public UnitTest{
public:
  void onRun() override;

private:
  /*
   *  Test client-proxy-server connection without any operation
   * @port proxy listening port, default:4000
   */
  void test_simple_proxy(std::string port="4000");
};

#endif