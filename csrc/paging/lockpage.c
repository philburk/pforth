/***************************************************************
** Demand Paged Memory Region Locking
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

#if PF_DEMAND_PAGING == 0

void pfResetLockedMemory(void) {
}

const uint8_t *pfLockMemoryReadOnly(vm_address_t vp, uint32_t numBytes) {
    return ((const uint8_t *)(vp));
}

uint8_t *pfLockMemoryReadWrite(vm_address_t vp, uint32_t numBytes) {
    return ((uint8_t *)(vp));
}

int pfUnlockMemory(vm_address_t vp, const uint8_t *pp) {
    return 0;
}

int pfIsAddressInPagedMemory(vm_address_t p) {
    return FALSE;
}

void *pfCopyFromVirtualMemory(void *destination,
                              vm_address_t source,
                              uint32_t numBytes) {
    return pfCopyMemory(destination, (const void *)source, numBytes);
}

vm_address_t pfCopyToVirtualMemory(vm_address_t destination,
                            const void *source,
                            uint32_t numBytes) {
    return (vm_address_t) pfCopyMemory((void *)destination, source, numBytes);
}

void pfFreeVirtualMemory(vm_address_t address) {
    pfFreeMem((void *)address);
}

vm_address_t pfSetVirtualMemory(vm_address_t destination,
                         uint8_t value,
                         uint32_t numBytes) {
    return (vm_address_t) pfSetMemory((void *)destination, value, numBytes);
}

#else /* PF_DEMAND_PAGING */

#define DP_MAGIC  (0x5A9E)

struct RegionControlBlock {
    vm_address_t virtual;
    uint32_t magic;
    uint32_t length;
    uint8_t writable;
    uint8_t locked;
    uint8_t pad1;
    uint8_t pad2;
    uint8_t physical[DP_MAX_REGION_SIZE];
};

struct RegionControlBlock sLockedRegions[DP_MAX_REGIONS];

void pfResetLockedMemory(void) {
    int i;
    pfSetMemory(sLockedRegions, 0, sizeof(sLockedRegions));
    for (i = 0; i < DP_MAX_REGIONS; i++) {
        struct RegionControlBlock *region = &sLockedRegions[i];
        region->magic = DP_MAGIC;
    }
}

uint8_t *pfLockMemoryInternal(vm_address_t vp, uint32_t numBytes, int writable) {
    int i;
    struct RegionControlBlock *region = NULL;
    if (pfIsAddressInPagedMemory(vp) == 0) { /* not in paged memory so locking not needed */
        return (uint8_t *) vp;
    }
    PF_ASSERT(numBytes <= DP_MAX_REGION_SIZE);

    /* Find an empty region. */
    for (i = 0; i < DP_MAX_REGIONS; i++) {
        if (sLockedRegions[i].locked == 0) {
            region = &sLockedRegions[i];
            break;
        }
    }
    if (region == NULL) {
        printf("ERROR: no available region!");
        return NULL;
    }

    /* Read serial data into the buffer. */
    cell_t numRead = pfReadPagedMemory(&region->physical[0], vp, numBytes);
    if (numRead != numBytes) {
        printf("ERROR: could not read data for region!");
        return NULL;
    }

    region->virtual = vp;
    region->length = (uint32_t) numBytes;
    region->locked = 1;
    region->writable = writable;
    return &region->physical[0];
}

const uint8_t *pfLockMemoryReadOnly(vm_address_t vp, uint32_t numBytes) {
    return ((const uint8_t *)(pfLockMemoryInternal(vp, numBytes, 0)));
}

uint8_t *pfLockMemoryReadWrite(vm_address_t vp, uint32_t numBytes) {
    return pfLockMemoryInternal(vp, numBytes, 1);
}

int pfUnlockMemory(vm_address_t vp, const uint8_t *pp) {
    if (vp == pp ) {
        return 0; /* not paged memory */
    }
    const size_t kPhysicalOffset = (((void *)&sLockedRegions[0].physical[0]) - ((void *)&sLockedRegions[0]));
    struct RegionControlBlock *region = (struct RegionControlBlock *)(pp - kPhysicalOffset);
    if (region->magic != DP_MAGIC) {
        printf("ERROR: physical address was not in a region!\n");
        return -1;
    }
    if (region->locked == 0) {
        printf("ERROR: physical address was not locked!\n");
        return -1;
    }
    if (region->virtual != vp) {
        printf("ERROR: virtual address did not match region, %p != %p!\n",
               region->virtual, vp);
        return -1;
    }
    if (region->writable != 0) {
        /* write back to serial memory */
        cell_t numWritten = pfWritePagedMemory(vp, &region->physical[0], region->length);

        if (numWritten != region->length) {
            printf("ERROR: virtual could not be written back, only wrote %d bytes\n",
                   region->length);
            return -1;
        }
    }
    region->virtual = NULL;
    region->length = 0;
    region->locked = 0;
    region->writable = 0;
    return 0;
}

