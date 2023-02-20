#include "HttpConnectorTest.h"
#include "HttpParserTest.h"
#include "TcpConnectorTest.h"

int main() {
  HPROXY_RUN_TEST(HttpParserTest);
  HPROXY_RUN_TEST(TcpConnectorTest);
  // HPROXY_RUN_TEST(HttpConnectorTest);
}
