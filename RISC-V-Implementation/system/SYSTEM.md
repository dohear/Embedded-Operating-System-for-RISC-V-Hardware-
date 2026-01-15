# System Directory Documentation

This directory contains the core kernel implementation for Embedded Xinu on RISC-V. It includes boot code, process management, scheduling, interrupt handling, memory management, and device drivers.

---

## File Overview

| File | Language | Purpose |
|------|----------|---------|
| `start.S` | Assembly | Boot entry point and CPU initialization |
| `initialize.c` | C | System initialization and null process |
| `platforminit.c` | C | Hardware-specific initialization |
| `create.c` | C | Process creation |
| `kill.c` | C | Process termination |
| `ready.c` | C | Move process to ready state |
| `resched.c` | C | Lottery scheduler |
| `ctxsw.S` | Assembly | Context switching |
| `interrupt.S` | Assembly | Interrupt entry point |
| `dispatch.c` | C | Interrupt/syscall dispatcher |
| `xtrap.c` | C | Exception handler |
| `criticalerr.S` | Assembly | Critical error handler |
| `syscall_dispatch.c` | C | System call dispatcher |
| `queue.c` | C | Process queue operations |
| `clkinit.c` | C | Clock initialization |
| `clkhandler.c` | C | Clock interrupt handler |
| `kprintf.c` | C | Kernel console I/O |
| `pgInit.c` | C | Physical page initialization |
| `pgalloc.c` | C | Physical page allocation |
| `pgFree.c` | C | Physical page freeing |
| `map.c` | C | Virtual memory mapping |
| `vm_kerninit.c` | C | Kernel page table setup |
| `vm_userinit.c` | C | User page table setup |
| `mmu.S` | Assembly | MMU operations |
| `random.c` | C | Random number generator |
| `getstk.c` | C | Stack allocation (legacy) |
| `testcases.c` | C | Test suite |
| `Makerules` | Make | Build rules |

---

## Boot Sequence

### 1. `start.S` — Entry Point

**Location:** First code executed when CPU starts at `0x42000000`

**Boot Flow:**
```
_start:
    │
    ▼
reset_handler:
    │
    ├── Set up initial stack (16KB above _end)
    │
    ├── Zero BSS section
    │
    ├── Configure privilege modes:
    │   ├── Set MPP to S-mode (for mret)
    │   ├── Delegate exceptions to S-mode
    │   └── Delegate interrupts to S-mode
    │
    ├── Enable S-mode interrupts (SIE register):
    │   ├── SSIE (software)
    │   ├── STIE (timer)
    │   └── SEIE (external)
    │
    ├── Configure PMP (full memory access)
    │
    ├── Set trap vectors:
    │   ├── stvec → interrupt (S-mode traps)
    │   └── mtvec → criticalerr (M-mode traps)
    │
    ├── Set mepc to nulluser
    │
    └── mret → Jump to nulluser in S-mode
```

**Key Register Setup:**
| Register | Value | Purpose |
|----------|-------|---------|
| `sp` | `_end + 16384` | Initial stack pointer |
| `mstatus.MPP` | S-mode | Previous privilege for `mret` |
| `medeleg` | U-mode ecalls | Delegate user syscalls to S-mode |
| `mideleg` | All interrupts | Handle all interrupts in S-mode |
| `stvec` | `interrupt` | S-mode trap handler |
| `mtvec` | `criticalerr` | M-mode trap handler |
| `pmpaddr0` | Max address | Allow S-mode full memory access |
| `mepc` | `nulluser` | Return address for `mret` |

---

### 2. `initialize.c` — System Initialization

**Entry Point:** `nulluser()` — called after `mret` in `start.S`

**Initialization Flow:**
```c
nulluser()
    │
    ├── platforminit()     // Hardware setup
    │
    ├── sysinit()          // OS data structures
    │   ├── Initialize process table
    │   ├── Set up null process (PID 0)
    │   ├── Create ready queue
    │   ├── Initialize clock
    │   └── Seed random number generator
    │
    ├── welcome()          // Print boot info
    │
    ├── pgInit()           // Initialize free page list
    │
    ├── vm_kerninit()      // Enable kernel paging
    │
    ├── main()             // User's main function
    │
    └── Create nullproc and reschedule
```

