#include "server.h"

#include "connection_core.h"

#include <stdio.h>

void server_test() {
    // this is test
    printf("this is test.\n");

    connection_core *core = new connection_core();

    char buff[4096];

    core->createDirectory("hello");
    core->changeDirectory("hello");
    core->getCurrentDirectory((uint8_t *) buff);
    puts(buff);
    core->changeDirectory("..");
    core->getCurrentDirectory((uint8_t *) buff);
    puts(buff);

    delete core;
}