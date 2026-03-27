//
// Created by tuan on 3/26/26.
//

#ifndef CODECRAFTERS_REDIS_CPP_COMMAND_H
#define CODECRAFTERS_REDIS_CPP_COMMAND_H
#include <string>
#include <iostream>
#include <vector>

std::string handleOutput(std::string &s);
std::string toLowerStr(std::string s);
void handleInput(const std::string &s, int& client_fd);
std::string toLowerStr(std::string s);

#endif //CODECRAFTERS_REDIS_CPP_COMMAND_H