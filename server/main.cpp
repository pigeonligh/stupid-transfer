#include <cstdio>
#include <signal.h>
#include <stdlib.h>
#include <string.h>

#include "server.h"

void handler(int sign) {
    server_stop();
    printf("Close server\n");
    exit(0);
}

void print_usage() {
    puts("Usage:");
    puts("    ./server CONFIG_FILE");
    puts("or  ./server --test");
    fflush(stdout);
}

void load_config(const char *config, char *dir, int *port) {
    // TODO: load config from file
    memcpy(dir, "./data/", 1);
    *port = DEFAULT_SERVER_PORT;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        print_usage();
        return 0;
    }

    if (strcmp(argv[1], "--test") == 0) {
        server_test();
    } else {
        signal(SIGINT, handler);

        char dir[256];
        int port;

        load_config(argv[1], dir, &port);

        server_start(dir, port);
    }
}
