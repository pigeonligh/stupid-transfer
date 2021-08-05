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
#include "lock.h"

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

void show_help() {
    printf("this is help!\n");
}

void process_command(const char* cmd) {
    packet pack;
    memset(&pack, 0, sizeof pack);
    char param[2048];
    char temp[2048], temp2[2048];

    if (strcmp(cmd, "exit") == 0) {
        client_stop();
        exit(0);
    } else if (strcmp(cmd, "show") == 0) {
        gets(param);
        set_lock();
        int number;
        if (sscanf(param, "%d", &number) == 1) {
            show_mails(number);
        } else {
            show_mails(1);
        }
        unset_lock();
    } else if (strcmp(cmd, "showall") == 0) {
        gets(param);
        set_lock();
        show_all_mails();
        unset_lock();
    } else if (strcmp(cmd, "say") == 0) {
        gets(param);
        set_lock();
        int len = strlen(param);
        pack.length = PACKET_HEADER_SIZE + len;
        pack.type = TYPE_SAY;
        memcpy(pack.data, param, len);
        send_packet(&pack);
        unset_lock();
    } else if (strcmp(cmd, "ls") == 0) {
        // ls
        gets(param);
        set_lock();
        set_status(STATUS_TRANSFERING);
        pack.length = PACKET_HEADER_SIZE + 4;
        pack.type = TYPE_REQUEST;
        packet_data *data = (packet_data*) pack.data;
        data->option = REQUEST_LS;
        set_command(data->option);
        send_packet(&pack);
        unset_lock();
    } else if (strcmp(cmd, "cd") == 0) {
        // cd
        gets(param);
        set_lock();
        if (sscanf(param, "%s", temp) == 1) {
            set_status(STATUS_WAITING);
            int len = strlen(temp);
            pack.length = PACKET_HEADER_SIZE + 4 + len;
            pack.type = TYPE_REQUEST;
            packet_data *data = (packet_data*) pack.data;
            data->option = REQUEST_CD;
            set_command(data->option);
            memcpy(data->data, temp, len);
            send_packet(&pack);
        } else {
            printf("command error\n");
        }
        unset_lock();
    } else if (strcmp(cmd, "rm") == 0) {
        // rm
        gets(param);
        set_lock();
        if (sscanf(param, "%s", temp) == 1) {
            set_status(STATUS_WAITING);
            int len = strlen(temp);
            pack.length = PACKET_HEADER_SIZE + 4 + len;
            pack.type = TYPE_REQUEST;
            packet_data *data = (packet_data*) pack.data;
            data->option = REQUEST_RM;
            set_command(data->option);
            memcpy(data->data, temp, len);
            send_packet(&pack);
        } else {
            printf("command error\n");
        }
        unset_lock();
    } else if (strcmp(cmd, "pwd") == 0) {
        // pwd
        gets(param);
        set_lock();
        set_status(STATUS_WAITING);
        pack.length = PACKET_HEADER_SIZE + 4;
        pack.type = TYPE_REQUEST;
        packet_data *data = (packet_data*) pack.data;
        data->option = REQUEST_PWD;
        set_command(data->option);
        send_packet(&pack);
        unset_lock();
    } else if (strcmp(cmd, "rmdir") == 0) {
        // rmkdir
        gets(param);
        set_lock();
        if (sscanf(param, "%s", temp) == 1) {
            set_status(STATUS_WAITING);
            int len = strlen(temp);
            pack.length = PACKET_HEADER_SIZE + 4 + len;
            pack.type = TYPE_REQUEST;
            packet_data *data = (packet_data*) pack.data;
            data->option = REQUEST_RMDIR;
            set_command(data->option);
            memcpy(data->data, temp, len);
            send_packet(&pack);
        } else {
            printf("command error\n");
        }
        unset_lock();
    } else if (strcmp(cmd, "mkdir") == 0) {
        // mkdir
        gets(param);
        set_lock();
        if (sscanf(param, "%s", temp) == 1) {
            set_status(STATUS_WAITING);
            int len = strlen(temp);
            pack.length = PACKET_HEADER_SIZE + 4 + len;
            pack.type = TYPE_REQUEST;
            packet_data *data = (packet_data*) pack.data;
            data->option = REQUEST_MKDIR;
            set_command(data->option);
            memcpy(data->data, temp, len);
            send_packet(&pack);
        } else {
            printf("command error\n");
        }
        unset_lock();
    } else if (strcmp(cmd, "upload") == 0) {
        // upload
        gets(param);
        set_lock();
        if (sscanf(param, "%s %s", temp, temp2) == 2) {
            set_status(STATUS_TRANSFERING);
            set_filename(temp);
            int len = strlen(temp2);
            pack.length = PACKET_HEADER_SIZE + 4 + len;
            pack.type = TYPE_REQUEST;
            packet_data *data = (packet_data*) pack.data;
            data->option = REQUEST_UPLOAD;
            set_command(data->option);
            memcpy(data->data, temp2, len);
            send_packet(&pack);
        } else {
            printf("command error\n");
        }
        unset_lock();
    } else if (strcmp(cmd, "download") == 0) {
        // download
        gets(param);
        set_lock();
        if (sscanf(param, "%s %s", temp, temp2) == 2) {
            set_status(STATUS_TRANSFERING);
            set_filename(temp2);
            int len = strlen(temp);
            pack.length = PACKET_HEADER_SIZE + 4 + len;
            pack.type = TYPE_REQUEST;
            packet_data *data = (packet_data*) pack.data;
            data->option = REQUEST_DOWNLOAD;
            set_command(data->option);
            memcpy(data->data, temp, len);
            send_packet(&pack);
        } else {
            printf("command error\n");
        }
        unset_lock();
    } else if (strcmp(cmd, "help") == 0) {
        gets(param);
        show_help();
    } else {
        printf("command error\n");
    }
}
