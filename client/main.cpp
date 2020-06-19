#include <cstdio>
#include <signal.h>
#include <stdlib.h>
#include <string.h>

#include "client.h"
#include "packet.h"

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
            gets(command);
            process_command(command);
        } else {
            char ipaddr[64];
            int port;
            printf("input server address: ");
            gets(ipaddr);
            printf("input server port: ");
            scanf("%d", &port);

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
    if (strcmp(cmd, "exit") == 0) {
        client_stop();
        connected = false;
        printf("disconnected\n");
    } else {
        int len = strlen(cmd);
        if (len <= 0) {
            return;
        }
        packet pack;
        pack.length = PACKET_HEADER_SIZE + len;
        pack.type = TYPE_REQUEST;
        memcpy(pack.data, cmd, 512);
        send_packet(&pack);
    }
}
