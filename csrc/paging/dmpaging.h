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

typedef uint8_t *vm_address_t; /** an address that may be in physical or paged memory */
typedef uint8_t *paging_address_t; /** an address that may be in paged memory */

/* Basic memory access macros. */
#if (PF_DEMAND_PAGING == 0)
/* Straight memory access. */
#define DP_FETCH_U8(address)    (*((const uint8_t *)(address)))
#define DP_FETCH_U16(address)   (*((const uint16_t *)(address)))
#define DP_FETCH_CELL(address)  (*((const cell_t *)(address)))
#define DP_FETCH_FLOAT(address) (*((const PF_FLOAT *)(address)))

#define DP_STORE_U8(address, value)    *((uint8_t *)(address)) = (uint8_t)(value)
#define DP_STORE_U16(address, value)   *((uint16_t *)(address)) = (uint16_t)(value)
#define DP_STORE_CELL(address, value)  *((cell_t *)(address)) = (cell_t)(value)
#define DP_STORE_FLOAT(address, value) *((PF_FLOAT *)(address)) = (PF_FLOAT)(value)
#else
/* Use either physical or paged memory. */
#define DP_FETCH_U8(address)    pfFetchVirtualU8((const uint8_t *)(address))
#define DP_FETCH_U16(address)   pfFetchVirtualU16((const uint16_t *)(address))
#define DP_FETCH_CELL(address)  pfFetchVirtualCell((const cell_t *)(address))
#define DP_FETCH_FLOAT(address) pfFetchVirtualFloat((const PF_FLOAT *)(address))

#define DP_STORE_U8(address, value)    pfStoreVirtualU8(((uint8_t *)(address)), (uint8_t)(value))
#define DP_STORE_U16(address, value)   pfStoreVirtualU16(((uint16_t *)(address)), (uint16_t)(value))
#define DP_STORE_CELL(address, value)  pfStoreVirtualCell(((cell_t *)(address)), (cell_t)(value))
#define DP_STORE_FLOAT(address, value) pfStoreVirtualFloat(((PF_FLOAT *)(address)), (PF_FLOAT)(value))
#endif

uint8_t  pfFetchVirtualU8(const uint8_t *address);
uint16_t pfFetchVirtualU16(const uint16_t *address);
cell_t   pfFetchVirtualCell(const cell_t *address);

void pfStoreVirtualU8(uint8_t *address, uint8_t value);
void pfStoreVirtualU16(uint16_t *address, uint16_t value);
void pfStoreVirtualCell(cell_t *address, cell_t value);

#ifdef PF_SUPPORT_FP
PF_FLOAT pfFetchVirtualFloat(const PF_FLOAT *address);
void pfStoreVirtualFloat(PF_FLOAT *address, PF_FLOAT value);
#endif /* PF_SUPPORT_FP */

/* Memory region locking and unlocking
 * A maximum of DP_MAX_REGIONS can be locked at one time.
 * The regions cannot overlap.
 * Adjacent virtual regions will not be adjacent in physical memory!
 */

/* maximum number of locked regions */
#define DP_MAX_REGIONS        (4)
/* maximum number of bytes per region */
#define DP_MAX_REGION_SIZE  (256)

void pfResetLockedMemory(void);

/**
 * Lock a read-only region of demand paging in physical memory.
 * Load the data from demand paging if not already loaded.
 * A region should only be locked temporarily, for example within
 * a single primitive.
 * If the virtual memory address is not in paged memory then it will be passed through.
 * You cannot lock more than DP_MAX_REGION_SIZE bytes.
 * @return physical address that can be used by normal C code as “const” memory.
 */
const uint8_t *pfLockMemoryReadOnly(vm_address_t vp, uint32_t numBytes);

/**
 * Lock a read-write region of demand paging in physical memory.
 * Load the data from demand paging if not already loaded.
 * If the virtual memory address is not in paged memory then it will be passed through.
 * You cannot lock more than DP_MAX_REGION_SIZE bytes.
 * @return physical address that can be read or written by normal C code.
 */
uint8_t *pfLockMemoryReadWrite(vm_address_t vp, uint32_t numBytes);

/**
 * Release a previously locked region of memory.
 * Writable regions will be written back to serial memory.
 * Regions may be kept in physical memory on an LRU basis
 * to improve performance.
 * @param vp virtual memory address
 * @param pp physical memory address
 * @return negative code if an error occured else zero
 */
int pfUnlockMemory(vm_address_t vp, const uint8_t *pp);

/** Copy from virtual to physical memory.
 * @param destination in physical memory
 * @param source The data may be in physical or paged memory.
 * @return destination
 */
void *pfCopyFromVirtualMemory(void *destination,
                              vm_address_t source,
                              uint32_t numBytes);

/** Copy from physical to virtual memory.
 * @param destination The target may be in physical or paged memory.
 * @param source in physical memory
 * @return destination
 */
void *pfCopyToVirtualMemory(vm_address_t destination,
                            void * source,
                            uint32_t numBytes);

/** Free virtual memory.
 * @param address may be in physical or paged memory
 */
void pfFreeVirtualMemory(vm_address_t address);

/** Set a region of virtual memory to a byte value.
 * @param destination in physical or paged memory
 * @param value fill memory with this value
 * @param numBytes how many bytes to set
 * @return destination
 */
void *pfSetVirtualMemory(vm_address_t destination,
                         uint8_t value,
                         uint32_t numBytes);

#ifdef __cplusplus
}
#endif

#endif /* _pf_dmpaging_h */
