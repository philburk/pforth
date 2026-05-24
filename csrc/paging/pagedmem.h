#ifndef _pf_pagedmem_h
#define _pf_pagedmem_h

/***************************************************************
** Include file for PForth Paged Memory
**
** If you want to use demand paging then you will need to replace
** the paged memory simulator in pagedmem.c with your own function.
**
** Author: Phil Burk
** Copyright 1994 3DO, Phil Burk, Larry Polansky, David Rosenboom
**
** Permission to use, copy, modify, and/or distribute this
** software for any purpose with or without fee is hereby granted.
**
** THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
** WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
** WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL
** THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR
** CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING
** FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
** CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
** OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
**
***************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/* Serial Memory Access
 * A demand paging simulator is provided for testing the framework on a host.
 * The actual demand paging interface must be provided by the user.
 */

/** Is the given address pointing to paged memory?
 * @return 1 if in paged memory, else 0
 */
int pfIsAddressInPagedMemory(vm_address_t p);

#if PF_DEMAND_PAGING
#define DP_ALIGNMENT_SIZE    (16)

/**
 * Reset the memory allocator.
 * This may or may not clear the memory.
 */
void pfResetPagedMemory(void);

/**
 * Check the paged memory for corruption.
 * This is only implemented for the memory simulator.
 * This can be a NOOP in a real implementation.
 * @return zero if memory untouched else number of bad bytes
 */
int pfCheckPagedMemory(void);

/** Allocate demand paged memory from SPI or other storage.
 * Memory blocks will be aligned on DP_ALIGNMENT_SIZE byte boundaries.
 */
paging_address_t pfAllocatePagedMemory(ucell_t numBytes);

/** Free demand paged memory.
 * This may simply be a NOOP. So just allocate
 * what you need at the beginning and don't rely on being able to free it.
 */
void pfFreePagedMemory(vm_address_t p);

/** Read from Paged to physical memory.
 * @return number of bytes read or 0 if timed out
 */
size_t pfReadPagedMemory(void *destination,
                         paging_address_t source,
                         uint32_t numBytes);

/** Write to Paged memory from Physical memory.
 * @return number of bytes written or 0 if timed out
 */
size_t pfWritePagedMemory(paging_address_t destination,
                          const void *source,
                          uint32_t numBytes);

#endif /* PF_DEMAND_PAGING */

#ifdef __cplusplus
}
#endif

#endif /* _pf_pagedmem_h */
