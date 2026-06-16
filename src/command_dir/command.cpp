//
// Created by tuan on 3/26/26.
//

#include "command.h"
#include "../resp_utls.h"
#include "../global.h"
#include "stream_command.h"
#include "transaction_command.h"
#include <string>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <cstring>
#include <sys/socket.h>
#include "../command_dir/list_command.h"

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

// -------- Helper -------------
void send_resp_int(long long num, int& client_fd) {
    std::string s = ":" + std::to_string(num) + "\r\n";
    send(client_fd, s.c_str(), s.size(),0);
}

void send_resp_int(const char *message, int& client_fd) {
    send(client_fd, message, strlen(message),0);
    //return message;
}

void send_resp_string(const char *message, int& client_fd) {
    send(client_fd, message, strlen(message),0);
}

void send_resp_string(std::string& s, int& client_fd) {
    std::string respond ="$" + std::to_string(s.length()) + "\r\n" + s + "\r\n";
    send(client_fd,respond.c_str(),respond.size(),0);
    //return s;
}

std::string str_to_resp_string(std::string s)
{
    std::string respond ="$" + std::to_string(s.length()) + "\r\n" + s + "\r\n";
    return respond;
}

int translate_posion(const int& position, const int& number_of_element) {
    int value = position;
    if (position < 0) {
        value = value + number_of_element;
        if (value < 0)
            value = 0;
    }
    else {
        value = std::min(value,number_of_element);
    }
    return value;
}

std::string resp_list(std::deque<std::string>& my_list, int start_p, int end_p, int& client_fd) {
    int list_size = my_list.size();
    std::string respond ="";
    int start_position = translate_posion(start_p, list_size);
    int end_position = translate_posion(end_p, list_size);
    std::cout << "start pos: " << start_position << " ;end pos: " << end_position << std::endl;
    if (start_position <= end_position && start_p*end_p >= 0) {
        int i = 0;
        int count = 0;
        for (i = start_position; i <= end_position && i < list_size; i++) {
            count++;
            respond = respond + "$" + std::to_string(my_list[i].size()) + "\r\n" + my_list[i] + "\r\n";
        }
        respond = "*" + std::to_string(count) + "\r\n" +respond;
    }
    else {
        respond = "*0\r\n";
    }
    //send(client_fd, respond.c_str(),respond.size(),0);
    return respond;
}

std::string resp_vector_str(std::vector<std::string> str_arr, int& client_fd)
{
    int arr_size = str_arr.size();
    std::string respond = "*" + std::to_string(arr_size) + "\r\n";
    for (auto x:str_arr)
    {
        respond += x;
    }
    return respond;
}
// --------- Checking system -----------------

bool check_str_is_int(std::string s) {
    int i = 0;
    if (s[i] == '-' || s[i] == '+') i++;
    for (i; i< s.length(); i++) {
        if (!(s[i] >= '0' && s[i] <= '9'))
            return false;
    }
    return true;
}

bool check_str_is_double(std::string s) {
    double value;
    try {
        value = std::stod(s);
    }
    catch (std::invalid_argument&) {
        return false;
    }
    catch (std::out_of_range&) {
        return false;
    }
    return true;

}

bool check_valid_varname(std::string& name) {
    if (name.length() > 0) {
        char c = name[0];
        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c == '_'))
            return true;
        else
            return false;
    }
    else
        return false;
}

// -------------------------------------------
// -------------- COMMAND --------------------
// -------------------------------------------

// -------------- Command test server --------------

// ------------ Command for catching error -----------------

std::string handle_unknown_cmd(int& client_fd) {
    //resp_string("-Unknown command\r\n", client_fd);
    return "-Unknown command\r\n";
}


// ------------  Input/Output logic -------------

