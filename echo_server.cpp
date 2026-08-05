#include <algorithm>
#include <format>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

int main() {
  // Create the socket
  int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (socket_fd < 0) {
    std::cerr << "Error in creating socket!";
    std::exit(1);
  }
  std::clog << std::format("Socket has been created successfully: {}\n",
                           socket_fd);

  // Bind the socket with the port

  struct sockaddr_in address;
  constexpr int port{8080};
  address.sin_family = AF_INET;
  address.sin_port = htons(port);
  address.sin_addr.s_addr = INADDR_ANY;
  if (bind(socket_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
    std::cerr << std::format("Bind failed! Port {} might be in use", port)
              << std::endl;
    std::exit(1);
  } else {
    std::clog << "Socket successfully bind to the port " << port << "\n";
  }

  // Implementing listen() on server to listen incoming request on mentioned
  // port
  constexpr int backlog{5};
  if (listen(socket_fd, backlog) < 0) {
    std::cerr << "Listening failed! Port might be in use" << std::endl;
    std::exit(1);
  } else {
    std::cout << "Server is listening successfully on port " << port << "..."
              << std::endl;
  }
  while (true) {
    // Implementing accpet to accept request from client
    struct sockaddr_in client_address;
    socklen_t client_len = sizeof(client_address);
    std::clog << "Waiting for connection to arrive: ..." << std::endl;
    int client_fd =
        accept(socket_fd, (struct sockaddr *)&client_address, &client_len);
    if (client_fd < 0) {
      std::cerr << "Failed to establish connection." << std::endl;
      continue;
    } else {
      std::clog << std::format("Connection Established! file descriptor {}",
                               client_fd)
                << std::endl;
    }
    char buffer[1024]{0};
    while (true) {
      std::fill(std::begin(buffer), std::end(buffer), 0);
      auto bytes_recieved = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
      if (bytes_recieved < 0) {
        std::cerr << "Error in recieving bytes" << std::endl;
        break;
      } else if (bytes_recieved == 0) {
        std::clog << "Client disconnected before sending messages" << std::endl;
        break;
      }
      std::cout << "Client says: " << buffer << std::endl;
      // std::string response{"Hi from Bismaya's server.\n"};
      send(client_fd, buffer, bytes_recieved, 0);
    }
    close(client_fd);
  }
  close(socket_fd);

  return 0;
}