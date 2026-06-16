//
// Created by tuan on 6/16/26.
//

#include "transaction_command.h"
#include "./command.h"
#include "../global.h"
//#include <string>
void handle_incr_cmd(std::vector<std::string> &inp_arr,int& client_fd)
{
    // INCR <variable name>
    size_t check = inp_arr.size();
    if (check == 2)
    {
        std::string var_name = inp_arr[1];
        // Check if that variable is available
        bool is_availabe = true;
        auto x = client_data_string.find(var_name);
        if (x == client_data_string.end()) {
            is_availabe = false;
        }
        if (is_availabe)
        {
            std::string str_value = client_data_string[var_name].value;
            bool is_int = check_str_is_int(str_value);
            if (is_int)
            {
                long long value = std::stoll(str_value);
                value++;
                client_data_string[inp_arr[1]].value = std::to_string(value);
                send_resp_int(value,client_fd);
            }
            else
            {
                send_resp_string("-ERR value is not an integer or out of range\r\n",client_fd);
            }
        }
        else
        {
            client_data_string[var_name].value = "1";
            send_resp_int(1,client_fd);
        }
    }
    else
    {
        send_resp_string("-Syntax error. Please try again with INCR <variable name>\r\n", client_fd);
    }
}

void handle_multi_cmd(std::vector<std::string> &inp_arr,int& client_fd)
{
    if (inp_arr.size() == 1)
    {
        client_has_multi[client_fd] = true;
        send_resp_string("+OK\r\n", client_fd);
    }
    else
    {
        send_resp_string("-Syntax error. Try MULTI\r\n", client_fd);
    }
}

void handle_exec_cmd(std::vector<std::string> &inp_arr,int& client_fd)
{
    if (inp_arr.size() == 1)
    {
        if (client_has_multi[client_fd])
        {

        }
        else
        {
            send_resp_string("-ERR EXEC without MULTI\r\n", client_fd);
        }
    }
}