#include "potato.hpp"

#include <netdb.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include <algorithm>
#include <cstring>
#include <iostream>
#include <vector>

// use this function to check the arguments validation 
void checkargv(int num_players, int num_hops) {
    if (num_players <= 1) {
        fprintf(stderr, "Number of players must be greater than 1!\n");
        exit(EXIT_FAILURE);
    }
    if (num_hops < 0 || num_hops > 512) {
        fprintf(stderr, "Number of hops must be greater than or equal to 0 and less than or equal to 512!\n");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char ** argv) {
    // check the inputs
    if (argc != 4) {
        fprintf(stderr, "The program should only take 3 command line argument!\n");
        exit(EXIT_FAILURE);
    }
    int num_players = atoi(argv[2]);
    int num_hops = atoi(argv[3]);
    checkargv(num_players, num_hops);

    // create a potato
    Potato my_potato(num_hops);

    // print basic game info
    cout << "Potato Ringmaster" << endl;
    cout << "Players = " << num_players << endl;
    cout << "Hops = " << num_hops << endl;

    // create host server
    RingMaster host;
    // cout << "create host" << endl;
    host.createHost(argv[1]);
    // cout << "create host done" << endl;
    // wait people join and send id and player_num
    // cout << "waitjoining" << endl;
    host.waitJoining(num_players);
    // cout << "waitjoining done" << endl;
    
    // get enough player, now send the neighbor
    // cout << "sendNeighbors" << endl;
    host.sendNeighbors(num_players);
    // cout << "sendNeighbors done" << endl;

    // now wait for three connected success notifications
    int counter = 0;
    int notification;
    while (true) {
        for (int i = 0; i < num_players; i++) {
            recv(host.player_fds[i], &notification, sizeof(notification), 0);
            if (notification == 11111) {
                counter++;
                cout << "Player " << i << " is ready to play" << endl;
            }
        }
        if (counter == num_players) {
            cout << "Ready to start the game, ";
            break;
        }
    }
    
    // start playing
    // // if hops = 0
    // if (my_potato.hops == 0) {
    //     cout << "Trace of potato:" << endl;
    //     for (int i = 0; i < num_players; i++) {
    //         send(host.player_fds[i], &my_potato, sizeof(my_potato), 0);
    //         cout << i;
    //         if (i != num_players-1) {
    //             cout << ",";
    //         }
    //         else {
    //             cout << endl;
    //         }
    //     }
    //     return EXIT_SUCCESS;
    // }

    // send to random player
    srand((unsigned int)time(NULL) + num_players);
    int randomStarter = rand() % num_players;
    cout << "sending potato to player " << randomStarter << endl;
    //my_potato.trace[0] = randomStarter;
    send(host.player_fds[randomStarter], &my_potato, sizeof(my_potato), 0);

    // waiting for the final potato

    // use SELECT
    fd_set final_fds;
    FD_ZERO(&final_fds);
    int max_fd = 0;
    for (int i = 0; i < num_players; i++) {
        FD_SET(host.player_fds[i], &final_fds);
        max_fd = (host.player_fds[i] > max_fd) ? host.player_fds[i] : max_fd;
    }
    int final_select = select(max_fd + 1, &final_fds, NULL, NULL, NULL);
    if ((final_select < 0) && (errno != EINTR)) {
        cerr << "select error" << endl;
    }
    // receive potato
    for (int i = 0; i < num_players; i++) {
        if (FD_ISSET(host.player_fds[i], &final_fds)) {
            // string done;
            // recv(host.player_fds[i], &done, sizeof(done), 0);
            // cout << done << endl;
            ssize_t bytes_read = recv(host.player_fds[i], &my_potato, sizeof(my_potato), 0);
            break;
        }
    }

    // if hops = 0
    if (my_potato.hops == 0) {
        for (int i = 0; i < num_players; i++) {
            send(host.player_fds[i], &my_potato, sizeof(my_potato), 0);
        }
    }

    // print message
    my_potato.printPotato();

    return EXIT_SUCCESS;
}