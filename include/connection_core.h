#ifndef _CONNECTION_CORE_H_
#define _CONNECTION_CORE_H_

#include <stdint.h>
#include <vector>
#include <string>

#define CONNECTION_READY 0

struct connection_core {
    connection_core();
    ~connection_core();

    bool changeDirectory(const std::string &dir);
    bool createDirectory(const std::string &dir);
    bool removeDirectory(const std::string &dir);

    bool removeFile(const std::string &file);

    uint32_t getCurrentDirectory(uint8_t *data);

    bool setWorkingStatus(uint8_t sign, const std::string &file);
    uint32_t getData(bool checked, uint8_t *data);

private:
    int32_t fd;    
    uint8_t status;
    uint8_t *buff;
    uint32_t length;

    std::vector<std::string> path;

    bool prepareData();
};

#endif /* _CONNECTION_CORE_H_ */
