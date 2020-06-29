/*
Copyright 2020 The FileTransfer Authors.

Licensed under the GNU General Public License, v3.0

See LICENSE in the project root for license information.
*/

#ifndef _MAILS_H_
#define _MAILS_H_

#include <string>

void append_mail(const std::string &);

void show_mails(int count);
void show_all_mails();
int count_mails();

#endif /* _MAILS_H_ */
