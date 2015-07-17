#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

/* Network headers */
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include "mumble_status.h"

/**
 * Composes a mumble ping according to the mumble protocol.
 */
void compose_mumble_ping(unsigned char *request,
                         int request_length,
                         int outbound_ident)
{
    /* Clear out the rw_buffer */
    memset(request, 0, request_length);

    /* Denotes the request type  */
    for (int i = 0; i < 4; i++) {
        request[i] = 0;
    }

    /* Used to identify the reponse */
    for (int i = 4; i < 4 + 8; i++) {
        request[i] = outbound_ident >> ((4 + 8 - 1) - i);
    }

}

/**
 * Dissects a mumble pong according to the mumble protocol.
 */
void dissect_mumble_pong(unsigned char *response,
                         struct mumble_response *mr)
{
    /* Extract the version number from bits [1-3] (0 is useless) */
    mr->version_major = response[1];
    mr->version_minor = response[2];
    mr->version_patch = response[3];

    /* Extract the timestamp that was sent from bytes [4-11] */
    mr->ident = (int)(response[4]  << 7 |
                      response[5]  << 6 |
                      response[6]  << 5 |
                      response[7]  << 4 |
                      response[8]  << 3 |
                      response[9]  << 2 |
                      response[10] << 1 |
                      response[11] << 0);


    /* Extract the current and max users from bytes [12-19] */
    mr->current_users = (int)(response[12] << 24 |
                              response[13] << 16 |
                              response[14] << 8  |
                              response[15] << 0);

    mr->maximum_users = (int)(response[16] << 24 |
                              response[17] << 16 |
                              response[18] << 8  |
                              response[19] << 0);

    /* Extract the bitrate from bytes [20-23] */
    mr->bitrate = (int)(response[20] << 24 |
                        response[21] << 16 |
                        response[22] << 8  |
                        response[23] << 0) / 1000;

}

/**
 * Main function of the program.
 *
 * @param argc the number of command-line arguments the program was invoked
 *     with.
 * @param argv pointer to an array of character strings that contain the
 *     arguments.
 *
 * @returns integer denoting the status of the program at termination.
 *     0 means normal termination
 */
int main(int argc, const char *argv[])
{
    int sockfd;                         /* Socket file descriptor */
    struct addrinfo hints;              /* Hints argument of getaddrinfo() */
    struct addrinfo *getaddr_result;    /* getaddr_result of getaddrinfo() */
    struct addrinfo *serv_addr;         /* Temp copy of getaddr_result to iterate over */
    int n;                              /* Used to store the return value of getaddrinfo(), read() and write() */

    /* Buffer to save the outbound and inbound traffic */
    unsigned char rw_buffer[BUF_SIZE] = {0};

    /* Check if right arguments were supplied */
    if (argc < 3) {
        printf("Usage: %s <host> <port>\n", argv[0]);
        printf("(Default mumble port is 64738)\n");
        return -1;
    }

    /* Obtain address(es) matching host/port */
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;

    /* Look up the address info */
    if ((n = getaddrinfo(argv[1], argv[2], &hints, &getaddr_result)) != 0) {
        printf("Error getting host info\n");
        return -1;
    }

    /* Try each address until we can connect  */
    for (serv_addr = getaddr_result; serv_addr != NULL; serv_addr = serv_addr->ai_next) {
        sockfd = socket(serv_addr->ai_family, serv_addr->ai_socktype, serv_addr->ai_protocol);

        if (sockfd != -1) {
            if (connect(sockfd, serv_addr->ai_addr, serv_addr->ai_addrlen) != -1) {
                break;
            } else {
                close(sockfd);
            }
        }
    }

    if (serv_addr == NULL) {
        printf("Error connecting\n");
        return -1;
    }

    freeaddrinfo(getaddr_result);

    /* Compose a ping according to the mumble protocol */
    compose_mumble_ping(rw_buffer, BUF_SIZE, clock());

    /* Send ping */
    if ((n = write(sockfd, rw_buffer, 12)) < 0) {
        printf("Error writing to socket\n");
        return -1;
    }

    /* Receive pong */
    memset(rw_buffer, 0, BUF_SIZE);

    if ((n = read(sockfd, rw_buffer, BUF_SIZE)) < 0) {
        printf("Error reading from socket: %i\n", n);
        return -1;
    }

    /* Split the received data according to the mumble protocol */
    struct mumble_response mr;
    dissect_mumble_pong(rw_buffer, &mr);

    float ping_time = (clock() - mr.ident);

    /* Print the split data */
    printf("Version %i.%i.%i, %i/%i Users, %3.1fms, %ikbit/s\n",
           mr.version_major, mr.version_minor, mr.version_patch,
           mr.current_users, mr.maximum_users,
           ping_time,
           (int) mr.bitrate);

    /* Exit */
    return 0;
}
