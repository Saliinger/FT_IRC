#include <cstdlib>
#include <iostream>

#include "../include/Server.hpp"

int main(int argc, char const* argv[]) {
  if (argc != 3) {
    std::cerr << "Usage: ./ircserv <port> <password>" << std::endl;
    return 1;
  }

  // av[1] = port
  // av[2] = password

  int port = std::atoi(argv[1]);
  std::string password(argv[2]);
  Server serv(port, password);
  try {
    serv.run();
  } catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
    return 1;
  }
  return 0;
}
