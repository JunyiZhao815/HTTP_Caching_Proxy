CC=g++
CFLAGS=-std=c++11 -ggdb3 -pedantic -Wall

alltest:
	${CC} ${CFLAGS} -o alltest *.cpp ../src/*.cpp ../src/test/UnitTest.cpp -lboost_system -lpthread


.PHONY: clean
clean:
	rm -f *~ alltest *.out proxy.log *.o