**Global Variables Initialized:**
| Variable | Type | Description |
|----------|------|-------------|
| `proctab[]` | `pcb[NPROC]` | Process table |
| `readylist` | `qid_typ` | Ready queue ID |
| `numproc` | `int` | Active process count |
| `currpid` | `int` | Current process ID |
| `interruptVector[]` | Function pointers | IRQ handlers |
| `pgfreelist` | Linked list | Free physical pages |

---

### 3. `platforminit.c` — Hardware Setup

**Purpose:** Initialize platform-specific hardware

**Actions:**
1. Set platform identification strings
2. Configure memory bounds (`minaddr`, `maxaddr`)
3. Initialize UART:
   - Disable interrupts
   - Reset FIFOs
   - Enable FIFO mode

**Platform Configuration:**
```c
platform.manufacturer = "Sispeed";
platform.family = "Allwinner D1";
platform.type = "Nezha";
platform.minaddr = 0x0;
platform.maxaddr = 0x78FFFFFF;  // ~2GB
```

---

## Process Management

### `create.c` — Process Creation

**Function:** `syscall create(void *funcaddr, ulong ssize, uint priority, char *name, ulong nargs, ...)`

**Process:**
1. Validate/adjust stack size (minimum `MINSTK`)
2. Allocate stack page via `pgalloc()`
3. Find free process slot via `newpid()`
4. Initialize PCB:
   - State: `PRSUSP`
   - Stack base, length, pointer
   - Priority (tickets for lottery scheduler)
   - Process name
5. Create page table via `vm_userinit()`
6. Initialize stack:
   - Stack magic number
   - Accounting block (PID, stack info)
   - Context area (32 registers)
7. Set initial context:
   - `CTX_PC` → function address
   - `CTX_RA` → `userret` (cleanup on return)
   - `CTX_SP` → stack pointer
8. Copy arguments to argument registers

**Stack Layout (top to bottom):**
```
┌────────────────┐  ← Stack top (saddr + 512)
│  STACKMAGIC    │
├────────────────┤
│      PID       │
├────────────────┤
│   Stack len    │
├────────────────┤
│  Stack base    │
├────────────────┤
│  Extra args    │  (if nargs > 8)
├────────────────┤
│                │
│   Context      │  32 registers
│   (a0-a7,      │
│    s0-s11,     │
│    ra, sp, pc, │
│    etc.)       │
│                │
├────────────────┤  ← stkptr
│                │
│   Available    │
│    Stack       │
│                │
└────────────────┘  ← Stack base
```

---

### `kill.c` — Process Termination

**Function:** `syscall kill(int pid)`

**Process:**
1. Validate PID
2. Decrement `numproc`
3. Handle based on state:
   - `PRCURR`: Mark free, call `resched()` (suicide)
   - `PRREADY`: Remove from queue, mark free
   - Other: Just mark free

---

### `ready.c` — Ready a Process

**Function:** `syscall ready(pid_typ pid, bool resch)`

**Process:**
1. Set process state to `PRREADY`
2. Add to ready queue via `enqueue()`
3. If `resch == RESCHED_YES`, call `resched()`

---

## Scheduling

### `resched.c` — Lottery Scheduler

**Function:** `syscall resched(void)`

**Algorithm:**
1. If current process is running (`PRCURR`):
   - Change state to `PRREADY`
   - Add to ready queue
2. Calculate total tickets from all ready/current processes
3. Generate random ticket number
4. Find winning process:
   - Iterate through process table
   - Accumulate tickets until random ticket is reached
5. Context switch to winning process

**Lottery Scheduling:**
```
Process   Tickets   Cumulative   Range
───────────────────────────────────────
P1        3         3            [0, 3)
P2        1         4            [3, 4)
P3        2         6            [4, 6)

Random ticket = 4 → P3 wins
```

**Helper Function:** `get_total_tickets()`
- Sums tickets from all `PRCURR` and `PRREADY` processes

---

### `ctxsw.S` — Context Switch

**Function:** `void ctxsw(void **oldstack, void **newstack, ulong satp)`

**Parameters:**
- `a0`: Address of old process stack pointer
- `a1`: Address of new process stack pointer
- `a2`: SATP value for new process

**Process:**

**Save Context (Old Process):**
```asm
addi sp, sp, -32*8          # Allocate context frame
sd ra, CTX_PC*8(sp)         # Save return address as PC
sd x1-x31, ...              # Save all registers
sd sp, (a0)                 # Store stack pointer to PCB
```

