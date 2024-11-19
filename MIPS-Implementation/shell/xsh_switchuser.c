/**
 * @file     xsh_switchuser.c
 * @provides xsh_switchuser
 *
 */
/* Embedded XINU, Copyright (C) 2009.  All rights reserved. */

#include <xinu.h>

/**
 * Shell command (switchuser) is switchusering hook.
 * @param args array of arguments
 * @return OK for success, SYSERR for syntax error
 */
command xsh_switchuser(int nargs, char *args[])
{
    int id;

    if (userid < SUPERUID)
    {
        printf("Must login first\n");
        return SYSERR;
    }

    //Attempt authentication
    if (nargs == 2)
    {
        id = login(args[1]);
    }
    else
    {
        id = login(NULL);
    }
    if (SYSERR == id)
    {
        printf("Login failure.\n");
        return SYSERR;
    }
    userid = id;
    printf("Success!\n");
    return OK;
}
