//
// Created by tuan on 3/27/26.
//

#include "global.h"

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

// Explain about how data store in client_data_string
/*
 *   var_name1   var_name...          <unordered_map> -> also do not know the var name
 *       |
 *      data                          <string>  -> data
 *
 *  For example, user with id 4
 *  client_data_string[4][-------]
 *  We find data of var_name1 = "hi"
 *  client_data_string[4]["var_name1"] -> return "hi"
 */

std::unordered_map<std::string, data> client_data_string;


// Explain about how data store in client_data_list
/*
 *      list1    listn...          <unordered_map>
 *       |
 *    element1, element2,....      <deque<int>> -> for add and delete purpose
 *
 *  For example, user with id 4
 *  client_data_list[4][-------]
 *  data of list ls1(int) client_data_list[4][ls1]
 *  
 */
std::unordered_map<std::string,std::deque<std::string>> client_data_list;

std::deque<client_time_data> blocked_clients;

client_time_data nearest_expired (std::deque<client_time_data> &blocked_clients) {
    client_time_data temp;
    if (blocked_clients.size() > 0) {
        temp = blocked_clients[0];
        for (auto x:blocked_clients) {
            if (x.has_expired && x.expired_time < temp.expired_time)
                temp = x;
        }
    }
    else {
        temp.client_fd = -1;
        temp.has_expired = false;
    }
    return temp;
    // if client_fd = -1, there are nothing to work
}

timeval change_time_to_timeval(client_time_data &time_need_change) {
    if (time_need_change.has_expired) {
        auto duration = time_need_change.expired_time - std::chrono::steady_clock::now();
        long long mili_sec = std::chrono::duration_cast<std::chrono::microseconds>(duration).count();
        if (mili_sec <= 0) {
            return{0,0};
        }
        else {
            timeval temp;
            temp.tv_sec = mili_sec/1'000'000;
            temp.tv_usec = mili_sec%1'000'000;
            return temp;
        }
    }
    else {
        return {0,0};
    }
}