**Restore Context (New Process):**
```asm
ld sp, (a1)                 # Load new stack pointer
ld x1-x31, ...              # Restore all registers
ld t0, CTX_PC*8(sp)         # Load program counter
addi sp, sp, 32*8           # Deallocate context frame
sd sp, (a1)                 # Update stack pointer in PCB
```

**Mode Handling:**
```asm
beq t0, ra, switch          # If PC == RA, normal return
csrw satp, a2               # Switch page tables
sfence.vma zero, zero       # Flush TLB
csrc sstatus, SSTATUS_S_MODE # Clear S-mode bit
csrw sepc, t0               # Set return PC
sret                        # Return to user mode

switch:
ret                         # Normal function return
```

---

## Interrupt Handling

### `interrupt.S` — Interrupt Entry

**Entry Point:** `interrupt` — set as `stvec` handler

**Flow:**
```
interrupt:
    │
    ├── Save a0 to sscratch
    │
    ├── Save all registers to swap area (SWAPAREAADDR)
    │
    ├── Load kernel page table and stack
    │   from swap area
    │
    ├── Switch to kernel page table (satp)
    │
    ├── Call dispatch(scause, stval, sp, sepc)
    │
    ├── Switch back to process page table
    │
    ├── Restore all registers from swap area
    │
    └── sret → Return from interrupt
```

**Register Save Area:**
- Located at virtual address `SWAPAREAADDR` (0x3FFFFFE000)
- Per-process swap area allocated in `vm_userinit()`
- Contains space for all 32 registers plus kernel SATP and SP

---

### `dispatch.c` — Interrupt Dispatcher

**Function:** `ulong dispatch(ulong cause, ulong val, ulong *frame, ulong *program_counter)`

**Decision Tree:**
```
if (cause > 0):  # Synchronous trap (exception)
    │
    ├── cause == E_ENVCALL_FROM_UMODE (8):
    │   │
    │   ├── Get syscall number from a7
    │   ├── Call syscall_dispatch()
    │   ├── Store return value in a0
    │   └── Increment PC by 4
    │
    └── Other exception:
        └── Call xtrap() to handle/display error

else:  # Asynchronous interrupt
    │
    └── cause == I_SUPERVISOR_EXTERNAL (9):
        │
        ├── Read IRQ number from PLIC
        ├── Acknowledge interrupt
        └── Call registered handler
```

---

### `xtrap.c` — Exception Handler

**Function:** `void xtrap(ulong *frame, ulong cause, ulong address, ulong *pc)`

**Purpose:** Display exception information and halt

**Exception Names:**
| Code | Name |
|------|------|
| 0 | Instruction address misaligned |
| 1 | Instruction access fault |
| 2 | Illegal instruction |
| 3 | Breakpoint |
| 4 | Load address misaligned |
| 5 | Load access fault |
| 8 | Environment call from U-mode |
| 12 | Instruction page fault |
| 13 | Load page fault |
| 15 | Store/AMO page fault |

**Output:**
```
XINU Exception [Illegal instruction]
Faulting code: 0x0000000042001234
Faulting address: 0x0000000078000000
[registers dump...]
```

---

### `criticalerr.S` — Machine Mode Errors

**Purpose:** Handle unrecoverable errors in M-mode

**Process:**
1. Save all registers to fixed address (0x52000000)
2. Extract cause from `mcause`
3. Call `xtrap()` with error info

---

### `syscall_dispatch.c` — System Call Handler

**System Call Table:**

| Code | Name | Handler | Args |
|------|------|---------|------|
| 0 | NONE | `sc_none` | 5 |
| 1 | YIELD | `sc_yield` | 0 |
| 2 | SLEEP | `sc_none` | 1 |
| 3 | KILL | `sc_kill` | 0 |
| 8 | GETC | `sc_getc` | 1 |
| 9 | PUTC | `sc_putc` | 2 |

**User-Mode Wrappers:**

Each `user_*` function uses the `SYSCALL` macro:
```c
#define SYSCALL(num) int status; \
    asm("li a7, %0" : : "i"(SYSCALL_##num)); \
    asm("ecall"); \
    asm("mv %0, a0" : "=r"(status)); \
    return status;
```

