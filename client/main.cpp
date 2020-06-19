#include <cstdio>
#include <signal.h>
#include <stdlib.h>

#include "client.h"

bool connected = false;

void handler(int sign) {
    if (connected) {
        client_stop();
        printf("disconnected\n");
    } else {
        printf("exited\n");
        exit(0);
    }
}

int main() {
    signal(SIGINT, handler);

    // client_start(DEFAULT_SERVER_PORT);
}
