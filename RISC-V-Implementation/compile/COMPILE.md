# Compile Directory Documentation

This directory contains the build system for Embedded Xinu on RISC-V. It uses GNU Make with a hierarchical configuration that supports multiple platforms.

---

## Directory Structure

```
compile/
├── Makefile                    # Main build orchestrator
├── mkvers.sh                   # Version string generator
├── vn                          # Build number counter
├── version                     # Generated version string
├── arch/
│   └── riscv/
│       ├── platformVars        # RISC-V architecture defaults
│       └── ld.script           # Linker script
└── platforms/
    ├── riscv-qemu/
    │   └── platformVars        # QEMU-specific settings
    └── nezha/
        └── platformVars        # Nezha board settings
```

---

## Building the Kernel

### Basic Build

```bash
cd compile
make                    # Build for default platform (Nezha)
```

### Platform Selection

```bash
make PLATFORM=nezha         # Sipeed Nezha D1 board (default)
make PLATFORM=riscv-qemu    # QEMU RISC-V emulator
```

### Build Targets

| Target | Description |
|--------|-------------|
| `make` | Build `xinu.boot` (default) |
| `make xinu.elf` | Build ELF executable only |
| `make clean` | Remove object files |
| `make realclean` | Remove everything including version files |
| `make debug` | Build with debug symbols |
| `make qemu` | Build and run in QEMU |
| `make qemu-debug` | Build and run in QEMU with GDB server |

### Running in QEMU

```bash
make PLATFORM=riscv-qemu
make qemu
```

Or with debugging:
```bash
make qemu-debug    # Starts QEMU waiting for GDB on port 1234
# In another terminal:
riscv64-linux-gnu-gdb xinu.elf
(gdb) target remote :1234
```

---

## Build Outputs

| File | Description |
|------|-------------|
| `xinu.elf` | ELF executable with debug symbols |
| `xinu.bin` | Raw binary image |
| `xinu.boot` | U-Boot bootable image (Nezha only) |

---

## File Reference

### `Makefile` — Main Build File

**Key Variables:**

| Variable | Default | Description |
|----------|---------|-------------|
| `PLATFORM` | `nezha` | Target platform |
| `BOOTIMAGE` | `xinu.boot` | Final output image name |
| `TOPDIR` | `..` | Path to source root |

**Compiler Flags (`CFLAGS`):**

| Flag | Purpose |
|------|---------|
| `-c` | Compile only, no linking |
| `-Os` | Optimize for size |
| `-g -gdwarf` | Include debug information |
| `-Wall` | Enable warnings |
| `-Wstrict-prototypes` | Require function prototypes |
| `-nostdinc` | Don't use system headers |
| `-fno-builtin` | Don't replace with gcc builtins |
| `-fno-strict-aliasing` | Safe pointer aliasing |
| `-ffunction-sections` | Enable dead code elimination |
| `-fwrapv` | Defined signed overflow behavior |

**Linker Flags (`LDFLAGS`):**

| Flag | Purpose |
|------|---------|
| `--static` | Static linking |
| `--gc-sections` | Remove unused sections |
| `-T$(LDSCRIPT)` | Use custom linker script |

**Build Process:**

1. Include `platforms/$(PLATFORM)/platformVars`
2. Include `$(TOPDIR)/system/Makerules` for source files
3. Compile all `.c` and `.S` files to `.o`
4. Link into `xinu.elf`
5. Convert to platform-specific boot image

---

### `arch/riscv/platformVars` — RISC-V Architecture Settings

**Configuration:**

```makefile
TEMPLATE_ARCH := riscv
ARCH_PREFIX := riscv64-linux-gnu-
PLATFORM_NAME := RISC-V 64-bit
```

**Architecture-Specific Flags:**

| Flag | Purpose |
|------|---------|
| `-march=rv64g` | Target RV64G (IMAFD extensions) |
| `-fno-stack-protector` | No stack canaries |
| `-mcmodel=medany` | Medium/any code model for large addresses |
| `-mstrict-align` | No unaligned memory access |

**Object Copy Flags:**
```makefile
OCFLAGS := -I binary -O elf64-littleriscv -B riscv
```

**Define:**
- `-D_XINU_ARCH_RISCV_` — For conditional compilation

---

### `arch/riscv/ld.script` — Linker Script

**Purpose:** Defines memory layout for the kernel image.

**Entry Point:**
```
ENTRY(_start)
```

**Base Address:** `0x42000000`

**Sections:**

| Section | Address | Contents |
|---------|---------|----------|
| `.init` | 0x42000000 | `_start` entry point |
| `.text` | Page-aligned | Code + read-only data |
| `.ctxswsec` | Page-aligned | Context switch code |
| `.interruptsec` | Page-aligned | Interrupt handler code |
| `.data` | Page-aligned | Initialized data |
| `.bss` | Page-aligned | Uninitialized data |

**Symbols Exported:**

| Symbol | Description |
|--------|-------------|
| `_ctxsws` / `_ctxswe` | Start/end of context switch section |
| `_interrupts` / `_interrupte` | Start/end of interrupt section |
| `_datas` | Start of data section |
| `_bss` | Start of BSS section |
| `_end` | End of kernel image |