**Example:** `user_yield()`:
1. Load syscall number into `a7`
2. Execute `ecall` instruction
3. Trap to S-mode via `interrupt.S`
4. `dispatch()` calls `syscall_dispatch(1, args)`
5. `sc_yield()` calls `resched()`
6. Return value placed in `a0`

---

## Clock & Timer

### `clkinit.c` — Clock Initialization

**Function:** `void clkinit(void)`

**Setup:**
1. Configure Timer0:
   - Interval: 12000 cycles (1ms at 24MHz with prescaler 2)
   - Mode: Periodic
   - Source: 24MHz oscillator
2. Enable Timer0 interrupt
3. Configure PLIC:
   - Set priority for Timer0 (IRQ 75)
   - Enable Timer0 interrupt for S-mode
4. Register `clkhandler` in interrupt vector

**Timer Configuration:**
```c
t->t0_intv = 12000;                    // 1ms interval
t->t0_ctrl = TMR0_MODE_PERIODIC |      // Auto-reload
             TMR0_CLK_SRC_OSC24M |     // 24MHz clock
             TMR0_CLK_PRES_2;          // Prescaler 2
t->t0_ctrl |= TMR0_RELOAD;             // Load interval
t->t0_ctrl |= TMR0_EN;                 // Enable timer
t->irq_en = TMR0_IRQ_EN;               // Enable interrupt
```

---

### `clkhandler.c` — Clock Interrupt Handler

**Function:** `interrupt clkhandler(void)`

**Actions:**
1. Increment `clkticks`
2. Clear pending interrupt
3. Reload and re-enable timer
4. If 1000 ticks reached:
   - Increment `clktime` (seconds)
   - Reset `clkticks`
5. Decrement preemption counter
6. If `preempt <= 0`, call `resched()`

**Preemption:**
- `QUANTUM = 3` — Preempt every 3ms
- `preempt` reset to `QUANTUM` after each reschedule

---

## Memory Management

### `pgInit.c` — Page List Initialization

**Function:** `void pgInit(void)`

**Process:**
1. Calculate total pages: `(maxaddr - memheap) / PAGE_SIZE`
2. Call `pgfreerange(memheap, maxaddr)`

---

### `pgalloc.c` — Page Allocation

**Function:** `void *pgalloc(void)`

**Process:**
1. If `pgfreelist == NULL`, return `SYSERR`
2. Remove first page from free list
3. Zero the page (`bzero`)
4. Return page address

---

### `pgFree.c` — Page Freeing

**Functions:**

`syscall pgfreerange(void *start, void *end)`:
- Iterate from `start` to `end` by `PAGE_SIZE`
- Call `pgfree()` for each page

`syscall pgfree(void *addr)`:
- Validate page alignment
- Add page to head of `pgfreelist`

---

### `map.c` — Virtual Address Mapping

**Functions:**

`syscall mapPage(pgtbl pagetable, page pg, ulong virtualaddr, int attr, ulong physicaladdr)`:
- Map single page at virtual address

`syscall mapAddress(pgtbl pagetable, ulong virtualaddr, ulong physicaladdr, ulong length, int attr)`:
- Map range of addresses (calls `mapPage` in loop)

**Page Table Traversal (`pgTraverseAndCreate`):**

```
Virtual Address (39-bit Sv39):
┌────────┬────────┬────────┬────────────┐
│ VPN[2] │ VPN[1] │ VPN[0] │   Offset   │
│ 9 bits │ 9 bits │ 9 bits │  12 bits   │
└────────┴────────┴────────┴────────────┘

Level 2 (Root)     Level 1           Level 0 (Leaf)
┌──────────┐      ┌──────────┐      ┌──────────┐
│          │      │          │      │          │
│──────────│      │──────────│      │──────────│
│  VPN[2]  │─────►│  VPN[1]  │─────►│  VPN[0]  │────► Physical Page
│──────────│      │──────────│      │──────────│
│          │      │          │      │          │
└──────────┘      └──────────┘      └──────────┘
```

**Algorithm:**
1. Extract VPN[2], VPN[1], VPN[0] from virtual address
2. For each level:
   - If PTE valid, follow to next level
   - If not valid, allocate new page table via `pgalloc()`
3. At leaf level, set physical address and attributes

---

### `vm_kerninit.c` — Kernel Page Tables

**Function:** `void vm_kerninit(void)`

**Mappings Created:**

