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

    uint32_t getData(bool checked, uint8_t *data);
    bool setData(uint8_t *data);

private:
    FILE *fd;
    DIR *dir;

    uint8_t status;
    uint8_t *buff;
    uint32_t length;

    std::vector<std::string> path;

    bool prepareData();
};

void set_root(const std::string &dir);

#endif /* _CONNECTION_CORE_H_ */
