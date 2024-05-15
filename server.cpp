#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <iostream>

using namespace std;

static void msg(const char *msg) {
    cerr << msg << endl;
    fprintf(stderr, "%s\n", msg);
    //use sterr for C
}

static void die(const char * msg){
    //error number for the last system call
    int err = errno;
    cerr << "[" << err << "] " << msg << endl;
    abort();
}

static void do_something(int connfd){
    char rbuf[64] = {};
    ssize_t n = read(connfd, rbuf, sizeof(rbuf) - 1);
    if(n<0){
        msg("read() error");
        return;
    }
    else{
        cout << "client says: " << rbuf << endl;
    }

    char wbuf[] = "World!";
    write(connfd, wbuf, strlen(wbuf));
}

int32_t main(){
    //creating a socket using IPv4 and TCP and 0 is for auto selection of protocol
    int fd = socket(AF_INET, SOCK_STREAM, 0);

    // configuiring different aspects of the socket
    //RESUSEADDR server will be able to bind to the address if it's restarted
    int val = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));

    //let's bind the socket to some random address for now
    struct sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = ntohs(1234); // random port 1234 //network to host
    addr.sin_addr.s_addr = ntohl(0); // random IP 0.0.0.0
    int rv = bind(fd, (const sockaddr *)&addr, sizeof(addr));
    if(rv){
        die("bind()");
    }

    //listen
    rv = listen(fd, SOMAXCONN); //SOMAXCONN to allow maximum number of client to join at a time
    if(rv){
        die("listen()");
    }
    //loop to do something

    while(true){
        //accept
        struct sockaddr_in client_addr = {};
        socklen_t socklen = sizeof(client_addr);
        int connfd = accept(fd, (struct sockaddr *)&client_addr, &socklen);
        if(connfd<0){
            continue; //error
        }

        do_something(connfd);
        close(connfd);
    }
}