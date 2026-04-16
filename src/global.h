//
// Created by tuan on 3/27/26.
//

#ifndef REDIS_STARTER_CPP_GLOBAL_H
#define REDIS_STARTER_CPP_GLOBAL_H
#include <chrono>
#include <deque>
#include <unordered_map>
#include <string>
#include <vector>

struct data {
    std::string value;
    bool has_expired;
    std::chrono::steady_clock::time_point expired_time;
};

struct client_time_data {
    int client_fd;
    bool has_expired;
    double expired_duration;
    std::chrono::steady_clock::time_point expired_time;
};



extern std::deque<client_time_data> blocked_clients;

extern std::unordered_map<std::string, data> client_data_string;

extern std::unordered_map<std::string,std::deque<std::string>> client_data_list;

extern std::deque<client_time_data> client_expired_time;

extern client_time_data nearest_expired(std::deque<client_time_data> &expired_clients);
extern timeval change_time_to_timeval(client_time_data &time_need_change);
#endif //REDIS_STARTER_CPP_GLOBAL_H
