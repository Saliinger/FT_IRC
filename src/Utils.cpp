#include "../include/Utils.hpp"

std::vector<std::string> split(const std::string &str, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;

    while(std::getline(ss, token, delimiter)) {
        if(!token.empty())
            tokens.push_back(token);
    }

    return tokens;
}