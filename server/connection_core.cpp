#include "connection_core.h"
#include "packet.h"

#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <fcntl.h>

std::string root;

void set_root(const std::string &dir) {
    root = dir;
}

bool get_path(std::string dir, std::vector<std::string> &path) {
    if (dir[dir.size() - 1] != '/')
        dir += std::string("/");
    if (dir.size() == 0)
        return false;
    if (dir[0] == '/') {
        dir = dir.substr(1, dir.size() - 1);
        path.clear();
    }
    if (dir.size() == 0 || dir[0] == '/')
        return false;
    std::vector<std::string> _path(path);
    int last = 0;
    for (int i = 0; i < dir.size(); ++ i) {
        if (dir[i] == '/') {
            if (last >= i)
                return false;
            std::string _dir = dir.substr(last, i - last);
            if (_dir == std::string(".")) {
                last = i + 1;
                continue ;
            }
            else if (_dir == std::string("..")) {
                if (_path.size() != 0)
                    _path.pop_back();
                else
                    return false;
                last = i + 1;
            }
            else {
                _path.push_back(_dir);
                last = i + 1;
            }
        }
    }
    std::swap(path, _path);
    return true;
}

bool remove_dir(const std::string &dir) {
    DIR *dirp;
    struct dirent *dp;
    struct stat dir_stat;
    if (access(dir.data(), 0) == -1)
        return false;
    if (stat(dir.data(), &dir_stat) < 0)
        return false;
    if (S_ISREG(dir_stat.st_mode)) {
        remove(dir.data());
        return true;
    }
    else if (S_ISDIR(dir_stat.st_mode)) {
        dirp = opendir(dir.data());
        while ((dp = readdir(dirp)) != NULL) {
            if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0)
                continue;
            std::string dir_name = dir + std::string("/") + std::string(dp->d_name);
            remove_dir(dir_name);
        }
        closedir(dirp);
        rmdir(dir.data());
        return true;
    }
    return false;
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
    std::vector<std::string> _path(path);
    bool result = get_path(dir, _path);
    if (!result)
        return false;
    std::string res = root;
    for (int i = 0; i < _path.size(); ++ i)
        res += (_path[i] + std::string("/"));
    if (access(res.data(), 0) == -1)
        return false;
    std::swap(path, _path);
    return true;
}

bool connection_core::createDirectory(const std::string &dir) {
    std::string res = root;
    std::vector<std::string> _path(path);
    bool result = get_path(dir, _path);
    if (!result)
        return false;
    bool flag_exist = 1;
    for (int i = 0; i < _path.size(); ++ i) {
        res += (_path[i] + std::string("/"));
        if (access(res.data(), 0) == -1) {
            flag_exist = 0;
            if (mkdir(res.data(), 0755) == -1)
                return false;
        }
    }
    if (flag_exist)
        return false;
    return true;
}

bool connection_core::removeDirectory(const std::string &dir) {
    if (dir.size() == 0)
        return false;
    std::string res = root;
    std::vector<std::string> _path(path);
    bool result = get_path(dir, _path);
    if (!result)
        return false;
    for (int i = 0; i < _path.size(); ++ i)
        res += (_path[i] + std::string("/"));
    if (access(res.data(), 0) == -1)
        return false;
    return remove_dir(res);
}

bool connection_core::removeFile(const std::string &file) {
    if (file.size() == 0)
        return false;
    std::string res = root;
    std::vector<std::string> _path(path);
    bool result = get_path(file, _path);
    if (!result)
        return false;
    for (int i = 0; i < _path.size(); ++ i) {
        res += _path[i];
        if (i != _path.size() - 1)
            res += std::string("/");
    }
    if (access(res.data(), 0) == -1)
        return false;
    remove(res.data());
    return true;
}

uint32_t connection_core::getCurrentDirectory(uint8_t *data) {
    std::string result("/");
    for (int i = 0; i < path.size(); ++ i)
        result += (path[i] + std::string("/"));
    strcpy(reinterpret_cast<char*>(data), result.data());
    return result.length();
}

void connection_core::unsetWorkingStatus() {
    if (fd != nullptr) {
        fclose(fd);
        fd = 0;
    }
    if (dir != nullptr) {
        closedir(dir);
        dir = nullptr;
    }
    status = CONNECTION_READY;
}

bool connection_core::setWorkingStatus(uint8_t sign, const std::string &file) {
    unsetWorkingStatus();
    if (sign == REQUEST_LS) {
        dir = opendir(file.c_str());
        if (dir == nullptr) {
            return false;
        }
        status = CONNECTION_LISTING;
    } else if (sign == REQUEST_DOWNLOAD) {
        fd = fopen(file.c_str(), "rb");
        if (fd == nullptr) {
            return false;
        }
        status = CONNECTION_DOWNLOADING;
    } else if (sign == REQUEST_UPLOAD) {
        fd = fopen(file.c_str(), "wb");
        if (fd == nullptr) {
            return false;
        }
        status = CONNECTION_UPLOADING;
    } else {
        return false;
    }
    return true;
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
    if (status == CONNECTION_LISTING) {
        if (dir == nullptr) {
            return false;
        }
        // TODO: 
        // 1. list file/dir from `dir`
        // 2. write into a send_data object(one filename per 256 positions, 7 filenames per packet)
        // 3. list `.` and `..`(if can) first
        // 4. hash it
        // 5. copy data into buff
        // 6. set length = MAX_PACKET_SIZE
    } else if (status == CONNECTION_DOWNLOADING) {
        if (fd == nullptr) {
            return false;
        }
        // TODO: 
        // 1. load file from fd
        // 2. write into a send_data object
        // 3. hash it
        // 4. copy data into buff
        // 5. set length = MAX_PACKET_SIZE
    } else {
        return false;
    }
    return true;
}

bool connection_core::setData(uint8_t *data) {
    if (status == CONNECTION_UPLOADING) {
        if (fd == nullptr) {
            return false;
        }
        // TODO: 
        // 1. data -> (send_data *)
        // 1. check hash
        // 2. write data into fd
        // notice: check length by char '\0'
    } else {
        return false;
    }
    return true;
}
