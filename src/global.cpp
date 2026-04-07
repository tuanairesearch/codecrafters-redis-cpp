//
// Created by tuan on 3/27/26.
//

#include "global.h"
#include <string>
#include <vector>
#include <unordered_map>

// Explain about how data store in client_data_string
/*
 *   var_name1   var_name...          <unordered_map> -> also do not know the var name
 *       |
 *      data                          <string>  -> data
 *
 *  For example, user with id 4
 *  client_data_string[4][-------]
 *  We find data of var_name1 = "hi"
 *  client_data_string[4]["var_name1"] -> return "hi"
 */
std::unordered_map<std::string, data> client_data_string;


// Explain about how data store in client_data_list
/*
 *      list1    listn...          <unordered_map>
 *       |
 *    element1, element2,....      <vector<int>> -> for add and delete purpose
 *
 *  For example, user with id 4
 *  client_data_list[4][-------]
 *  data of list ls1(int) client_data_list[4][ls1]
 *  
 */
std::unordered_map<std::string,std::deque<std::string>> client_data_list;