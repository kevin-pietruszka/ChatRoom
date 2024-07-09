# Simple Chat Application

## Overview

This project implements a simple chat application with a server and client component. It allows multiple users to connect to a central server and exchange messages in a chatroom-like environment.

## Components

1. Chat Server (UDP/TCP)
2. Chat Client (UDP/TCP)

## Features

- Multi-client support
- Passcode-protected server access
- Real-time message broadcasting
- Special client commands for expressing emotions and time-related messages
- Server-side client listing

## Requirements

- C++11 compatible compiler
- POSIX-compliant system (Linux, macOS, etc.)
- pthread library

## Compilation
`make`

## Usage

### Start Server
`./chatserver_[tcp/udp] --start --port <port_number> --passcode <passcode>`
- port_number: Must be between 5000 and 7000
- passcode: Required for client authentication
### Join Client
`./chatclient_[tcp/udp] --join --host <hostname> --port <port_number> --username <username> --passcode <passcode>`
- hostname: Usually "localhost" or "127.0.0.1" for local testing
- port_number: Same as the server's port
- username: Your desired chat username
- passcode: Same as the server's passcode
