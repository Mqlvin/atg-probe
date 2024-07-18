#include "socket.h"
#include "transpiler.h"

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#define CODE_TANK_INV "\x01i20100"


bool validIp(char *str) {
    struct sockaddr_in sa;
    return inet_pton(AF_INET, str, &(sa.sin_addr)) != 0;
}

int main(int argc, char **argv) {
    if(argc < 2) {
        printf("No arguments supplied. Usage:\n %s [ip] <port, default 10001>\n", argv[0]);
	exit(1);
    } else if (argc == 2 && !strcmp(argv[1], "-h") || !strcmp(argv[1], "--help")) {
	printf("Usage:\n %s [ip] <port, default 10001>\n", argv[0]);
	exit(1);
    } else if(!validIp(argv[1])) {
	printf("Invalid IP address '%s'. Usage:\n %s [ip] <port, default 10001>\n", argv[1], argv[0]);
	exit(1);
    } else if(argc == 3 && !atoi(argv[2])) {
        printf("Invalid port '%s'. Usage:\n %s [ip] <port, default 10001>\n", argv[2], argv[0]);
	exit(1);
    }

    int sock_tcp;
    sock_tcp = socket(AF_INET, SOCK_STREAM, 0);
    
    int port = 10001;
    if(argc == 3) port = atoi(argv[2]);
    if(port < 1 || port > 65535) {
        printf("Invalid port '%s'. Usage:\n %s [ip] <port, default 10001>\n", argv[2], argv[0]);
	exit(1);
    }
    connect_socket(sock_tcp, port, inet_addr(argv[1]));

    char inst[8] = CODE_TANK_INV;
    send_atg_inst(sock_tcp, inst);
    
    char recv_buf[RECV_BUF_SIZE];
    read_atg(sock_tcp, recv_buf);

    close(sock_tcp);

    transpile(recv_buf, inst);

    printf("File exported ./intank.md\n");

    return 0;
}
