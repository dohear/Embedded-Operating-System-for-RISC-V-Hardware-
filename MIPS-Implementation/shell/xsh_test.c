/**
 * COSC 3250 - Project 8
 * This file contains the function to test sbfreeblock and filedelete
 * @author Adam Samuelson & Daniel O'Hear
 * Instructor Dr. Brylow
 * TA-BOT:MAILTO adam.samuelson@marquette.edu daniel.ohear@marquette.edu
 */
/**
 * @file     xsh_test.c
 * @provides xsh_test
 *
 */
/* Embedded XINU, Copyright (C) 2009.  All rights reserved. */

#include <xinu.h>

/**
 * Shell command (test) is testing hook.
 * @param args array of arguments
 * @return OK for success, SYSERR for syntax error
 */
command xsh_test(int nargs, char *args[])
{
    //TODO: Test your O/S.

    xsh_diskstat(0, NULL);
    int j = fileCreate('poopsock');
    xsh_diskstat(0, NULL);
    fileDelete(j);
    xsh_diskstat(0, NULL);
    return OK;

}
