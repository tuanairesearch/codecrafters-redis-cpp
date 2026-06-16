//
// Created by tuan on 4/23/26.
//

#ifndef REDIS_STARTER_CPP_STREAM_COMMAND_H
#define REDIS_STARTER_CPP_STREAM_COMMAND_H
#include <string>
#include <vector>
#include "../global.h"
std::string data_type_of(std::string var_name);
std::string handle_type_cmd(std::vector<std::string> &inp_arr,int& client_fd);
std::string handle_xadd_cmd(std::vector<std::string> &inp_arr,int& client_fd);
std::string handle_xrange_cmd(std::vector<std::string> &inp_arr,int& client_fd);
std::string handle_xread_cmd(std::vector<std::string> &inp_arr,int& client_fd);
void handle_blocked_stream_clients(std::vector<std::string> &inp_arr);
#endif //REDIS_STARTER_CPP_STREAM_COMMAND_H
