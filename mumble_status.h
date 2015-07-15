#ifndef MUMBLE_STATUS_H
#define MUMBLE_STATUS_H

#define RQ_TYPE_BYTES 4
#define ID_BYTES 8
#define BUF_SIZE 256

void dissect_mumble_ping(unsigned char *response,
                         int *major,
                         int *minor,
                         int *patch,
                         int *ident,
                         int *users,
                         int *maxusers,
                         int *bitrate);

void compose_mumble_ping(unsigned char *request,
                         int outbound_ident);

#endif /* MUMBLE_STATUS_H */
