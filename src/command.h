//
// Created by tuan on 3/26/26.
//

#ifndef CODECRAFTERS_REDIS_CPP_COMMAND_H
#define CODECRAFTERS_REDIS_CPP_COMMAND_H
#include <string>
#include <iostream>
#include <vector>
#include <deque>

std::string handleOutput(std::string &s);
std::string toLowerStr(std::string s);
void handleInput(const std::string &s, int& client_fd);
std::string toLowerStr(std::string s);
extern void send_resp_int(long long num, int& client_fd);
extern void send_respstring(const char *message, int& client_fd);
void send_resp_string(std::string& s, int& client_fd);
void send_resp_string(const char *message, int& client_fd);
void send_resp_list(std::deque<std::string>& my_list, int start_p, int end_p, int& client_fd);
std::string handleOutput(std::string &s);
int translate_posion(const int& position, const int& number_of_element);
bool check_str_is_int(std::string s);
bool check_str_is_double(std::string s);
bool check_valid_varname(std::string& name);
void handle_unknown_cmd(int& client_fd);
#endif //CODECRAFTERS_REDIS_CPP_COMMAND_H