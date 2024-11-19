/**
 * COSC 3250 - Project 8
 * This file contains the function to delete a file
 * @author Adam Samuelson & Daniel O'Hear
 * Instructor Dr. Brylow
 * TA-BOT:MAILTO adam.samuelson@marquette.edu daniel.ohear@marquette.edu
 */
/* fileDelete.c - fileDelete */
/* Copyright (C) 2008, Marquette University.  All rights reserved. */
/*                                                                 */
/* Modified by                                                     */
/*                                                                 */
/* and                                                             */
/*                                                                 */
/*                                                                 */

#include <kernel.h>
#include <memory.h>
#include <file.h>

/*------------------------------------------------------------------------
 * fileDelete - Delete a file.
 *------------------------------------------------------------------------
 */
devcall fileDelete(int fd)
{
    // TODO: Unlink this file from the master directory index,
    //  and return its space to the free disk block list.
    //  Use the superblock's locks to guarantee mutually exclusive
    //  access to the directory index.

    wait(supertab->sb_dirlock);

    filetab[fd].fn_length = 0;
    filetab[fd].fn_cursor = 0;
    filetab[fd].fn_state = FILE_FREE;

    sbFreeBlock(supertab, fd);

    signal(supertab->sb_dirlock);

    return OK;
}
