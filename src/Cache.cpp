#include "Cache.hpp"
#include <fstream>
#include <iostream>
#include "HttpConnector.h"
#include <algorithm>
std::ofstream out("/var/log/erss/proxy.log");

Response Cache::getResponse(Request request){
    std::string URI = request.getURI();
    return map[URI]->response;
}

void Cache::putResponse(Request request, Response response){
    Node* pointer = head;
    std::string URI = request.getURI();
    while(pointer != NULL){
        if(pointer->URI == URI){
            pointer->response = response;
            return;
        }
    }
    // Add Node to the end of linkedlist
    Node* temp = new Node(URI, response);       
    if(size == 0){
        head = temp;
        tail = temp;
    }else{
        tail->next = temp;
        tail->next->prev = tail;
        tail = tail->next;
    }
    map[URI] = temp;
    ++size; 
    // Delete the first Node if the size excceed the capacity
    if(this->size > capacity){
        Node* first = head;
        map[first->URI] = NULL;
        head = head->next;
        if(head == NULL){
            tail = NULL;
        }else{
            head->prev = NULL;
        }
        --size;
    }
}


int get_freshness_lifetime(std::string max_age, std::string expires, std::string last_modified, Request request, Response response){
    if(max_age != ""){
        return convert_string2timet(max_age);
    }else if(expires != ""){
        time_t expire_time = convert_string2timet(expires);
        time_t response_time = convert_string2timet(response.getDate());
        return expire_time - response_time;
    }else if(last_modified != ""){
        time_t last_modified_time = convert_string2timet(last_modified);
        time_t date = convert_string2timet(response.getDate());
        time_t heuristic_lifetime = difftime(date last_modified_time) / 10;
        time_t current_age = get_current_age(request, response);
        if(current_age > 24 * 60 * 60 && response.getWarning() == ""){
            std::string new_header =  response.getHeader() + "\r\n" + "warn-code: 133" + "\r\n\r\n";

        }
        return heuristic_lifetime;
    }
}
time_t convert_string2timet(std::string time){
    size_t gmt = time.find("GMT");
    if(gmt != std::string::npos){
        time.erase(gmt-1, 4);
    }
    struct tm tm;
    strptime(time.c_str(), "%a, %d %b %Y %H:%M:%S", &tm);
    return mktime(&tm);
}
time_t get_current_age(Request request, Response response){
    time_t age_value = atoi(response.getAge());
    time_t date_value = convert_string2timet(response.getDate());
    time_t now = time(NULL);
    time_t request_time = convert_string2timet(request.getDate());
    time_t response_time = convert_string2timet(response.getDate());
    time_t apparent_age = response_time - date_value < 0? 0: response_time - date_value;
    time_t response_delay = response_time - request_time;
    time_t corrected_age_value = age_value + response_delay;
    time_t corrected_initial_age = apparent_age > corrected_age_value? apparent_age: corrected_age_value;
    time_t resident_time = now - response_time;
    time_t current_age = corrected_initial_age + resident_time;
    return current_age;
}
bool Cache::isFresh(Request request, Response response, std::ostream &out, int user_id){
    Response response_old = this->getResponse(request);
    if(response_old.isNull == true){
        std::cout << "Cannot find the response you are trying to check freshing or not" << std::endl;
    }
    std::string cache_control = response_old.getCacheControl();
    if(cache_control == ""){
        return false;
    }else{
        std::string max_age = response_old.getMaxAge();
        std::string expires = response_old.getExpires();
        std::string last_modified = response_old.getLastModified();
        int freshness_lifetime = get_freshness_lifetime(max_age, expires, last_modified, request, response);
        int current_age = get_current_age(request, response);
        if(freshness_lifetime > current_age){
            std::cout<<"it is fresh" << std::endl;
            return true;
        }else{
            std::cout << "it is not fresh" << std::endl;
            return false;
        }
    }
}



void Cache::revalidation(int client_fd, int server_fd, int user_id, Response response, Request request){
    std::string etag=response.getEtag();
    std::string lastModified=response.getLastModified();
    //1. Sending a Validation Request
    std::string header_request = request.getHeader();
    TcpConnector tcp = TcpConnector();
    if(etag != ""){
        std::string new_header =  header_request + "\r\n" + "If-None-Match: " + etag + "\r\n\r\n";
        send(client_fd, new_header.data(),new_header.size()+1,0);
        //receive new response  
        std::vector<char> vector(65536, 0);
        int a = recv(client_fd, &vector.data()[0],vector.size(),0);
        if(a == -1){
            std::cerr << "Error: receive error" << std::endl;
            exit(EXIT_FAILURE);
        }
        std::string isChunk = response.getTransferEncoding();
        if(isChunk == "chunked"){

        }else{

        }



        

    }else if(lastModified != ""){
        std::string new_header =  header_request + "\r\n" + "If-Modified-Since: " + lastModified + "\r\n\r\n";
        tcp.sendMessage(client_fd, &new_header, sizeof(new_header));
    }else{
        
    }
}
void check_etag_validation(std::string etag, int server_fd, int user_id, Request request, Response reponse){
    out << user_id << ": NOTE ETag: " << etag << std::endl;
    std::string header_request = request.getHeader();
    std::string new_header =  header_request + "\r\n" + "If-None-Match: " + etag + "\r\n\r\n";
    TcpConnector tcp = TcpConnector();
    tcp.sendMessage(server_fd, &new_header, sizeof(new_header));
    //std::vector<char>new_response(65536, 0);
    std::string new_response;
    recv(server_fd, &new_response, sizeof(new_response), MSG_WAITALL);
    HttpParser httpParser;
    size_t len;
    Response* r = httpParser.parseResponse(const_cast<char*>(new_response.c_str()), len);
    if(r->getStatusCode() == "304"){
        // respond from cache

    }else if(r->getStatusCode() == "200"){

    }

}
void check_lastModified_validation(std::string etag, int server_fd, int user_id, Request request, Response reponse){

}
