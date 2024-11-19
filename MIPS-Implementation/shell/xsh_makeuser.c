/**
 * COSC 3250 - Project 10
 * This file contains the function to make a user and store their password and username
 * @author Adam Samuelson & Daniel O'Hear
 * Instructor Dr. Brylow
 * TA-BOT:MAILTO adam.samuelson@marquette.edu daniel.ohear@marquette.edu
 */
/**
 * @file     xsh_makeuser.c
 * @provides xsh_makeuser
 *
 */
/* Embedded XINU, Copyright (C) 2009.  All rights reserved. */

#include <xinu.h>

/**
 * Shell command (makeuser) makes a new user account.
 * @param args array of arguments
 * @return OK for success, SYSERR for errors.
 */
command xsh_makeuser(int nargs, char *args[])
{
/**
 * TODO:
 * This function creates a new entry in the global table of users,
 * with a valid password hash and salt, and updates the passwd file
 * on disk.  You may break this task down into any number of helper
 * functions within this file, and also may rely on helper functions
 * that already exist, such as getusername(), hasspassword(), and
 * passwdFileWrite().
 *
 * Steps:
 * 1) Find a free usertab entry for a new user, and set it USERUSED.
 * 2) If the shell did not provide a user name, prompt for one.
 * 3) Prompt for a new password, and calculate the hash.
 * 4) Initialize the fields of the new user entry.
 * 5) Commit the changes to the passwd file on disk.
 * 6) Printf "Successfully created user ID %d\n" with the new user ID.
 *
 * Errors to watch for:
 * 1) There is not already a user logged in.
 *    Error text = "Must login first\n". if global user = syserr no one is in 0 is super user
 * 2) The logged in userid is not already SUPERUID.
 *    Error text = "ERROR: Only superusr can make new users!\n".
 * 3) There are no more unused slots in usertab.
 *    Error text = "ERROR: No more users available in usertab!\n".
 */
int i = 0;
ulong passhash;
char username[MAXUSERLEN + 1];

if(userid == SYSERR){
    printf("Must login first\n");
    return SYSERR;
}
if(userid != 0){
    printf("ERROR: Only superusr can make new users!\n");
    return SYSERR;
}

while (usertab[i].state != USERFREE && i < MAXUSERS){
    i++;
}
if (i >= MAXUSERS){
    printf("ERROR: No more users available in usertab!\n");
    return SYSERR;
}
usertab[i].state == USERUSED;

if (usertab[i].username == NULL){
    getusername(username, MAXUSERLEN);
    strncpy(usertab[i].username, username, MAXUSERLEN);
}

usertab[i].passhash = hashpassword(SALT);
usertab[i].salt = SALT;

passwdFileWrite();

printf("Successfully created user ID %d\n", i);

return OK;

}
