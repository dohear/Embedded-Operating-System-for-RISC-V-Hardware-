/**
 * @file auth.h
 *
 */
/* Embedded Xinu, Copyright (C) 2024.  All rights reserved. */

#ifndef _AUTH_H_
#define _AUTH_H_



#define MAXUSERLEN 8
#define MAXPASSLEN 256

#define SALT 0x5F3F1F72UL

#define SUPERUID	0

#define USERFREE        0
#define USERUSED        1

struct userent
{
    int state;
    char username[MAXUSERLEN];
    ulong passhash;
    ulong salt;
};

#define MAXUSERS   (DISKBLOCKLEN / sizeof(struct userent))

extern struct userent usertab[MAXUSERS];
extern int userid;

ulong xinuhash(char *passtext, int maxlen, ulong salt);
int getusername(char *buffer, int maxlen);
int searchname(char *username);
ulong hashpassword(ulong salt);
int login(char *given);

#endif                          /* _AUTH_H_ */
