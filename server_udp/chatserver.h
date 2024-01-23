/**
 * Author: Kevin Pietruszka
 * GTID: 903497239
 * GT Email: kpietruszka3@gatech.edu
 */

#pragma once

#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<unistd.h>
#include<getopt.h>
#include<netinet/in.h>
#include<string.h>
#include<thread>
#include<arpa/inet.h>
#include<map>
#include<string.h>
#include<iostream>
#include<mutex>

#define BACKLOG 24

struct client {

  int sock;
  int verified;
  std::string *name;
  sockaddr_in *address;

};
std::mutex clients_mutex;
std::map<std::string, client *> clients;
std::string passcode;
int start_flag;
int port_num;
int sockfd;
struct sockaddr_in host_address;


int main(int argc, char *argv[]);
