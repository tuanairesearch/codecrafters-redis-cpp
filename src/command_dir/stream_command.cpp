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
std::pair<std::string, std::string> check_type_of_xadd_input(std::string s) {
    if (s == "*")
        return {"*","*"};
    else {
        size_t pos = s.find("-");
        std::string ms_str = s.substr(0,pos);
        std::string seq_str = s.substr(pos, s.length() - pos);
        if (check_str_is_int(ms_str)) {
            if (seq_str == "*") {
                return {ms_str,seq_str};
            }
            else {
                if (check_str_is_int(seq_str)) {
                    return {ms_str,seq_str};
                }
                else {
                    // hanlde invalid seqence (seq)
                    return {"invalid","invalid"};
                }
            }
        }
        else {
            // handle invalid id
            return {"invalid","invalid"};
        }
    }
}


// Helper

void add_StreamID(std::string key_name, StreamID stream_id, std::vector<std::pair<std::string, std::string>> pair_data) {
    if (stream_data[key_name].empty()) {
        stream_data[key_name].insert({stream_id, pair_data});
    }
    else {
        auto it = stream_data[key_name].rbegin();
        if (stream_id.sequence_number == -1) {
            if (stream_id.stream_id < (it->first).stream_id) {
                stream_id.sequence_number = 0;
            }
            else if (stream_id.stream_id == (it->first).stream_id) {
                stream_id.sequence_number = (it->first).sequence_number + 1;
            }
            else {
                // Handle error
                std::cerr << "Error 1 at add StreamID" << std::endl;
                return;
            }
            stream_data[key_name].insert({stream_id, pair_data});
        }
        else if ((it->first) < stream_id) {
            stream_data[key_name].insert({stream_id, pair_data});
        }
        else {
            std::cerr << "Error 2 at add StreamID" << std::endl;
            return;
        }
    }

}

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
        std::string key_name = inp_arr[1];
        auto input_type = check_type_of_xadd_input(inp_arr[2]);
        // Build vector with data
        for (int i = 3; i < inp_arr.size(); i += 2) {
            std::pair<std::string, std::string> temp_pair;
            temp_pair.first = inp_arr[i];
            temp_pair.second = inp_arr[i+1];
            arr.push_back(temp_pair);
        }
        if (input_type.first != "invalid") {
            StreamID stream_id;
            if (input_type.first == "*") {

            }
            else if (input_type.second == "*") {
                stream_id.stream_id = std::stoll(input_type.first);
            }
            else {
                stream_id.stream_id = std::stoll(input_type.first);
                stream_id.sequence_number = std::stoll(input_type.second);
            }
            if (check_valid_id_seq(key_name,stream_id)) {
                add_StreamID(key_name,stream_id,arr);
                send_resp_string(inp_arr[2],client_fd);
            }
            else {
                send_resp_string("-ERR The ID specified in XADD is equal or smaller than the target stream top item\r\n", client_fd);
            }
        }
        else {
            // Handle error
            send_resp_string("-ERR The ID specified in XADD is equal or smaller than the target stream top item\r\n", client_fd);
        }
    }
}