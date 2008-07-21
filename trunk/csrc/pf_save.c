/* @(#) pf_save.c 98/01/26 1.3 */
/***************************************************************
** Save and Load Dictionary
** for PForth based on 'C'
**
** Compile file based version or static data based version
** depending on PF_NO_FILEIO switch.
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
****************************************************************
** 940225 PLB Fixed CodePtr save, was using NAMEREL instead of CODEREL
**            This would only work if the relative location
**            of names and code was the same when saved and reloaded.
** 940228 PLB Added PF_NO_FILEIO version
** 961204 PLB Added PF_STATIC_DIC
** 000623 PLB Cast chars as uint32 before shifting for 16 bit systems.
***************************************************************/

#include "pf_all.h"

/* If no File I/O, then force static dictionary. */
#ifdef PF_NO_FILEIO
	#ifndef PF_STATIC_DIC
		#define PF_STATIC_DIC
	#endif
#endif

#ifdef PF_STATIC_DIC
	#include "pfdicdat.h"
#endif

/*
Dictionary File Format based on IFF standard.
The chunk IDs, sizes, and data values are all Big Endian in conformance with the IFF standard.
The dictionaries may be big or little endian.
	'FORM'
	size
	'P4TH'  -  Form Identifier

Chunks
	'P4DI'
	size
	struct DictionaryInfoChunk

	'P4NM'
	size
	Name and Header portion of dictionary. (Big or Little Endian) (Optional)

	'P4CD'
	size
	Code portion of dictionary. (Big or Little Endian) 
*/


/***************************************************************/
/* Endian-ness tools. */
uint32 ReadLongBigEndian( const uint32 *addr )
{
	const unsigned char *bp = (const unsigned char *) addr;
/* We must cast char to uint32 before shifting because
** of systems with 16 bit ints. 000623 */
	uint32 temp = ((uint32)bp[0])<<24;
	temp |= ((uint32)bp[1])<<16;
	temp |= ((uint32)bp[2])<<8;
	temp |= ((uint32)bp[3]);
	return temp;
}
/***************************************************************/
uint16 ReadShortBigEndian( const uint16 *addr )
{
	const unsigned char *bp = (const unsigned char *) addr;
	return (uint16) ((bp[0]<<8) | bp[1]);
}

/***************************************************************/
uint32 ReadLongLittleEndian( const uint32 *addr )
{
	const unsigned char *bp = (const unsigned char *) addr;
/* We must cast char to uint32 before shifting because
** of systems with 16 bit ints. 000623 */
	uint32 temp = ((uint32)bp[3])<<24;
	temp |= ((uint32)bp[2])<<16;
	temp |= ((uint32)bp[1])<<8;
	temp |= ((uint32)bp[0]);
	return temp;
}
/***************************************************************/
uint16 ReadShortLittleEndian( const uint16 *addr )
{
	const unsigned char *bp = (const unsigned char *) addr;
	return (uint16) ((bp[1]<<8) | bp[0]);
}

#ifdef PF_SUPPORT_FP

/***************************************************************/
static void ReverseCopyFloat( const PF_FLOAT *src, PF_FLOAT *dst );

static void ReverseCopyFloat( const PF_FLOAT *src, PF_FLOAT *dst )
{
	int i;
	unsigned char *d = (unsigned char *) dst;
	const unsigned char *s = (const unsigned char *) src;

	for( i=0; i<sizeof(PF_FLOAT); i++ )
	{
		d[i] = s[sizeof(PF_FLOAT) - 1 - i];
	}
}

/***************************************************************/
void WriteFloatBigEndian( PF_FLOAT *addr, PF_FLOAT data )
{
	if( IsHostLittleEndian() )
	{
		ReverseCopyFloat( &data, addr );
	}
	else
	{
		*addr = data;
	}
}

/***************************************************************/
PF_FLOAT ReadFloatBigEndian( const PF_FLOAT *addr )
{
	PF_FLOAT data;
	if( IsHostLittleEndian() )
	{
		ReverseCopyFloat( addr, &data );
		return data;
	}
	else
	{
		return *addr;
	}
}

