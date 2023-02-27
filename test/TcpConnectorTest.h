#ifndef __TCP_CONNECTOR_TEST_H__
#define __TCP_CONNECTOR_TEST_H__
#include "../src/TcpConnector.h"
#include "../src/test/UnitTest.h"

class TcpConnectorTest : public UnitTest {
public:
  void onRun() override;
};
#endif