**Memory Layout:**
```
0x42000000  ┌─────────────┐
            │   .init     │  _start
            ├─────────────┤
            │   .text     │  Kernel code
            ├─────────────┤
            │ .ctxswsec   │  Context switch (page-aligned)
            ├─────────────┤
            │.interruptsec│  Interrupt handler (page-aligned)
            ├─────────────┤
            │   .data     │  Initialized data
            ├─────────────┤
            │   .bss      │  Uninitialized data
            ├─────────────┤
            │             │  _end
            │   Heap      │  Dynamic memory
            │             │
0x78FFFFFF  └─────────────┘  platform.maxaddr
```

---

### `platforms/nezha/platformVars` — Sipeed Nezha Configuration

**Platform Info:**
```makefile
PLATFORM_NAME := Sipeed Nezha
```

**Define:**
- `-D_XINU_PLATFORM_RISCV_NEZHA_`

**Build Target:**

The Nezha board uses U-Boot, so the kernel must be packaged as a U-Boot image:

```makefile
UBOOTOPTS := -A riscv -O u-boot -T kernel -a 0x42000000 \
             -C none -e 0x42000000 -n Xinu

$(BOOTIMAGE): xinu.bin
    mkimage $(UBOOTOPTS) -d xinu.bin xinu.boot

xinu.bin: xinu.elf
    $(OBJCOPY) -O binary $^ $@
```

**U-Boot Options:**
| Option | Value | Description |
|--------|-------|-------------|
| `-A riscv` | | Architecture: RISC-V |
| `-O u-boot` | | OS type: U-Boot |
| `-T kernel` | | Image type: Kernel |
| `-a 0x42000000` | | Load address |
| `-e 0x42000000` | | Entry point |
| `-n Xinu` | | Image name |
| `-C none` | | No compression |

---

### `platforms/riscv-qemu/platformVars` — QEMU Configuration

**Platform Info:**
```makefile
PLATFORM_NAME := RISCV QEMU
```

**Define:**
- `-D_XINU_PLATFORM_RISCV_QEMU_`

**Build Target:**

QEMU can boot raw binaries directly:

```makefile
$(BOOTIMAGE): xinu.elf
    $(OBJCOPY) -O binary $^ $@
```

**QEMU Command (from Makefile):**
```bash
qemu-system-riscv64 -machine virt -bios none -kernel xinu.elf \
                    -m 128M -smp 1 -nographic
```

| Option | Description |
|--------|-------------|
| `-machine virt` | Use QEMU's generic RISC-V machine |
| `-bios none` | No firmware, boot kernel directly |
| `-kernel xinu.elf` | Kernel image to load |
| `-m 128M` | 128MB of RAM |
| `-smp 1` | Single CPU |
| `-nographic` | Serial console only |

---

### `mkvers.sh` — Version Generator

**Purpose:** Generates a version string with build metadata.

**Usage:**
```bash
sh mkvers.sh platformobj
```

**Process:**
1. Increment build number in `vn`
2. Get username and hostname
3. Generate version string with timestamp
4. Write to `version` file

**Output Format:**
```
(Embedded Xinu) (nezhaobj) #154 (user@host) Fri Apr 5 03:18:58 PM CDT 2024
```

---

### `vn` — Version Number

Contains the current build number (integer). Incremented on each build.

```
154
```

---

### `version` — Generated Version String

Contains the full version string generated by `mkvers.sh`.

```
(Embedded Xinu) (nezhaobj) #154 (dohear@traken.mscsnet.mu.edu) Fri Apr 5 03:18:58 PM CDT 2024
```

This string is embedded in `include/version.h` and displayed at boot.

---

## Cross-Compiler Setup

The build system expects a RISC-V cross-compiler with the prefix `riscv64-linux-gnu-`.

**Required Tools:**
- `riscv64-linux-gnu-gcc` — C compiler
- `riscv64-linux-gnu-ld` — Linker
- `riscv64-linux-gnu-ar` — Archiver
- `riscv64-linux-gnu-objcopy` — Binary converter
- `riscv64-linux-gnu-strip` — Symbol stripper

**For Nezha board:**
- `mkimage` — U-Boot image tool (from `u-boot-tools` package)

**Override Compiler Path:**
```bash
make COMPILER_ROOT=/path/to/toolchain/bin/riscv64-unknown-elf-
```

---

## Customization

### Adding a New Platform

1. Create `platforms/newplatform/platformVars`
2. Include architecture defaults:
   ```makefile
   include arch/riscv/platformVars
   PLATFORM_NAME := My New Platform
   DEFS += -D_XINU_PLATFORM_NEWPLATFORM_
   ```
3. Define boot image target if needed
4. Build with `make PLATFORM=newplatform`

### Adding Source Files

Edit `system/Makerules` to add new source files:
```makefile
COMP_SRC += ${DIR}/myfile.c
```

### Conditional Compilation

Use platform defines in C code:
```c
#ifdef _XINU_PLATFORM_RISCV_NEZHA_
    // Nezha-specific code
#endif

#ifdef _XINU_PLATFORM_RISCV_QEMU_
    // QEMU-specific code
#endif
```
