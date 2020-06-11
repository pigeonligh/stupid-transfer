#include <cstdio>

#include "server.h"

int main() {
    printf("this is server.\n");

    server_start(DEFAULT_SERVER_PORT);
}