/***************************************************************/
void WriteFloatLittleEndian( PF_FLOAT *addr, PF_FLOAT data )
{
	if( IsHostLittleEndian() )
	{
		*addr = data;
	}
	else
	{
		ReverseCopyFloat( &data, addr );
	}
}

/***************************************************************/
PF_FLOAT ReadFloatLittleEndian( const PF_FLOAT *addr )
{
	PF_FLOAT data;
	if( IsHostLittleEndian() )
	{
		return *addr;
	}
	else
	{
		ReverseCopyFloat( addr, &data );
		return data;
	}
}

#endif /* PF_SUPPORT_FP */

/***************************************************************/
void WriteLongBigEndian( uint32 *addr, uint32 data )
{
	unsigned char *bp = (unsigned char *) addr;

	bp[0] = (unsigned char) (data>>24);
	bp[1] = (unsigned char) (data>>16);
	bp[2] = (unsigned char) (data>>8);
	bp[3] = (unsigned char) (data);
}

/***************************************************************/
void WriteShortBigEndian( uint16 *addr, uint16 data )
{
	unsigned char *bp = (unsigned char *) addr;

	bp[0] = (unsigned char) (data>>8);
	bp[1] = (unsigned char) (data);
}

/***************************************************************/
void WriteLongLittleEndian( uint32 *addr, uint32 data )
{
	unsigned char *bp = (unsigned char *) addr;

	bp[0] = (unsigned char) (data);
	bp[1] = (unsigned char) (data>>8);
	bp[2] = (unsigned char) (data>>16);
	bp[3] = (unsigned char) (data>>24);
}
/***************************************************************/
void WriteShortLittleEndian( uint16 *addr, uint16 data )
{
	unsigned char *bp = (unsigned char *) addr;

	bp[0] = (unsigned char) (data);
	bp[1] = (unsigned char) (data>>8);
}

/***************************************************************/
/* Return 1 if host CPU is Little Endian */
int IsHostLittleEndian( void )
{
    static int gEndianCheck = 1;
	unsigned char *bp = (unsigned char *) &gEndianCheck;
	return (int) (*bp); /* Return byte pointed to by address. If LSB then == 1 */
}

#if defined(PF_NO_FILEIO) || defined(PF_NO_SHELL)

int32 ffSaveForth( const char *FileName, ExecToken EntryPoint, int32 NameSize, int32 CodeSize)
{
	TOUCH(FileName);
	TOUCH(EntryPoint);
	TOUCH(NameSize);
	TOUCH(CodeSize);

	pfReportError("ffSaveForth", PF_ERR_NOT_SUPPORTED);
	return -1;
}

#else /* PF_NO_FILEIO or PF_NO_SHELL */

/***************************************************************/
static int32 WriteLong( FileStream *fid, int32 Val )
{
	int32 numw;
	uint32 pad;

	WriteLongBigEndian(&pad,Val);
	numw = sdWriteFile( (char *) &pad, 1, sizeof(int32), fid );
	if( numw != sizeof(int32) ) return -1;
	return 0;
}

/***************************************************************/
static int32 WriteChunk( FileStream *fid, int32 ID, char *Data, int32 NumBytes )
{
	int32 numw;
	int32 EvenNumW;

	EvenNumW = EVENUP(NumBytes);

	if( WriteLong( fid, ID ) < 0 ) goto error;
	if( WriteLong( fid, EvenNumW ) < 0 ) goto error;

	numw = sdWriteFile( Data, 1, EvenNumW, fid );
	if( numw != EvenNumW ) goto error;
	return 0;
error:
	pfReportError("WriteChunk", PF_ERR_WRITE_FILE);
	return -1;
}

