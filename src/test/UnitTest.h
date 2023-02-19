#ifndef __UNIT_TEST_H__
#define __UNIT_TEST_H__

class UnitTest {
public:
  void run(int times);
  virtual void onRun() = 0;

public:
  template <class T> static void runTest(int times) {
    T test;
    test.run(times);
  }

#define HPROXY_RUN_TEST(TEST) UnitTest::runTest<TEST>(1);
};

#endif
