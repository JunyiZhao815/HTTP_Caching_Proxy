#include "UnitTest.h"

void UnitTest::run(int times) {
  for (int m = 0; m < times; m++) {
    onRun();
  }
}
