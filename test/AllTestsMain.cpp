#include "ConnectMethodTest.h"
#include "GetMethodTest.h"
#include "HttpConnectorTest.h"
#include "HttpParserTest.h"
#include "PostMethodTest.h"
#include "TcpConnectorTest.h"

int main() {
  HPROXY_RUN_TEST(HttpParserTest);
  HPROXY_RUN_TEST(TcpConnectorTest);
  // HPROXY_RUN_TEST(HttpConnectorTest);
  // HPROXY_RUN_TEST(ConnectMethodTest);
  // HPROXY_RUN_TEST(GetMethodTest);
  // HPROXY_RUN_TEST(PostMethodTest);
}
