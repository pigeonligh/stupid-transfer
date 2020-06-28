#include "server.h"

#include "connection_core.h"

#include <stdio.h>
#include <iostream>
#include <string.h>

void server_test()
{
    // this is test
    printf("this is test.\n");

    set_root("/mnt/c/Users/palad/Desktop/workspace/git/FileTransfer/bin/data/");

    connection_core *core = new connection_core();

    char buff[4096];
    char line[4096];
    for (;;) {
        printf(">> ");
        std::cin.getline(line, 4096);
        char cmd[4096], content[4096];
        sscanf(line, "%s", cmd);
        if (strcmp(cmd, "cd") == 0) {
            sscanf(line, "%s%s", cmd, content);
            if (core->changeDirectory(std::string(content)) == false)
                printf(">> Failed. Something wrong with your command.\n");
        }
        else if (strcmp(cmd, "mk") == 0) {
            sscanf(line, "%s%s", cmd, content);
            if (core->createDirectory(std::string(content)) == false)
                printf(">> Failed. Something wrong with your command.\n");
        }
        else if (strcmp(cmd, "rm") == 0) {
            sscanf(line, "%s%s", cmd, content);
            if (strcmp(content, "-rf") == 0) {
                sscanf(line, "%s%s%s", cmd, cmd, content);
                if (core->removeDirectory(std::string(content)) == false)
                    printf(">> Failed. Something wrong with your command.\n");
            }
            else {
                if (core->removeFile(std::string(content)) == false)
                    printf(">> Failed. Something wrong with your command.\n");
            }
        }
        else if (strcmp(cmd, "pt") == 0) {
            printf(">> ");
            core->getCurrentDirectory((uint8_t *) buff);
            printf("%s\n", buff);
        }
        else if (strcmp(cmd, "exit") == 0) {
            bool flag_exit = 0;
            for (;;) {
                printf(">> Are you sure to exit? [Y/N] ");
                std::cin.getline(line, 4096);
                sscanf(line, "%s", content);
                if (strcmp(content, "Y") == 0 || strcmp(content, "y") == 0) {
                    flag_exit = 1;
                    break ;
                }
                else if (strcmp(content, "N") == 0 || strcmp(content, "n") == 0)
                    break ;
                else
                    continue ;
            }
            if (flag_exit)
                break ;
        }
        else if (strcmp(cmd, "help") == 0) {
            printf(">> Usage: \n");
        }
        else {
            printf(">> Sorry. No such command.\n   Input [help] to get usage.\n");
        }
    }
    // printf("%d\n", core->createDirectory("hello/hi"));
    // printf("%d\n", core->createDirectory("hello/hih"));
    // printf("%d\n", core->createDirectory("hello/hio"));
    // printf("%d\n", core->changeDirectory("hello"));
    // core->getCurrentDirectory((uint8_t *)buff);
    // puts(buff);
    // printf("%d\n", core->changeDirectory("./hi"));
    // core->getCurrentDirectory((uint8_t *)buff);
    // puts(buff);
    // printf("%d\n", core->changeDirectory("../../"));
    // core->getCurrentDirectory((uint8_t *)buff);
    // puts(buff);
    // printf("%d\n", core->removeDirectory("hello"));
    // printf("%d\n", core->removeFile("file"));

    delete core;
}