/****************************************************************
** Save Dictionary in File.
** If EntryPoint is NULL, save as development environment.
** If EntryPoint is non-NULL, save as turnKey environment with no names.
*/
int32 ffSaveForth( const char *FileName, ExecToken EntryPoint, int32 NameSize, int32 CodeSize)
{
	FileStream *fid;
	DictionaryInfoChunk SD;
	int32 FormSize;
	int32 NameChunkSize = 0;
	int32 CodeChunkSize;
	uint32 rhp, rcp;
	uint32 *p;
	int   i;

	fid = sdOpenFile( FileName, "wb" );
	if( fid == NULL )
	{
		pfReportError("pfSaveDictionary", PF_ERR_OPEN_FILE);
		return -1;
	}

/* Save in uninitialized form. */
	pfExecIfDefined("AUTO.TERM");

/* Write FORM Header ---------------------------- */
	if( WriteLong( fid, ID_FORM ) < 0 ) goto error;
	if( WriteLong( fid, 0 ) < 0 ) goto error;
	if( WriteLong( fid, ID_P4TH ) < 0 ) goto error;

/* Write P4DI Dictionary Info  ------------------ */
	SD.sd_Version = PF_FILE_VERSION;

	rcp = ABS_TO_CODEREL(gCurrentDictionary->dic_CodePtr.Byte); /* 940225 */
	SD.sd_RelCodePtr = rcp; 
	SD.sd_UserStackSize = sizeof(cell) * (gCurrentTask->td_StackBase - gCurrentTask->td_StackLimit);
	SD.sd_ReturnStackSize = sizeof(cell) * (gCurrentTask->td_ReturnBase - gCurrentTask->td_ReturnLimit);
	SD.sd_NumPrimitives = gNumPrimitives;  /* Must match compiled dictionary. */

#ifdef PF_SUPPORT_FP
	SD.sd_FloatSize = sizeof(PF_FLOAT);  /* Must match compiled dictionary. */
#else
	SD.sd_FloatSize = 0;
#endif

	SD.sd_Reserved = 0;

/* Set bit that specifiec whether dictionary is BIG or LITTLE Endian. */
	{
#if defined(PF_BIG_ENDIAN_DIC)
		int eflag = SD_F_BIG_ENDIAN_DIC;
#elif defined(PF_LITTLE_ENDIAN_DIC)
		int eflag = 0;
#else
		int eflag = IsHostLittleEndian() ? 0 : SD_F_BIG_ENDIAN_DIC;
#endif
		SD.sd_Flags = eflag;
	}

	if( EntryPoint )
	{
		SD.sd_EntryPoint = EntryPoint;  /* Turnkey! */
	}
	else
	{
		SD.sd_EntryPoint = 0;
	}

/* Do we save names? */
	if( NameSize == 0 )
	{
		SD.sd_RelContext = 0;
		SD.sd_RelHeaderPtr = 0;
		SD.sd_NameSize = 0;
	}
	else
	{
/* Development mode. */
		SD.sd_RelContext = ABS_TO_NAMEREL(gVarContext);
		rhp = ABS_TO_NAMEREL(gCurrentDictionary->dic_HeaderPtr.Byte);
		SD.sd_RelHeaderPtr = rhp;

/* How much real name space is there? */
		NameChunkSize = QUADUP(rhp);  /* Align */

/* NameSize must be 0 or greater than NameChunkSize + 1K */
		NameSize = QUADUP(NameSize);  /* Align */
		if( NameSize > 0 )
		{
			NameSize = MAX( NameSize, (NameChunkSize + 1024) );
		}
		SD.sd_NameSize = NameSize;
	}

/* How much real code is there? */
	CodeChunkSize = QUADUP(rcp);
	CodeSize = QUADUP(CodeSize);  /* Align */
	CodeSize = MAX( CodeSize, (CodeChunkSize + 2048) );
	SD.sd_CodeSize = CodeSize;

	
/* Convert all fields in structure from Native to BigEndian. */
	p = (uint32 *) &SD;
	for( i=0; i<((int)(sizeof(SD)/sizeof(int32))); i++ )
	{
		WriteLongBigEndian( &p[i], p[i] );
	}

	if( WriteChunk( fid, ID_P4DI, (char *) &SD, sizeof(DictionaryInfoChunk) ) < 0 ) goto error;

/* Write Name Fields if NameSize non-zero ------- */
	if( NameSize > 0 )
	{
		if( WriteChunk( fid, ID_P4NM, (char *) NAME_BASE,
			NameChunkSize ) < 0 ) goto error;
	}

/* Write Code Fields ---------------------------- */
	if( WriteChunk( fid, ID_P4CD, (char *) CODE_BASE,
		CodeChunkSize ) < 0 ) goto error;

	FormSize = sdTellFile( fid ) - 8;
	sdSeekFile( fid, 4, PF_SEEK_SET );
	if( WriteLong( fid, FormSize ) < 0 ) goto error;

	sdCloseFile( fid );



/* Restore initialization. */

	pfExecIfDefined("AUTO.INIT");

	return 0;

error:
	sdSeekFile( fid, 0, PF_SEEK_SET );
	WriteLong( fid, ID_BADF ); /* Mark file as bad. */
	sdCloseFile( fid );

/* Restore initialization. */

	pfExecIfDefined("AUTO.INIT");

	return -1;
}

