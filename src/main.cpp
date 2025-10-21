#include "../include/Server.hpp"
#include <cstdlib>

int main(int argc, char const *argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: ./ircserv <port> <password>" << std::endl;
        return 1;
    }

    // av[1] = port
    // av[2] = password
    
    return 0;
}
