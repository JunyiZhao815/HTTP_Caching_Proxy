CC=g++
CFLAGS=-std=c++11 -ggdb3 -pedantic -Wall

http_proxy:
	${CC} ${CFLAGS} -o http_proxy *.cpp -lboost_system -lpthread

.PHONY: clean
clean:
	rm -f *~ *.out http_proxy
