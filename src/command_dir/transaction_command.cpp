//
// Created by tuan on 6/16/26.
//

#include "transaction_command.h"
#include "./command.h"
#include "../global.h"
//#include <string>



std::string handle_incr_cmd(std::vector<std::string> &inp_arr,int& client_fd)
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
                return ":" + std::to_string(value) + "\r\n";
            }
            else
            {
                //resp_string("-ERR value is not an integer or out of range\r\n",client_fd);
                return "-ERR value is not an integer or out of range\r\n";
            }
        }
        else
        {
            client_data_string[var_name].value = "1";
            //resp_int(1,client_fd);
            return ":1\r\n";
        }
    }
    else
    {
        send_resp_string("-Syntax error. Please try again with INCR <variable name>\r\n", client_fd);
    }
}

std::string handle_multi_cmd(std::vector<std::string> &inp_arr,int& client_fd)
{
    if (inp_arr.size() == 1)
    {
        client_has_multi[client_fd] = true;
        //resp_string("+OK\r\n", client_fd);
        return "+OK\r\n";
    }
    else
    {
        //resp_string("-Syntax error. Try MULTI\r\n", client_fd);
        return "-Syntax error. Try MULTI\r\n";
    }
}

std::string handle_exec_cmd(std::vector<std::string> &inp_arr,int& client_fd)
{
    if (inp_arr.size() == 1)
    {
        auto x = client_has_multi.find(client_fd);
        std::vector <std::string> result;
        if (x != client_has_multi.end())
        {
            client_has_multi.erase(client_fd);
            if (!multi_cmd_data[client_fd].empty())
            {
                std::string RESULT;
                for (auto val : multi_cmd_data[client_fd])
                {
                    auto inp_arr = val;
                    std::string key_word = toLowerStr(inp_arr[0]);
                    result.push_back(handleCMD(inp_arr,client_fd,key_word));
                    // return RESULT;
                }
                RESULT = resp_vector_str(result,client_fd);
                multi_cmd_data.erase(client_fd);
                return RESULT;
            }
            else
            {
                return "*0\r\n";
            }
        }
        else
        {
            //send_resp_string("-ERR EXEC without MULTI\r\n", client_fd);
            return "-ERR EXEC without MULTI\r\n";
        }
    }
}

std::string handle_discard_cmd(std::vector<std::string> &inp_arr,int& client_fd)
{
    if (inp_arr.size() == 1)
    {
        if (client_has_multi.find(client_fd) != client_has_multi.end())
        {
            if (multi_cmd_data.find(client_fd) != multi_cmd_data.end())
            {
                multi_cmd_data.erase(client_fd);
            }
            client_has_multi.erase(client_fd);
            return "+OK\r\n";
        }
        else
        {
            return "-ERR DISCARD without MULTI\r\n";
        }
    }
    else
    {
        return "";
    }
}