| Virtual Range | Physical Range | Permissions |
|---------------|----------------|-------------|
| UART (0x2500000) | Same | R, W |
| Kernel code | Same | R, X |
| Context switch | Same | R, X (page-aligned) |
| Interrupt code | Same | R, X (page-aligned) |
| Kernel data | Same | R, W |
| Heap/RAM | Same | R, W |

**Actions:**
1. Allocate root page table
2. Create identity mappings for kernel
3. Store page table in current PCB
4. Set `_kernpgtbl` and `_kernsp` globals
5. Activate via `set_satp()`

---

### `vm_userinit.c` — User Page Tables

**Function:** `pgtbl vm_userinit(int pid, page stack)`

**Mappings Created:**

| Virtual Range | Physical Range | Permissions |
|---------------|----------------|-------------|
| UART | Same | R, W, U |
| Kernel code | Same | R, X, U |
| Context switch | Same | R, X |
| Interrupt code | Same | R, X |
| Kernel data | Same | R, U |
| Process stack | Allocated | R, W, U |
| Swap area | Allocated | R, W |

**Key Points:**
- User code can read kernel data but not write
- Context switch and interrupt code not user-accessible
- Per-process swap area stores kernel SATP and SP for interrupt handling

---

### `mmu.S` — MMU Operations

**Function:** `void set_satp(unsigned long)`

**Process:**
```asm
sfence.vma zero, zero   # Flush TLB
csrw satp, a0           # Set page table register
sfence.vma zero, zero   # Flush TLB again
ret
```

---

## Queue Operations

### `queue.c` — Process Queues

**Global:** `struct qentry queuetab[NQENT]`

**Queue Structure:**
- Entries 0 to NPROC-1: Process entries
- Entries NPROC+: Head/tail sentinels

**Queue ID Encoding:**
```c
qid = (head << 16) | tail
```

**Functions:**

`pid_typ enqueue(pid_typ pid, qid_typ q)`:
- Insert at tail of queue

`pid_typ dequeue(qid_typ q)`:
- Remove from head of queue

`pid_typ remove(pid_typ pid)`:
- Remove from anywhere in queue

`qid_typ newqueue(void)`:
- Allocate head/tail pair
- Return encoded queue ID

---

## Console I/O

### `kprintf.c` — Kernel I/O

**Functions:**

`syscall kgetc()`:
1. Check unget buffer first
2. Poll UART LSR for data ready
3. Return received character

`syscall kputc(uchar c)`:
1. Poll UART LSR for transmit ready
2. Write character to THR

`syscall kprintf(const char *format, ...)`:
- Formatted output using `_doprnt()`

`syscall kungetc(unsigned char c)`:
- Push character back to buffer (max 10)

`syscall kcheckc(void)`:
- Check if character available (non-blocking)

---

## Utility Functions

### `random.c` — Random Numbers

**Algorithm:** Linear congruential generator

```c
seed = (seed * 1103515245) + 12345;
return (seed >> 16) % max;
```

**Functions:**
- `seed_random(uint x)` — Set seed
- `random(uint max)` — Return random in [0, max)

---

### `getstk.c` — Legacy Stack Allocation

**Function:** `void *getstk(ulong nbytes)`

**Note:** This is a broken placeholder. Allocates from top of memory downward. Should be replaced with proper heap allocation.

---

## Test Suite

### `testcases.c` — Test Functions

**Test Cases:**

| Key | Test |
|-----|------|
| `0` | Create process and print page table |
| `1` | Test memory protection (TODO) |
| `2` | Test read-only kernel access (TODO) |
| `3` | Null pointer exception (extra credit) |
| `4` | Print fake page table structure |

**Helper Functions:**

`void printPageTable(pgtbl pagetable)`:
- Recursively print valid page table entries
- Distinguishes leaf vs. non-leaf entries

`pgtbl createFakeTable(void)`:
- Create test page table for debugging

---

## Build Rules

### `Makerules`

**Content:**
```makefile
COMP = system
DIR = ${TOPDIR}/${COMP}
COMP_SRC += ${DIR}/start.S $(wildcard ${DIR}/*.S) \
            ${DIR}/initialize.c $(wildcard ${DIR}/*.c)
```

**Purpose:** Add all `.S` and `.c` files to build, with `start.S` and `initialize.c` first to ensure proper link order.
