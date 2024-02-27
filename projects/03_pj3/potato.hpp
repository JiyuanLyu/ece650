#ifndef POTATO_HPP
#define POTATO_HPP

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <sys/select.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>

#include <cstdio>
#include <cstdlib>
#include <vector>
#include <map>
#include <string>
#include <queue>
#include <random>

using namespace std;

class Potato {
 public:
    int hops;
    int trace[512];
    size_t traceCounter;

    Potato() : hops(0), trace(), traceCounter(0) {};
    Potato(int hop_counter) : hops(hop_counter), trace(), traceCounter(0) {};
    
    void printPotato() const {
        cout << "Trace of potato:\n";
        for (size_t i = 0; i <= traceCounter; i++) {
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
    struct addrinfo * host_info_list;
    const char * hostname = NULL;
    int port;

    // default constructor
    Server() : socket_fd(-1),host_info_list(nullptr) {}
    // constructor
    Server(int _port) : port(_port), hostname(nullptr), socket_fd(-1), status(0), host_info_list(nullptr) {}
    // destructor
    ~Server() {
        if (host_info_list != nullptr) {
            freeaddrinfo(host_info_list);
        }
        if (socket_fd != -1) {
            close(socket_fd);
        }
    }

    void createHost(const char * port);
    void createClient(const char * hostname, const char * port);
    int acceptConnection(string ip);
    int getPort(int fd);
};

class RingMaster : public Server {
 public:
    vector<int> player_fds;
    vector<int> player_ports;
    vector<string> player_ips;

    RingMaster() : Server() {};
    ~RingMaster() {
        for (size_t i = 0; i < player_fds.size(); i++) {
            if (player_fds[i] != -1) {
                close(player_fds[i]);
            }
        }
    }

    void waitJoining(int num_players);
    void sendNeighbors(int num_players);
};

class Player : public Server {
 public:
    vector<int> neighbor_fds;
    vector<int> neighbor_ports;
    vector<string> neighbor_ips;

    Player() : Server() {};
    ~Player() {
        for (size_t i = 0; i < neighbor_fds.size(); i++) {
            if (neighbor_fds[i] != -1) {
                close(neighbor_fds[i]);
            }
        }
    }
};

#endif