#include "connection.h"

#include <unistd.h>
#include <vector>

typedef std::vector<connection_info *> vci;
std::vector<connection_info *> connections;
std::vector<connection_info *>::iterator g_it;

void init_connections()
{
    connections.clear();
}

connection_info *find_connection(int32_t fd)
{
    for (vci::iterator iter = connections.begin(); iter != connections.end(); ++iter)
        if ((*iter)->fd == fd)
            return *iter;

    return nullptr;
}

connection_info *first_connection()
{
    if (connections.size() == 0)
        return nullptr;
    g_it = connections.begin();
    return *g_it;
}

connection_info *next_connection()
{
    ++g_it;
    if (g_it == connections.end())
        return nullptr;
    return *g_it;
}

bool new_connection(int fd)
{
    if (find_connection(fd) == nullptr)
        return false;

    connections.push_back(new connection_info(fd, time(nullptr), 0));
    return true;
}

bool close_connection(int fd)
{
    for (vci::iterator iter = connections.begin(); iter != connections.end(); ++iter)
        if ((*iter)->fd == fd)
        {
            delete (*iter);
            iter = connections.erase(iter);
            close(fd);
            return true;
        }

    return false;
}

void close_all_connections()
{
    for (vci::iterator iter = connections.begin(); iter != connections.end(); ++iter)
    {
        delete (*iter);
        close((*iter)->fd);
    }
    connections.clear();
}
