//
// Created by tuan on 4/23/26.
//

#ifndef REDIS_STARTER_CPP_LIST_COMMAND_H
#define REDIS_STARTER_CPP_LIST_COMMAND_H
#include "./command.h"
#include "../global.h"
#include <string>
#include <iostream>
#include <vector>


std::string handle_ping_cmd(int& client_fd);
std::string handle_echo_cmd(std::vector<std::string> &inp_arr, int& client_fd);
std::string handle_get_cmd(std::vector<std::string> &inp_arr,  int &client_fd);
std::string handle_set_cmd(std::vector<std::string> &inp_arr,  int &client_fd);
std::string handle_rpush_cmd(std::vector<std::string> &inp_arr, int& client_fd);
std::string handle_lpush_cmd(std::vector<std::string> &inp_arr, int& client_fd);
std::string handle_llen_cmd(std::vector<std::string> &inp_arr, int& client_fd);
std::string handle_lpop_cmd(std::vector<std::string> &inp_arr, int& client_fd);
std::string handle_lrange_cmd(std::vector<std::string> &inp_arr, int& client_fd);
std::string handle_blpop_cmd(std::vector<std::string> &inp_arr, int& client_fd);
void handle_blocked_list_clients(std::vector<std::string> &inp_arr,int left_or_right, int &client_fd);
//void handle_blocked_list_clients(std::vector<std::string> &inp_arr,
    //std::unordered_map<std::string,std::deque<std::string>> &client_data_list,std::deque<client_time_data> &blocked_clients);

#endif //REDIS_STARTER_CPP_LIST_COMMAND_H
