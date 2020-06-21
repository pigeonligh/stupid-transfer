#include "connection_core.h"
#include "packet.h"

#include "string.h"

std::string root;

void set_root(const std::string &dir) {
    root = dir;
}

connection_core::connection_core() {
    fd = -1;
    buff = new uint8_t[MAX_PACKET_SIZE];
    status = CONNECTION_READY;

    path.clear();
}

connection_core::~connection_core() {
    delete[] buff;
    path.clear();
}

bool connection_core::changeDirectory(const std::string &dir) {
    // TODO: change directory
    return false;
}

bool connection_core::createDirectory(const std::string &dir) {
    // TODO: create directory
    return false;
}

bool connection_core::removeDirectory(const std::string &dir) {
    // TODO: remove directory
    return false;
}

bool connection_core::removeFile(const std::string &file) {
    // TODO: remove file
    return false;
}

uint32_t connection_core::getCurrentDirectory(uint8_t *data) {
    // TODO: put result into data and return length
    return 0;
}

bool connection_core::setWorkingStatus(uint8_t sign, const std::string &file) {
    // TODO: prepare for List/Upload/Download
    return false;
}

uint32_t connection_core::getData(bool checked, uint8_t *data) {
    if (checked) {
        if (prepareData() == false) {
            return -1;
        }
    }
    memcpy(data, buff, length);
    return length;
}

bool connection_core::prepareData() {
    // TODO: load new data into buff
    return false;
}

bool connection_core::setData(uint8_t *data) {
    // TODO: receive data
    return false;
}