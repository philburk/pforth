/***************************************************************
** Demand Paged Memory Simulator
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

#define PF_DP_AVAILABLE_SPACE   (256*1024)
static uint8_t sFakeSerialRAM[PF_DP_AVAILABLE_SPACE];
static cell_t sDpNextAvailable = 0;
#define DP_ALIGNMENT_MASK (DP_ALIGNMENT_SIZE - 1)


void pfResetPagedMemory(void) {
    sDpNextAvailable = 0;
}

int pfIsAddressInPagedMemory(void * p) {
    cell_t offset = (uint8_t *)p - sFakeSerialRAM;
    return (offset >= 0) && (offset < PF_DP_AVAILABLE_SPACE);
}

vm_address_t pfAllocatePagedMemory(const cell_t numBytes) {
    cell_t alignedNumBytes = (numBytes + DP_ALIGNMENT_MASK) & (~DP_ALIGNMENT_MASK);
    cell_t finalAvailable = sDpNextAvailable + alignedNumBytes;
    if (finalAvailable > PF_DP_AVAILABLE_SPACE) {
        printf("ERROR - Out of Demand Paged Memory!\n");
        return 0;
    }
    vm_address_t virtualAddress = (vm_address_t) &sFakeSerialRAM[sDpNextAvailable];
    sDpNextAvailable = finalAvailable;
    return virtualAddress;
}

void pfFreePagedMemory(vm_address_t p) {}

int pfReadPagedMemory(uint8_t *destination,
                      vm_address_t source,
                      size_t numBytes,
                      int async) {
    /* TODO check boundaries? */
    pfCopyMemory(destination, source, numBytes);
    return numBytes;
}

int pfWaitPendingVirtualRead(void) {
    return 0;
}

int pfWritePagedMemory(vm_address_t destination,
                       uint8_t *source,
                       size_t numBytes,
                       int async) {
    /* TODO check boundaries? */
    pfCopyMemory(destination, source, numBytes);
    return numBytes;
 }
