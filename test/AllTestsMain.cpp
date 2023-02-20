#include "HttpParserTest.h"
#include "TcpConnectorTest.h"
#include "HttpConnectorTest.h"

int main() {
  HPROXY_RUN_TEST(HttpParserTest);
  HPROXY_RUN_TEST(TcpConnectorTest);
  HPROXY_RUN_TEST(HttpConnectorTest);
}
