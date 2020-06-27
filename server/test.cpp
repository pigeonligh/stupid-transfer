#include "server.h"

#include "connection_core.h"

#include <stdio.h>

void server_test()
{
    // this is test
    printf("this is test.\n");

    set_root("/mnt/c/Users/palad/Desktop/workspace/git/FileTransfer/bin/data/");

    connection_core *core = new connection_core();

    char buff[4096];

    printf("%d\n", core->createDirectory("hello/hi"));
    printf("%d\n", core->createDirectory("hello/hih"));
    printf("%d\n", core->createDirectory("hello/hio"));
    printf("%d\n", core->changeDirectory("hello"));
    core->getCurrentDirectory((uint8_t *)buff);
    puts(buff);
    printf("%d\n", core->changeDirectory("./hi"));
    core->getCurrentDirectory((uint8_t *)buff);
    puts(buff);
    printf("%d\n", core->changeDirectory("../../"));
    core->getCurrentDirectory((uint8_t *)buff);
    puts(buff);
    printf("%d\n", core->removeDirectory("hello"));
    printf("%d\n", core->removeFile("file"));

    delete core;
}
