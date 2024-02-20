#include "potato.hpp"

int main(int argc, char ** argv) {
    // check the inputs
    if (argc != 3) {
        fprintf(stderr, "The program should only take 2 command line argument!\n");
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}