/*
Copyright 2020 The FileTransfer Authors.

Licensed under the GNU General Public License, v3.0

See LICENSE in the project root for license information.
*/

#include <cstdio>
#include <signal.h>
#include <stdlib.h>
#include <string.h>

#include <string>

#include "client.h"
#include "packet.h"
#include "mails.h"

bool connected = false;

void process_command(const char* cmd);

void handler(int sign) {
    if (connected) {
        process_command("exit");
    }
    printf("exited\n");
    exit(0);
}

int main() {
    signal(SIGINT, handler);

    while (true) {
        if (connected) {
            char command[512];
            while (is_waiting())
                fflush(stdout);
            printf("> ");
            scanf("%s", command);
            process_command(command);
        } else {
            char ipaddr[64];
            int port;
            printf("input server address: ");
            gets(ipaddr);
            printf("input server port: ");
            scanf("%d", &port);
            getchar();

            fflush(stdin);

            if (client_start(ipaddr, port)) {
                printf("connected\n");
                connected = true;
            } else {
                printf("connect failed\n");
            }
        }
    }
}

void process_command(const char* cmd) {
    packet pack;
    memset(&pack, 0, sizeof pack);
    char param[2048];
    char temp[2048];

    if (strcmp(cmd, "exit") == 0) {
        client_stop();
        connected = false;
        printf("disconnected\n");
    } else if (strcmp(cmd, "show") == 0) {
        // show mail
        gets(param);
        int number;
        if (sscanf(param, "%d", &number) == 1) {
            show_mails(number);
        } else {
            show_mails(1);
        }
    } else if (strcmp(cmd, "showall") == 0) {
        show_all_mails();
    } else if (strcmp(cmd, "say") == 0) {
        // say
        gets(param);
        int len = strlen(param);
        pack.length = PACKET_HEADER_SIZE + len;
        pack.type = TYPE_SAY;
        memcpy(pack.data, param, len);
        send_packet(&pack);
    }
}
