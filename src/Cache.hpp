#include <cstdlib>
#include <iostream>
#include <map>
#include "Request.h"
#include "Response.h"
#include "TcpConnector.h"
#include <vector>
#include<iostream>

class Node{
    public:
        Request request;
        Response response;
        Node* prev;
        Node* next;
        Node(Request request, Response response):request(request), response(response){}
};
class Cache{
    private:
        // Here I use First In First Out(FIFO) to implement the replacement policy
        size_t capacity;
        size_t size;
        Node* head;
        Node* tail;
        Cache(size_t capacity, size_t size){
            this->capacity = capacity;
            this->size = size;
        }

        Response Cache::getResponse(Request request);
        void putResponse(Request request, Response response);
        bool isFresh(Request request, std::ostream &out, int user_id);
        void revalidation(int client_fd, int server_fd, int user_id, std::string request);
};
