//
// Created by tuan on 3/26/26.
//

#include "resp_utls.h"
#include <iostream>
#include <string>
#include <vector>

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

std::vector<std::string> handleArray(const std::string s, int& str_pos) {
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
  std::cout << "----------- string got ---------- " << std::endl;
  for (auto x:vec_str) {
    std::cout << x << std::endl;
  }
  std::cout << "----------- string end ---------- " << std::endl;
  return vec_str;
}
