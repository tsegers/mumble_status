#ifndef MUMBLE_STATUS_H
#define MUMBLE_STATUS_H

#define RQ_TYPE_BYTES 4
#define ID_BYTES 8
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
                         int outbound_ident);

#endif /* MUMBLE_STATUS_H */
