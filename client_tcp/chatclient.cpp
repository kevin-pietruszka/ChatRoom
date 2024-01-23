/**
 * Author: Kevin Pietruszka
 * GTID: 903497239
 * GT Email: kpietruszka3@gatech.edu
 */
#include "chatclient.h"


void server_listen() {

  while (1) {
    char server_reply[1024] = {0};
    int r = recv(sockfd, server_reply, 1024, 0);
    if (r > 0) {
      std::cout << server_reply << std::endl;
    }
  }

}

std::string read_input(std::string msg) {

  
  if (strcmp(msg.c_str(), ":)") == 0) {

    return "[feeling happy]";

  } else if (strcmp(msg.c_str(), ":(") == 0) {

    return "[feeling sad]";

  } else if (strcmp(msg.c_str(), ":mytime") == 0) {

    auto curr_time = std::chrono::system_clock::now();

    std::time_t time = std::chrono::system_clock::to_time_t(curr_time);
    
    std::string out = std::ctime(&time);

    return out.substr(0, out.length() - 1);
    
  } else if (strcmp(msg.c_str(), ":+1hr") == 0) {
    auto curr_time = std::chrono::system_clock::now();

    std::time_t time = std::chrono::system_clock::to_time_t(curr_time)+ 3600;
    std::string out = std::ctime(&time);

    return out.substr(0, out.length() - 1);

  } else if (strcmp(msg.c_str(), ":Exit") == 0) {
    
    return ":Exit";

  } else {

    return msg;
    
  }
  
}

void send_text() {

  while(1){

    std::string bufman;
    std::getline(std::cin, bufman);

    if (bufman.length() > 0) {

      std::string msg_out = read_input(bufman);
      
      if (strcmp(msg_out.c_str(), ":Exit") == 0) {
        send(sockfd, msg_out.c_str(), strlen(msg_out.c_str()), 0);
        shutdown(sockfd, 0);
        exit(EXIT_SUCCESS);
      }

      send(sockfd, msg_out.c_str(),strlen(msg_out.c_str()), 0);
      
    }
  }
}

int main(int argc, char *argv[]) {

  //Args
  joinflag = 0;
  host_name = "";
  username = "";
  passcode = "";
  port_num = 0;

  //printf("\n%d\n", argc);

  //Grab the parameters
  int opt;

  while (1) {

      int option_index = 0;
      static struct option long_options[] = {

        {"join", no_argument, 0, 'j'},
        {"host", required_argument, 0, 'h'},
        {"port", required_argument, 0, 'p'},
        {"username", required_argument, 0, 'u'},
        {"passcode", required_argument, 0, 'c'},
        {0,0,0,0}
      };

      opt = getopt_long_only(argc, argv, "hpuc", long_options, &option_index);

      if (opt == -1) break;

      switch (opt) {

        case 'j':
          joinflag = 1;
          break;
        case 'c':
          passcode = std::string(optarg);
          break;
        case 'h':
          host_name = std::string(optarg);
          break;
        case 'p':
          port_num = atoi(optarg);
          break;
        case 'u':
          username = std::string(optarg);
          break;
      }
      
  }

  //std::cout << joinflag << passcode << host_name << port_num << username << std::endl;

  if (joinflag == 0) {
    perror("Use -join");
    exit(EXIT_FAILURE);
  }else if (port_num < 5000 || port_num > 7000) {
    perror("Invalid port: should be in [5000, 7000]");
    exit(EXIT_FAILURE);
  } else if (strcmp(passcode.c_str(), "") == 0) {
    perror("No passcode was entered.");
    exit(EXIT_FAILURE);
  } else if (strcmp(username.c_str(), "") == 0) {
    perror("No username was entered.");
    exit(EXIT_FAILURE);
  }

  //start server connection

  struct sockaddr_in server_address;

  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("Failed to make socket.");
    exit(EXIT_FAILURE);
  }
  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(port_num);

  if (inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr) <= 0) {
    perror("Address is invalid");
    exit(EXIT_FAILURE);
  }

  if (connect(sockfd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
    perror("Cannot connect to server");
    exit(EXIT_FAILURE);
  }

  std::string pnu = passcode + "&" + username;

  send(sockfd, pnu.c_str(), (size_t) strlen(pnu.c_str()), 0);

  char response[1024] = {0};
  int resp = recv(sockfd, &response, (size_t) 1024, 0);

  if (resp < 0) {
    exit(EXIT_FAILURE);
  }

  if (strcmp(response, "accessdenied") == 0) {
    std::cout << "Incorrect passcode" << std::endl;
    exit(EXIT_FAILURE);
  }

  if (strcmp(response, "connected") == 0) {

    std::cout << "Connected to " << host_name << " on port " << port_num << std::endl;

  }

  std::thread new_msgs(server_listen);
  std::thread send_input(send_text);
  
  new_msgs.join();
  send_input.join();
  

  return 0;
}
