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
#include<arpa/inet.h>
#include<thread>
#include<string>
#include<string.h>
#include<iostream>
#include<chrono>

int joinflag = 0;
std::string host_name = "";
std::string username = "";
std::string passcode = "";
int port_num = 0;

int sockfd;

int main(int argc, char *argv[]);
