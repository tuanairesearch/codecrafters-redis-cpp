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


void handle_ping_cmd(int& client_fd);
void handle_echo_cmd(std::vector<std::string> &inp_arr, int& client_fd);
void handle_get_cmd(std::vector<std::string> &inp_arr, std::unordered_map<std::string, data> &client_data, int &client_fd);
void handle_set_cmd(std::vector<std::string> &inp_arr, std::unordered_map<std::string, data> &client_data, int &client_fd);
void handle_rpush_cmd(std::vector<std::string> &inp_arr, std::unordered_map<std::string,std::deque<std::string>> &client_data_list,int& client_fd);
void handle_lpush_cmd(std::vector<std::string> &inp_arr, std::unordered_map<std::string,std::deque<std::string>> &client_data_list,int& client_fd);
void handle_llen_cmd(std::vector<std::string> &inp_arr, std::unordered_map<std::string,std::deque<std::string>> &client_data_list,int& client_fd);
void handle_lpop_cmd(std::vector<std::string> &inp_arr, std::unordered_map<std::string,std::deque<std::string>> &client_data_list,int& client_fd);
void handle_lrange_cmd(std::vector<std::string> &inp_arr, std::unordered_map<std::string,std::deque<std::string>> &client_data_list,int& client_fd);
void handle_blpop_cmd(std::vector<std::string> &inp_arr, std::unordered_map<std::string,std::deque<std::string>> &client_data_list,int& client_fd);
void handle_blocked_clients(std::vector<std::string> &inp_arr,
    std::unordered_map<std::string,std::deque<std::string>> &client_data_list,std::deque<client_time_data> &blocked_clients);

#endif //REDIS_STARTER_CPP_LIST_COMMAND_H
