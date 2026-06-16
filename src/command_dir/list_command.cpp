//
// Created by tuan on 4/23/26.
//

#include "./command.h"
#include "../global.h"
#include <string>
#include <iostream>
#include <vector>
#include <sys/socket.h>

std::string handle_ping_cmd(int& client_fd) {
    //send_resp_string("+PONG\r\n", client_fd);
    return "+PONG\r\n";
}

std::string handle_echo_cmd(std::vector<std::string> &inp_arr, int& client_fd) {
    std::cout << "-----------------------" << std::endl;
    std::cout << inp_arr.size() << std::endl;
    std::cout << "-----------------------" << std::endl;
    if (inp_arr.size() == 2) {
        std::string s = handleOutput(inp_arr[1]);
        std::cout << "result = " << s << std::endl;
        //send_resp_string(s.c_str(),client_fd);
        return s.c_str();
    }
    else {
        //send_resp_string("-Missing field. Try \"echo <text>\"\r\n",client_fd);
        return "-Missing field. Try \"echo <text>\"\r\n";
    }
}

// -------------- Command handle var - string data ----------------

std::string handle_get_cmd(std::vector<std::string> &inp_arr, int &client_fd) {
    size_t check = inp_arr.size();
    if (check == 2) {
        std::cout << "\"" << inp_arr[1] << "\"" << std::endl;
        if (client_data_string.contains(inp_arr[1])) {
            // Check has_expired
            // If has_expired == false -> show variable
            // If has_expired == true -> check if now() < expired_time -> show variable
            bool check_valid = ((client_data_string[inp_arr[1]].has_expired == true
                && std::chrono::steady_clock::now() <= client_data_string[inp_arr[1]].expired_time)
                || client_data_string[inp_arr[1]].has_expired == false);
            if (check_valid) {
                std::string s = handleOutput(client_data_string[inp_arr[1]].value);
                //send(client_fd,s.c_str(),s.length(),0);
                return s.c_str();
            }
            else {

                // if variable expired -> delete it too (this is call lazy check)

                if ((client_data_string[inp_arr[1]].has_expired == true
                && std::chrono::steady_clock::now() > client_data_string[inp_arr[1]].expired_time)) {
                    client_data_string.erase(inp_arr[1]);
                }
                //send_resp_string("$-1\r\n",client_fd);
                return "$-1\r\n";
            }
        }
        else {
            //send_resp_string("$-1\r\n",client_fd);
            return "$-1\r\n";
        }
    }
    else {
        //send_resp_string("-Syntax Error. Try \"GET <var_name> <value>\"\r\n",client_fd);
        return "-Syntax Error. Try \"GET <var_name> <value>\"\r\n";
    }
}

std::string handle_set_cmd(std::vector<std::string> &inp_arr, int &client_fd) {
    size_t check = inp_arr.size();
    struct data temp;
    temp.value = inp_arr[2];
    if (check == 3) {
        temp.has_expired = false;
        temp.expired_time = std::chrono::steady_clock::now();
        client_data_string[inp_arr[1]] = temp;
        //send_resp_string("+OK\r\n",client_fd);
        return "+OK\r\n";
    }
    else if (check == 5) {
        temp.has_expired = true;
        std::string time_promt = toLowerStr(inp_arr[3] );
        // px = milliseconds duration
        // ex = seconds duration
        if (time_promt == "px" || time_promt == "ex")
        {
            try {
                long long expired_point = std::stoll(inp_arr[4]);
                if (time_promt == "px") {
                    temp.expired_time = std::chrono::steady_clock::now() + std::chrono::milliseconds(expired_point);
                }
                else {
                    temp.expired_time = std::chrono::steady_clock::now() + std::chrono::seconds(expired_point);
                }
            } catch (std::invalid_argument&) {
                //send_resp_string("-Invalid number\r\n",client_fd);
                return "-Invalid number\r\n";
            } catch (std::out_of_range&) {
                //send_resp_string("-Out of range\r\n",client_fd);
                return "-Out of range\r\n";
            }
            client_data_string[inp_arr[1]] =  temp;
            //send_resp_string("+OK\r\n",client_fd);
            return "+OK\r\n";
        }
        else {
            //send_resp_string("-Syntax Error. Try \"SET <var_name> <value> <option_field>\"\r\n",client_fd);
            return "-Syntax Error. Try \"SET <var_name> <value> <option_field>\"\r\n";
        }
    }
    else {
        //send_resp_string("-Syntax Error. Try \"SET <var_name> <value> <option_field>\"\r\n",client_fd);
        return "-Syntax Error. Try \"SET <var_name> <value> <option_field>\"\r\n";
    }
}

