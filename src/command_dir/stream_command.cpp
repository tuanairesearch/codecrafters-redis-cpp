//
// Created by tuan on 4/23/26.
//

#include "stream_command.h"
#include "command.h"
#include "list_command.h"
#include "../global.h"
#include <iostream>
#include <string>
#include <pstl/execution_defs.h>
/*
 * 1) std::unordered_map<std::string, data> client_data_string;
 * 2) std::unordered_map<std::string,std::deque<std::string>> client_data_list;
 */


std::string data_type_of(std::string var_name) {
    {
        auto x = client_data_string.find(var_name);
        if (x != client_data_string.end()) {
            return "string";
        }
    }
    {
        auto x = stream_data.find(var_name);
        if (x != stream_data.end()) {
            return "stream";
        }
    }
    return "none";
}



// Checking helper



// This function use to make id-seq base on key_name (name of key) and key_value id-seq user input
StreamID make_id_seq(std::string stream_name, std::string value) {
    StreamID stream_id;
    auto it = stream_data[stream_name].rbegin();

    // handle 0-0 case seperately
    // hanle * input -> system automaticly create valid id-seq
    if (value == "*") {
        if (stream_data[stream_name].empty())
        {
            stream_id.sequence_number = 0;
        }
        else
        {
            if (it->first.stream_id == stream_id.stream_id) {
                stream_id.sequence_number = it->first.sequence_number + 1;
            }
            else if (it->first.stream_id < stream_id.stream_id) {
                stream_id.sequence_number = 0;
            }
            else {
                stream_id.stream_id = it->first.stream_id;
                stream_id.sequence_number = it->first.sequence_number + 1;
            }
        }
    }
    // hanlde id - * and id - seq
    else {
        size_t pos = value.find("-");
        std::string ms_str = value.substr(0,pos);
        std::string seq_str = value.substr(pos + 1, value.length() - pos);

        // id - *
        if (seq_str == "*" && check_str_is_int(ms_str)) {
            stream_id.stream_id = std::stoll(ms_str);
            // data is empty -> just add
            if (stream_data[stream_name].empty()) {
                stream_id.sequence_number = 0;
            }
            else {
                if (it->first.stream_id < stream_id.stream_id) {
                    stream_id.sequence_number = 0;
                }
                else if (it->first.stream_id == stream_id.stream_id) {
                    stream_id.sequence_number = it->first.sequence_number + 1;
                }
                else {
                    // Error
                    stream_id.stream_id = -1;
                }
            }
        }
        // id - seq
        else {
            stream_id.stream_id = std::stoll(ms_str);
            stream_id.sequence_number = std::stoll(seq_str);
            if (!(stream_data[stream_name].empty() || it->first < stream_id))
            {
                //error
                stream_id.stream_id = -1;
            }
        }
    }
    if (value == "0-0")
    {
        stream_id.stream_id = 0;
        stream_id.sequence_number = 0;
    }
    else if (stream_id.stream_id == 0 && stream_id.sequence_number == 0)
    {
        stream_id.sequence_number = 1;
    }
    /*
     * output maybe
     * 0-0
     * id-seq
     * -1 - ? -> #fail signal
     */
    return stream_id;
}


// Helper

std::string cstr_to_redis_str(std::string s)
{
    uint8_t length = s.length();
    return "$" + std::to_string(length) + "\r\n" + s + "\r\n";
}

std::string build_array_from_vector(std::vector<std::pair<std::string,std::string>> &arr)
{
    std::string result = "";
    int arr_size = arr.size() * 2;
    result = result + "*" + std::to_string(arr_size) + "\r\n";
    for (auto x:arr)
    {
        result += cstr_to_redis_str(x.first);
        result += cstr_to_redis_str(x.second);
    }
    return result;
}

std::string build_arr_from_map_pair(std::pair<const StreamID , std::vector<std::pair<std::string, std::string>>> &data)
{
    std::string result = "";
    result = "*2\r\n";
    result = result + cstr_to_redis_str(data.first.to_str()) + build_array_from_vector(data.second);
    return result;
}

std::string build_output_from_map(
    std::map<StreamID,std::vector<std::pair<std::string, std::string>>>::iterator &begin,
    std::map<StreamID,std::vector<std::pair<std::string, std::string>>>::iterator &end)
{
    int count = 0;
    std::string result = "";
    for (auto it = begin; it != end; it++)
    {
        result = result + build_arr_from_map_pair(*it);
        count++;
    }
    result = "*" + std::to_string(count) + "\r\n" + result;
    return result;
}

