/**
 * COSC 3250 - Project 1
 * This traverses and creates pages 
 * @author Daniel O'Hear & Adam Samuelson
 * Instructor Dr. Brylow
 * TA-BOT:MAILTO daniel.ohear@marquette.edu adam.samuelson@marquette.edu
 */

#include <xinu.h>

static ulong *pgTraverseAndCreate(pgtbl pagetable, ulong virtualaddr, int attr, ulong physicaladdr);

/**
 * Maps a page to a specific virtual address
 * @param pagetable     the base pagetable
 * @param page          the page that will be stored at the virtual address
 * @param virtualaddr   the virtual address at which the page will reside
 * @param attr          any attributes to set on the page table entry
 * @return              OK if successful, otherwise a SYSERR
 */
syscall mapPage(pgtbl pagetable, page pg, ulong virtualaddr, int attr, ulong physicaladdr)
{
    ulong addr;

    addr = (ulong)truncpage(virtualaddr);

    if (pgTraverseAndCreate(pagetable, addr, attr, physicaladdr) == (ulong *)SYSERR)
    {
        return SYSERR;
    }

    return OK;
}

/**
 * Maps a given virtual address range to a corresponding physical address range.
 * @param pagetable    the base pagetable
 * @param virtualaddr  the start of the virtual address range. This will be truncated to the nearest page boundry.
 * @param physicaladdr the start of the physical address range
 * @param length       the length of the range to map
 * @param attr         any attributes to set on the page table entry
 * @return             OK if successful, otherwise a SYSERR
 */
syscall mapAddress(pgtbl pagetable, ulong virtualaddr, ulong physicaladdr,
               ulong length, int attr)
{
    ulong addr, end;
    ulong nlength;


    if (length == 0)
    {
        return SYSERR;
    }

    // Round the length to the nearest page size
    nlength = roundpage(length);
    addr = (ulong)truncpage(virtualaddr);
    end = addr + nlength;

    // Loop over the entire range
    for (; addr < end; addr += PAGE_SIZE, physicaladdr += PAGE_SIZE)
    {
        // Create a page table entry if one doesn't exist. Otherwise, get the existing page table entry.
        if (pgTraverseAndCreate(pagetable, addr, attr, physicaladdr) == (ulong *)SYSERR)
        {
            return SYSERR;
        }
    }

    return OK;
}

/**
 * Starting at the base pagetable, tranverse the hierarchical page table structure for the virtual address.  Create pages along the way if they don't exist.
 * @param pagetable    the base pagetable
 * @param virtualaddr  the virtual address to find the it's corresponding page table entry.
 * @param attr	       the attributes to set on the leaf page
 * @return             OK
 */
static ulong *pgTraverseAndCreate(pgtbl pagetable, ulong virtualaddr, int attr, ulong physicaladdr)
{
    /**
    * TODO:
    * For each level in the page table, get the page table entry by masking and shifting the bits in the virtualaddr depending on the level
    * If the valid bit is set, use that pagetable for the next level
    * Otherwise create the page by calling pgalloc().  Make sure to setup the page table entry accordingly. Call sfence_vma once finished to flush TLB
    * Once you've tranversed all three levels, set the attributes (attr) for the leaf page (don't forget to set the valid bit!)
    */

   ulong VA2 = virtualaddr >> 30 & 0x1FF; 
   ulong VA1 = ((virtualaddr << 9) >> 30 & 0x1FF);
   ulong VA0 = ((virtualaddr << 18 ) >> 30 & 0x1FF);

    pgtbl lvl1tbl;
    pgtbl lvl0tbl;
    if(pagetable[VA2] & PTE_V){
        lvl1tbl = PTE2PA(pagetable[VA2]);
        if(lvl1tbl[VA1] & PTE_V){
            lvl0tbl = PTE2PA(lvl1tbl[VA1]);
        }
        else{
            lvl0tbl = pgalloc();
            lvl1tbl[VA1] = PA2PTE(lvl0tbl);
            lvl1tbl[VA1] = lvl1tbl[VA1] | PTE_V;
        }
    }
    else{
        lvl1tbl = pgalloc();
        pagetable[VA2] = PA2PTE(lvl1tbl);
        pagetable[VA2] = pagetable[VA2] | PTE_V;
        
        lvl0tbl = pgalloc();
        lvl1tbl[VA1] = PA2PTE(lvl0tbl);
        lvl1tbl[VA1] = lvl1tbl[VA1] | PTE_V;
    }
    lvl0tbl[VA0] = PA2PTE(physicaladdr);
    lvl0tbl[VA0] = lvl0tbl[VA0] | attr | PTE_V;
   

    return (ulong *)OK;
}

