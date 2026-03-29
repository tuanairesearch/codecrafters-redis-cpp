//
// Created by tuan on 3/27/26.
//

#ifndef REDIS_STARTER_CPP_GLOBAL_H
#define REDIS_STARTER_CPP_GLOBAL_H
#include <chrono>
#include <unordered_map>
#include <string>
#include <vector>

struct data {
    std::string value;
    bool has_expired;
    std::chrono::steady_clock::time_point expired_time;
};

extern std::unordered_map<int,std::unordered_map<std::string, data>> client_data;


#endif //REDIS_STARTER_CPP_GLOBAL_H
