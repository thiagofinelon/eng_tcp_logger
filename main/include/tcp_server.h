#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#define PORT 8001
#define KEEPALIVE_IDLE 5
#define KEEPALIVE_INTERVAL 5
#define KEEPALIVE_COUNT 3


void tcp_server(void *args);
#endif