#include "mails.h"

void append_mail(const std::string &msg) {
    // TODO: append
}

bool show() {
    // TODO: 
    //   show the earliest message, and return if there are any unread message
    //   if there are no message to show, print some tips
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
    // TODO: return the number of unread messages
}