// -------------- Command handle list - string data ----------------

std::string handle_rpush_cmd(std::vector<std::string> &inp_arr, int& client_fd) {
    size_t check = inp_arr.size();
    if (check >= 3) {
        bool check_name = check_valid_varname(inp_arr[1]);
        if (check_name) {
            for (int i = 2; i < check; i++) {
                client_data_list[inp_arr[1]].push_back(inp_arr[i]);
            }
            check = client_data_list[inp_arr[1]].size();
            //send_resp_int(check,client_fd);
            return (":" + std::to_string(check) + "\r\n").c_str();
        }
        else {
            //send_resp_string("-Invalid variable name. The name start with '_' or alphabet character.\r\n",client_fd);
            return "-Invalid variable name. The name start with '_' or alphabet character.\r\n";
        }
    }
    else {
        //resp_string("-Syntax Error. Try \"RPUSH <var_name> <element> [element1 element2 ...\"\r\n",client_fd);
        return "-Invalid variable name. The name start with '_' or alphabet character.\r\n";
    }
}

std::string handle_lpush_cmd(std::vector<std::string> &inp_arr, int& client_fd) {
    size_t check = inp_arr.size();
    if (check >= 3) {
        bool check_name = check_valid_varname(inp_arr[1]);
        if (check_name) {
            for (int i = 2; i < check; i++) {
                client_data_list[inp_arr[1]].push_front(inp_arr[i]);
            }
            check = client_data_list[inp_arr[1]].size();
            //send_resp_int(check,client_fd);
            return (":" + std::to_string(check) + "\r\n").c_str();
        }
        else {
            //send_resp_string("-Invalid variable name. The name start with '_' or alphabet character.\r\n",client_fd);
            return "-Invalid variable name. The name start with '_' or alphabet character.\r\n";
        }
    }
    else {
        //send_resp_string("-Syntax Error. Try \"LPUSH <var_name> <element> [element1 element2 ...\"\r\n",client_fd);
        return "-Syntax Error. Try \"LPUSH <var_name> <element> [element1 element2 ...\"\r\n";
    }
}

std::string handle_llen_cmd(std::vector<std::string> &inp_arr, int& client_fd) {
    size_t check = inp_arr.size();
    if (check == 2) {
        bool check_name = check_valid_varname(inp_arr[1]);
        if (check_name) {
            //send_resp_int(client_data_list[inp_arr[1]].size(),client_fd);
            return (":" + std::to_string(client_data_list[inp_arr[1]].size()) + "\r\n").c_str();
        }
        else {
            //send_resp_string("-Invalid variable name. The name start with '_' or alphabet character.\r\n",client_fd);
            return "-Invalid variable name. The name start with '_' or alphabet character.\r\n";
        }
    }
}

std::string handle_lpop_cmd(std::vector<std::string> &inp_arr, int& client_fd) {
    size_t check = inp_arr.size();
    int number_element_remove;
    if (check == 2 && client_data_list[inp_arr[1]].size() != 0) {
        //send_resp_string(client_data_list[inp_arr[1]][0],client_fd);
        client_data_list[inp_arr[1]].pop_front();
        return client_data_list[inp_arr[1]][0];
        // CHECK BEHAVIOUR --------------------------------------------------------------------------------
    } else if (check == 3) {
        if (check_str_is_int(inp_arr[2])) {
            number_element_remove = std::stoi(inp_arr[2]);
        }
        else {
            //send_resp_string("-Invalid number of element\r\n",client_fd);
            return "-Invalid number of element\r\n";
        }

        //resp_list(client_data_list[inp_arr[1]],0,number_element_remove - 1 ,client_fd);
        return resp_list(client_data_list[inp_arr[1]],0,number_element_remove - 1 ,client_fd);
        for (int i = 0; i < number_element_remove && client_data_list[inp_arr[1]].size() != 0;i++) {
            client_data_list[inp_arr[1]].pop_front();
        }
    }
}

