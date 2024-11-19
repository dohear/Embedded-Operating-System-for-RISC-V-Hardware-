/**
 * @file     xsh_login.c
 * @provides xsh_login
 *
 */
/* Embedded XINU, Copyright (C) 2009.  All rights reserved. */

#include <xinu.h>

/* Global variables for authentication                      */
/* Current logged in userid, or SYSERR if none.             */
int userid;
/* User table stores user names, password hashes and salt.  */
/* User IDs are implicit index in the table.                */
struct userent usertab[MAXUSERS];

/**
 * xinuhash calculates a simple one-way hash function.
 * Don't use this for any real cryptographic implementation.
 * @param passtext String of password text.
 * @param maxlen   Max length of passtext.  Should be multiple of ulong size.
 * @param salt     Cryptographic salt to prevent trivial dictionary attacks.
 * @return resulting 32-bit hash value.
 */
ulong xinuhash(char *passtext, int maxlen, ulong salt)
{
    ulong result = salt;
    int i = 0, j = 0;
    ulong *p = NULL;
    for (j = 0; j < 3; j++)
    {
        p = (ulong *)passtext;
        for (i = 0; i < (maxlen / sizeof(ulong)); i++)
        {
            result = result ^ *p ^ (*p << 23) ^ (*p >> 13);
            p++;
        }
        result = result ^ (result << 24) ^ (result >> 8);
    }
    return result;
}

/**
 * getusername prompts for usernames from console, discarding whitespace.
 * @param buffer   Storage location for username.
 * @param maxlen   Max length of passtext.  Should be multiple of ulong size.
 * @return Last character read, or EOF.
 */
int getusername(char *buffer, int maxlen)
{
    int len = 0;
    int c;
    int i = 0;
    printf("Enter username (max %d): ", maxlen);
    c = getc(CONSOLE);
    while (isspace(c))
    {
        c = getc(CONSOLE);
    }
    do
    {
        buffer[i] = c;
        i++;
    }
    while ((EOF != (c = getc(CONSOLE))) && !isspace(c) && (i < maxlen));
    return c;
}

/**
 * searchname searches usertab for a matching username.
 * @param username  Sought after username.
 * @return Matching user ID, or SYSERR if not found.
 */
int searchname(char *username)
{
    int i = 0;
    for (i = 0; i < MAXUSERS; i++)
    {
        if ((usertab[i].state)
            && (0 == strncmp(username, usertab[i].username, MAXUSERLEN)))
            return i;
    }
    return SYSERR;
}

/**
 * hashpassword prompts for a password, and calculates the hash.
 * @param salt   Cryptographic salt.
 * @return hash of password
 */
ulong hashpassword(ulong salt)
{
    char passtext[MAXPASSLEN+1];
    int c, i = 0;
    ulong hash = 0;
    bzero(passtext, MAXPASSLEN+1);

    c = getc(CONSOLE);
    while (isspace(c))
    {
        c = getc(CONSOLE);
    }
    do
    {
        passtext[i] = c;
        i++;
    }
    while ((EOF != (c = getc(CONSOLE)))
           && !isspace(c) && (i < MAXPASSLEN));

    printf("\nPassword was: %s\n", passtext);
    hash = xinuhash(passtext, MAXPASSLEN, salt);
    printf("Hash is 0x%08X\n", hash);
    return hash;
}

/**
 * login attempts to authenticate a given or prompted user.
 * @param given   Optional user name provided by shell.
 * @return user ID of authenticated user, or SYSERR.
 */
int login(char *given)
{
    char username[MAXUSERLEN + 1];
    int id;
    ulong passhash;
    bzero(username, MAXUSERLEN + 1);
    /* If a user name was given, use that.  Otherwise, prompt for one. */
    if (NULL != given)
    {
        strncpy(username, given, MAXUSERLEN);
    }
    else
    {
        getusername(username, MAXUSERLEN);
    }
    /* See if user name exists in user table. */
    id = searchname(username);
    if ((id < SUPERUID) || (id > MAXUSERS))
    {
        printf("User name %s not found.\n", username);
        return SYSERR;
    }
    /* Prompt for password and calculate hash. */
    printf("Enter password for user %s: ", username);
    passhash = hashpassword(usertab[id].salt);
    if (passhash != usertab[id].passhash)
    {
        printf("Password for user %s does not match!\n", username);
        return SYSERR;
    }
    return id;
}

/**
 * Shell command (login) is for authentication
 * @param args array of arguments
 * @return OK for success, SYSERR for syntax error
 */
command xsh_login(int nargs, char *args[])
{
    char username[MAXUSERLEN + 1];
    int id;
    char superusername[] = "superusr";

    /* If superusr's entry in the user table is unused, then initialize */
    /*  the superusr password for the first time.                       */
    if (usertab[SUPERUID].state == USERFREE)
    {
        usertab[SUPERUID].state = USERUSED;
        strncpy(usertab[SUPERUID].username, superusername, MAXUSERLEN);
        usertab[SUPERUID].salt = SALT;
        printf("Password for %s has not been set.\n", superusername);
        printf("Enter password for user %s: ", superusername);
        usertab[SUPERUID].passhash = hashpassword(usertab[SUPERUID].salt);
        passwdFileWrite();
    }


    /* Check to see if a legal user ID is already logged in. */
    if (userid >= SUPERUID)
    {
        bzero(username, MAXUSERLEN + 1);
        strncpy(username, usertab[userid].username, MAXUSERLEN);
        printf("Already logged in as user %s.\n", username);
	return SYSERR;
    }
    /* If a user name was given from the shell, pass along to login(). */
    if (nargs == 2)
    {
	id = login(args[1]);
    }
    else
    {
	id = login(NULL);
    }
    
    if (id < SUPERUID)
    {
	printf("Login failure.\n");
	return SYSERR;
    }
    userid = id;
    printf("Success!\n");
    return OK;
}
