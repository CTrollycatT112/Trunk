## Trunk week 3 report

Week 3 Is the smallest report, as I was gone half of It.
Nothing really got done, technically we stepped back, but for a good reason..

## Week 3

# Memory management

Our previous memory management system was completely broken..
I didn't set It up properly, so nothing worked.

After failing to fix it, i decided to restart and do it properly.

Now, I have just finished up the physical memory layer.
Everything is working with it, next will be the virtual memory manager

# Warnings

I cleaned up some compiler warnings, including:

ignoring return types - memblock
const char - version

# Linker

Replaced .bss with .stack in trunk.ld

# Renaming

I renamed a bunch of folders and files, to fit a better style.

# Planned drivers

For the far future, i listed out all of the drivers i plan to support..

cfiles/drv/

# Page faults

Hooked up our fault handler to interrupt 14 (page fault)

ENDING VERSION OF TRUNK AS OF WEEK 3:
0.16.16
