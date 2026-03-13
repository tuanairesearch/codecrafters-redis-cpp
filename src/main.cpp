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

//void handleEcho(std::string)

std::string handleBulkString(const std::string s, int& str_pos)
{
  // Input form
  // $<length>\r\n<string>\r\n 
  // ^
  // |
  // str_pos

  int first_rn = s.find("\r\n", str_pos + 1);
  int length;
  if(first_rn != s.npos)
  {
    // Update str_pos to first_rn
    length = std::stoi(s.substr(str_pos + 1, first_rn - str_pos - 1));
    str_pos = first_rn;

    std::cout << "Found first \\r\\n at " << first_rn << std::endl; 
    std::cout << "Length is " << length << std::endl;
    
    // Find second_rn
    int second_rn = s.find("\r\n", first_rn + 1);
    if(second_rn != s.npos)
    {
      std::cout << "Found second \\r\\n at " << second_rn << std::endl;
      // Update str_pos to second_rn
      str_pos = second_rn;

      // Get string, move 2 blocks to first character (pass \r\n)
      if(second_rn - first_rn - 2 == length)
      {
        std::string result = s.substr(first_rn + 2, length);
        std::cout << "String = " << result << std::endl;
        if (second_rn + 2 < s.length())
        {
          str_pos = second_rn + 2;
          std::cout << "str_pos change to: " << str_pos << std::endl;
        }
        else
        {
          std::cout << "str_pos is currently in the end of the string " << str_pos << std::endl;
        }
        return result;
      }
      else
      {
        std::cout << "Error -1: Mismatch between length and (second_rn - first_rn)" << std::endl;
        return "";
      }
    }
    else
    {
      std::cout << "Error -1: Cannot found second_rn" << std::endl;
      return "";
    }
  }
  else
  {
    std::cout << "Error -1: Cannot found first_rn" << std::endl;
    return "";
  }
}

std::vector<std::string> handleArray(const std::string s, int& str_pos)
{
  // Input form
  // *<number_of_elements>\r\n<element_1>\r\n<element_2>\r\n...
  // ^
  // |
  // str_pos

  // Find the number of Elements
  int found = s.find("\r\n", str_pos + 1);
  int number = std::stoi(s.substr(str_pos + 1, found - str_pos - 1));
  std::cout << "Number of Array: " << number << std::endl;
  
  // Array to store element
  std::vector <std::string> vec_str;

  // Move str_pos to the first element of data
  // *<number_of_elements>\r\n<element_1>\r\n<element_2>\r\n...
  //                      ^        ^
  //                      |        |
  //                     old ->  str_pos
  str_pos = found + 2;
  
  // Handle to process data from array
  for(int i = 0; i < number; i++)
  {
    switch (s[str_pos])
    {
      case '*':
        handleArray(s,str_pos);
        break;
      case '$':
        vec_str.push_back(handleBulkString(s,str_pos));
        break;
      default:
        break;
    }
  }
  
  // Suppose we only handle a string
  return vec_str;
}

void handleOutput(std::string s)
{
  std::string outp = "$" + s.length() + std::string("\r\n") + s + "\r\n";
}

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
    //for(auto x:inp_arr)
      //std::cout << x << std::endl;
    //std::cout << "Check end" << std::endl;

    if(toLowerStr(inp_arr[0]) == "echo")
    {
      inp_arr[1] = "+" + inp_arr[1] +"\r\n";
      char result[inp_arr[1].length()+1];
      strcpy(result, inp_arr[1].c_str());
      std::cout << "result = " << result << std::endl;
      send(client_fd, result, strlen(result),0);
    }
    std::cout << "Checked" << std::endl;
  }
}


void showRAW(std::string s)
{
  if(s.length() != 0)
  {
    for(char c:s)
    {
      std::cout << "char -> ";
      if(c == '\n')
        std::cout << "\\n";
      else if(c == '\r')
        std::cout << "\\r";
      else if(c == '\0')
        std::cout << "\\0";
      else 
        std::cout << c;
        std::cout << " <- char" << std::endl;
    }
  }
  else
  {
    std::cout << "Nothing to show!" << std::endl;
  }
    
}



void handleClient(int &client_fd) {
  // Initiate neccesary variables
  char buffer[1024];
  const char *response = "+PONG\r\n";
  // Handle in loop to keep connect alive after a request

  int size_of_buffer = recv(client_fd, buffer, sizeof(buffer), 0);
  if (size_of_buffer < 0) {
    std::cout << "Client discoonedted!" << std::endl;
  }

  if (size_of_buffer == 0) {
    perror("recv");
    close(client_fd);
  }
  if(size_of_buffer >= 0)
  {
    std::cout << "size_of_buffer: " << size_of_buffer << std::endl;
    std::string str_buffer(buffer, size_of_buffer);
    std::cout << "This is check input" << std::endl;
    //std::cout << "Raw file: " << std::endl;
    //showRAW(str_buffer);
    handleInput(str_buffer, client_fd);
    std::cout << "End check input" << std::endl;
    send(client_fd, response, strlen(response), 0);
  }
  
  
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
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) <
      0) {
    std::cerr << "setsockopt failed\n";
    return 1;
  }

  struct sockaddr_in server_addr;
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(6379);

  if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) !=
      0) {
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

  // You can use print statements as follows for debugging, they'll be visible
  // when running tests.
  std::cout << "Logs from your program will appear here!\n";

  // Uncomment the code below to pass the first stage
  //

  char buffer[1024];
  std::vector<int> client_fd;
  int max_fd = 0;
  while (true) {
    fd_set readfds; // fds = file descriptors
    FD_ZERO(&readfds);

    FD_SET(server_fd, &readfds);
    max_fd = server_fd;
    for (auto x : client_fd) {
      FD_SET(x, &readfds);
      max_fd = std::max(max_fd, x);
    }

    std::cout << std::endl;
    int retval; // retval = return value
    retval = select(max_fd + 1, &readfds, NULL, NULL, NULL);
    if (retval < 0) {
      perror("retval: ");
    } else {
      std::cout << retval << std::endl;
    }
    // if has new client
    if (FD_ISSET(server_fd, &readfds)) {
      std::cout << "Has new client" << std::endl;
      socklen_t client_addr_len = sizeof(client_addr);
      client_fd.push_back(accept(server_fd, (struct sockaddr *)&client_addr,
                                 (socklen_t *)&client_addr_len));
    }

    // if client sends request
    std::cout << "Client FD: ";
    for (auto x : client_fd)
      std::cout << x << " ";
    std::cout << std::endl;
    for (auto client : client_fd) {
      if (FD_ISSET(client, &readfds)) {
        // char* response = "+PONG\r\n";

        std::cout << "Get data from:" << std::endl;
        std::cout << client << std::endl;
        handleClient(client);
      }
    }
    std::cout << std::endl;
  }

  // int client_fd = accept(server_fd, (struct sockaddr*)&client_addr,
  // (socklen_t*)&client_addr_len); std::cout << "Client id " <<client_fd << "
  // connected\n"; handleClient(client_fd);

  close(server_fd);

  return 0;
}