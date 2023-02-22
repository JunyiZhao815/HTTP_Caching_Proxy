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
        std::string URI;
        Response response;
        Node* prev;
        Node* next;
        Node(std::string URI, Response response):URI(URI), response(response){}
};
class Cache{
    public:
        // Here I use First In First Out(FIFO) to implement the replacement policy
        std::map<std::string, Node*>map;
        size_t capacity;
        size_t size;
        Node* head;
        Node* tail;
        Cache(size_t capacity, size_t size){
            this->capacity = capacity;
            this->size = size;
        }
        void usingCache(Request request, int user_id);
        Response getResponse(Request request);
        void putResponse(Request request, Response response);
        bool isFresh(Request request, Response response, std::ostream &out, int user_id);
        void revalidation(int client_fd, int server_fd, int user_id);
};
