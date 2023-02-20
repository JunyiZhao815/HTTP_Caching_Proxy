#include "Cache.hpp"



Response Cache::getResponse(Request request){
    Node* pointer = head;
    while(pointer != NULL){
        if(pointer->request == key){
            return pointer->response;
        }
    }
    return new Response()
}

void Cache::putResponse(Request request, Response response){
    Node* pointer = head;
    while(pointer != NULL){
        if(pointer->request == request){
            pointer->response = response;
            return;
        }
    }
    // Add Node to the end of linkedlist
    Node* temp = new Node(request, response);       
    if(size == 0){
        head = temp;
        tail = temp;
    }else{
        tail->next = temp;
        tail->next->prev = tail;
        tail = tail->next;
    }
    ++size;
    // Delete the first Node if the size excceed the capacity
    if(this->size > capacity){
        Node* first = head;
        head = head->next;
        if(head == NULL){
            tail = NULL;
        }else{
            head->prev = NULL;
        }
        free(first);
        --size;
    }
}



bool Cache::isFresh(Request request, int user_id){
    Response response_old = this->getResponse(request);
    if(response_old.size() == 0){
        std::cout << "Cannot find the response you are trying to check freshing or not" << std::endl;
    }


}





