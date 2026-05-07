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

static uint8_t sFakeSerialRAM[PF_DP_AVAILABLE_SPACE];
static cell_t sDpNextAvailable = 0;
#define DP_ALIGNMENT_MASK (DP_ALIGNMENT_SIZE - 1)

/* By munging the virtual address, we can cause memory errors if it is not converted.
 * This will help us catch areas where we need to use functions like pfLockMemoryReadWrite().
 * You may need to use a different mask on your system depending on the memory layout.
 */
#if 1
#define PF_DP_MUNGE_KEY  (0)
#elif (PF_64BIT)
#define PF_DP_MUNGE_KEY  (0x005A000000000000)
#elif (PF_32BIT)
#define PF_DP_MUNGE_KEY  (0x50000000)
#endif

#define PF_DP_MUNGE(vaddr) ((vm_address_t)(((cell_t)vaddr) ^ PF_DP_MUNGE_KEY))
#define PF_DP_UNMUNGE(vaddr) ((vm_address_t)(((cell_t)vaddr) ^ PF_DP_MUNGE_KEY))

void pfResetPagedMemory(void) {
    printf("pfResetPagedMemory: cell = %d\n", (int)sizeof(cell_t));
    sDpNextAvailable = 0;
}

int pfIsAddressInPagedMemory(vm_address_t p) {
    vm_address_t vaddr = PF_DP_UNMUNGE(p);
    cell_t offset = (uint8_t *)vaddr - sFakeSerialRAM;
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
    vm_address_t virtualAddress = (vm_address_t) &sFakeSerialRAM[sDpNextAvailable];
    sDpNextAvailable = finalAvailable;
    return PF_DP_MUNGE(virtualAddress);
}

void pfFreePagedMemory(vm_address_t p) {}

size_t pfReadPagedMemory(void *destination,
                         vm_address_t source,
                         uint32_t numBytes) {
    if (!pfIsAddressInPagedMemory(source)) {
        printf("ERROR: not a paged address = %p\n", source);
        return 0;
    }
    /* TODO check upper boundary */
    void *pAddr = PF_DP_UNMUNGE(source);
    pfCopyMemory(destination, pAddr, numBytes);
    return numBytes;
}

size_t pfWritePagedMemory(paging_address_t destination,
                          const void *source,
                          uint32_t numBytes) {
    if (!pfIsAddressInPagedMemory(destination)) {
        printf("ERROR: not a paged address = %p\n", destination);
        return 0;
    }
    /* TODO check upper boundary */
    void *pAddr = PF_DP_UNMUNGE(destination);
    pfCopyMemory(pAddr, source, numBytes);
    return numBytes;
 }

#endif /* PF_DEMAND_PAGING */
