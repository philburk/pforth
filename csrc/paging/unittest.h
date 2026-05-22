#ifndef _pf_unittest_h
#define _pf_unittest_h
/***************************************************************
** Unit Test support for pForth
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

extern int pfQaNumPassed;
extern int pfQaNumFailed;

/* You must use this macro exactly once in each test program. */
#define PFQA_INSTANTIATE_GLOBALS\
    int pfQaNumPassed = 0;\
    int pfQaNumFailed = 0

#if PF_CELL_SIZE == 4
#define CELL_FORMAT "%ld"
#elif PF_CELL_SIZE == 8
#define CELL_FORMAT "%lld"
#endif
/*------------------- Macros ------------------------------*/
/* Print ERROR if it fails. Tally success or failure. Odd  */
/* do-while wrapper seems to be needed for some compilers. */
#define CHECK_TRUE(_exp, _on_error) \
    do \
    { \
        if (_exp) {\
            pfQaNumPassed++; \
        } \
        else { \
            printf("ERROR at %s:%d, (%s) not true\n", \
                __FILE__, __LINE__, #_exp ); \
            pfQaNumFailed++; \
            _on_error; \
        } \
    } while(0)

#define ASSERT_TRUE(_exp) CHECK_TRUE(_exp, goto error)
#define EXPECT_TRUE(_exp) CHECK_TRUE(_exp, (void)0)

#define CHECK_AB(_a, _b, _op, _opn, _on_error) \
    do \
    { \
        cell_t mA = (cell_t)(_a); \
        cell_t mB = (cell_t)(_b); \
        if (mA _op mB) {\
            pfQaNumPassed++; \
        } \
        else { \
            printf("ERROR at %s:%d, (%s) %s (%s), " CELL_FORMAT " %s " CELL_FORMAT "\n", \
                __FILE__, __LINE__, #_a, #_opn, #_b, mA, #_opn, mB ); \
            pfQaNumFailed++; \
            _on_error; \
        } \
    } while(0)

#define ASSERT_AB(_a, _b, _op, _opn) CHECK_AB(_a, _b, _op, _opn, goto error)
#define ASSERT_EQ(_a, _b) ASSERT_AB(_a, _b, ==, !=)
#define ASSERT_NE(_a, _b) ASSERT_AB(_a, _b, !=, ==)
#define ASSERT_GT(_a, _b) ASSERT_AB(_a, _b, >, <=)
#define ASSERT_GE(_a, _b) ASSERT_AB(_a, _b, >=, <)
#define ASSERT_LT(_a, _b) ASSERT_AB(_a, _b, <, >=)
#define ASSERT_LE(_a, _b) ASSERT_AB(_a, _b, <=, >)

#define EXPECT_AB(_a, _b, _op, _opn) CHECK_AB(_a, _b, _op, _opn, (void)0)
#define EXPECT_EQ(_a, _b) EXPECT_AB(_a, _b, ==, !=)
#define EXPECT_NE(_a, _b) EXPECT_AB(_a, _b, !=, ==)
#define EXPECT_GT(_a, _b) EXPECT_AB(_a, _b, >, <=)
#define EXPECT_GE(_a, _b) EXPECT_AB(_a, _b, >=, <)
#define EXPECT_LT(_a, _b) EXPECT_AB(_a, _b, <, >=)
#define EXPECT_LE(_a, _b) EXPECT_AB(_a, _b, <=, >)

#define HOPEFOR(_exp) \
    do \
    { \
        if ((_exp)) {\
            pfQaNumPassed++; \
        } \
        else { \
            printf("\nERROR - 0x%x - %s for %s\n", result, Pa_GetErrorText(result), #_exp ); \
            pfQaNumFailed++; \
        } \
    } while(0)

#define PFQA_PRINT_RESULT \
        printf("QA Report: %d passed, %d failed.\n", pfQaNumPassed, pfQaNumFailed )

#define PFQA_EXIT_RESULT \
        (((pfQaNumFailed > 0) || (pfQaNumPassed == 0)) ? EXIT_FAILURE : EXIT_SUCCESS)

#endif /* _pf_unittest_h */
