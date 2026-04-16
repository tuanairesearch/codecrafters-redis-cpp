#include "server.h"
#include "client.h"
#include "global.h"
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

Server::Server(int port): port_(port), server_fd_(-1) {}

Server::~Server() {
    if (server_fd_ >= 0) close(server_fd_);
    for (auto fd:client_fds_) close(fd);
}

fd_set Server::buildFdSet(int &max_fd) {
    fd_set readfds; // fds = file descriptors
    FD_ZERO(&readfds); // set value to 0

    // Add server
    FD_SET(server_fd_, &readfds);

    // Add clients
    max_fd = server_fd_;
    for (auto x : client_fds_) {
        FD_SET(x, &readfds);
        max_fd = std::max(max_fd, x);
    }
    return readfds;
}

void Server::init() {
    server_fd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd_ < 0) {
        std::cerr << "Failed to create server socket\n";
    }

    // Since the tester restarts your program quite often, setting SO_REUSEADDR
    // ensures that we don't run into 'Address already in use' errors
    int reuse = 1;
    if (setsockopt(server_fd_, SOL_SOCKET, SO_REUSEADDR,
        &reuse, sizeof(reuse)) < 0)
    {
        std::cerr << "setsockopt failed\n";
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(6379);

    if (bind(server_fd_, (struct sockaddr *)&server_addr,
        sizeof(server_addr)) != 0)
    {
        std::cerr << "Failed to bind to port 6379 - Tuna\n";
        return;
    }

    int connection_backlog = 5;
    if (listen(server_fd_, connection_backlog) != 0) {
        std::cerr << "listen failed\n";
    }

    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    std::cout << "Server has been created on port: " << port_ << std::endl;
    std::cout << "Waiting for a client to connect...\n";
    std::cout << "Logs from your program will appear here!\n";

}

void Server::acceptNewClient(fd_set& readfds) {
    struct sockaddr_in client_addr;

    if (FD_ISSET(server_fd_, &readfds)) {
        std::cout << "Has new client" << std::endl;
        socklen_t client_addr_len = sizeof(client_addr);
        client_fds_.push_back(accept(server_fd_, (struct sockaddr *)&client_addr,
                                   (socklen_t *)&client_addr_len));
    }
}

void Server::handleExistingClients(fd_set& readfds) {
    std::cout << "Client FD: ";
    for (auto x : client_fds_)
        std::cout << x << " ";
    std::cout << std::endl;
    int client;
    for (int i = 0; i < client_fds_.size();)
    {
        client = client_fds_[i];
        if (FD_ISSET(client, &readfds)) {
            // char* response = "+PONG\r\n";

            std::cout << "Get data from:" << std::endl;
            std::cout << client << std::endl;
            bool stil_alive = handleClient(client);
            if(!stil_alive)
            {
                client_fds_.erase(client_fds_.begin()+i);
                close(client);
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
    std::cout << std::endl;
}

void Server::run() {
    std::cout << "Run\n";
    while (true) {
        std::cout << "In While\n";
        int max_fd = server_fd_;
        fd_set readfds = buildFdSet(max_fd);
        auto nearest_time = nearest_expired(blocked_clients);
        int retval;
        if (blocked_clients.size() == 0 || nearest_time.has_expired == false) {
            retval=select(max_fd + 1, &readfds, NULL, NULL, NULL);
        }
        else {
            timeval t_out = change_time_to_timeval(nearest_time);
            //timeval t_test = {0,100'000};
            int sec = nearest_time.expired_duration + 1;
            int u_sec = (nearest_time.expired_duration - sec) * 1'000'000;
            timeval t_test = {sec,u_sec};
            auto start_point = std::chrono::steady_clock::now();
            //std::cerr << "tv_sec=" << t_out.tv_sec << " tv_usec=" << t_out.tv_usec << "\n";
            retval = select(max_fd + 1, &readfds, NULL, NULL, &t_test);
            auto end_point = std::chrono::steady_clock::now();
            auto duration_us = std::chrono::duration_cast<std::chrono::microseconds>(end_point - start_point);
            std::cerr << "duration: " << duration_us << std::endl;
        }
        std::cerr << "retval: " << retval << std::endl;
        if (retval < 0) {
            perror("retval: ");
            continue;
        }
        else if (retval == 0) {
            // Time out
            for (int i = 0; i < blocked_clients.size();) {
                if (blocked_clients[i].has_expired && blocked_clients[i].expired_time <= std::chrono::steady_clock::now()) {
                    send_resp_string("*-1\r\n",blocked_clients[i].client_fd);
                    for (int j = 0; j < client_fds_.size();) {
                        if (blocked_clients[i].client_fd == client_fds_[j])
                            client_fds_.erase(client_fds_.begin()+j);
                        else
                            j++;
                    }
                    blocked_clients.erase(blocked_clients.begin()+i);
                }
                else {
                    i++;
                }
            }
        }
        else
        {
            std::cout << "Request found\n";
        }
        if (FD_ISSET(server_fd_, &readfds)) {
            acceptNewClient(readfds);
        }
        handleExistingClients(readfds);
    }
}
