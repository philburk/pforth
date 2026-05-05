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

#ifndef PF_DEMAND_PAGING

void pfResetLockedMemory(void) {
}

const uint8_t *pfLockMemoryReadOnly(vm_address_t vp, cell_t numBytes) {
    return ((const uint8_t *)(vp));
}

uint8_t *pfLockMemoryReadWrite(vm_address_t vp, cell_t numBytes) {
    return ((uint8_t *)(vp));
}

void pfUnlockMemory(vm_address_t vp, const uint8_t *pp) {
}

vm_address_t pfConvertPhysicalToVirtual(const uint8_t *p) {
    return (vm_address_t) p;
}

void pfUnlockMemory(vm_address_t vp, const uint8_t *pp);
#else

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
    pfSetMemory(sLockedRegions, 0, sizeof(sLockedRegions));
    for (int i = 0; i < DP_MAX_REGIONS; i++) {
        struct RegionControlBlock *region = &sLockedRegions[i];
        region->magic = DP_MAGIC;
    }
}

uint8_t *pfLockMemoryInternal(vm_address_t vp, cell_t numBytes, int writable) {
    struct RegionControlBlock *region = NULL;
    if (pfIsAddressInPagedMemory(vp) == 0) { /* not in paged memory so locking not needed */
        return (uint8_t *) vp;
    }
    /* Find an empty region. */
    for (int i = 0; i < DP_MAX_REGIONS; i++) {
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
    cell_t numRead = pfReadPagedMemory(&region->physical[0], vp, numBytes, 0);
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

const uint8_t *pfLockMemoryReadOnly(vm_address_t vp, cell_t numBytes) {
    return ((const uint8_t *)(pfLockMemoryInternal(vp, numBytes, 0)));
}

uint8_t *pfLockMemoryReadWrite(vm_address_t vp, cell_t numBytes) {
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
        cell_t numWritten = pfWritePagedMemory(vp, &region->physical[0], region->length, 0);

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

uint8_t  pfFetchVirtualU8(uint8_t *address) {
    uint8_t value;
    if (pfIsAddressInPagedMemory(address)) {
        pfReadPagedMemory(&value, (paging_address_t) address, sizeof(uint8_t), DP_TIMEOUT_MICROS);
        return value;
    } else {
        return (*((uint8_t *)(address)));
    }

}
uint16_t pfFetchVirtualU16(uint16_t *address) {
    uint16_t value;
    if (pfIsAddressInPagedMemory(address)) {
        pfReadPagedMemory(&value, (paging_address_t) address, sizeof(uint16_t), DP_TIMEOUT_MICROS);
        return value;
    } else {
        return (*((uint16_t *)(address)));
    }
}
cell_t   pfFetchVirtualCell(cell_t *address) {
    cell_t value;
    if (pfIsAddressInPagedMemory(address)) {
        pfReadPagedMemory(&value, (paging_address_t) address, sizeof(cell_t), DP_TIMEOUT_MICROS);
        return value;
    } else {
        return (*((cell_t *)(address)));
    }
}
PF_FLOAT pfFetchVirtualFloat(PF_FLOAT *address) {
    PF_FLOAT value;
    if (pfIsAddressInPagedMemory(address)) {
        pfReadPagedMemory(&value, (paging_address_t) address, sizeof(PF_FLOAT), DP_TIMEOUT_MICROS);
        return value;
    } else {
        return (*((PF_FLOAT *)(address)));
    }
}

void pfStoreVirtualU8(uint8_t *address, uint8_t value) {
    if (pfIsAddressInPagedMemory(address)) {
        pfWritePagedMemory((paging_address_t) address, &value, sizeof(uint8_t), DP_TIMEOUT_MICROS);
    } else {
        *address = value;
    }
}

void pfStoreVirtualU16(uint16_t *address, uint16_t value) {
    if (pfIsAddressInPagedMemory(address)) {
        pfWritePagedMemory((paging_address_t) address, &value, sizeof(uint16_t), DP_TIMEOUT_MICROS);
    } else {
        *address = value;
    }
}
void pfStoreVirtualCell(cell_t *address, cell_t value) {
    if (pfIsAddressInPagedMemory(address)) {
        pfWritePagedMemory((paging_address_t) address, &value, sizeof(cell_t), DP_TIMEOUT_MICROS);
    } else {
        *address = value;
    }
}
void pfStoreVirtualFloat(PF_FLOAT *address, PF_FLOAT value) {
    if (pfIsAddressInPagedMemory(address)) {
        pfWritePagedMemory((paging_address_t) address, &value, sizeof(PF_FLOAT), DP_TIMEOUT_MICROS);
    } else {
        *address = value;
    }
}

void *pfCopyFromVirtualMemory(void *destination,
                              vm_address_t source,
                              size_t numBytes) {
    if (pfIsAddressInPagedMemory(source)) {
        pfReadPagedMemory(destination, (paging_address_t) source, numBytes, DP_TIMEOUT_MICROS); /* TODO check result */
        return destination;
    } else {
        return pfCopyMemory(destination, source, numBytes);
    }
}

#endif

