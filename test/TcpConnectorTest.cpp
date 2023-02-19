#include "TcpConnectorTest.h"

void TcpConnectorTest::onRun() {
    {
        TcpConnector tcp;
        const int server_fd = tcp.initializeServerSocket("4000");
        const int client_fd = tcp.waitAcceptConnect(server_fd);
        size_t len;
        char* msg = (char*) tcp.receiveMessage(client_fd, len);
        std::cout<<msg<<std::endl;
    }
}
