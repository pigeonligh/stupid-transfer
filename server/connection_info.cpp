/*
Copyright 2020 The FileTransfer Authors.

Licensed under the GNU General Public License, v3.0

See LICENSE in the project root for license information.
*/

#include "connection_info.h"
#include "connection_core.h"
#include "packet.h"

#include <vector>

#include <time.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>

typedef std::vector<connection_info *> vci;
std::vector<connection_info *> connections;
std::vector<connection_info *>::iterator g_it;

void init_connections() {
    connections.clear();
}

connection_info* find_connection(int32_t fd) {
    for (vci::iterator iter = connections.begin(); iter != connections.end(); ++iter)
        if ((*iter)->fd == fd)
            return *iter;

    return nullptr;
}

connection_info* first_connection() {
    if (connections.size() == 0)
        return nullptr;
    g_it = connections.begin();
    return *g_it;
}

connection_info* next_connection() {
    ++g_it;
    if (g_it == connections.end())
        return nullptr;
    return *g_it;
}

connection_info* new_connection(int fd) {
    if (find_connection(fd) != nullptr)
        return nullptr;

    connection_info *ci = new connection_info();
    ci->fd = fd;
    ci->secs = time(nullptr);
    ci->count = 0;
    ci->core = new connection_core();
    connections.push_back(ci);
    return ci;
}

void close_ci(connection_info* ci) {
    close(ci->fd);
    delete ci->core;
    delete ci;
}

bool close_connection(int fd) {
    for (vci::iterator iter = connections.begin(); iter != connections.end(); ++iter) {
        if ((*iter)->fd == fd) {
            close_ci(*iter);
            iter = connections.erase(iter);
            return true;
        }
    }

    return false;
}

void close_all_connections()
{
    for (vci::iterator iter = connections.begin(); iter != connections.end(); ++iter)
    {
        close_ci(*iter);
    }
    connections.clear();
}

void connection_info::deal_request(struct packet_data *data) {
    packet pack;
    memset(&pack, 0, sizeof pack);
    packet_data *pdata = (packet_data *)pack.data;
    if (data->option == REQUEST_LS) {
        // TODO: ls
    } else if (data->option == REQUEST_CD) {
        pack.type = TYPE_RESPONSE;
        if (core->changeDirectory(std::string(data->data))) {
            pdata->option = STATUS_SUCCEED
        }
        else {
            pdata->option = STATUS_FAILED;
        }
        pack.length = PACKET_HEADER_SIZE + 4;
    } else if (data->option == REQUEST_RM) {
        pack.type = TYPE_RESPONSE;
        if (core->removeFile(std::string(data->data))) {
            pdata->option = STATUS_SUCCEED;
        }
        else {
            pdata->option = STATUS_FAILED;
        }
        pack.length = PACKET_HEADER_SIZE + 4;
    } else if (data->option == REQUEST_PWD) {
        pack.type = TYPE_RESPONSE;
        if (core->getCurrentDirectory(pdata->data)) {
            pdata->option = STATUS_SUCCEED;
        }
        else {
            pdata->option = STATUS_FAILED;
        }
        pack.length = PACKET_HEADER_SIZE + 4 + strlen(pdata->data);
    } else if (data->option == REQUEST_MKDIR) {
        pack.type = TYPE_RESPONSE;
        if (core->createDirectory(std::string(data->data))) {
            pdata->option = STATUS_SUCCEED;
        }
        else {
            pdata->option = STATUS_FAILED;
        }
        pack.length = PACKET_HEADER_SIZE + 4;
    } else if (data->option == REQUEST_RMDIR) {
        pack.type = TYPE_RESPONSE;
        if (core->removeDirectory(std::string(data->data))) {
            pdata->option = STATUS_SUCCEED;
        }
        else {
            pdata->option = STATUS_FAILED;
        }
        pack.length = PACKET_HEADER_SIZE + 4;
    } else if (data->option == REQUEST_UPLOAD) {
        // TODO: upload
    } else if (data->option == REQUEST_DOWNLOAD) {
        // TODO: download
    } else if (data->option == SEND_CONTINUE) {
        // TODO: continue
    } else if (data->option == SEND_REPEAT) {
        // TODO: repeat
    } else if (data->option == SEND_DONE) {
        // TODO: done
    }
    send_packet(&pack);
}

void connection_info::deal_send(struct send_data *data) {
    packet pack;
    memset(&pack, 0, sizeof pack);
    packet_data *pdata = (packet_data*) pack.data;
    pack.type = TYPE_RESPONSE;
    if (check_hash(data)) {
        if (core->setData(data->data)) {
            pdata->option = SEND_CONTINUE;
        } else {
            pdata->option = STATUS_FAILED;
        }
    } else {
        pdata->option = SEND_REPEAT;
    }
    pack.length = PACKET_HEADER_SIZE + 4;

    send_packet(&pack);
}

void connection_info::send_packet(struct packet *pack) {
    int length = pack->length;
    send(fd, pack, length, 0);
}
