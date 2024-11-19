/* passwdFileWrite.c - passwdFileWrite */
/* Copyright (C) 2024, Marquette University.  All rights reserved. */

#include <xinu.h>

/*------------------------------------------------------------------------
 * passwdFileWrite - Write a password file to the filesystem.
 *------------------------------------------------------------------------
 */
devcall passwdFileWrite(void)
{
    int c, i, fd;
    char *p = (char *)usertab;
    fd = fileOpen("passwd");
    if (SYSERR == fd)
    {
        printf("Creating new passwd file.\n");
        fd = fileCreate("passwd");
        if (SYSERR == fd)
        {
            printf("Could not create new passwd file!\n");
            return SYSERR;
        }
    }
    fileSeek(fd, 0);
    for (i = 0; (i < sizeof(usertab)) && (OK == filePutChar(fd, p[i]));
         i++)
        ;
    fileClose(fd);
    return OK;
}
