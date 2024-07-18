#include "socket.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void connect_socket(int sock, int port, unsigned int addr) {
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = addr;

    int conn = connect(sock, (struct sockaddr *) &server_addr, sizeof(server_addr));

    if(conn == 0) {
        printf("Connected to ATG successfully (code %d)\n", conn);
    } else {
        printf("Connection error, ATG may be offline (%d)\n", conn);
	    exit(1);
    }
}


void send_atg_inst(int sock, char inst[8]) {
    ssize_t send_status;
    send_status = send(sock, (void *) inst, strlen(inst), 0);

    if(send_status == -1) {
        printf("Error sending instruction (%d)\n", send_status);
	    exit(1);
    } else {
        printf("Instruction sent (%d bytes)\n", send_status);
    }
}

void read_atg(int sock, char buf[RECV_BUF_SIZE]) {
    int buf_size = 0; // size of received, incremental

    while(1) {
        ssize_t recv_size;
        recv_size = recv(sock, buf + buf_size, RECV_BUF_SIZE - buf_size, 0);

        if(recv_size < 1) {
            printf("Error receiving data (%d)\n", recv_size);
	        exit(1);
        } 

        buf_size += recv_size;

	    if(buf_size > RECV_BUF_SIZE) {
            printf("Read buffer out of memory (%d/%d)\n", buf_size, RECV_BUF_SIZE);
            exit(1);
        }

        if(buf_size > 7 && *(buf + buf_size - 7) == '&' && *(buf + buf_size - 6) == '&') { // if termination chars present
           break;
        }
    }
    
    *(buf + buf_size - 1) = '\0';

    printf("ATG responded (%d bytes)\n", buf_size);
}
