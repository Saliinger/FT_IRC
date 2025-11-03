#include "../include/Errors.hpp"

// Format: :<server> <code> <target> <message>
std::string formatReply(const std::string &code, const std::string &target, const std::string &message)
{
    return (":ft_irc " + code + " " + target + " " + message + "\r\n");
}

// Format error with parameters
std::string formatError(const std::string &code, const std::string &target, const std::string &param, const std::string &message)
{
    return (":ft_irc " + code + " " + target + " " + param + " " + message + "\r\n");
}
