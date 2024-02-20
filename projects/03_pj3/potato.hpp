#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

using namespace std;

class Potato {
 public:
    int hops;
    int trace[512];
    size_t traceLength;

//  private:
    Potato() : hops(0), trace(), traceLength(0) {};
    Potato(int hop_counter) : hops(hop_counter), trace(), traceLength(0) {};
    // is the rule of three needed here?
    // Potato & operator=(const Potato & rhs) {
    //     if (this != &rhs) {
    //         this->hops = rhs.hops;
    //         this->traceLength = rhs.traceLength;
    //         for (size_t i = 0; i < traceLength; i++) {
    //             this->trace[i] = rhs.trace[i];
    //         }
    //     }
    // }
    
    void printPotato() const {
        cout << "Trace of potato:\n";
        for (size_t i = 0; i < traceLength; i++) {
            cout << this->trace[i] << ",";
        }
        cout << endl;
    }
};