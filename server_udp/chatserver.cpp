/**
 * Author: Kevin Pietruszka
 * GTID: 903497239
 * GT Email: kpietruszka3@gatech.edu
 */

#include "chatserver.h"

void send_to_all(std::string sender ,std::string msg) {

  clients_mutex.lock();
  auto it = clients.begin();
  while (it != clients.end()) {

    std::string e = *it->second->name;

    if (strcmp(sender.c_str(), e.c_str()) != 0) {
      client *temp = it->second;
      sendto(sockfd, msg.c_str(), msg.length(), 0, 
                (sockaddr *) temp->address, sizeof(*temp->address));
    }
    it++;
  }
  clients_mutex.unlock();

}

void server_listen(){

  while (1) {

    sockaddr_in *client_address = new sockaddr_in();
    char bufman[1024] = {0};
    int len = sizeof(*client_address);

    int r = recvfrom(sockfd, bufman, (size_t)1024, 0,
              (sockaddr *)client_address, (socklen_t *)&len);


    if (r < 0) {
      perror("Receiving failed");
      exit(EXIT_FAILURE);
    }

    std::string msg = std::string(bufman);
    int divider = msg.find("~");
    std::string name = msg.substr(0,divider);

    int exist = clients.count(name);

    if (exist > 0) {

      clients_mutex.lock();
      client *temp = clients[name];
      clients_mutex.unlock();

      std::string send = msg.substr(divider + 1, msg.length());


      if (strcmp(send.c_str(), ":Exit") == 0) {

        clients_mutex.lock();
        clients.erase(name);
        clients_mutex.unlock();

        std::cout << *temp->name << " left the chatroom" << std::endl;
        send_to_all(*temp->name, *temp->name + " left the chatroom");


      } else {

        std::cout << *temp->name << ": " << send << std::endl;
        send_to_all(*temp->name, *temp->name + ": " + send);
        
      }
      

    } else { //User does not exist yet

      std::string pc = msg.substr(divider + 1, msg.length());

      client *new_client = new client();

      new_client->name = new std::string(name);
      new_client->address = client_address;
      new_client->verified = 1;
      new_client->sock = sockfd;

      if (strcmp(passcode.c_str(), pc.c_str()) == 0) {
        
        clients_mutex.lock();
        clients[name] = new_client;
        clients_mutex.unlock();

        sendto(sockfd, "connected", 10, 0, (sockaddr *) new_client->address, (socklen_t) (sizeof(*new_client->address)));
  
        std::cout << *new_client->name << " joined the chatroom" << std::endl;
        send_to_all(*new_client->name,  *new_client->name + " joined the chatroom");

      } else {
        
        sendto(sockfd, "accessdenied", 13, 0, (sockaddr *) new_client->address, (socklen_t) (sizeof(*new_client->address)));
        
      }

    }
    

  }

}

void server_commands() {

  while(1) {
    std::string bufman;
    std::getline(std::cin, bufman);

    if (strcmp(bufman.c_str(), "listclients") == 0) {
      clients_mutex.lock();
      auto it = clients.begin();
      while (it != clients.end()) {
        std::cout << it->first << " ";
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

  if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0) ) < 0) {
    perror("Error creating socket");
    exit(EXIT_FAILURE);
  }

  host_address.sin_family = AF_INET;
  host_address.sin_addr.s_addr = inet_addr("127.0.0.1");
  host_address.sin_port = htons(port_num);

  if ((bind(sockfd, (sockaddr *) &host_address, sizeof(host_address))) < 0) {
    perror("Failure to bind socket");
    exit(EXIT_FAILURE);
  }

  std::cout << "Server started on port " << port_num << ". Accepting connections" << std::endl;

  std::thread connections(server_listen);
  std::thread commands(server_commands);

  connections.join();
  commands.join();


  return 0;
}
