#include "global.h"

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

// This is Stream data structure
// Layer 1: name_of_key ----------> unordered_map
// Layer 2: <id - sequence> ----------> map (due to sequence, use this for speed find arange)
// Layer 3: vector ----------> store pair<string,string>
// Layer 4: std::pair<std::string, std:: string>
// Demo:
// key_name
//    |
//    |___ <id - sequence 1 >
//                |___________ pair_1 <name, value>
//                |___________ pair_2 <name, value>
//                |___________ ...
//    |___ <id - sequence 2 >
//                |___________ pair_1 <name, value>
//                |___________ pair_2 <name, value>
//                |___________ ...

std::unordered_map<std::string, std::map<StreamID,std::vector<std::pair<std::string, std::string>>>> stream_data;

//*****************************************************************************************************************
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

//std::deque<client_time_data> blocked_clients;

std::deque<stream_cilent_blocked_element> stream_blocked_clients;

std::deque<blocked_client> blocked_clients;

blocked_client nearest_expired()
{
    // check in list
    blocked_client temp;
    blocked_client temp_data;
    temp_data.client_fd = -1;
    if (blocked_clients.size() > 0)
    {
        temp_data = blocked_clients[0];
        for (auto x:blocked_clients) {
            if (x.expired_time < temp_data.expired_time)
            {
                temp_data = x;
            }
        }
    }
    return  temp_data;
}

client_time_data nearest_expired_fix (std::deque<client_time_data> &blocked_clients) {
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

timeval change_time_to_timeval(blocked_client &data)
{
    auto duration = data.expired_time - std::chrono::steady_clock::now();
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