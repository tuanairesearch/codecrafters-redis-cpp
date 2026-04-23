//
// Created by tuan on 4/23/26.
//

#include "stream_command.h"
#include "command.h"
#include "list_command.h"
#include "../global.h"
#include <iostream>
#include <string>
/*
 * 1) std::unordered_map<std::string, data> client_data_string;
 * 2) std::unordered_map<std::string,std::deque<std::string>> client_data_list;
 */


std::string data_type_of(std::string var_name) {
    {
        auto x = client_data_string.find(var_name);
        std::cout <<"Size of client_data_list: " << client_data_list.size() << std::endl;
        if (x != client_data_string.end()) {
            return "string";
        }
    }
    return "none";
}

void handle_type_cmd(std::vector<std::string> &inp_arr,int& client_fd) {
    size_t check = inp_arr.size();
    if (check == 2) {
        std::string result = data_type_of(inp_arr[1]);
        send_resp_string(result.c_str(),client_fd);
    }
    else {
        send_resp_string("-Syntax error. Try TYPE <var_name>\r\n",client_fd);
    }
}
