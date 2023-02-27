CC=g++
CFLAGS=-std=c++11 -pthread

http_proxy:
	${CC} ${CFLAGS} -o http_proxy *.cpp

.PHONY: clean
clean:
	rm -f *~ *.out http_proxy proxy.log