#endif /* !PF_NO_FILEIO and !PF_NO_SHELL */


#ifndef PF_NO_FILEIO

/***************************************************************/
static int32 ReadLong( FileStream *fid, int32 *ValPtr )
{
	int32 numr;
	uint32 temp;

	numr = sdReadFile( &temp, 1, sizeof(int32), fid );
	if( numr != sizeof(int32) ) return -1;
	*ValPtr = ReadLongBigEndian( &temp );
	return 0;
}

/***************************************************************/
PForthDictionary pfLoadDictionary( const char *FileName, ExecToken *EntryPointPtr )
{
	pfDictionary_t *dic = NULL;
	FileStream *fid;
	DictionaryInfoChunk *sd;
	int32 ChunkID;
	int32 ChunkSize;
	int32 FormSize;
	int32 BytesLeft;
	int32 numr;
	uint32 *p;
	int   i;
	int   isDicBigEndian;

DBUG(("pfLoadDictionary( %s )\n", FileName ));

/* Open file. */
	fid = sdOpenFile( FileName, "rb" );
	if( fid == NULL )
	{
		pfReportError("pfLoadDictionary", PF_ERR_OPEN_FILE);
		goto xt_error;
	}

/* Read FORM, Size, ID */
	if (ReadLong( fid, &ChunkID ) < 0) goto read_error;
	if( ChunkID != ID_FORM )
	{
		pfReportError("pfLoadDictionary", PF_ERR_WRONG_FILE);
		goto error;
	}

	if (ReadLong( fid, &FormSize ) < 0) goto read_error;
	BytesLeft = FormSize;

	if (ReadLong( fid, &ChunkID ) < 0) goto read_error;
	BytesLeft -= 4;
	if( ChunkID != ID_P4TH )
	{
		pfReportError("pfLoadDictionary", PF_ERR_BAD_FILE);
		goto error;
	}

/* Scan and parse all chunks in file. */
	while( BytesLeft > 0 )
	{
		if (ReadLong( fid, &ChunkID ) < 0) goto read_error;
		if (ReadLong( fid, &ChunkSize ) < 0) goto read_error;
		BytesLeft -= 8;

		DBUG(("ChunkID = %4s, Size = %d\n", &ChunkID, ChunkSize ));

		switch( ChunkID )
		{
		case ID_P4DI:
			sd = (DictionaryInfoChunk *) pfAllocMem( ChunkSize );
			if( sd == NULL ) goto nomem_error;

			numr = sdReadFile( sd, 1, ChunkSize, fid );
			if( numr != ChunkSize ) goto read_error;
			BytesLeft -= ChunkSize;
			
/* Convert all fields in structure from BigEndian to Native. */
			p = (uint32 *) sd;
			for( i=0; i<((int)(sizeof(*sd)/sizeof(int32))); i++ )
			{
				p[i] = ReadLongBigEndian( &p[i] );
			}

			isDicBigEndian = sd->sd_Flags & SD_F_BIG_ENDIAN_DIC;

			if( !gVarQuiet )
			{
				MSG("pForth loading dictionary from file "); MSG(FileName);
					EMIT_CR;
				MSG_NUM_D("     File format version is ", sd->sd_Version );
				MSG_NUM_D("     Name space size = ", sd->sd_NameSize );
				MSG_NUM_D("     Code space size = ", sd->sd_CodeSize );
				MSG_NUM_D("     Entry Point     = ", sd->sd_EntryPoint );
				MSG( (isDicBigEndian ? "     Big Endian Dictionary" :
				                       "     Little  Endian Dictionary") );
				if( isDicBigEndian == IsHostLittleEndian() ) MSG(" !!!!");
					EMIT_CR;
			}

			if( sd->sd_Version > PF_FILE_VERSION )
			{
				pfReportError("pfLoadDictionary", PF_ERR_VERSION_FUTURE );
				goto error;
			}
			if( sd->sd_Version < PF_EARLIEST_FILE_VERSION )
			{
				pfReportError("pfLoadDictionary", PF_ERR_VERSION_PAST );
				goto error;
			}
			if( sd->sd_NumPrimitives > NUM_PRIMITIVES )
			{
				pfReportError("pfLoadDictionary", PF_ERR_NOT_SUPPORTED );
				goto error;
			}

/* Check to make sure that EndianNess of dictionary matches mode of pForth. */
#if defined(PF_BIG_ENDIAN_DIC)
			if(isDicBigEndian == 0)
#elif defined(PF_LITTLE_ENDIAN_DIC)
			if(isDicBigEndian == 1)
#else
			if( isDicBigEndian == IsHostLittleEndian() )
#endif
			{
				pfReportError("pfLoadDictionary", PF_ERR_ENDIAN_CONFLICT );
				goto error;
			}

/* Check for compatible float size. */
#ifdef PF_SUPPORT_FP
			if( sd->sd_FloatSize != sizeof(PF_FLOAT) )
#else
			if( sd->sd_FloatSize != 0 )
#endif
			{
				pfReportError("pfLoadDictionary", PF_ERR_FLOAT_CONFLICT );
				goto error;
			}

			dic = pfCreateDictionary( sd->sd_NameSize, sd->sd_CodeSize );
			if( dic == NULL ) goto nomem_error;
			gCurrentDictionary = dic;
			if( sd->sd_NameSize > 0 )
			{
				gVarContext = (char *) NAMEREL_TO_ABS(sd->sd_RelContext); /* Restore context. */
				gCurrentDictionary->dic_HeaderPtr.Byte = (uint8 *)
					NAMEREL_TO_ABS(sd->sd_RelHeaderPtr);
			}
			else
			{
				gVarContext = 0;
				gCurrentDictionary->dic_HeaderPtr.Byte = NULL;
			}
			gCurrentDictionary->dic_CodePtr.Byte = (uint8 *) CODEREL_TO_ABS(sd->sd_RelCodePtr);
			gNumPrimitives = sd->sd_NumPrimitives;  /* Must match compiled dictionary. */
/* Pass EntryPoint back to caller. */
			if( EntryPointPtr != NULL ) *EntryPointPtr = sd->sd_EntryPoint;
			pfFreeMem(sd);
			break;

		case ID_P4NM:
#ifdef PF_NO_SHELL
			pfReportError("pfLoadDictionary", PF_ERR_NO_SHELL );
			goto error;
#else
			if( NAME_BASE == NULL )
			{
				pfReportError("pfLoadDictionary", PF_ERR_NO_NAMES );
				goto error;
			}
			if( gCurrentDictionary == NULL )
			{
				pfReportError("pfLoadDictionary", PF_ERR_BAD_FILE );
				goto error;
			}
			if( ChunkSize > NAME_SIZE )
			{
				pfReportError("pfLoadDictionary", PF_ERR_TOO_BIG);
				goto error;
			}
			numr = sdReadFile( NAME_BASE, 1, ChunkSize, fid );
			if( numr != ChunkSize ) goto read_error;
			BytesLeft -= ChunkSize;
#endif /* PF_NO_SHELL */
			break;

		case ID_P4CD:
			if( gCurrentDictionary == NULL )
			{
				pfReportError("pfLoadDictionary", PF_ERR_BAD_FILE );
				goto error;
			}
			if( ChunkSize > CODE_SIZE )
			{
				pfReportError("pfLoadDictionary", PF_ERR_TOO_BIG);
				goto error;
			}
			numr = sdReadFile( CODE_BASE, 1, ChunkSize, fid );
			if( numr != ChunkSize ) goto read_error;
			BytesLeft -= ChunkSize;
			break;

		default:
			pfReportError("pfLoadDictionary", PF_ERR_BAD_FILE );
			sdSeekFile( fid, ChunkSize, PF_SEEK_CUR );
			break;
		}
	}

	sdCloseFile( fid );

	if( NAME_BASE != NULL)
	{
		int32 Result;
/* Find special words in dictionary for global XTs. */
		if( (Result = FindSpecialXTs()) < 0 )
		{
			pfReportError("pfLoadDictionary: FindSpecialXTs", Result);
			goto error;
		}
	}

DBUG(("pfLoadDictionary: return 0x%x\n", dic));
	return (PForthDictionary) dic;

nomem_error:
	pfReportError("pfLoadDictionary", PF_ERR_NO_MEM);
	sdCloseFile( fid );
	return NULL;

read_error:
	pfReportError("pfLoadDictionary", PF_ERR_READ_FILE);
error:
	sdCloseFile( fid );
xt_error:
	return NULL;
}