// this function need checked input
StreamID translate_start_end_xrange(std::string &value)
{
    // '-' mean 0-1
    // '+' mean 18446744073709551615-18446744073709551615 (max of uint64_t)
    // return stoll(value)
    StreamID result;
    if (value == "-")
    {
        result.stream_id = 0;
        result.sequence_number = 1;
    }
    else if (value == "+")
    {
        // this will go to end of data type
        result.stream_id = -1;
        result.sequence_number = -1;
    }
    else
    {
        size_t pos = value.find("-");
        std::string ms_str = value.substr(0,pos);
        std::string seq_str = value.substr(pos + 1, value.length() - pos);
        if (check_str_is_int(ms_str) && check_str_is_int(seq_str))
        {
            result.stream_id = std::stoll(ms_str);
            result.sequence_number = std::stoll(seq_str);
        }
        else
        {
            result.stream_id = 0;
            result.sequence_number = 0;
        }
    }
    return result;
}

StreamID translate_start_end_xread(std::string value, std::string key_name)
{
    // '$' mean end of stream
    // return stoll(value)
    StreamID result;
    if (value != "$")
    {
        // there are two cases
        // 1) <id>-<seq>
        // 2) <id> (only)
        if (value.find("-") != std::string::npos)
        {
            size_t pos = value.find("-");
            std::string ms_str = value.substr(0,pos);
            std::string seq_str = value.substr(pos + 1, value.length() - pos);
            if (check_str_is_int(ms_str) && check_str_is_int(seq_str))
            {
                result.stream_id = std::stoll(ms_str);
                result.sequence_number = std::stoll(seq_str);
            }
            else
            {
                result.stream_id = -1;
                result.sequence_number = -1;
            }
        }
        else
        {
            if (check_str_is_int(value))
            {
                result.stream_id = std::stoll(value);
                result.sequence_number = 0;
            }
            else
            {
                result.stream_id = -1;
                result.sequence_number = -1;
            }
        }
    }
    else
    {
        if (!stream_data[key_name].empty())
            result = stream_data[key_name].rbegin()->first;
        else
        {
            result.stream_id = 0;
            result.sequence_number = 0;
        }
    }
    return result;
}

std::string handle_type_cmd(std::vector<std::string> &inp_arr,int& client_fd) {
    size_t check = inp_arr.size();
    if (check == 2) {
        std::string result = data_type_of(inp_arr[1]);
        //resp_string(("+" + result + "\r\n").c_str(),client_fd);
        return ("+" + result + "\r\n");
    }
    else {
        //resp_string("-Syntax error. Try TYPE <var_name>\r\n",client_fd);
        return "-Syntax error. Try TYPE <var_name>\r\n";
    }
}

std::string handle_xadd_cmd(std::vector<std::string> &inp_arr,int& client_fd) {
    size_t check = inp_arr.size();
    std::vector<std::pair<std::string, std::string>> arr;
    if (check >= 5 && check % 2 == 1) {
        // set up key name and key value
        std::string stream_name = inp_arr[1];
        std::string key_value = inp_arr[2];
        // Build vector with data
        for (int i = 3; i < inp_arr.size(); i += 2) {
            std::pair<std::string, std::string> temp_pair;
            temp_pair.first = inp_arr[i];
            temp_pair.second = inp_arr[i+1];
            arr.push_back(temp_pair);
        }
        // Create ID-SEQ
        StreamID stream_id = make_id_seq(stream_name, key_value);
        if (stream_id.stream_id == 0 && stream_id.sequence_number == 0)
        {
            //resp_string("-ERR The ID specified in XADD must be greater than 0-0\r\n", client_fd);
            return "-ERR The ID specified in XADD must be greater than 0-0\r\n";
        }
        else if (stream_id.stream_id != -1)
        {
            stream_data[stream_name].insert({stream_id,arr});
            std::string message = std::to_string(stream_id.stream_id) + "-" + std::to_string(stream_id.sequence_number);
            //send_resp_string(message,client_fd);
            return message;
        }
        else
        {
            //resp_string("-ERR The ID specified in XADD is equal or smaller than the target stream top item\r\n", client_fd);
            return "-ERR The ID specified in XADD is equal or smaller than the target stream top item\r\n";
        }
    }
    return "-1\r\n";
}