std::string handle_lrange_cmd(std::vector<std::string> &inp_arr, int& client_fd) {
    size_t check = inp_arr.size();
    if (check == 4) {
        if (check_str_is_int(inp_arr[2]) && check_str_is_int(inp_arr[3])) {
            int start_p = std::stoi(inp_arr[2]);
            int end_p = std::stoi(inp_arr[3]);
            return resp_list(client_data_list[inp_arr[1]],start_p,end_p,client_fd);
        }
        else {
            //resp_string("-Syntax Error. Try LRANGE <var_name> <start> <stop>\r\n", client_fd);
            return "-Syntax Error. Try LRANGE <var_name> <start> <stop>\r\n";
        }
    }
    else {
        //resp_string("-Syntax Error. Try LRANGE <var_name> <start> <stop>\r\n", client_fd);
        return "-Syntax Error. Try LRANGE <var_name> <start> <stop>\r\n";
    }
}

std::string handle_blpop_cmd(std::vector<std::string> &inp_arr, int& client_fd)
{
    size_t check = inp_arr.size();
    if (check == 3) {
        if (check_str_is_double(inp_arr[2]) && check_valid_varname(inp_arr[1])) {
            double blocking_time = std::stod(inp_arr[2]);
            //struct client_time_data temp_data;
            blocked_client temp_data;
            temp_data.type = 0;
            if (blocking_time == 0.0) {
                temp_data.client_fd = client_fd;
                temp_data.expired_time = std::chrono::steady_clock::time_point::max();
            }
            else {
                auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::duration<double>(blocking_time));
                temp_data.client_fd = client_fd;
                //temp_data.has_expired = true;
                temp_data.expired_time = std::chrono::steady_clock::now() + duration;
                //temp_data.expired_duration = blocking_time;
            }
            if (client_data_list[inp_arr[1]].size() > 0) {
                client_data_list[inp_arr[1]].pop_front();
                return client_data_list[inp_arr[1]].front();
            }
            else {
                blocked_clients.push_back(temp_data);
                return "";
            }
        }
        else {
            //resp_string("-Invalid name or time.\r\n",client_fd);
            return "-Invalid name or time.\r\n";
        }
    }
    else {
        //resp_string("-Syntax Error. Try BLPOP <var_name> <time_out>\r\n",client_fd);
        return "-Syntax Error. Try BLPOP <var_name> <time_out>\r\n";
    }
}

std::deque<std::string> two_str_to_list(std::string s1, std::string s2)
{
    std::deque<std::string> my_list;
    my_list.push_back(s1);
    my_list.push_back(s2);
    return my_list;
}

std::string handle_blocked_list_clients(std::vector<std::string> &inp_arr, int left_or_right, int &client_fd) {
    for (int i = 0; i < blocked_clients.size(); )
    {
        if (blocked_clients[i].type == 0 && client_data_list[inp_arr[1]].size() > 0)
        {
            // left = 0, right = 1
            if (left_or_right == 0)
            {
                auto result = two_str_to_list(inp_arr[1],client_data_list[inp_arr[1]].front());
                client_data_list[inp_arr[1]].pop_front();
                blocked_clients.erase(blocked_clients.begin() + i);
                return resp_list(result,0,1,blocked_clients[i].client_fd);

            }
            else if (left_or_right == 1)
            {
                auto result = two_str_to_list(inp_arr[1],*client_data_list[inp_arr[1]].rend());
                client_data_list[inp_arr[1]].pop_back();
                blocked_clients.erase(blocked_clients.begin() + i);
                return resp_list(result,0,1,blocked_clients[i].client_fd);
            }
            else
            {
                i++;
            }

        }
        else
        {
            i++;
        }
    }
}