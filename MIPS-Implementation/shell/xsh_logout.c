/**
 * @file     xsh_logout.c
 * @provides xsh_logout
 *
 */
/* Embedded XINU, Copyright (C) 2009.  All rights reserved. */

#include <xinu.h>

/**
 * Shell command (logout) sets userid back to SYSERR
 * @param args array of arguments
 * @return OK for success, SYSERR for syntax error
 */
command xsh_logout(int nargs, char *args[])
{
    if (userid < SUPERUID)
    {
        printf("Already logged out\n");
    }
    else
    {
        userid = SYSERR;
    }
    return OK;
}
