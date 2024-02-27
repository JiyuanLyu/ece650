// #include "potato.hpp"
#include "potato.hpp"

// implement Server class

void Server::createHost(const char * port) {
    int status;

    memset(&host_info, 0, sizeof(host_info));
    host_info.ai_family = AF_UNSPEC;
    host_info.ai_socktype = SOCK_STREAM;
    host_info.ai_flags = AI_PASSIVE;
    
    status = getaddrinfo(hostname, port, &host_info, &host_info_list);
    if (status != 0) {
        cerr << "Error: cannot get address info for host" << endl;
        cerr << "  (" << hostname << "," << port << ")" << endl;
        ::exit(EXIT_FAILURE);
    }

    // connect SOCKET
    socket_fd = socket(host_info_list->ai_family, 
                        host_info_list->ai_socktype,
                        host_info_list->ai_protocol);
    if (socket_fd == -1) {
        cerr << "Error: cannot create socket" << endl;
        cerr << "  (" << hostname << "," << port << ")" << endl;
        ::exit(EXIT_FAILURE);
    }

    // bind
    int yes = 1;
    status = setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
    if (status == -1) {
        cerr << "Error: setsockopt" << endl;
        ::exit(EXIT_FAILURE);
    }
    status = bind(socket_fd, host_info_list->ai_addr, host_info_list->ai_addrlen);
    if (status == -1) {
        cerr << "Error: cannot bind socket: " << strerror(errno) << endl;
        cerr << "  (" << hostname << "," << port << ")" << endl;
        ::exit(EXIT_FAILURE);
    }

    // start listen
    status = listen(socket_fd, 100);
    if (status == -1) {
        cerr << "Error: cannot listen on socket" << endl; 
        cerr << "  (" << hostname << "," << port << ")" << endl;
        ::exit(EXIT_FAILURE);
    }
    freeaddrinfo(host_info_list);
}

void Server::createClient(const char * hostname, const char * port) {
    memset(&host_info, 0, sizeof(host_info));
    host_info.ai_family   = AF_UNSPEC;
    host_info.ai_socktype = SOCK_STREAM;
    int status;

    // hostname = NULL;
    status = getaddrinfo(hostname, port, &host_info, &host_info_list);
    if (status != 0) {
        cerr << "Error: cannot get address info for host" << endl;
        cerr << "  (" << hostname << "," << port << ")" << endl;
        ::exit(EXIT_FAILURE);
    }
    socket_fd = socket(host_info_list->ai_family, 
                host_info_list->ai_socktype, 
                host_info_list->ai_protocol);
    if (socket_fd == -1) {
        cerr << "Error: cannot create socket" << endl;
        cerr << "  (" << hostname << "," << port << ")" << endl;
        ::exit(EXIT_FAILURE);
    }
    
    status = connect(socket_fd, host_info_list->ai_addr, host_info_list->ai_addrlen);
    if (status == -1) {
        cerr << "Error: cannot connect to socket: " << strerror(errno) << endl;
        cerr << "  (" << hostname << "," << port << ")" << endl;
        ::exit(EXIT_FAILURE);
    }
    freeaddrinfo(host_info_list);
}

int Server::acceptConnection(string ip) {
    struct sockaddr_storage socket_addr;
    socklen_t socket_addr_len = sizeof(socket_addr);
    int client_connection_fd;
    client_connection_fd = accept(socket_fd, (struct sockaddr *)&socket_addr, &socket_addr_len);
    if (client_connection_fd == -1) {
        cerr << "Error: cannot accept connection on socket" << endl;
        ::exit(EXIT_FAILURE);
    }
    return client_connection_fd;
}

int Server::getPort(int fd) {
    struct sockaddr_in sin;
    socklen_t len = sizeof(sin);
    if (getsockname(fd, (struct sockaddr *) &sin, &len) == -1) {
        cerr << "Error: cannot getsockname" << endl;
        ::exit(EXIT_FAILURE);
    }
    int port = ntohs(sin.sin_port);
    return port;
}

void RingMaster::waitJoining(int num_players) {
    int player_port;
    int player_join_fd;
    string player_ip;

    // wait for port
    for (int i = 0; i < num_players; i++) {
        player_join_fd = acceptConnection(player_ip);

        recv(player_join_fd, &player_port, sizeof(player_port), 0);
        player_fds.push_back(player_join_fd);
        player_ports.push_back(player_port);
        player_ips.push_back(player_ip);

        send(player_join_fd, &num_players, sizeof(num_players), 0);
        send(player_join_fd, &i, sizeof(i), 0);
    }
}

void RingMaster::sendNeighbors(int num_players) {
    for (int i = 0; i < num_players; i++) {
        // find the index for the last neighbor
        int prev_nb = (i - 1 + num_players) % num_players;

        // send prev port
        int prev_nb_port = player_ports[prev_nb];
        send(player_fds[i], &prev_nb_port, sizeof(prev_nb_port), 0);

        // send prev ip
        char prev_nb_ip[100];
        memset(prev_nb_ip, 0, sizeof(prev_nb_ip));
        strcpy(prev_nb_ip, player_ips[prev_nb].c_str());
        send(player_fds[i], &prev_nb_ip, sizeof(prev_nb_ip), 0);
    }
}