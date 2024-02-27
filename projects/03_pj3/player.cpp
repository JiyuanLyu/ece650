#include "potato.hpp"

int main(int argc, char ** argv) {
    // check the inputs
    if (argc != 3) {
        fprintf(stderr, "The program should only take 2 command line argument!\n");
        exit(EXIT_FAILURE);
    }

    // create a server
    Player * player;
    // connect to server
    player->connectToServer(argv[1], argv[2]);

    // wait for neighbor info from neighbor
    int num_players;
    char * prev_neighbor_port;
    string prev_neighbor_ip;
    char * next_neighbor_port;
    string next_neighbor_ip;
    // receive id
    recv(player->socket_fd, &player->id, sizeof(player->id), 0);
    // receive num_player
    recv(player->socket_fd, &num_players, sizeof(num_players), 0);
    // receive prev port
    recv(player->socket_fd, &prev_neighbor_port, sizeof(prev_neighbor_port), 0);
    // receive prev ip
    recv(player->socket_fd, &prev_neighbor_ip, sizeof(prev_neighbor_ip), 0);
    // receive next port
    recv(player->socket_fd, &next_neighbor_port, sizeof(next_neighbor_port), 0);
    // receive next ip
    recv(player->socket_fd, &next_neighbor_ip, sizeof(next_neighbor_ip), 0);

    // connect to prev neighbor
    player->connectToNeighbors(neighbor_ip.c_str(), neighbor_port);

    // let the host know the connect is done
    string connectMsg = "Neighbor connection done";
    player->sendMessage(player->socket_fd, connectMsg);
    cout << "Connected as player " << player->id << " out of " << num_players << " total players";

    return EXIT_SUCCESS;
}