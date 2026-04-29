#ifndef _pf_dmpaging_h
#define _pf_dmpaging_h

/***************************************************************
** Include file for PForth Demand Paging
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

typedef cell_t vm_address_t;

/* Basic memory access macros. */
#define DP_FETCH_CELL(address) (*((cell_t *)(address)))
#define DP_FETCH_U8(address)   (*((uint8_t *)(address)))
#define DP_FETCH_U16(address)  (*((uint16_t *)(address)))

#define DP_STORE_CELL(address, value) *((cell_t *)(address)) = (cell_t)(value)
#define DP_STORE_U8(address, value)   *((uint8_t *)(address)) = (uint8_t)(value)
#define DP_STORE_U16(address, value)  *((uint16_t *)(address)) = (uint16_t)(value)

/* Memory region locking and unlocking
 * A maximum of DP_MAX_REGIONS can be locked at one time. The regions cannot overlap.
 * Adjacent virtual regions will not be adjacent in physical memory!
 */

/* maximum number of locked regions */
#define DP_MAX_REGIONS        (4)
/* maximum number of bytes per region */
#define DP_MAX_REGION_SIZE  (256)

/**
 * Lock a read-only region of demand paging in physical memory.
 * Load the data from demand paging if not already loaded.
 * A region should only be locked temporarily, for example within
 * a single primitive.
 * A physical address will be passed through.
 * @return physical address that can be used by normal C code as “const” memory.
 */
#define DP_LOCK_READ_ONLY(virtual_address, numbytes)

/**
 * Lock a read-write region of demand paging in physical memory.
 * Load the data from demand paging if not already loaded.
 * @return physical address that can be used by normal C code as “const” memory.
 */
#define DP_LOCK_READ_WRITE(virtual_address, numbytes)

/**
 * Release a previously locked region of memory.
 * Writable regions will be written back to serial memory.
 * Regions may be kept in physical memory on an LRU basis
 * to improve performance.
 */
#define DP_UNLOCK(virtual_address)

/* Serial Memory Access
 * A demand paging simulator is provided for testing the framework on a host.
 * The actual demand paging interface must be provided by the user.
 */

/** Free demand paged memory. */
int pfIsAddressInPagedMemory(vm_address_t p);

/** Allocate demand paged memory from SPI or other storage.
 */
vm_address_t pfAllocatePagedMemory(size_t numBytes);

/** Free demand paging */
void pfFreePagedMemory(vm_address_t p);

/** Read from virtual to physical memory.
 * Only one async read can be pending at a time.
 */
int pfReadPagedMemory(uint8_t *destination,
                      vm_address_t source,
                      size_t numBytes,
                      int sync);

/* Wait for asynchronous virtual read to complete.
 * This is useful for read-ahead buffering. */
int pfWaitPendingVirtualRead(void);

int pfWritePagedMemory(vm_address_t destination,
                       uint8_t *source,
                       size_t numBytes);

#ifdef __cplusplus
}
#endif


#endif /* _pf_dmpaging_h */
