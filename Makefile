CC = g++
CFLAGS=-std=c++98 -pedantic -Wall -ggdb3
all: httpParser tcpConnector
main: HttpParser.h TcpConnector.h
	${CC} ${CFLAGS} -o main main.cpp HttpParser.cpp TcpConnector.cpp
httpParser: HttpParser.h HttpParser.cpp
	${CC} ${CFLAGS} -o httpParser HttpParser.cpp

tcpConnector: TcpConnector.h TcpConnector.cpp
	${CC} ${CFLAGS} -o tcpConnector TcpConnector.cpp

.PHONY: clean
clean:
	rm -f httpParser tcpConnector main *.o *~ *.out
