#pragma once

#include <string>
#include <vector>

class PureMethod
{
public:
    PureMethod() : pure(false) {}
    bool pure;
    std::string name;
    //std::vector<std::string> body;
    std::string body;
    std::vector<std::vector<std::string>> blocks;
};
