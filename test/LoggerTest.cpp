#include "LoggerTest.h"
#include <pthread.h>
void LoggerTest::onRun() { test_log_multi_thread(); }

void LoggerTest::test_log_multi_thread() {
  pthread_t t1, t2, t3, t4, t5;
  pthread_create(&t1, NULL, log, NULL);
  pthread_create(&t2, NULL, log, NULL);
  pthread_create(&t3, NULL, log, NULL);
  pthread_create(&t4, NULL, log, NULL);
  pthread_create(&t5, NULL, log, NULL);

  pthread_join(t1, NULL);
  pthread_join(t2, NULL);
  pthread_join(t3, NULL);
  pthread_join(t4, NULL);
  pthread_join(t5, NULL);
}

void *LoggerTest::log(void *arg) {
  Logger::getLogger().proxyLog(1, "ok");
  return NULL;
}
