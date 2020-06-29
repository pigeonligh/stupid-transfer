/*
Copyright 2020 The FileTransfer Authors.

Licensed under the GNU General Public License, v3.0

See LICENSE in the project root for license information.
*/

#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <dirent.h>
#include <limits.h>

#include "server.h"

void handler(int sign) {
    server_stop();
    printf("Close server\n");
    exit(0);
}

void print_usage() {
    fprintf(stderr, "This is a simple FTP server. \n\n");
    fprintf(stderr, "Usage: \n\n");
    fprintf(stderr, "    ./server [arguments] \n\n");
    fprintf(stderr, "The arguments are: \n\n");
    fprintf(stderr, "    -c file        point a config file for server \n");
    fprintf(stderr, "    -d dir         point a directory to save data \n");
    fprintf(stderr, "    -p port        point a port run this service \n");
    fprintf(stderr, "    -t             run in test mode \n");
    fprintf(stderr, "    -h             show help \n");
    fflush(stdout);
}

void set_default_config(char *dir, int *port) {
    memcpy(dir, "./data/", 8);
    *port = DEFAULT_SERVER_PORT;
}

void load_config(const char *config, char *dir, int *port) {
    FILE *file = fopen(config, "r");
    if (file != nullptr) {
        if (fscanf(file, "%s %d", dir, port) != 2) {
            fprintf(stderr, "Error: load config failed.\n\n");
            exit(1);
        }
        fclose(file);
    } else {
        fprintf(stderr, "Error: load config failed.\n\n");
        exit(1);
    }
}

int main(int argc, char **argv) {
    char opt;
    const char *optstring = "c:d:p:th";

    char dir[256];
    int port;
    bool test_mode = false;

    set_default_config(dir, &port);

    while ((opt = getopt(argc, argv, optstring)) != -1) {
        if (opt == 'c') {
            load_config(optarg, dir, &port);
        } else if (opt == 'd') {
            uint32_t len = strlen(optarg);
            memcpy(dir, optarg, len);
        } else if (opt == 'p') {
            if (sscanf(optarg, "%d", &port) != 1) {
                fprintf(stderr, "Error: get port failed.\n\n");
                return 1;
            }
        } else if (opt == 't') {
            test_mode = true;
        } else if (opt == 'h') {
            print_usage();
            return 1;
        } else {
            fprintf(stderr, "Error: unkowon parameter.\n\n");
            return 1;
        }
    }

    DIR *d = opendir(dir);
    if (d == nullptr) {
        fprintf(stderr, "Error: data directory not exist, please create it.\n\n");
        return 1;
    }
    closedir(d);

    char dir2[256];
    if (!realpath(dir, dir2)) {
        fprintf(stderr, "Error: data directory not exist, please create it.\n\n");
    }
    int len = strlen(dir2);
    dir2[len++] = '/';
    dir2[len] = 0;

    signal(SIGINT, handler);
    if (test_mode) {
        server_test(dir2, port);
    } else {
        server_start(dir2, port);
    }
}
