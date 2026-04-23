//
// Created by tuan on 3/26/26.
//
#ifndef SERVER_H
#define SERVER_H

#include <vector>
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
#include "command_dir/command.h"
#include "global.h"

class Server {
public:
    Server(int port);
    ~Server();

    void init(); // initiate a new server

    void run(); // start server
private:
    int port_;
    int server_fd_;
    std::vector<int> client_fds_;

    void acceptNewClient(fd_set& readfds);
    void handleExistingClients(fd_set& readfds);
    fd_set buildFdSet(int& max_fd);
};
#endif //SERVER_H