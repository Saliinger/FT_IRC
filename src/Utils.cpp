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

void sendToClient(int fd, const std::string &msg)
{
	ssize_t bytes = send(fd, msg.c_str(), msg.size(), 0);
	if (bytes == -1)
	{
		std::cerr << "send failed" << std::endl;
	}
}