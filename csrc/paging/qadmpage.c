
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

static int pfQaTestReadWrite(void) {
    printf("pfQaDemandPaging : pfQaTestReadWrite\n");
    uint8_t buffer1[73];
    uint8_t buffer2[sizeof(buffer1)];
    const int kBufferSize = sizeof(buffer1);
    for (int i = 0; i < kBufferSize; i++) {
        buffer1[i] = i;
    }
    pfResetPagedMemory();
    vm_address_t vm1 = pfAllocatePagedMemory(kBufferSize);
    ASSERT_NE(vm1, 0);
    cell_t written = pfWritePagedMemory(vm1, buffer1, kBufferSize, 0);
    ASSERT_EQ(written, kBufferSize);
    cell_t numRead = pfReadPagedMemory(buffer2, vm1, kBufferSize, 0);
    ASSERT_EQ(numRead, kBufferSize);
    for (int i = 0; i < kBufferSize; i++) {
        ASSERT_EQ(buffer2[i], buffer1[i]);
    }

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

    printf("pfQaDemandPaging ended\n");

error:
    PFQA_PRINT_RESULT;
    return PFQA_EXIT_RESULT;
}
