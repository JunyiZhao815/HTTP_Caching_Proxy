CC=g++
CFLAGS=-std=c++11 -ggdb3 -pedantic -Wall

test:
	${CC} ${CFLAGS} -o alltest *.cpp ../src/*.cpp ../src/test/UnitTest.cpp 

.PHONY: clean
clean:
	rm -f *~ alltest *.out proxy.log
