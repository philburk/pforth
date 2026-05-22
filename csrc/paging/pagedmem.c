/***************************************************************
** Demand Paged Memory Simulator
**
** This should be replaced by a hardware specific implementation
** when doing actual demand paging. The real code may, for example,
** access SPI RAM.
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

#include "../pf_all.h"
#include "dmpaging.h"

#if PF_DEMAND_PAGING

#ifndef PF_DP_AVAILABLE_SPACE
#define PF_DP_AVAILABLE_SPACE   (512*1024)
#endif

static cell_t sDpNextAvailable = 0;
#define DP_ALIGNMENT_MASK (DP_ALIGNMENT_SIZE - 1)

/* By munging the virtual address, we can cause memory errors if it is not converted.
 * This will help us catch areas where we need to use functions like pfLockMemoryReadWrite().
 * You may need to use a different mask on your system depending on the memory layout.
 */
#define MUNGE_BY_OFFSET 0
#if (MUNGE_BY_OFFSET)
    /* Allocate twice the needed space. Munged addresses point to the high half. */
    static uint8_t sFakeSerialRAM[2 * PF_DP_AVAILABLE_SPACE];
    /* Munge by offsetting the address. This might be helpful if high address bits are ignored,
     * causing the XOR bits to be ignored. */
    #define PF_DP_MUNGE(addr) ((vm_address_t)((PTR_TO_VMA(addr)) + PF_DP_AVAILABLE_SPACE))
    #define PF_DP_UNMUNGE(paging_addr) ((uintptr_t)((paging_addr) - PF_DP_AVAILABLE_SPACE))
    #define DEAD_MARKER  ((uint8_t)0x00)
#else
    static uint8_t sFakeSerialRAM[PF_DP_AVAILABLE_SPACE];
    #if (PF_POINTER_SIZE == 8)
    #define PF_DP_MUNGE_KEY  (0x005A000000000000)
    #elif (PF_POINTER_SIZE == 4)
    #define PF_DP_MUNGE_KEY  (0x50000000)
    #endif

    #define PF_DP_MUNGE(addr) ((vm_address_t)((PTR_TO_VMA(addr)) ^ PF_DP_MUNGE_KEY))
    #define PF_DP_UNMUNGE(paging_addr) ((uintptr_t)((paging_addr) ^ PF_DP_MUNGE_KEY))
#endif

void pfResetPagedMemory(void) {
    printf("pfResetPagedMemory: cell = %d\n", (int)sizeof(cell_t));
    sDpNextAvailable = 0;

#if (MUNGE_BY_OFFSET)
    /* Fill memory that should not be touched with a marker. */
    memset(&sFakeSerialRAM[PF_DP_AVAILABLE_SPACE], DEAD_MARKER, PF_DP_AVAILABLE_SPACE);
#endif
}

int pfCheckPagedMemory(void) {
    /* Check to see if anything wrote to the high memory. */
#if (MUNGE_BY_OFFSET)
    uint32_t i;
    int numErrors = 0;
    uint8_t *ram = &sFakeSerialRAM[PF_DP_AVAILABLE_SPACE]; /* point to high unused half */
    for (i = 0; i < PF_DP_AVAILABLE_SPACE; i++) {
        uint8_t value = *ram++;
        if (value != DEAD_MARKER) {
            printf("pfCheckPagedMemory: bad byte 0x%02X at offset %u\n", value, i);
            numErrors++;
        }
    }
    printf("pfCheckPagedMemory: found %d errors\n", numErrors);
    return numErrors;
#else
    return 0;
#endif
}
int pfIsAddressInPagedMemory(vm_address_t p) {
    uintptr_t addr = PF_DP_UNMUNGE(p);
    cell_t offset = (uint8_t *)addr - sFakeSerialRAM;
    return (offset >= 0) && (offset < PF_DP_AVAILABLE_SPACE);
}

vm_address_t pfAllocatePagedMemory(const ucell_t numBytes) {
    cell_t alignedNumBytes = (numBytes + DP_ALIGNMENT_MASK) & (~DP_ALIGNMENT_MASK);
    cell_t finalAvailable = sDpNextAvailable + alignedNumBytes;
    if (finalAvailable > PF_DP_AVAILABLE_SPACE) {
        printf("ERROR - Out of Demand Paged Memory! need %d, have %d\n",
               (int)numBytes, (int)(PF_DP_AVAILABLE_SPACE - sDpNextAvailable));
        return 0;
    }
    vm_address_t virtualAddress = PTR_TO_VMA(&sFakeSerialRAM[sDpNextAvailable]);
    sDpNextAvailable = finalAvailable;
    return PF_DP_MUNGE(virtualAddress);
}

void pfFreePagedMemory(vm_address_t p) {}

size_t pfReadPagedMemory(void *destination,
                         paging_address_t source,
                         uint32_t numBytes) {
    PF_ASSERT(pfIsAddressInPagedMemory(source));
    if (numBytes == 0) return 0;
    PF_ASSERT(pfIsAddressInPagedMemory(source + numBytes - 1));
    void *pAddr = (void *)(uintptr_t) PF_DP_UNMUNGE(source);
    pfCopyMemory(destination, pAddr, numBytes);
    return numBytes;
}

size_t pfWritePagedMemory(paging_address_t destination,
                          const void *source,
                          uint32_t numBytes) {
    PF_ASSERT(pfIsAddressInPagedMemory(destination));
    if (numBytes == 0) return 0;
    PF_ASSERT(pfIsAddressInPagedMemory(destination + numBytes - 1));
    void *pAddr = (void *)(uintptr_t) PF_DP_UNMUNGE(destination);
    pfCopyMemory(pAddr, source, numBytes);
    return numBytes;
 }

#endif /* PF_DEMAND_PAGING */