uint8_t  pfFetchVirtualU8(const uint8_t *address) {
    uint8_t value;
    if (pfIsAddressInPagedMemory((vm_address_t)address)) {
        pfReadPagedMemory(&value, (paging_address_t) address, sizeof(uint8_t));
        return value;
    } else {
        return *address;
    }

}
uint16_t pfFetchVirtualU16(const uint16_t *address) {
    uint16_t value;
    if (pfIsAddressInPagedMemory((vm_address_t)address)) {
        pfReadPagedMemory(&value, (paging_address_t) address, sizeof(uint16_t));
        return value;
    } else {
        return *address;
    }
}
cell_t   pfFetchVirtualCell(const cell_t *address) {
    cell_t value;
    if (pfIsAddressInPagedMemory((vm_address_t)address)) {
        pfReadPagedMemory(&value, (paging_address_t) address, sizeof(cell_t));
        return value;
    } else {
        return *address;
    }
}

#ifdef PF_SUPPORT_FP
PF_FLOAT pfFetchVirtualFloat(const PF_FLOAT *address) {
    PF_FLOAT value;
    if (pfIsAddressInPagedMemory((vm_address_t)address)) {
        pfReadPagedMemory(&value, (paging_address_t) address, sizeof(PF_FLOAT));
        return value;
    } else {
        return *address;
    }
}
#endif /* PF_SUPPORT_FP */

void pfStoreVirtualU8(uint8_t *address, uint8_t value) {
    if (pfIsAddressInPagedMemory((vm_address_t)address)) {
        pfWritePagedMemory((paging_address_t) address, &value, sizeof(uint8_t));
    } else {
        *address = value;
    }
}

void pfStoreVirtualU16(uint16_t *address, uint16_t value) {
    if (pfIsAddressInPagedMemory((vm_address_t)address)) {
        pfWritePagedMemory((paging_address_t) address, &value, sizeof(uint16_t));
    } else {
        *address = value;
    }
}
void pfStoreVirtualCell(cell_t *address, cell_t value) {
    if (pfIsAddressInPagedMemory((vm_address_t)address)) {
        pfWritePagedMemory((paging_address_t) address, &value, sizeof(cell_t));
    } else {
        *address = value;
    }
}

#ifdef PF_SUPPORT_FP
void pfStoreVirtualFloat(PF_FLOAT *address, PF_FLOAT value) {
    if (pfIsAddressInPagedMemory((vm_address_t)address)) {
        pfWritePagedMemory((paging_address_t) address, &value, sizeof(PF_FLOAT));
    } else {
        *address = value;
    }
}
#endif /* PF_SUPPORT_FP */

void *pfCopyFromVirtualMemory(void *destination,
                              vm_address_t source,
                              uint32_t numBytes) {
    if (pfIsAddressInPagedMemory(source)) {
        pfReadPagedMemory(destination, (paging_address_t) source, numBytes); /* TODO check result */
        return (void *) destination;
    } else {
        return pfCopyMemory(destination, (void *) source, numBytes);
    }
}

vm_address_t pfCopyToVirtualMemory(vm_address_t destination,
                            const void *source,
                            uint32_t numBytes) {
    if (pfIsAddressInPagedMemory(destination)) {
        pfWritePagedMemory((paging_address_t) destination, source, numBytes); /* TODO check result */
        return destination;
    } else {
        return (vm_address_t) pfCopyMemory((void *) destination, source, numBytes);
    }
}

void pfFreeVirtualMemory(vm_address_t address) {
    if (pfIsAddressInPagedMemory(address)) {
        pfFreePagedMemory((paging_address_t) address);
    } else {
        pfFreeMem((void *) address);
    }
}

vm_address_t pfSetVirtualMemory(vm_address_t destination,
                         uint8_t value,
                         uint32_t numBytes) {
    if (pfIsAddressInPagedMemory(destination)) {
        /* Set memory in blocks that will fit in locked regions. */
        vm_address_t vp = destination;
        uint8_t buffer[16];
        pfSetMemory(buffer, value, sizeof(buffer));
        while (numBytes > 0) {
            uint32_t bytesToWrite = (numBytes < sizeof(buffer)) ? numBytes : sizeof(buffer);
            pfWritePagedMemory(vp, buffer, bytesToWrite);
            numBytes -= bytesToWrite;
            vp += bytesToWrite;
        }
        return destination;
    } else {
        return pfSetMemory(destination, value, numBytes);
    }
}

#endif /* PF_DEMAND_PAGING */