#else

PForthDictionary pfLoadDictionary( const char *FileName, ExecToken *EntryPointPtr )
{
	(void) FileName;
	(void) EntryPointPtr;
	return NULL;
}
#endif /* !PF_NO_FILEIO */



/***************************************************************/
PForthDictionary pfLoadStaticDictionary( void )
{
#ifdef PF_STATIC_DIC
	int32 Result;
	pfDictionary_t *dic;
	int32 NewNameSize, NewCodeSize;
	
	if( IF_LITTLE_ENDIAN != IsHostLittleEndian() )
	{
		MSG( (IF_LITTLE_ENDIAN ?
			     "Little Endian Dictionary on " :
				 "Big Endian Dictionary on ") );
		MSG( (IsHostLittleEndian() ?
			     "Little Endian CPU" :
				 "Big Endian CPU") );
		EMIT_CR;
	}
	
/* Check to make sure that EndianNess of dictionary matches mode of pForth. */
#if defined(PF_BIG_ENDIAN_DIC)
	if(IF_LITTLE_ENDIAN == 1)
#elif defined(PF_LITTLE_ENDIAN_DIC)
	if(IF_LITTLE_ENDIAN == 0)
#else /* Code is native endian! */
	if( IF_LITTLE_ENDIAN != IsHostLittleEndian() )
#endif
	{
		pfReportError("pfLoadStaticDictionary", PF_ERR_ENDIAN_CONFLICT );
		goto error;
	}


#ifndef PF_EXTRA_HEADERS
	#define PF_EXTRA_HEADERS  (20000)
#endif
#ifndef PF_EXTRA_CODE
	#define PF_EXTRA_CODE  (40000)
#endif

/* Copy static const data to allocated dictionaries. */
	NewNameSize = sizeof(MinDicNames) + PF_EXTRA_HEADERS;
	NewCodeSize = sizeof(MinDicCode) + PF_EXTRA_CODE;

	DBUG_NUM_D( "static dic name size = ", NewNameSize );
	DBUG_NUM_D( "static dic code size = ", NewCodeSize );
	
	gCurrentDictionary = dic = pfCreateDictionary( NewNameSize, NewCodeSize );
	if( !dic ) goto nomem_error;

	pfCopyMemory( dic->dic_HeaderBase, MinDicNames, sizeof(MinDicNames) );
	pfCopyMemory( dic->dic_CodeBase, MinDicCode, sizeof(MinDicCode) );
	DBUG("Static data copied to newly allocated dictionaries.\n");

	dic->dic_CodePtr.Byte = (uint8 *) CODEREL_TO_ABS(CODEPTR);
	gNumPrimitives = NUM_PRIMITIVES;

	if( NAME_BASE != NULL)
	{
/* Setup name space. */
		dic->dic_HeaderPtr.Byte = (uint8 *) NAMEREL_TO_ABS(HEADERPTR);
		gVarContext = (char *) NAMEREL_TO_ABS(RELCONTEXT); /* Restore context. */

/* Find special words in dictionary for global XTs. */
		if( (Result = FindSpecialXTs()) < 0 )
		{
			pfReportError("pfLoadStaticDictionary: FindSpecialXTs", Result);
			goto error;
		}
	}

	return (PForthDictionary) dic;

error:
	return NULL;

nomem_error:
	pfReportError("pfLoadStaticDictionary", PF_ERR_NO_MEM);
#endif /* PF_STATIC_DIC */

	return NULL;
}

