#ifndef __LOGGER_TEST_H__
#define __LOGGER_TEST_H__
#include "../src/Logger.h"
#include "../src/test/UnitTest.h"

class LoggerTest : public UnitTest {
public:
  void onRun() override;

private:
  void test_log_multi_thread();

  static void *log(void *);
};

#endif
