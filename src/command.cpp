//
// Created by tuan on 3/26/26.
//

#include "command.h"
#include "resp_utls.h"
#include <string>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <cstring>
#include <sys/socket.h>

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

void handleInput(const std::string &s, int& client_fd)
{
    int str_pos = 0;
    std::vector<std::string> inp_arr;
    if(s[0] == '*')
    {
        //std::cout << "Check string arr" << std::endl;
        inp_arr = handleArray(s, str_pos);

        if(toLowerStr(inp_arr[0]) == "echo")
        {
            inp_arr[1] = handleOutput(inp_arr[1]);
            char result[inp_arr[1].length()+1];
            strcpy(result, inp_arr[1].c_str());
            std::cout << "result = " << inp_arr[1] << std::endl;
            send(client_fd, result, strlen(result),0);
        }
        else if(toLowerStr(inp_arr[0]) == "ping")
        {
            char result[] = "+PONG\r\n";
            send(client_fd, result, strlen(result),0);
        }
        std::cout << "Checked" << std::endl;
    }
}

std::string handleOutput(std::string s)
{
    std::string outp =  "$" + std::to_string(s.length()) + "\r\n" + s + "\r\n";
    return outp;
}
