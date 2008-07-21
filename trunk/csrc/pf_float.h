/* @(#) pf_float.h 98/01/28 1.1 */
#ifndef _pf_float_h
#define _pf_float_h

/***************************************************************
** Include file for PForth, a Forth based on 'C'
**
** Author: Phil Burk
** Copyright 1994 3DO, Phil Burk, Larry Polansky, David Rosenboom
**
** The pForth software code is dedicated to the public domain,
** and any third party may reproduce, distribute and modify
** the pForth software code or any derivative works thereof
** without any compensation or license.  The pForth software
** code is provided on an "as is" basis without any warranty
** of any kind, including, without limitation, the implied
** warranties of merchantability and fitness for a particular
** purpose and their equivalents under the laws of any jurisdiction.
**
***************************************************************/

typedef double PF_FLOAT;

/* Define pForth specific math functions. */

#define fp_acos   acos
#define fp_asin   asin
#define fp_atan   atan
#define fp_atan2  atan2
#define fp_cos    cos
#define fp_cosh   cosh  
#define fp_fabs   fabs
#define fp_floor  floor
#define fp_log    log  
#define fp_log10  log10
#define fp_pow    pow
#define fp_sin    sin
#define fp_sinh   sinh
#define fp_sqrt   sqrt
#define fp_tan    tan
#define fp_tanh   tanh

#endif
