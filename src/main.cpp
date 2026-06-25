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
#include "client.h"
#include "command_dir/command.h"
#include "resp_utls.h"
#include <unordered_map>
#include "server.h"

std::string findPort(std::vector<std::string> input_arr);

int main(int argc, char **argv) {
  // Flush after every std::cout / std::cerr
  std::vector<std::string> input_arr;
  for (int i = 1; i < argc; i++)
  {
    std::string temp = argv[i];
    std::cout << temp << std::endl;
    input_arr.push_back(temp);
  }
  std::string str_port = findPort(input_arr);
  int port = 6379;
  if (check_str_is_int(str_port))
  {
    port = std::stoi(str_port);
  }
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;
  Server server(port);
  server.init();
  server.run();
  return 0;
}
std::string findPort(std::vector<std::string> input_arr)
{
  for (int i = 0; i < input_arr.size(); i ++)
  {
    if (input_arr[i] == "--port")
    {
      if (i + 1 < input_arr.size())
      {
        return input_arr[i+1];
      }
    }
  }
  return "6379";
}