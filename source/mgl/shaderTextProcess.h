#pragma once

#include <string>
#include <regex>
#include <vector>

// split string to vector<string> with delim "\\s+"
inline std::vector<std::string> split(std::string &src)
{
    std::regex re("\\s+");
    std::regex_token_iterator<std::string::const_iterator> res(src.begin(), src.end(), re, -1);
    std::vector<std::string> ans(res, std::sregex_token_iterator());
    for(auto i = ans.begin(); i != ans.end();) {
        if(*i == "") i = ans.erase(i); 
        else i++;
    }
    return ans;
}