std::string handleCMD(std::vector<std::string> &inp_arr, int& client_fd, std::string command)
{
    std::string key_word = command;
    std::string respond;
    if(key_word == "echo")
        {
            std::cout << "Handle echo command" << std::endl;
            handle_echo_cmd(inp_arr, client_fd);
        }
        else if(key_word == "ping")
        {
            std::cout << "Handle ping command" << std::endl;
            respond = handle_ping_cmd(client_fd);
        }
        else if (key_word == "get") {
            std::cout << "Handle get command" << std::endl;
            respond = handle_get_cmd(inp_arr,client_fd);
        }
        else if (key_word == "set") {
            std::cout << "Handle set command" << std::endl;
            respond = handle_set_cmd(inp_arr,client_fd);
        }
        else if (key_word == "rpush") {
            std::cout << "Handle rpush command" << std::endl;
            respond = handle_rpush_cmd(inp_arr,client_fd);
            handle_blocked_list_clients(inp_arr, 0,client_fd);
        }
        else if (key_word == "lpush") {
            std::cout << "Handle lpush command" << std::endl;
            respond = handle_lpush_cmd(inp_arr,client_fd);
            handle_blocked_list_clients(inp_arr, 1,client_fd);
        }
        else if (key_word == "lrange") {
            std::cout << "Handle lrange command" << std::endl;
            respond = handle_lrange_cmd(inp_arr,client_fd);
        }
        else if (key_word == "llen") {
            std::cout << "Handle llen command" << std::endl;
            respond = handle_llen_cmd(inp_arr,client_fd);
        }
        else if (key_word == "lpop") {
            std::cout << "Handle lpop command" << std::endl;
            respond = handle_lpop_cmd(inp_arr,client_fd);
        }
        else if (key_word == "blpop") {
            std::cout << "Handle blpop command" << std::endl;
            respond = handle_blpop_cmd(inp_arr,client_fd);
        }
        else if (key_word == "type") {
            std::cout << "Handle type command" << std::endl;
            respond = handle_type_cmd(inp_arr,client_fd);
        }
        else if (key_word == "xadd") {
            std::cout << "Handle xadd command" << std::endl;
            respond = handle_xadd_cmd(inp_arr,client_fd);
            handle_blocked_stream_clients(inp_arr);
        }
        else if (key_word == "xrange")
        {
            std::cout << "Handle xrange command" << std::endl;
            respond = handle_xrange_cmd(inp_arr, client_fd);
        }
        else if (key_word == "xread")
        {
            std::cout << "Handle xread command" << std::endl;
            respond = handle_xread_cmd(inp_arr, client_fd);
        }
        else if (key_word == "incr")
        {
            std::cout << "Handle incr command" << std::endl;
            respond = handle_incr_cmd(inp_arr, client_fd);
        }
        else if (key_word == "multi")
        {
            std::cout << "Handle multi command" << std::endl;
            respond = handle_multi_cmd(inp_arr, client_fd);
        }
        else if (key_word == "exec")
        {
            std::cout << "Handle exec command" << std::endl;
            respond = handle_exec_cmd(inp_arr, client_fd);
        }
        else {
            std::cout << "Handle unkown command" << std::endl;
            respond = handle_unknown_cmd(client_fd);
        }
    return respond;
}


void handleInput(const std::string &s, int& client_fd)
{
    int str_pos = 0;
    std::vector<std::string> inp_arr; // Store user's input

    // Check if this client turned multi
    bool is_multi = false;
    auto x = client_has_multi.find(client_fd);
    if (x != client_has_multi.end())
        is_multi = true;
    if(s[0] == '*')
    {
        inp_arr = handleArray(s, str_pos);
        std::string key_word = toLowerStr(inp_arr[0]);
        if (is_multi && key_word != "exec")
        {
            multi_cmd_data[client_fd].push_back(inp_arr);
            send_resp_string("+QUERED\r\n", client_fd);
        }
        else
        {
            send_resp_string(handleCMD(inp_arr,client_fd,key_word).c_str(),client_fd);
        }
        std::cout << "Checked" << std::endl;
    }
}

std::string handleOutput(std::string &s)
{
    std::string outp =  "$" + std::to_string(s.length()) + "\r\n" + s + "\r\n";
    return outp;
}
