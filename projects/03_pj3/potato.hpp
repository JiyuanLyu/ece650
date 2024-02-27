#ifndef POTATO_HPP
#define POTATO_HPP

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>

#include <cstdio>
#include <cstdlib>
#include <vector>
#include <map>
#include <string>

using namespace std;

class Potato {
 public:
    int hops;
    int trace[512];
    size_t traceLength;

    Potato() : hops(0), trace(), traceLength(0) {};
    Potato(int hop_counter) : hops(hop_counter), trace(), traceLength(0) {};
    
    void printPotato() const {
        cout << "Trace of potato:\n";
        for (size_t i = 0; i < traceLength; i++) {
            cout << this->trace[i] << ",";
        }
        cout << endl;
    }
};

class Server {
 public:
    int status;
    int socket_fd;
    struct addrinfo host_info;
    struct addrinfo *host_info_list;
    const char * hostname;
    const char * port;

    // default constructor
    Server() : socket_fd(-1),host_info_list(nullptr) {
        memset(&host_info, 0, sizeof(host_info));
        host_info.ai_family   = AF_UNSPEC;
        host_info.ai_socktype = SOCK_STREAM;
        host_info.ai_flags    = AI_PASSIVE;
    }
    // constructor
    Server(const char * _port) : port(_port), hostname(nullptr), socket_fd(-1), status(0), host_info_list(nullptr) {
        memset(&host_info, 0, sizeof(host_info));

        host_info.ai_family   = AF_UNSPEC;
        host_info.ai_socktype = SOCK_STREAM;
        host_info.ai_flags    = AI_PASSIVE;
    }
    // destructor
    ~Server() {
        if (host_info_list != nullptr) {
            freeaddrinfo(host_info_list);
        }
        if (socket_fd != -1) {
            close(socket_fd);
        }
    }

    void createServer(const char * port);
    void sendMessage(int fd, const string & message);
    string receiveMessage(int fd);
    const char * getPort(int fd);
    // virtual void connectToServer(const char * hostname, const char * port) = 0;
};

class RingMaster : public Server {
 public:
    vector<int> player_fds;
    vector<string> player_ips;

    RingMaster() : Server() {};

    void waitForPlayers(int num_players);
    void sendNeighborInfo();
};

class Player : public Server {
 public:
    int id;
    vector<int> neighbor_fds;
    vector<string> neighbor_ips;

    Player() : Server() {};

    void connectToServer(const char * hostname, const char * port);
    void connectToNeighbors(const char * ip, const char * port);
};

#endif