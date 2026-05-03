
/***************************************************************
** Unit Tests file for pForth Demand Paging
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
#include "unittest.h"

PFQA_INSTANTIATE_GLOBALS;

static int pfQaTestAllocate(void) {
    printf("pfQaDemandPaging : pfQaTestAllocate\n");
    pfResetPagedMemory();
    vm_address_t vm1 = pfAllocatePagedMemory(1);
    ASSERT_NE(vm1, 0);
    vm_address_t vm2 = pfAllocatePagedMemory(128);
    ASSERT_NE(vm2, 0);
    /* Check alignment. */
    ASSERT_EQ((vm2 - vm1), DP_ALIGNMENT_SIZE);

    int x = 0;
    ASSERT_EQ(pfIsAddressInPagedMemory(&x), 0);
    ASSERT_EQ(pfIsAddressInPagedMemory((void *)(vm2 + 5)), 1);
    return 0;
error:
    return 1;
}

static int pfQaTestFetchStore(void) {
    printf("pfQaDemandPaging : pfQaTestFetchStore\n");
    PF_FLOAT f1 = 3.14159;
    PF_FLOAT f2;
    cell_t c1 = 123456;
    cell_t c2;
    uint16_t w1 = 1234;
    uint16_t w2;
    uint8_t b1 = 91;
    uint8_t b2;

    pfResetPagedMemory();
    vm_address_t vm1 = pfAllocatePagedMemory(1024);
    ASSERT_NE(vm1, 0);

    DP_STORE_U8(vm1 + 16, b1);
    DP_STORE_U16(vm1 + 32, w1);
    DP_STORE_CELL(vm1 + 48, c1);
    DP_STORE_FLOAT(vm1 + 64, f1);

    b2 = DP_FETCH_U8(vm1 + 16);
    ASSERT_EQ(b1, b2);
    w2 = DP_FETCH_U16(vm1 + 32);
    ASSERT_EQ(w1, w2);
    c2 = DP_FETCH_CELL(vm1 + 48);
    ASSERT_EQ(c1, c2);
    f2 = DP_FETCH_FLOAT(vm1 + 64);
    ASSERT_EQ(f1, f2);
    return 0;
error:
    return 1;
}

static int pfQaTestReadWrite(void) {
    printf("pfQaDemandPaging : pfQaTestReadWrite\n");
    uint8_t buffer1[73];
    uint8_t buffer2[sizeof(buffer1)];
    const int kBufferSize = sizeof(buffer1);
    int i;
    for (i = 0; i < kBufferSize; i++) {
        buffer1[i] = i;
    }
    pfResetPagedMemory();
    vm_address_t vm1 = pfAllocatePagedMemory(kBufferSize);
    ASSERT_NE(vm1, 0);
    printf("pfQaTestReadWrite: vm1 = %p\n", vm1);
    cell_t written = pfWritePagedMemory(vm1, buffer1, kBufferSize, 0);
    ASSERT_EQ(written, kBufferSize);
    cell_t numRead = pfReadPagedMemory(buffer2, vm1, kBufferSize, 0);
    ASSERT_EQ(numRead, kBufferSize);
    for (i = 0; i < kBufferSize; i++) {
        ASSERT_EQ(buffer2[i], buffer1[i]);
    }

    return 0;
error:
    return 1;
}

static int pfQaTestRegionLock(void) {
    printf("pfQaDemandPaging : pfQaTestRegionLock\n");
    pfResetLockedMemory();
    const int kBufferSize = 123;
    int result = 0;
    int i;
    pfResetPagedMemory();
    vm_address_t vm1 = pfAllocatePagedMemory(kBufferSize);
    ASSERT_NE(0, vm1);

    uint8_t *pm1 = pfLockMemoryReadWrite(vm1, kBufferSize);
    ASSERT_NE(pm1, NULL);
    for (i = 0; i < kBufferSize; i++) {
        pm1[i] = i;
    }
    result = pfUnlockMemory(vm1, pm1);
    ASSERT_EQ(0, result);

    const uint8_t *pm2 = pfLockMemoryReadOnly(vm1, kBufferSize);
    ASSERT_NE(pm2, NULL);
    for (i = 0; i < kBufferSize; i++) {
        ASSERT_EQ(pm2[i], i);
    }
    result = pfUnlockMemory(vm1 + 8, pm2); /* Pass bad virtual address! */
    ASSERT_GE(0, result);
    result = pfUnlockMemory(vm1, pm2 + 8); /* Pass bad physical address! */
    ASSERT_GE(0, result);
    result = pfUnlockMemory(vm1, pm2); /* GOOD */
    ASSERT_EQ(0, result);
    result = pfUnlockMemory(vm1, pm2); /* Unlock twice! */
    ASSERT_GE(0, result);

    return 0;
error:
    return 1;
}

int pfQaDemandPaging(void) {
    printf("pfQaDemandPaging\n");
    int x = 4;
    int y = 4;
    ASSERT_EQ(x,y);

    ASSERT_EQ(sizeof(vm_address_t), sizeof(cell_t));

    ASSERT_EQ(pfQaTestAllocate(), 0);
    ASSERT_EQ(pfQaTestReadWrite(), 0);
    ASSERT_EQ(pfQaTestRegionLock(), 0);
    ASSERT_EQ(pfQaTestFetchStore(), 0);

    printf("pfQaDemandPaging ended\n");

error:
    PFQA_PRINT_RESULT;
    return PFQA_EXIT_RESULT;
}
