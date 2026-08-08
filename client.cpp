#include <arpa/inet.h>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>


constexpr int PORT{8080};

int main() {
  int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (socket_fd < 0) {
    std::cerr << "Error in creating socket descriptor!" << std::endl;
    std::exit(1);
  }
  struct sockaddr_in server_address;
  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(PORT);
  if (inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr) <= 0) {
    std::cerr << "Invalid address! or Address isn't supported." << std::endl;
    close(socket_fd);
    std::exit(1);
  }
  if (connect(socket_fd, (struct sockaddr *)&server_address,
              sizeof(server_address)) < 0) {
    std::cerr << "Failed to establish connection!" << std::endl;
    close(socket_fd);
    std::exit(1);
  }
  std::clog << "Connection established successfully!\n";
  char welcome_buff[1024]{0};
  recv(socket_fd, welcome_buff, sizeof(welcome_buff) - 1, 0);
  std::cout << welcome_buff;
  std::string client_input{};
  while (true) {
    client_input.clear();
    std::cout << "<client>: ";
    std::getline(std::cin, client_input);
    client_input += '\n';
    send(socket_fd, client_input.c_str(), client_input.length(), 0);
    if (client_input == "QUIT\n")
      break;
    char server_msg_buffer[1024]{0};
    auto bytes_recieved =
        recv(socket_fd, server_msg_buffer, sizeof(server_msg_buffer) - 1, 0);
    if (bytes_recieved <= 0) {
      std::cerr << "No response recieved from the server end!" << std::endl;
      break;
    }
    std::cout << "<server>: " << server_msg_buffer;
  }
  close(socket_fd);
  return 0;
}