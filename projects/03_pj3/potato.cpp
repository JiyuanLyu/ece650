#include "potato.hpp"

// implement Server class

void Server::createServer(const char * port) {
    status = getaddrinfo(hostname, port, &host_info, &host_info_list);
    if (status != 0) {
        cerr << "Error: cannot get address info for host" << endl;
        cerr << "  (" << hostname << "," << port << ")" << endl;
        exit(-1);
    }

    // connect SOCKET
    socket_fd = socket(host_info_list->ai_family, 
                        host_info_list->ai_socktype,
                        host_info_list->ai_protocol);
    if (socket_fd == -1) {
        cerr << "Error: cannot create socket" << endl;
        cerr << "  (" << hostname << "," << port << ")" << endl;
        exit(-1);
    }

    // bind
    int yes = 1;
    status = setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
    status = bind(socket_fd, host_info_list->ai_addr, host_info_list->ai_addrlen);
    if (status == -1) {
        cerr << "Error: cannot bind socket" << endl;
        cerr << "  (" << hostname << "," << port << ")" << endl;
        exit(-1);
    }

    // start listen
    status = listen(socket_fd, 100);
    if (status == -1) {
        cerr << "Error: cannot listen on socket" << endl; 
        cerr << "  (" << hostname << "," << port << ")" << endl;
        exit(-1);
    }
}

void Server::sendMessage(int fd, const string & message) {
    int numbytes = send(fd, message.c_str(), message.length(), 0);
    if (numbytes == -1) {
        cerr << "Error: RingMaster cannot send message" << endl;
    }
}

string Server::receiveMessage(int fd) {
    char buffer[512];
    ssize_t bytes_received = recv(socket_fd, buffer, sizeof(buffer) - 1, 0);
    if (bytes_received == -1) {
        cerr << "Error: RingMaster cannot receive message" << endl;
        return "";
    }
    buffer[bytes_received] = '\0';
    return string(buffer);
}

const char * getPort(int fd) {
    struct sockaddr_in sin;
    socklen_t len = sizeof(sin);
    if (getsockname(fd, (struct sockaddr *) &sin, &len) == -1) {
        cerr << "Error: cannot getsockname" << endl;
        exit(EXIT_FAILURE);
    }
    int port = ntohs(sin.sin_port);
    string portStr = to_string(port);
    const char * portChar = portStr.c_str();
    return portChar;
}

// implement RingMaster class

void RingMaster::waitForPlayers(int num_players) {
    struct sockaddr_storage socket_addr;
    socklen_t socket_addr_len = sizeof(socket_addr);
    int client_connection_fd;
    for (int i = 0; i < num_players; i++) {
        client_connection_fd = accept(socket_fd, (struct sockaddr *)&socket_addr, &socket_addr_len);
        if (client_connection_fd == -1) {
            // cerr << "Error: RingMaster cannot accept connection on socket" << endl;
            perror("Error: RingMaster cannot accept connection on socket");
            continue;
        }
        struct sockaddr_in * addr = (struct sockaddr_in *) & socket_addr;
        // string player_ip = inet_ntoa(addr->sin_addr);
        player_fds.push_back(client_connection_fd);
        player_ips.push_back(inet_ntoa(addr->sin_addr));
    }
}

void RingMaster::sendNeighborInfo() {
    int num_players = player_fds.size();

    for (int i = 0; i < num_players; ++i) {
        int prev = (i - 1 + num_players) % num_players;
        int next = (i + 1) % num_players;

        // send id
        send(player_fds[i], &i, sizeof(i), 0);
        // send player_num
        send(player_fds[i], &num_players, sizeof(num_players), 0);
        // send prev port
        const char * prev_port = getPort(player_fds[prev]);
        send(player_fds[i], &prev_port, sizeof(prev_port), 0);
        // send prev ip
        string prev_ip = player_ips[prev];
        send(player_fds[i], &prev_ip, sizeof(prev_ip), 0);
        // send next port
        const char * next_port = getPort(player_fds[next]);
        send(player_fds[i], &next_port, sizeof(next_port), 0);
        // send next ip
        string next_ip = player_ips[next];
        send(player_fds[i], &next_ip, sizeof(next_ip), 0);
    }
}


// implementing Player class

void Player::connectToServer(const char * hostname, const char * port) {
    int connect_status = getaddrinfo(hostname, port, &host_info, &host_info_list);
    if (connect_status != 0) {
        cerr << "Error: Player cannot get address info for host" << endl;
        cerr << "  (" << hostname << "," << port << ")" << endl;
        exit(-1);
    }

    socket_fd = socket(host_info_list->ai_family, 
                       host_info_list->ai_socktype, 
                       host_info_list->ai_protocol);
    if (socket_fd == -1) {
        cerr << "Error: Player cannot create socket" << endl;
        cerr << "  (" << hostname << "," << port << ")" << endl;
        exit(-1);
    }

    connect_status = connect(socket_fd, host_info_list->ai_addr, host_info_list->ai_addrlen);
    if (connect_status == -1) {
        cerr << "Error: Player cannot connect to socket" << endl;
        cerr << "  (" << hostname << "," << port << ")" << endl;
        exit(-1);
    }
}

void Player::connectToNeighbors(const char * ip, const char * port) {
    int portNum = atoi(port);

    struct sockaddr_in neighbor_addr;
    memset(&neighbor_addr, 0, sizeof(neighbor_addr));
    neighbor_addr.sin_family = AF_INET;
    neighbor_addr.sin_port = htons(portNum);

    int result = inet_pton(AF_INET, ip, &neighbor_addr.sin_addr);
    if (result <= 0) {
        if (result == 0) {
            cerr << "Error: Player cannot convert neighbor ip" << endl;
        }
        else {
            perror("inet_pton");
        }
        exit(1);
    }

    int neighbor_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (neighbor_fd == -1) {
        cerr << "Error: Player cannot create socket with neighbor" << endl;
        exit(1);
    }

    int status = connect(neighbor_fd, (struct sockaddr *)&neighbor_addr, sizeof(neighbor_addr));
    if (status == -1) {
        cerr << "Error: Player cannot connect to socket with neighbor" << endl;
        close(neighbor_fd);
        exit(1);
    }

    neighbor_fds.push_back(neighbor_fd);
    neighbor_ips.push_back(ip);
}