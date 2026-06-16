//
// Created by tuan on 6/16/26.
//

#ifndef REDIS_STARTER_CPP_TRANSACTION_COMMAND_H
#define REDIS_STARTER_CPP_TRANSACTION_COMMAND_H
#include <vector>
#include <string>

std::string handle_incr_cmd(std::vector<std::string> &inp_arr,int& client_fd);
std::string handle_multi_cmd(std::vector<std::string> &inp_arr,int& client_fd);
std::string handle_exec_cmd(std::vector<std::string> &inp_arr,int& client_fd);
#endif //REDIS_STARTER_CPP_TRANSACTION_COMMAND_H