std::string handle_xrange_cmd(std::vector<std::string> &inp_arr,int& client_fd)
{
    size_t check = inp_arr.size();
    if (check == 4)
    {
        std::string stream_name = inp_arr[1];
        if (stream_data[stream_name].empty())
        {
            //resp_string("*0\r\n", client_fd);
            return "*0\r\n";
        }
        else
        {
            StreamID start_id = translate_start_end_xrange(inp_arr[2]);
            StreamID end_id = translate_start_end_xrange(inp_arr[3]);
            if (!((start_id.stream_id == 0 && start_id.sequence_number == 0) || (end_id.stream_id == 0 && end_id.sequence_number == 0)))
            {
                auto start_ptr = stream_data[inp_arr[1]].lower_bound(start_id);
                auto end_ptr = stream_data[inp_arr[1]].upper_bound(end_id);
                std::string result = build_output_from_map(start_ptr, end_ptr);
                //resp_string(result.c_str(),client_fd);
                return result;
            }
            else
            {
                //resp_string("-Invalid range\r\n",client_fd);
                return "-Invalid range\r\n";
            }
        }
    }
    return "-1\r\n";
}

std::string handle_xread_cmd(std::vector<std::string> &inp_arr,int& client_fd)
{
    size_t check = inp_arr.size();
    if (check % 2 == 0)
    {
        // Hanlde non-blocking
        // This can be multi key and id
        if("streams" == toLowerStr(inp_arr[1]))
        {
            // each element
            // first -> key_name
            // second -> id
            // XREAD STREAM <key1> <key2> <key3> <id1> <id2> <id3>
            //                ^                    ^
            //                |--(arr_inpt - 2)/2--|

            std::vector<std::pair<std::string, std::string>> vector_result;
            size_t number_of_key = (check - 2)/2;
            std::pair<std::string, std::string> temp;
            std::string result = "";
            int count = 0;
            for (int i = 0; i < number_of_key; i++)
            {
                // key_name
                temp.first = inp_arr[i + 2];
                // id-seq
                temp.second = inp_arr[i + 2 + number_of_key];

                StreamID start_id = translate_start_end_xread(temp.second, temp.first);
                auto start_ptr = stream_data[temp.first].upper_bound(start_id);
                auto end_ptr = stream_data[temp.first].end();

                std::string temp_data = build_output_from_map(start_ptr,end_ptr);
                if (temp_data != "*0\r\n")
                {
                    result = result + "*2\r\n" + cstr_to_redis_str(temp.first) + temp_data;
                    count++;
                }
            }
            result = "*" + std::to_string(count) + "\r\n" + result;
            return result;
        }
        else if ("block" == toLowerStr(inp_arr[1]) && check == 6)
        {
            // syntax:
            // XREAD BLOCK <time> STREAMS <key> <id>

            std::string const &time = inp_arr[2];
            std::string const &key = inp_arr[4];
            std::string const &id = inp_arr[5];
            if (check_str_is_int(time))
            {
                uint64_t blocking_time = std::stoll(time);

                StreamID start_id = translate_start_end_xread(id, key);
                auto start_ptr = stream_data[key].upper_bound(start_id);
                auto end_ptr = stream_data[key].end();
                std::string result = build_output_from_map(start_ptr,end_ptr);
                if (result == "*0\r\n")
                {
                    // blocking
                    blocked_client temp;
                    temp.stream_id = translate_start_end_xread(id,key);
                    temp.client_fd = client_fd;
                    temp.type = 1;
                    temp.stream_key = key;
                    if (blocking_time == 0)
                    {
                        // Block forever
                        temp.expired_time = std::chrono::steady_clock::time_point::max();
                    }
                    else
                    {
                        // Block for blocking_time
                        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::duration<uint64_t, std::milli>(blocking_time));
                        temp.expired_time = std::chrono::steady_clock::now() + duration;
                    }
                    blocked_clients.push_back(temp);
                }
                else
                {
                    result = "*1\r\n*2\r\n" + cstr_to_redis_str(key) + result;
                    //resp_string(result.c_str(),client_fd);
                    return result;
                }
            }
            else
            {
                // time error
            }
        }
        else
        {
            //error
        }
    }
    else
    {
        //error
    }
}

void handle_blocked_stream_clients(std::vector<std::string> &inp_arr) {
    for (int i = 0; i < blocked_clients.size();)
    {
        // type = 1 mean this is stream
        if (blocked_clients[i].type == 1)
        {
            auto start_ptr = stream_data[blocked_clients[i].stream_key].upper_bound(blocked_clients[i].stream_id);
            auto end_ptr = stream_data[blocked_clients[i].stream_key].end();
            std::string temp_data = build_output_from_map(start_ptr,end_ptr);

            // temp_data != (empty array) that mean there is data to handle
            if (temp_data != "*0\r\n")
            {
                std::string result = "*1\r\n*2\r\n" + cstr_to_redis_str(blocked_clients[i].stream_key) + temp_data;
                send_resp_string(result.c_str(), blocked_clients[i].client_fd);
                blocked_clients.erase(blocked_clients.begin() + i);
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