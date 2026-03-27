//
// Created by tuan on 3/26/26.
//

#include "command.h"
#include "resp_utls.h"
#include "global.h"
#include <string>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <cstring>
#include <sys/socket.h>
#include <unordered_map>


// ----------- Utils for debug ---------------

std::string toLowerStr(std::string s)
{
    //std::string result = "";
    for(int i = 0; i < s.length(); i++)
    {
        if(s[i] >= 'A' && s[i] <= 'Z')
            s[i] = s[i] + ('a' - 'A');
    }
    return s;
}
// -------- Utils for format data -------------



// ----------- Command -----------------------

void handle_ping_cmd(int& client_fd) {
    char result[] = "+PONG\r\n";
    send(client_fd, result, strlen(result),0);
}

void handle_echo_cmd(std::vector<std::string> &inp_arr, int& client_fd) {
    std::cout << "-----------------------" << std::endl;
    std::cout << inp_arr.size() << std::endl;
    std::cout << "-----------------------" << std::endl;
    if (inp_arr.size() == 2) {
        std::string s = handleOutput(inp_arr[1]);
        char result[s.length() + 1];
        strcpy(result, s.c_str());
        std::cout << "result = " << s << std::endl;
        send(client_fd, result, strlen(result),0);
    }
    else {
        char result[] = "-Missing filed. Try \"echo <text>\"\r\n";
        send(client_fd, result, strlen(result),0);
    }
}

void handle_get_cmd(std::vector<std::string> &inp_arr, std::vector<std::unordered_map<std::string, std::string>> &variable, int &client_fd) {
    size_t check = inp_arr.size();
    if (check == 2) {
        std::cout << "\"" << inp_arr[1] << "\"" << std::endl;
        if (variable[client_fd].contains(inp_arr[1])) {
            std::string s = handleOutput(variable[client_fd][inp_arr[1]]);
            char result[s.length() + 1];
            strcpy(result,s.c_str());
            send(client_fd,result,strlen(result),0);
        }
        else {
            char respond[] = "$-1\r\n";
            send(client_fd,respond,strlen(respond),0);
        }
    }
    else {
        char respond[] = "-Syntax Error. Try \"SET <var_name> <value> <option_field>\"\r\n";
        send(client_fd, respond,strlen(respond),0);
    }
}

void handle_set_cmd(std::vector<std::string> &inp_arr, std::vector<std::unordered_map<std::string, std::string>> &variable, int &client_fd) {
    size_t check = inp_arr.size();
    if (check == 3) {
        variable[client_fd].insert({inp_arr[1],inp_arr[2]});
        char respond[] = "+OK\r\n";
        send(client_fd, respond,strlen(respond),0);
    }
    else {
        char respond[] = "-Syntax Error. Try \"SET <var_name> <value> <option_field>\"\r\n";
        send(client_fd, respond,strlen(respond),0);
    }
}

void handle_unknown_cmd(int& client_fd) {
    char result[] = "-Unknown command\r\n";
    send(client_fd, result, strlen(result),0);
}


void handleInput(const std::string &s, int& client_fd)
{
    int str_pos = 0;
    std::vector<std::string> inp_arr; // Store user's input
    if(s[0] == '*')
    {
        //std::cout << "Check string arr" << std::endl;
        inp_arr = handleArray(s, str_pos);
        std::string key_word = toLowerStr(inp_arr[0]);
        if(key_word == "echo")
        {
            std::cout << "Handle echo command" << std::endl;
            handle_echo_cmd(inp_arr, client_fd);
        }
        else if(key_word == "ping")
        {
            std::cout << "Handle ping command" << std::endl;
            handle_ping_cmd(client_fd);
        }
        else if (key_word == "get") {
            std::cout << "Handle get command" << std::endl;
            handle_get_cmd(inp_arr,variable,client_fd);
        }
        else if (key_word == "set") {
            std::cout << "Handle set command" << std::endl;
            handle_set_cmd(inp_arr,variable,client_fd);
        }
        else {
            std::cout << "Handle unkown command" << std::endl;
            handle_unknown_cmd(client_fd);
        }
        std::cout << "Checked" << std::endl;
    }
}

std::string handleOutput(std::string &s)
{
    std::string outp =  "$" + std::to_string(s.length()) + "\r\n" + s + "\r\n";
    return outp;
}
