/**
 * COSC 3250 - Project 8
 * This file contains the function to add a block back into the freelist
 * @author Adam Samuelson & Daniel O'Hear
 * Instructor Dr. Brylow
 * TA-BOT:MAILTO adam.samuelson@marquette.edu daniel.ohear@marquette.edu
 */
/* sbFreeBlock.c - sbFreeBlock */
/* Copyright (C) 2008, Marquette University.  All rights reserved. */
/*                                                                 */
/* Modified by                                                     */
/*                                                                 */
/* and                                                             */
/*                                                                 */
/*                                                                 */

#include <kernel.h>
#include <device.h>
#include <memory.h>
#include <disk.h>
#include <file.h>

/*------------------------------------------------------------------------
 * sbFreeBlock - Add a block back into the free list of disk blocks.
 *------------------------------------------------------------------------
 */

devcall swizzle(int diskfd, struct freeblock *freeblk){
    struct freeblock *free2 = freeblk->fr_next;
    if(NULL == freeblk->fr_next){
        freeblk->fr_next = 0;
    }
    else{
        freeblk->fr_next = (struct freeblock *)freeblk->fr_next->fr_blocknum;
    }
    seek(diskfd, freeblk->fr_blocknum);
    if(SYSERR == write(diskfd, freeblk, sizeof(struct freeblock))){
        return SYSERR;
    }
    freeblk->fr_next = free2;
    return OK;
}

devcall swizzleSuperBlock(int diskfd, struct superblock *psuper){
    struct freeblock *swizzle = psuper->sb_freelst;
    struct dirblock *swizzle2 = psuper->sb_dirlst;

    psuper->sb_freelst = (struct freeblock *)swizzle->fr_blocknum;
    psuper->sb_dirlst = (struct dirblock *)swizzle2->db_blocknum;

    seek(diskfd, psuper->sb_blocknum);
    if(SYSERR == write(diskfd, psuper, sizeof(struct superblock))){
        return SYSERR;
    }
    psuper->sb_freelst = swizzle;
    psuper->sb_dirlst = swizzle2;
    return OK;
}

devcall sbFreeBlock(struct superblock *psuper, int block)
{
    // TODO: Add the block back into the filesystem's list of
    //  free blocks.  Use the superblock's locks to guarantee
    //  mutually exclusive access to the free list, and write
    //  the changed free list segment(s) back to disk.

    int diskfd;
    struct dentry *phw;

    if(psuper == NULL){
        return SYSERR;
    }

    phw = psuper -> sb_disk;

    if(block <= 0 || block > MAXFILES){
        return SYSERR;
    }

    diskfd = phw - devtab;

    wait(psuper->sb_freelock);

    struct freeblock *freeblk = psuper->sb_freelst;
    struct freeblock *newfreeblk;

    if(freeblk == NULL){
        freeblk = (struct freeblock *)malloc(sizeof(struct freeblock));
        freeblk->fr_blocknum = block;
        freeblk->fr_count = 0;
        freeblk->fr_next = NULL;
        psuper->sb_freelst = freeblk;
        swizzle(diskfd, freeblk);
        swizzleSuperBlock(diskfd, psuper);
        signal(psuper->sb_freelock);
        return OK;
    }

    while(freeblk->fr_next != NULL){
        newfreeblk = freeblk;
        freeblk = freeblk->fr_next;
    }

    if(freeblk->fr_count == FREEBLOCKMAX){
        freeblk = (struct freeblock *)malloc(sizeof(struct freeblock));
        freeblk->fr_blocknum = block;
        newfreeblk->fr_count = 0;
        newfreeblk->fr_next = NULL;
        freeblk->fr_next = newfreeblk;
        swizzle(diskfd, freeblk);
        signal(psuper->sb_freelock);
        return OK;
    }

    else{
        freeblk->fr_free[freeblk->fr_count] = block;
        freeblk->fr_count++;
        swizzle(diskfd, freeblk);
    }

    signal(psuper->sb_freelock);
    return OK; 
}
