#include <iostream>
#include <cstdlib>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <vector>



void handleClient(int &client_fd)
{
  // Initiate neccesary variables
  char buffer[1024];
  const char *response = "+PONG\r\n";
  // Handle in loop to keep connect alive after a request
  
  int size_of_buffer = recv(client_fd, buffer, sizeof(buffer),0);
  if(size_of_buffer < 0) {
    std::cout << "Client discoonedted!" << std::endl; 
  }

  if(size_of_buffer == 0)
  {
    perror("recv");
    close(client_fd);
  }

  std::cout << buffer << std::endl;
  send(client_fd,response,strlen(response),0);
}


int main(int argc, char **argv) {
  // Flush after every std::cout / std::cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;
  
  int server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd < 0) {
   std::cerr << "Failed to create server socket\n";
   return 1;
  }
  
  // Since the tester restarts your program quite often, setting SO_REUSEADDR
  // ensures that we don't run into 'Address already in use' errors
  int reuse = 1;
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
    std::cerr << "setsockopt failed\n";
    return 1;
  }
  
  struct sockaddr_in server_addr;
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(6379);
  
  if (bind(server_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) != 0) {
    std::cerr << "Failed to bind to port 6379\n";
    return 1;
  }
  
  int connection_backlog = 5;
  if (listen(server_fd, connection_backlog) != 0) {
    std::cerr << "listen failed\n";
    return 1;
  }
  
  struct sockaddr_in client_addr;
  socklen_t client_addr_len = sizeof(client_addr);
  std::cout << "Waiting for a client to connect...\n";

  // You can use print statements as follows for debugging, they'll be visible when running tests.
  std::cout << "Logs from your program will appear here!\n";

  // Uncomment the code below to pass the first stage
  // 

  char buffer[1024];
  std::vector<int> client_fd;
  int max_fd = 0;
  while(true)
  {
    fd_set readfds; //fds = file descriptors
    FD_ZERO(&readfds);

    FD_SET(server_fd, &readfds);
    max_fd = server_fd;
    for(auto x:client_fd)
    {
      FD_SET(x, &readfds);
      max_fd = std::max(max_fd,x);
    }

    std::cout << std::endl;
    int retval; // retval = return value
    retval = select(max_fd+1, &readfds, NULL, NULL, NULL);
    if(retval < 0)
    {
      perror("retval: ");
    }
    else
    {
      std::cout << retval << std::endl;
    }
    // if has new client
    if(FD_ISSET(server_fd, &readfds))
    {
      std::cout << "Has new client" << std::endl;
      socklen_t client_addr_len = sizeof(client_addr);
      client_fd.push_back(accept(server_fd,(struct sockaddr*)&client_addr, (socklen_t*)&client_addr_len));
    }
    // if client sends request
    std::cout << "Client FD: ";
    for(auto x:client_fd) std::cout << x << " ";
    std::cout << std::endl;
    for(auto client:client_fd)
    {
      if(FD_ISSET(client, &readfds))
      {
        //char* response = "+PONG\r\n";
        std::cout << "Get data from:" << std::endl;
        std::cout << client << std::endl;
        //send(client,response,strlen(response),0);
        handleClient(client);
      }
    }
    std::cout << std::endl;
  }

  //int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, (socklen_t*)&client_addr_len);
  //std::cout << "Client id " <<client_fd << " connected\n";
  //handleClient(client_fd);

  close(server_fd);

  return 0;
}
