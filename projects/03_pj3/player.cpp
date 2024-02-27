#include "potato.hpp"

int main(int argc, char ** argv) {
    // check the inputs
    if (argc != 3) {
        fprintf(stderr, "The program should only take 2 command line argument!\n");
        exit(EXIT_FAILURE);
    }

    // create a host server
    Player playerHost;
    playerHost.createHost("");
    // create a client server
    Player playerClient;
    playerClient.createClient(argv[1], argv[2]);
    int host_fd = playerClient.socket_fd;

    // send the port to the ringmaster
    playerHost.port = playerHost.getPort(playerHost.socket_fd);
    send(host_fd, &playerHost.port, sizeof(playerHost.port), 0);

    // receive the num_players and id
    int num_players;
    recv(host_fd, &num_players, sizeof(num_players), 0);
    int player_id;
    recv(host_fd, &player_id, sizeof(player_id), 0);

    cout << "Connected as player " << player_id << " out of " << num_players << " total players" << endl;

    // receive prev neighbor port and ip(MSG_WAITALL?)
    int prev_nb_port;
    recv(host_fd, &prev_nb_port, sizeof(prev_nb_port), 0);
    char prev_nb_ip[100];
    recv(host_fd, &prev_nb_ip, sizeof(prev_nb_ip), 0);

    // connect to prev as client
    Player prevClient;
    // void createClient(const char * hostname, const char * port);
    prevClient.createClient(prev_nb_ip, to_string(prev_nb_port).c_str());
    int prev_nb_id = (player_id == 0) ? num_players - 1 : player_id + 1;
    int prev_nb_fd = prevClient.socket_fd;

    // wait for next to connect
    string next_nb_ip;
    int next_nb_id = (player_id + 1 == num_players) ? 0 : player_id + 1;
    int next_nb_fd = playerHost.acceptConnection(next_nb_ip);

    // if connect done, send host this player is ready
    int ready = 11111;
    send(host_fd, &ready, sizeof(ready), 0);

    // start game, ready to receive potato
    Potato my_potato;
    vector<int> nb_ids;
    nb_ids.push_back(prev_nb_id);
    nb_ids.push_back(next_nb_id);
    // nb_ids.push_back(num_players);
    vector<int> nb_fds;
    nb_fds.push_back(prev_nb_fd);
    nb_fds.push_back(next_nb_fd);
    nb_fds.push_back(host_fd);

    // waiting for the potato

    while (true) {
        // waiting for the potato
        // using SELECT
        fd_set game_fds;
        FD_ZERO(&game_fds);
        ssize_t bytes_read;
        int max_fd = 0;
        for (int i = 0; i < 3; i++) {
            FD_SET(nb_fds[i], &game_fds);
            max_fd = (nb_fds[i] > max_fd) ? nb_fds[i] : max_fd;
        }
        int game_select = select(max_fd + 1, &game_fds, NULL, NULL, NULL);
        if ((game_select < 0) && (errno != EINTR)) {
            cerr << "select error" << endl;
        }
        for (int i = 0; i < 3; i++) {
            if (FD_ISSET(nb_fds[i], &game_fds)) {
                bytes_read = recv(nb_fds[i], &my_potato, sizeof(my_potato), 0);
                break;
            }
        }

        // if host send 0 hops -> exit
        if (my_potato.hops == 0) {
            break;
        }
        // continue sending
        if (my_potato.hops != 1) {
            my_potato.hops--;
            my_potato.trace[my_potato.traceCounter] = player_id;
            my_potato.traceCounter++;

            int randomNb = rand() % 2;
            send(nb_fds[randomNb], &my_potato, sizeof(my_potato), 0);
            cout << "Sending potato to " << nb_ids[randomNb] << endl;
        }
        // game end
        else {
            my_potato.hops--;
            my_potato.trace[my_potato.traceCounter] = player_id;
            my_potato.traceCounter++;
            send(host_fd, &my_potato, sizeof(my_potato), 0);

            cout << "I'm it" << endl;
        }
    }

    return EXIT_SUCCESS;
}