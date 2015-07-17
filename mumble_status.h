#ifndef MUMBLE_STATUS_H
#define MUMBLE_STATUS_H

#define BUF_SIZE 256

struct mumble_response {
    int version_major;
    int version_minor;
    int version_patch;

    int ident;

    int current_users;
    int maximum_users;
    int bitrate;
};

void dissect_mumble_ping(unsigned char *response,
                         struct mumble_response *mr);

void compose_mumble_ping(unsigned char *request,
                         int request_length,
                         int outbound_ident);

#endif /* MUMBLE_STATUS_H */
