#include <cstdio>
#include <signal.h>
#include <stdlib.h>

#include "server.h"

void handler(int sign) {
    server_close();
    printf("Close server\n");
    exit(0);
}

int main() {
    signal(SIGINT, handler);

    server_start(DEFAULT_SERVER_PORT);
}
