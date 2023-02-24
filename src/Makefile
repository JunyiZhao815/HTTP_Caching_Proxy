CC=g++
CFLAGS=-std=c++11 -ggdb3 -pedantic -Wall


test_cache:
	${CC} ${CFLAGS} -o test_cache *.cpp -lboost_system -lpthread

.PHONY: clean
clean:
	rm -f *~ *.out
