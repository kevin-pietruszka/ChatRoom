/**
 * Author: Kevin Pietruszka
 * GTID: 903497239
 * GT Email: kpietruszka3@gatech.edu
 */

#include "chatserver.h"

void send_to_all(int sender ,std::string msg) {

  clients_mutex.lock();
  //std::map<int, client *>::iterator it = clients.begin();
  auto it = clients.begin();
  while (it != clients.end()) {

    if (it->first != sender) {
      client *temp = it->second;
      send(temp->sock, msg.c_str(), (size_t)(msg.length()), 0);
    }
    it++;
  }
  clients_mutex.unlock();

}

void client_connection(client *user) {

  while (1)  {

    
    char bufman[1024] = {0};
    int receive = recv(user->sock, bufman, (size_t) 1024, 0);

    if (receive > 0) {

      if (user->verified == 0) {
        
        std::string msg = std::string(bufman);

        int divider = msg.find("&");

        std::string pc, un;
        if ((const long unsigned int)divider != std::string::npos) {
          pc = msg.substr(0, divider);
          un = msg.substr(divider + 1, msg.length());
        }

        if (strcmp(passcode.c_str(), pc.c_str()) == 0) {
          clients_mutex.lock();
          user->name = new std::string(un);
          user->verified = 1;
          clients_mutex.unlock();
          send(user->sock, "connected", 10, 0);
    
          std::cout << *user->name << " joined the chatroom" << std::endl;
          send_to_all(user->sock, un + " joined the chatroom");

        } else {
          
          send(user->sock, "accessdenied", 12, 0);
          
        }

      } else {

        std::string msg = std::string(bufman);

        if (strcmp(bufman, ":Exit") == 0) {

          clients_mutex.lock();
          clients.erase(user->sock);
          clients_mutex.unlock();
          shutdown(user->sock, 1);
          std::cout << *user->name << " left the chatroom" << std::endl;
          send_to_all(user->sock, *user->name + " left the chatroom");
        } else {

          std::cout << *user->name << ": " << bufman << std::endl;
          send_to_all(user->sock, *user->name + ": " + bufman);
        }

      }
    }
  }

}

void new_connections() {
  
  while (1) {
    
    if (listen(host_sock, 7) == 0) {
      int new_socket_fd;
      sockaddr_in *new_connection = new sockaddr_in();
      socklen_t addr_len = sizeof(new_connection);

      new_socket_fd = accept(host_sock, (struct sockaddr *)new_connection, &addr_len);

      if (new_socket_fd < 0) {
        perror("Failed to create new socket for new connection");
        continue;
      }

      client *new_client = new client();
      
      new_client->sock = new_socket_fd;
      new_client->name = new std::string("");
      new_client->verified = 0;

      new_client->address = *new_connection;
      new_connection = new sockaddr_in();

      clients_mutex.lock();
      clients[new_client->sock] = new_client;
      clients_mutex.unlock();

      std::thread new_listen(client_connection, new_client);
      new_listen.detach();
    }
  }

}



void server_commands() {
  while(1) {
    std::string bufman;
    std::getline(std::cin, bufman);

    if (strcmp(bufman.c_str(), "listclients") == 0) {
      clients_mutex.lock();
      std::map<int, client *>::iterator it = clients.begin();
      while (it != clients.end()) {
        client *temp = it->second;
        std::cout << temp->name << " ";
        it++;
      }
      std::cout << std::endl;
      clients_mutex.unlock();
    }
  }
}

int main(int argc, char *argv[]) {

  // Vars for the args //
  start_flag = 0;
  port_num = 0;
  // End //

  // Paramter  //
  int opt;

  while (1) {

      int option_index = 0;
      static struct option long_options[] = {

        {"start", no_argument, 0, 's'},
        {"port", required_argument, 0, 'p'},
        {"passcode", required_argument, 0, 'c'},
        {0,0,0,0}
      };

      opt = getopt_long_only(argc, argv, "csp", long_options, &option_index);

      if (opt == -1) break;
      switch (opt) {
        case 's':
          start_flag = 1;
          break;
        case 'p':
          port_num = atoi(optarg);
          break;
        case 'c':
          passcode = std::string(optarg);
        case '?':
          break;
      }
  }

  if (port_num < 5000 || port_num > 7000) {
    perror("Invalid port: should be in [5000, 7000]");
    exit(EXIT_FAILURE);
  } else if (passcode.length() == 0) {
    perror("Enter a valid passcode");
    exit(EXIT_FAILURE);
  } else if (start_flag != 1) {
    perror("Please use -start.");
    exit(EXIT_FAILURE);
  }
  // End //

  // TCP  //
  host_sock = socket(AF_INET, SOCK_STREAM, 0);
  if (host_sock < 0) exit(EXIT_FAILURE);
  
  
  host.sin_family = AF_INET;
  host.sin_addr.s_addr = inet_addr("127.0.0.1");
  host.sin_port = htons(port_num);

  if (bind(host_sock, (struct sockaddr *) &host, sizeof(host)) < 0) exit(EXIT_FAILURE);

  std::cout << "Server started on port " << port_num << ". Accepting connections" << std::endl;

  std::thread connections(new_connections);
  std::thread commands(server_commands);

  connections.join();
  commands.join();

  // End //

  return 0;
}