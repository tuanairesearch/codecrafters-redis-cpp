//
// Created by tuan on 3/26/26.
//

#ifndef CODECRAFTERS_REDIS_CPP_RESP_UTLS_H
#define CODECRAFTERS_REDIS_CPP_RESP_UTLS_H
#include <iostream>
#include <string>
#include <vector>
void showRAW(std::string s);
std::string handleBulkString(const std::string s, int& str_pos);
std::vector<std::string> handleArray(const std::string s, int& str_pos);
#endif //CODECRAFTERS_REDIS_CPP_RESP_UTLS_H