#include "keepalive.h"
#include "connection_info.h"
#include "packet.h"
#include "lock.h"

#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>

pthread_t thread;

void send_ka(connection_info* ci) {
    if (time(nullptr) - ci->secs > DEAD_TIME) {
        printf("client %d disconnected\n", ci->fd);
        close_connection(ci->fd);
    } else {
        ci->count--;
        if (ci->count <= 0) {
            ci->count = ALIVE_TIME;

            // send keepalive
            packet pack;
            pack.length = PACKET_HEADER_SIZE;
            pack.type = KEEPALIVE;
            send(ci->fd, &pack, pack.length, 0);
        }
    }
}

void* keepalive(void *obj) {
    while (true) {
        // printf("keep alive\n");
        sleep(1);
        set_lock();
        for (connection_info *ci = first_connection(); ci != nullptr; ci = next_connection()) {
            send_ka(ci);
        }
        unset_lock();
    }
    return nullptr;
}

void keepalive_start() {
    pthread_create(&thread, nullptr, keepalive, nullptr);
}

void keepalive_close() {
    pthread_cancel(thread);
}
