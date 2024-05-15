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

int32_t main(){
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd<0){
        die("socket()");
    }

    struct sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = ntohs(1234);
    addr.sin_addr.s_addr = ntohl(INADDR_LOOPBACK); //127.0.0.1
    int rv = connect(fd, (const struct sockaddr *)&addr, sizeof(addr));
    if(rv){
        die("connect()");
    }
    char msg[] = "Hello";
    write(fd, msg, strlen(msg));
    char rbuf[64] = {};
    ssize_t n = read(fd, rbuf, sizeof(rbuf) - 1);
    if(n<0){
        die("read()");
    }
    cout << "server says: " << rbuf << endl;
    close(fd);
}