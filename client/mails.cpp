/*
Copyright 2020 The FileTransfer Authors.

Licensed under the GNU General Public License, v3.0

See LICENSE in the project root for license information.
*/

#include "mails.h"
#include <queue>

std::queue<std::string> _mails;

void init_mail() {
    while (_mails.size())
        _mails.pop();
}

void append_mail(const std::string &msg) {
    _mails.push(msg);
}

bool show() {
    if (_mails.empty()) {
        printf("There is no unread message.\n");
    }
    else {
        printf("%s\n", _mails.front().c_str());
        _mails.pop();
    }
    return _mails.size();
}

void show_mails(int count) {
    for (int i = 0; i < count; i++) {
        if (!show()) {
            break;
        }
    }
}

void show_all_mails() {
    while (show());
}

int count_mails() {
    return _mails.size();
}
