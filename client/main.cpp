#include <cstdio>
#include <signal.h>
#include <stdlib.h>

#include "client.h"

bool connected = false;

void handler(int sign) {
    if (connected) {
        client_stop();
        connected = false;
        printf("disconnected\n");
    }
    printf("exited\n");
    exit(0);
}

int main() {
    signal(SIGINT, handler);

    while (true) {
        if (connected) {
            char command[512];
            while (is_waiting());
            printf("> ");
            scanf("%s", command);
            printf("command: %s\n", command);
        } else {
            char ipaddr[64];
            int port;
            printf("input server address: ");
            scanf("%s", ipaddr);
            printf("input server port: ");
            scanf("%d", &port);

            if (client_start(ipaddr, port)) {
                printf("connected\n");
                connected = true;
            } else {
                printf("connect failed\n");
            }
        }
    }
}
