/* Standard library */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>
#include <time.h>

/* Types */
#include <sys/types.h>

/* Sockets */
#include <sys/socket.h>

/* Some structs for ports and adresses */
#include <netinet/in.h>
#include <netdb.h>

#include "mumble_status.h"

/**
 * Composes a mumble ping according to the mumble protocol.
 */
void compose_mumble_ping(unsigned char *request,
                         int outbound_ident)
{
    /* Clear out the rw_buffer */
    bzero(request, BUF_SIZE);

    /* Denotes the request type  */
    for (int i = 0; i < RQ_TYPE_BYTES; i++) {
        request[i] = 0;
    }

    /* Used to identify the reponse */
    for (int i = RQ_TYPE_BYTES; i < RQ_TYPE_BYTES + ID_BYTES; i++) {
        request[i] = outbound_ident >> ((RQ_TYPE_BYTES + ID_BYTES - 1) - i);
    }

}

/**
 * Dissects a mumble ping according to the mumble protocol.
 */
void dissect_mumble_ping(unsigned char *response,
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


    /* Extract the bitrate from bytes [12-19] */
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
    /* read() and write() status variable */
    int n = 0;

    /* Buffer to save the outbound and inbound traffic */
    unsigned char rw_buffer[BUF_SIZE] = {0};

    /* Check if right arguments were supplied */
    if (argc < 3) {
        printf("Usage: %s <host> <port>\n", argv[0]);
        printf("(Default mumble port is 64738)\n");
        return -1;
    }

    /* Set up server info */
    struct hostent *server = gethostbyname(argv[1]);
    int portno = atoi(argv[2]);

    struct sockaddr_in serv_addr;
    bzero((char *) &serv_addr, sizeof(serv_addr));
    bcopy((char *) server->h_addr_list[0],
          (char *) &serv_addr.sin_addr.s_addr,
          server->h_length);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(portno);

    /* Connect to the server */
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        printf("Error connecting\n");
        return -1;
    }

    /* Compose a ping according to the mumble protocol */
    compose_mumble_ping(&rw_buffer[0], clock());

    /* Send data */
    if ((n = write(sockfd, rw_buffer, RQ_TYPE_BYTES + ID_BYTES)) < 0) {
        printf("Error writing to socket\n");
        return -1;
    }

    /* Receive data */
    bzero(rw_buffer, BUF_SIZE);

    if ((n = read(sockfd, rw_buffer, 255)) < 0) {
        printf("Error reading from socket\n");
        return -1;
    }

    /* Close the connection */
    close(sockfd);

    /* Split the received data according to the mumble protocol */
    struct mumble_response mr;
    dissect_mumble_ping(&rw_buffer[0], &mr);

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
