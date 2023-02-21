#include "Cache.hpp"



Response Cache::getResponse(Request request){
    Node* pointer = head;
    while(pointer != NULL){
        if(pointer->request == request){
            return pointer->response;
        }
    }
    Response res = Response();
    res.isNull = true;
    return res;
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



bool Cache::isFresh(Request request, std::ostream &out, int user_id){
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
        if(max_age != ""){
            check_max_age(max_age, response_old);
        }else if(expires != ""){
            check_expires(expires, response_old);
        }else if(last_modified != ""){
            check_last_modified(last_modified, response_old);
        }
        std::cout << "it is not fresh" << std::endl;
        return false;
    }
}



bool check_max_age(std::string max_age, Response response){
    int int_max_age = stoi(max_age);
    std::string date = response.getDate();
    size_t gmt = date.find("GMT");
    if(gmt != std::string::npos){
        date.erase(gmt-1, 4);
    }
    struct tm tm;
    strptime(date.c_str(), "%a, %d %b %Y %H:%M:%S", &tm);
    time_t prev_age = mktime(&tm) - 18000;

    time_t curr_age = time(NULL);
    // struct tm *curr = gmtime(&curr_age);
    // const char *curr_time_act = asctime(curr);
    // std::cout << "current time: " << curr_time_act << std::endl;

    time_t expire_age = prev_age + int_max_age;
    // struct tm *exp = gmtime(&expire_age);
    // const char *expire_time_act = asctime(exp);
    // std::cout << "expire time: " << expire_time_act << std::endl;
    if(curr_age <= expire_age){
        std::cout<<"it is fresh" << std::endl;
        return true;
    }else{
        std::cout << "it is not fresh" << std::endl;
        return false;
    }
}

bool check_expires(std::string expires, Response response){
    struct tm tm;
    size_t gmt = expires.find("GMT");
    if (gmt != std::string::npos) {
        expires.erase(gmt - 1, 4);
    }
    strptime(expires.c_str(), "%a, %d %b %Y %H:%M:%S", &tm);
    time_t expire_time = mktime(&tm) - 18000;
    // struct tm *exp = gmtime(&expire_time);
    // const char *expire_time_act = asctime(exp);
    time_t cur_time = time(NULL);

    if (cur_time <= expire_time) {
        std::cout << "it is fresh" << std::endl;
        return true;
    } else {
        // If it expires, then revalidate
        std::cout << "it is not fresh" << std::endl;
        return false;
    }
}

bool check_last_modified(std::string last_modified, Response response){
    std::string now_date = response.getDate();

    struct tm tm_now_date;
    size_t gmt = now_date.find("GMT");
    if (gmt != std::string::npos) {
        now_date.erase(gmt - 1, 4);
    }
    strptime(now_date.c_str(), "%a, %d %b %Y %H:%M:%S", &tm_now_date);
    time_t date_age = mktime(&tm_now_date) - 18000;

    struct tm tm_last_modified;
    gmt = last_modified.find("GMT");
    if (gmt != std::string::npos) {
        last_modified.erase(gmt - 1, 4);
    }
    strptime(last_modified.c_str(), "%a, %d %b %Y %H:%M:%S", &tm_last_modified);
    time_t last_age = mktime(&tm_last_modified) - 18000;

    time_t cur_age = time(NULL);
    time_t exp_age = cur_age + difftime(date_age, last_age) / 10;
    // struct tm *exp = gmtime(&exp_age);
    // const char *expire_time_act = asctime(exp);
    double cur_diff = difftime(cur_age, date_age);

    if (cur_diff <=  difftime(date_age, last_age) / 10) {
        // no need to revalidate
        std::cout << "it is fresh" << std::endl;
        return true;
    } else {
        // need revalidate
        std::cout << "it is not fresh" << std::endl;
        return false;
    }

}


void usingCache(std::string request){

}
void revalidation(int client_fd, int server_fd, int user_id, ){

}
