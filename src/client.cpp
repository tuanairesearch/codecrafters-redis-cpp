//
// Created by tuan on 3/26/26.
//

#include "client.h"
#include "command.h"
#include <arpa/inet.h>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <netdb.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>
bool handleClient(int &client_fd) {
    // Initiate neccesary variables
    char buffer[1024];
    const char *response = "+PONG\r\n";
    // Handle in loop to keep connect alive after a request

    int size_of_buffer = recv(client_fd, buffer, sizeof(buffer), 0);
    if (size_of_buffer < 0) {
        perror("recv");
        return false;
    }

    if (size_of_buffer == 0) {
        std::cout << "Client discoonedted!" << std::endl;
        return false;
    }

    if(size_of_buffer > 0)
    {
        std::cout << "size_of_buffer: " << size_of_buffer << std::endl;
        std::string str_buffer(buffer, size_of_buffer);
        std::cout << "This is check input" << std::endl;
        //std::cout << "Raw file: " << std::endl;
        //showRAW(str_buffer);
        handleInput(str_buffer, client_fd);
        std::cout << "End check input" << std::endl;
        //send(client_fd, response, strlen(response), 0);
    }
    return true;
}

