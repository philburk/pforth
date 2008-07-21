/* @(#) pf_save.h 96/12/18 1.8 */
#ifndef _pforth_save_h
#define _pforth_save_h

/***************************************************************
** Include file for PForth SaveForth
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
**	941031	rdg		fix redefinition of MAKE_ID and EVENUP to be conditional
**
***************************************************************/


typedef struct DictionaryInfoChunk
{
/* All fields are stored in BIG ENDIAN format for consistency in data files. */
/* All fileds must be the same size as int32 for easy endian conversion. */
	int32  sd_Version;
	int32  sd_RelContext;      /* relative ptr to Dictionary Context */
	int32  sd_RelHeaderPtr;    /* relative ptr to Dictionary Header Ptr */
	int32  sd_RelCodePtr;      /* relative ptr to Dictionary Header Ptr */
	ExecToken  sd_EntryPoint;  /* relative ptr to entry point or NULL */
	int32  sd_UserStackSize;   /* in bytes */
	int32  sd_ReturnStackSize; /* in bytes */
	int32  sd_NameSize;        /* in bytes */
	int32  sd_CodeSize;        /* in bytes */
	int32  sd_NumPrimitives;   /* To distinguish between primitive and secondary. */
	uint32 sd_Flags;
	int32  sd_FloatSize;       /* In bytes. Must match code. 0 means no floats. */
	uint32 sd_Reserved;
} DictionaryInfoChunk;

/* Bits in sd_Flags */
#define SD_F_BIG_ENDIAN_DIC    (1<<0)

#ifndef MAKE_ID
#define MAKE_ID(a,b,c,d) ((((uint32)a)<<24)|(((uint32)b)<<16)|(((uint32)c)<<8)|((uint32)d))
#endif

#define ID_FORM MAKE_ID('F','O','R','M')
#define ID_P4TH MAKE_ID('P','4','T','H')
#define ID_P4DI MAKE_ID('P','4','D','I')
#define ID_P4NM MAKE_ID('P','4','N','M')
#define ID_P4CD MAKE_ID('P','4','C','D')
#define ID_BADF MAKE_ID('B','A','D','F')

#ifndef EVENUP
#define EVENUP(n) ((n+1)&(~1))
#endif

#ifdef __cplusplus
extern "C" {
#endif

int32 ffSaveForth( const char *FileName, ExecToken EntryPoint, int32 NameSize, int32 CodeSize );

/* Endian-ness tools. */

int    IsHostLittleEndian( void );
uint32 ReadLongBigEndian( const uint32 *addr );
uint16 ReadShortBigEndian( const uint16 *addr );
uint32 ReadLongLittleEndian( const uint32 *addr );
uint16 ReadShortLittleEndian( const uint16 *addr );
void WriteLongBigEndian( uint32 *addr, uint32 data );
void WriteShortBigEndian( uint16 *addr, uint16 data );
void WriteLongLittleEndian( uint32 *addr, uint32 data );
void WriteShortLittleEndian( uint16 *addr, uint16 data );

#ifdef PF_SUPPORT_FP
void WriteFloatBigEndian( PF_FLOAT *addr, PF_FLOAT data );
PF_FLOAT ReadFloatBigEndian( const PF_FLOAT *addr );
void WriteFloatLittleEndian( PF_FLOAT *addr, PF_FLOAT data );
PF_FLOAT ReadFloatLittleEndian( const PF_FLOAT *addr );
#endif

#ifdef __cplusplus
}   
#endif

#endif /* _pforth_save_h */
