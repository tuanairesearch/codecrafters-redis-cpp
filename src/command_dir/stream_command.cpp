//
// Created by tuan on 4/23/26.
//

#include "stream_command.h"
#include "command.h"
#include "list_command.h"
#include "../global.h"
#include <iostream>
#include <string>
#include <pstl/execution_defs.h>
/*
 * 1) std::unordered_map<std::string, data> client_data_string;
 * 2) std::unordered_map<std::string,std::deque<std::string>> client_data_list;
 */


std::string data_type_of(std::string var_name) {
    {
        auto x = client_data_string.find(var_name);
        //std::cout <<"Size of client_data_list: " << client_data_list.size() << std::endl;
        if (x != client_data_string.end()) {
            return "string";
        }
    }
    {
        auto x = stream_data.find(var_name);
        //std::cout <<"Size of client_data_list: " << client_data_list.size() << std::endl;
        if (x != stream_data.end()) {
            return "stream";
        }
    }
    return "none";
}

// Checking helper

bool check_valid_id_seq(std::string key_name, StreamID inp) {
    if (stream_data[key_name].empty())
        return true;
    else {
        auto x = stream_data[key_name].rbegin();
        if (x->first < inp) {
            return true;
        }
        else {
            return false;
        }
    }
}

// This check what type of input user enter
// This can handle
StreamID make_id_seq(std::string key_name, std::string key_value) {
    StreamID stream_id;
    auto it = stream_data[key_name].rbegin();
    if (key_value == "*") {
        if (stream_data[key_name].empty())
        {
            stream_id.sequence_number = 0;
        }
        else
        {
            if (it->first.stream_id == stream_id.stream_id) {
                stream_id.sequence_number = it->first.sequence_number + 1;
            }
            else if (it->first.stream_id < stream_id.stream_id) {
                stream_id.sequence_number = 0;
            }
            else {
                stream_id.stream_id = it->first.stream_id;
                stream_id.sequence_number = it->first.sequence_number + 1;
            }
        }
    }
    else {
        size_t pos = key_value.find("-");
        std::string ms_str = key_value.substr(0,pos);
        std::string seq_str = key_value.substr(pos + 1, key_value.length() - pos);
        // id - *
        if (seq_str == "*" && check_str_is_int(ms_str)) {
            stream_id.stream_id = std::stoll(ms_str);
            if (stream_data[key_name].empty()) {
                stream_id.sequence_number = 0;
            }
            else {
                if (it->first.stream_id < stream_id.stream_id) {
                    stream_id.sequence_number = 0;
                }
                else if (it->first.stream_id == stream_id.stream_id) {
                    stream_id.sequence_number = it->first.sequence_number + 1;
                }
                else {
                    // Error
                    stream_id.stream_id = -1;
                }
            }
        }
        // id - seq
        else {
            stream_id.stream_id = std::stoll(ms_str);
            stream_id.sequence_number = std::stoll(seq_str);
            if (!(stream_data[key_name].empty() || it->first < stream_id))
            {
                //error
                stream_id.stream_id = -1;
            }
        }
    }
    return stream_id;
}


// Helper

// void add_StreamID(std::string key_name, StreamID stream_id, std::vector<std::pair<std::string, std::string>> pair_data) {
//     if (stream_data[key_name].empty()) {
//         stream_data[key_name].insert({stream_id, pair_data});
//     }
//     else {
//         auto it = stream_data[key_name].rbegin();
//         if (stream_id.sequence_number == -1) {
//             if (stream_id.stream_id < (it->first).stream_id) {
//                 stream_id.sequence_number = 0;
//             }
//             else if (stream_id.stream_id == (it->first).stream_id) {
//                 stream_id.sequence_number = (it->first).sequence_number + 1;
//             }
//             else {
//                 // Handle error
//                 std::cerr << "Error 1 at add StreamID" << std::endl;
//                 return;
//             }
//             stream_data[key_name].insert({stream_id, pair_data});
//         }
//         else if ((it->first) < stream_id) {
//             stream_data[key_name].insert({stream_id, pair_data});
//         }
//         else {
//             std::cerr << "Error 2 at add StreamID" << std::endl;
//             return;
//         }
//     }
//
// }

void handle_type_cmd(std::vector<std::string> &inp_arr,int& client_fd) {
    size_t check = inp_arr.size();
    if (check == 2) {
        std::string result = data_type_of(inp_arr[1]);
        send_resp_string(("+" + result + "\r\n").c_str(),client_fd);
    }
    else {
        send_resp_string("-Syntax error. Try TYPE <var_name>\r\n",client_fd);
    }
}

void handle_xadd_cmd(std::vector<std::string> &inp_arr,int& client_fd) {
    size_t check = inp_arr.size();
    std::vector<std::pair<std::string, std::string>> arr;
    if (check >= 5 && check % 2 == 1) {
        // set up key name and key value
        std::string key_name = inp_arr[1];
        std::string key_value = inp_arr[2];
        // Build vector with data
        for (int i = 3; i < inp_arr.size(); i += 2) {
            std::pair<std::string, std::string> temp_pair;
            temp_pair.first = inp_arr[i];
            temp_pair.second = inp_arr[i+1];
            arr.push_back(temp_pair);
        }
        // Create ID-SEQ
        StreamID stream_id = make_id_seq(key_name, key_value);
        if (stream_id.stream_id == 0 && stream_id.sequence_number == 0)
        {
            send_resp_string("-ERR The ID specified in XADD must be greater than 0-0\r\n", client_fd);
        }
        else if (stream_id.stream_id != -1)
        {
            stream_data[key_name].insert({stream_id,arr});
            std::string message = std::to_string(stream_id.stream_id) + "-" + std::to_string(stream_id.sequence_number);
            send_resp_string(message,client_fd);
        }
        else
        {
            send_resp_string("-ERR The ID specified in XADD is equal or smaller than the target stream top item\r\n", client_fd);
        }
    }
}