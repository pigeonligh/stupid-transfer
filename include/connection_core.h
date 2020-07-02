/*
Copyright 2020 The FileTransfer Authors.

Licensed under the GNU General Public License, v3.0

See LICENSE in the project root for license information.
*/

#ifndef _CONNECTION_CORE_H_
#define _CONNECTION_CORE_H_

#include <stdint.h>
#include <vector>
#include <string>
#include <dirent.h>

#define CONNECTION_READY 0
#define CONNECTION_LISTING 1
#define CONNECTION_UPLOADING 2
#define CONNECTION_DOWNLOADING 3

struct connection_core {
    connection_core();
    ~connection_core();

    bool changeDirectory(const std::string &dir);
    bool createDirectory(const std::string &dir);
    bool removeDirectory(const std::string &dir);

    bool removeFile(const std::string &file);

    uint32_t getCurrentDirectory(uint8_t *data);

    bool setWorkingStatus(uint8_t sign, const std::string &file);
    void unsetWorkingStatus();

    int32_t getData(bool checked, uint8_t *data);
    bool setData(uint8_t *data);

private:
    FILE *fd;
    DIR *dir;

    int8_t status;
    uint8_t *buff;
    int32_t length;

    std::vector<std::string> path;

    bool prepareData();
};

void set_root(const std::string &dir);

#endif /* _CONNECTION_CORE_H_ */
