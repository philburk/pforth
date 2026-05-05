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

#ifndef PF_DEMAND_PAGING
#define PF_DEMAND_PAGING 1
#endif

typedef uint8_t *vm_address_t; /** an address that may be in physical or paged memory */
typedef uint8_t *paging_address_t; /** an address that may be in paged memory */

/* Basic memory access macros. */
#if (PF_DEMAND_PAGING == 0)
/* Straight memory access. */
#define DP_FETCH_U8(address)    (*((uint8_t *)(address)))
#define DP_FETCH_U16(address)   (*((uint16_t *)(address)))
#define DP_FETCH_CELL(address)  (*((cell_t *)(address)))
#define DP_FETCH_FLOAT(address) (*((PF_FLOAT *)(address)))

#define DP_STORE_U8(address, value)    *((uint8_t *)(address)) = (uint8_t)(value)
#define DP_STORE_U16(address, value)   *((uint16_t *)(address)) = (uint16_t)(value)
#define DP_STORE_CELL(address, value)  *((cell_t *)(address)) = (cell_t)(value)
#define DP_STORE_FLOAT(address, value) *((PF_FLOAT *)(address)) = (PF_FLOAT)(value)
#else
/* Use either physical or paged memory. */
#define DP_FETCH_U8(address)    pfFetchVirtualU8((uint8_t *)(address))
#define DP_FETCH_U16(address)   pfFetchVirtualU16((uint16_t *)(address))
#define DP_FETCH_CELL(address)  pfFetchVirtualCell((cell_t *)(address))
#define DP_FETCH_FLOAT(address) pfFetchVirtualFloat((PF_FLOAT *)(address))

#define DP_STORE_U8(address, value)    pfStoreVirtualU8(((uint8_t *)(address)), (uint8_t)(value))
#define DP_STORE_U16(address, value)   pfStoreVirtualU16(((uint16_t *)(address)), (uint16_t)(value))
#define DP_STORE_CELL(address, value)  pfStoreVirtualCell(((cell_t *)(address)), (cell_t)(value))
#define DP_STORE_FLOAT(address, value) pfStoreVirtualFloat(((PF_FLOAT *)(address)), (PF_FLOAT)(value))
#endif

uint8_t  pfFetchVirtualU8(uint8_t *address);
uint16_t pfFetchVirtualU16(uint16_t *address);
cell_t   pfFetchVirtualCell(cell_t *address);
PF_FLOAT pfFetchVirtualFloat(PF_FLOAT *address);

void pfStoreVirtualU8(uint8_t *address, uint8_t value);
void pfStoreVirtualU16(uint16_t *address, uint16_t value);
void pfStoreVirtualCell(cell_t *address, cell_t value);
void pfStoreVirtualFloat(PF_FLOAT *address, PF_FLOAT value);

/* Memory region locking and unlocking
 * A maximum of DP_MAX_REGIONS can be locked at one time.
 * The regions cannot overlap.
 * Adjacent virtual regions will not be adjacent in physical memory!
 */

/* maximum number of locked regions */
#define DP_MAX_REGIONS        (4)
/* maximum number of bytes per region */
#define DP_MAX_REGION_SIZE  (256)
/* typical timeout value for accessing serial memory */
#define DP_TIMEOUT_MICROS  50000

void pfResetLockedMemory(void);

/**
 * Lock a read-only region of demand paging in physical memory.
 * Load the data from demand paging if not already loaded.
 * A region should only be locked temporarily, for example within
 * a single primitive.
 * If the virtual memory address is not in paged memory then it will be passed through.
 * @return physical address that can be used by normal C code as “const” memory.
 */
const uint8_t *pfLockMemoryReadOnly(vm_address_t vp, cell_t numBytes);

/**
 * Lock a read-write region of demand paging in physical memory.
 * Load the data from demand paging if not already loaded.
 * If the virtual memory address is not in paged memory then it will be passed through.
 * @return physical address that can be used by normal C code as “const” memory.
 */
uint8_t *pfLockMemoryReadWrite(vm_address_t vp, cell_t numBytes);

/**
 * Release a previously locked region of memory.
 * Writable regions will be written back to serial memory.
 * Regions may be kept in physical memory on an LRU basis
 * to improve performance.
 * @param vp virtual memory address
 * @param pp physical memory address
 * @return negative code if an error occured
 */
int pfUnlockMemory(vm_address_t vp, const uint8_t *pp);

/** Copy from virtual to physical memory synchronously.
 * @param destination in physical memory
 * @param source The data may be in physical or paged memory.
 * @return destination
 */
void *pfCopyFromVirtualMemory(void *destination,
                              vm_address_t source,
                              size_t numBytes);

/* Serial Memory Access
 * A demand paging simulator is provided for testing the framework on a host.
 * The actual demand paging interface must be provided by the user.
 */
#define DP_ALIGNMENT_SIZE    (16)

/** Is the given address pointing to paged memory?
 * @return 1 if in paged memory, else 0
 */
int pfIsAddressInPagedMemory(void *p);

/**
 * Reset the memory allocator.
 * This may or may not clear the memory.
 */
void pfResetPagedMemory(void);

/** Allocate demand paged memory from SPI or other storage.
 * Memory blocks will be aligned on DP_ALIGNMENT_SIZE byte boundaries.
 */
paging_address_t pfAllocatePagedMemory(ucell_t numBytes);

/** Free demand paged memory.
 * This may simply be a NOOP. So just allocate
 * what you need at the beginning and don't rely on being able to free it.
 */
void pfFreePagedMemory(vm_address_t p);

/** Read from virtual to physical memory.
 * Only one async read or write can be pending at a time.
 * @param micros if zero then issue an async transfer, else timeout in micros
 * @return number of bytes read or 0 if timed out
 */
size_t pfReadPagedMemory(void *destination,
                      paging_address_t source,
                      size_t numBytes,
                      int micros);

/* Wait for asynchronous paged memory read or write to complete.
 * This is useful for read-ahead buffering.
 * return 0 if finished in under the specified microseconds
 */
int pfWaitAsyncPagedMemoryAccess(int micros);

/** Read from virtual to physical memory.
 * Only one async read or write can be pending at a time.
 * @param micros if zero then issue an async transfer, else timeout in micros
 * @return number of bytes written or 0 if timed out
 */
size_t pfWritePagedMemory(paging_address_t destination,
                       void *source,
                       size_t numBytes,
                       int micros);

#ifdef __cplusplus
}
#endif


#endif /* _pf_dmpaging_h */
