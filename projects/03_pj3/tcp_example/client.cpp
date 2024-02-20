// #include <iostream>
// #include <cstring>
// #include <sys/socket.h>
// #include <netdb.h>
// #include <unistd.h>

// using namespace std;

// int main(int argc, char *argv[])
// {
//   int status;
//   int socket_fd;
//   struct addrinfo host_info;
//   struct addrinfo *host_info_list;
//   const char *hostname = argv[1];
//   const char *port     = "4444";
  
//   if (argc < 2) {
//       cout << "Syntax: client <hostname>\n" << endl;
//       return 1;
//   }

//   memset(&host_info, 0, sizeof(host_info));
//   host_info.ai_family   = AF_UNSPEC;
//   host_info.ai_socktype = SOCK_STREAM;

//   status = getaddrinfo(hostname, port, &host_info, &host_info_list);
//   if (status != 0) {
//     cerr << "Error: cannot get address info for host" << endl;
//     cerr << "  (" << hostname << "," << port << ")" << endl;
//     return -1;
//   } //if

//   socket_fd = socket(host_info_list->ai_family, 
// 		     host_info_list->ai_socktype, 
// 		     host_info_list->ai_protocol);
//   if (socket_fd == -1) {
//     cerr << "Error: cannot create socket" << endl;
//     cerr << "  (" << hostname << "," << port << ")" << endl;
//     return -1;
//   } //if
  
//   cout << "Connecting to " << hostname << " on port " << port << "..." << endl;
  
//   status = connect(socket_fd, host_info_list->ai_addr, host_info_list->ai_addrlen);
//   if (status == -1) {
//     cerr << "Error: cannot connect to socket" << endl;
//     cerr << "  (" << hostname << "," << port << ")" << endl;
//     return -1;
//   } //if

//   const char *message = "hi there!";
//   send(socket_fd, message, strlen(message), 0);

//   freeaddrinfo(host_info_list);
//   close(socket_fd);

//   return 0;
// }


#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

using namespace std;

int main(int argc, char *argv[])
{
    int status;
    int socket_fd;
    struct addrinfo host_info;
    struct addrinfo *host_info_list;
    const char *hostname = argv[1];
    const char *port = "4444";

    if (argc < 2) {
        cout << "Syntax: client <hostname>\n" << endl;
        return 1;
    }

    memset(&host_info, 0, sizeof(host_info));
    host_info.ai_family = AF_UNSPEC;
    host_info.ai_socktype = SOCK_STREAM;

    status = getaddrinfo(hostname, port, &host_info, &host_info_list);
    if (status != 0) {
        cerr << "Error: cannot get address info for host" << endl;
        cerr << "  (" << hostname << "," << port << ")" << endl;
        return -1;
    } //if

    socket_fd = socket(host_info_list->ai_family, 
                       host_info_list->ai_socktype, 
                       host_info_list->ai_protocol);
    if (socket_fd == -1) {
        cerr << "Error: cannot create socket" << endl;
        cerr << "  (" << hostname << "," << port << ")" << endl;
        return -1;
    } //if
  
    cout << "Connecting to " << hostname << " on port " << port << "..." << endl;
  
    status = connect(socket_fd, host_info_list->ai_addr, host_info_list->ai_addrlen);
    if (status == -1) {
        cerr << "Error: cannot connect to socket" << endl;
        cerr << "  (" << hostname << "," << port << ")" << endl;
        return -1;
    } //if

    const char *message = "hi there!";
    //vector<int> m = {1, 2};
    send(socket_fd, message, strlen(message), 0);

    // Additions start here
    char buffer[512];
    //int buffer[512];
    ssize_t bytes_received = recv(socket_fd, buffer, sizeof(buffer) - 1, 0);
    if (bytes_received == -1) {
        cerr << "Error: cannot receive message from server" << endl;
    } else {
        buffer[bytes_received] = '\0'; // Null-terminate the received data
        cout << "Client received: " << buffer << endl;
    }
    // Additions end here

    freeaddrinfo(host_info_list);
    close(socket_fd);

    return 0;
}