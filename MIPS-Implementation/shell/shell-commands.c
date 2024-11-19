/**
 * @file     shell-commands.c
 * @provides commandtab
 *
 */
/* Embedded XINU, Copyright (C) 2009.  All rights reserved. */

#include <xinu.h>

/* This structure describes commands available to the shell. */
struct centry commandtab[] = {
    { "cat", FALSE, xsh_cat },
    { "chpass", FALSE, xsh_chpass },
    { "clear", TRUE, xsh_clear },
    { "delete", FALSE, xsh_delete },
    { "diskstat", FALSE, xsh_diskstat },
    { "ethstat", FALSE, xsh_ethstat },
    { "exit", TRUE, xsh_exit },
    { "help", FALSE, xsh_help },
    { "kill", TRUE, xsh_kill },
    { "login", FALSE, xsh_login },
    { "logout", FALSE, xsh_logout },
    { "makeuser", FALSE, xsh_makeuser },
    { "memstat", FALSE, xsh_memstat },
    { "ps", FALSE, xsh_ps },
    { "switchuser", FALSE, xsh_switchuser },
    { "test", FALSE, xsh_test },
    { "whoami", FALSE, xsh_whoami },
    { "?", FALSE, xsh_help }
};

ulong ncommand = sizeof(commandtab) / sizeof(struct centry);
