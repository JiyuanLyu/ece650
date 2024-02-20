#include "potato.hpp"

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
    int num_players = atoi(argv[3]);
    int num_hops = atoi(argv[4]);
    checkargv(num_players, num_hops);

    // create a potato
    Potato my_potato(num_hops);

    // print basic game info
    cout << "Potato Ringmaster\n" << "Players = " << num_players << "\n";
    cout << "Hops = " << num_hops << "\n" << endl;

    //

    return EXIT_SUCCESS;
}