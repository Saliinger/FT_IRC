#pragma once

#include <vector>
#include <sstream>
#include <string>
#include <netinet/in.h>

std::vector<std::string> split(const std::string &str, char delimiter);
void sendToClient(int fd, const std::string &msg);