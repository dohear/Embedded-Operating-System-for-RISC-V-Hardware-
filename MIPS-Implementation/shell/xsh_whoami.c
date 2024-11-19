/**
 * @file     xsh_whoami.c
 * @provides xsh_whoami
 *
 */
/* Embedded XINU, Copyright (C) 2009.  All rights reserved. */

#include <xinu.h>

/**
 * Shell command (whoami) prints out current userid.
 * @param args array of arguments
 * @return OK for success, SYSERR for syntax error
 */
command xsh_whoami(int nargs, char *args[])
{
    char username[MAXUSERLEN + 1];
    bzero(username, MAXUSERLEN + 1);

    if (userid < SUPERUID)
    {
        printf("No user logged in.\n");
    }
    else
    {
        strncpy(username, usertab[userid].username, MAXUSERLEN);
        printf("User %s logged in.\n", username);
        printf("\tuid = %d, passhash = 0x%08X, salt = 0x%08X\n",
               userid, usertab[userid].passhash, usertab[userid].salt);
    }
    return OK;
}
