/* @(#) pf_inner.c 98/03/16 1.7 */
/***************************************************************
** Inner Interpreter for Forth based on 'C'
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
**
** 940502 PLB Creation.
** 940505 PLB More macros.
** 940509 PLB Moved all stack stuff into pfCatch.
** 941014 PLB Converted to flat secondary strusture.
** 941027 rdg added casts to ID_SP_FETCH, ID_RP_FETCH, 
**             and ID_HERE for armcc
** 941130 PLB Made w@ unsigned
**
***************************************************************/

#include "pf_all.h"

#ifdef WIN32
#include <crtdbg.h>
#endif

#define SYSTEM_LOAD_FILE "system.fth"

/***************************************************************
** Macros for data stack access.
** TOS is cached in a register in pfCatch.
***************************************************************/

#define STKPTR   (DataStackPtr)
#define M_POP    (*(STKPTR++))
#define M_PUSH(n) {*(--(STKPTR)) = (cell) (n);}
#define M_STACK(n) (STKPTR[n])

#define TOS      (TopOfStack)
#define PUSH_TOS M_PUSH(TOS)
#define M_DUP    PUSH_TOS;
#define M_DROP   { TOS = M_POP; }


/***************************************************************
** Macros for Floating Point stack access.
***************************************************************/
#ifdef PF_SUPPORT_FP
#define FP_STKPTR   (FloatStackPtr)
#define M_FP_SPZERO (gCurrentTask->td_FloatStackBase)
#define M_FP_POP    (*(FP_STKPTR++))
#define M_FP_PUSH(n) {*(--(FP_STKPTR)) = (PF_FLOAT) (n);}
#define M_FP_STACK(n) (FP_STKPTR[n])

#define FP_TOS      (fpTopOfStack)
#define PUSH_FP_TOS M_FP_PUSH(FP_TOS)
#define M_FP_DUP    PUSH_FP_TOS;
#define M_FP_DROP   { FP_TOS = M_FP_POP; }
#endif

/***************************************************************
** Macros for return stack access.
***************************************************************/

#define TORPTR (ReturnStackPtr)
#define M_R_DROP {TORPTR++;}
#define M_R_POP (*(TORPTR++))
#define M_R_PICK(n) (TORPTR[n])
#define M_R_PUSH(n) {*(--(TORPTR)) = (cell) (n);}

/***************************************************************
** Misc Forth macros
***************************************************************/
			
#define M_BRANCH   { InsPtr = (cell *) (((uint8 *) InsPtr) + READ_LONG_DIC(InsPtr)); }

/* Cache top of data stack like in JForth. */
#ifdef PF_SUPPORT_FP
#define LOAD_REGISTERS \
	{ \
		STKPTR = gCurrentTask->td_StackPtr; \
	 	TOS = M_POP; \
		FP_STKPTR = gCurrentTask->td_FloatStackPtr; \
		FP_TOS = M_FP_POP; \
		TORPTR = gCurrentTask->td_ReturnPtr; \
	 }
	 
#define SAVE_REGISTERS \
	{ \
		gCurrentTask->td_ReturnPtr = TORPTR; \
		M_PUSH( TOS ); \
		gCurrentTask->td_StackPtr = STKPTR; \
		M_FP_PUSH( FP_TOS ); \
		gCurrentTask->td_FloatStackPtr = FP_STKPTR; \
	 }
	 
#else
/* Cache top of data stack like in JForth. */
#define LOAD_REGISTERS \
	{ \
		STKPTR = gCurrentTask->td_StackPtr; \
	 	TOS = M_POP; \
		TORPTR = gCurrentTask->td_ReturnPtr; \
	 }
	 
#define SAVE_REGISTERS \
	{ \
		gCurrentTask->td_ReturnPtr = TORPTR; \
		M_PUSH( TOS ); \
		gCurrentTask->td_StackPtr = STKPTR; \
	 }
#endif

#define M_DOTS \
	SAVE_REGISTERS; \
	ffDotS( ); \
	LOAD_REGISTERS;
	
#define DO_VAR(varname) { PUSH_TOS; TOS = (cell) &varname; }

#ifdef PF_SUPPORT_FP
#define M_THROW(err) \
	{ \
		ExceptionReturnCode = (ThrowCode)(err); \
		TORPTR = InitialReturnStack; /* Will cause return to 'C' */ \
		STKPTR = InitialDataStack; \
		FP_STKPTR = InitialFloatStack; \
	}
#else
#define M_THROW(err) \
	{ \
		ExceptionReturnCode = (err); \
		TORPTR = InitialReturnStack; /* Will cause return to 'C' */ \
		STKPTR = InitialDataStack; \
	}
#endif

/***************************************************************
** Other macros
***************************************************************/

#define BINARY_OP( op ) { TOS = M_POP op TOS; }
#define endcase break
		
#if defined(PF_NO_SHELL) || !defined(PF_SUPPORT_TRACE)
	#define TRACENAMES /* no names */
#else
/* Display name of executing routine. */
static void TraceNames( ExecToken Token, int32 Level )
{
	char *DebugName;
	int32 i;
	
	if( ffTokenToName( Token, &DebugName ) )
	{
		cell NumSpaces;
		if( gCurrentTask->td_OUT > 0 ) EMIT_CR;
		EMIT( '>' );
		for( i=0; i<Level; i++ )
		{
			MSG( "  " );
		}
		TypeName( DebugName );
/* Space out to column N then .S */
		NumSpaces = 30 - gCurrentTask->td_OUT;
		for( i=0; i < NumSpaces; i++ )
		{
			EMIT( ' ' );
		}
		ffDotS();
/* No longer needed?		gCurrentTask->td_OUT = 0; */ /* !!! Hack for ffDotS() */
		
	}
	else
	{
		MSG_NUM_H("Couldn't find Name for ", Token);
	}
}

#define TRACENAMES \
	if( (gVarTraceLevel > Level) ) \
	{ SAVE_REGISTERS; TraceNames( Token, Level ); LOAD_REGISTERS; }
#endif /* PF_NO_SHELL */

/* Use local copy of CODE_BASE for speed. */
#define LOCAL_CODEREL_TO_ABS( a ) ((cell *) (((int32) a) + CodeBase))

static const char *pfSelectFileModeCreate( int fam );
static const char *pfSelectFileModeOpen( int fam );

/**************************************************************/
static const char *pfSelectFileModeCreate( int fam )
{
	const char *famText = NULL;
	switch( fam )
	{
	case (PF_FAM_WRITE_ONLY + PF_FAM_BINARY_FLAG):
		famText = PF_FAM_BIN_CREATE_WO;
		break;
	case (PF_FAM_READ_WRITE + PF_FAM_BINARY_FLAG):
		famText = PF_FAM_BIN_CREATE_RW;
		break;
	case PF_FAM_WRITE_ONLY:
		famText = PF_FAM_CREATE_WO;
		break;
	case PF_FAM_READ_WRITE:
		famText = PF_FAM_CREATE_RW;
		break;
	default:
		famText = "illegal";
		break;
	}
	return famText;
}

/**************************************************************/
static const char *pfSelectFileModeOpen( int fam )
{
	const char *famText = NULL;
	switch( fam )
	{
	case (PF_FAM_READ_ONLY + PF_FAM_BINARY_FLAG):
		famText = PF_FAM_BIN_OPEN_RO;
		break;
	case (PF_FAM_WRITE_ONLY + PF_FAM_BINARY_FLAG):
		famText = PF_FAM_BIN_CREATE_WO;
		break;
	case (PF_FAM_READ_WRITE + PF_FAM_BINARY_FLAG):
		famText = PF_FAM_BIN_OPEN_RW;
		break;
	case PF_FAM_READ_ONLY:
		famText = PF_FAM_OPEN_RO;
		break;
	case PF_FAM_WRITE_ONLY:
		famText = PF_FAM_CREATE_WO;
		break;
	case PF_FAM_READ_WRITE:
	default:
		famText = PF_FAM_OPEN_RW;
		break;
	}
	return famText;
}

/**************************************************************/
int pfCatch( ExecToken XT )
{
	register cell  TopOfStack;    /* Cache for faster execution. */
	register cell *DataStackPtr;
	register cell *ReturnStackPtr;
	register cell *InsPtr = NULL;
	register cell  Token;
	cell           Scratch;
	
#ifdef PF_SUPPORT_FP
	PF_FLOAT       fpTopOfStack;
	PF_FLOAT      *FloatStackPtr;
	PF_FLOAT       fpScratch;
	PF_FLOAT       fpTemp;
	PF_FLOAT      *InitialFloatStack;
#endif
#ifdef PF_SUPPORT_TRACE
	int32 Level = 0;
#endif
	cell          *LocalsPtr = NULL;
	cell           Temp;
	cell          *InitialReturnStack;
	cell          *InitialDataStack;
	cell           FakeSecondary[2];
	char          *CharPtr;
	cell          *CellPtr;
	FileStream    *FileID;
	uint8         *CodeBase = CODE_BASE;
	ThrowCode      ExceptionReturnCode = 0;
	
/* FIXME
	gExecutionDepth += 1;
	PRT(("pfCatch( 0x%x ), depth = %d\n", XT, gExecutionDepth ));
*/

/*
** Initialize FakeSecondary this way to avoid having stuff in the data section,
** which is not supported for some embedded system loaders.
*/
	FakeSecondary[0] = 0;
	FakeSecondary[1] = ID_EXIT; /* For EXECUTE */

/* Move data from task structure to registers for speed. */
	LOAD_REGISTERS;

/* Save initial stack depths for THROW */
	InitialReturnStack = TORPTR;
	InitialDataStack   = STKPTR ;
#ifdef PF_SUPPORT_FP
	InitialFloatStack  = FP_STKPTR;
#endif

	Token = XT;

	do
	{
DBUG(("pfCatch: Token = 0x%x\n", Token ));

/* --------------------------------------------------------------- */
/* If secondary, thread down code tree until we hit a primitive. */
		while( !IsTokenPrimitive( Token ) )
		{
#ifdef PF_SUPPORT_TRACE
			if((gVarTraceFlags & TRACE_INNER) )
			{
				MSG("pfCatch: Secondary Token = 0x");
				ffDotHex(Token);
				MSG_NUM_H(", InsPtr = 0x", InsPtr);
			}
			TRACENAMES;
#endif

/* Save IP on return stack like a JSR. */
			M_R_PUSH( InsPtr );
			
/* Convert execution token to absolute address. */
			InsPtr = (cell *) ( LOCAL_CODEREL_TO_ABS(Token) );

/* Fetch token at IP. */
			Token = READ_LONG_DIC(InsPtr++);
			
#ifdef PF_SUPPORT_TRACE
/* Bump level for trace display */
			Level++;
#endif
		}

		
#ifdef PF_SUPPORT_TRACE
		TRACENAMES;
#endif
			
/* Execute primitive Token. */
		switch( Token )
		{
			
	/* Pop up a level in Forth inner interpreter.
	** Used to implement semicolon.
	** Put first in switch because ID_EXIT==0 */
		case ID_EXIT:
			InsPtr = ( cell *) M_R_POP;
#ifdef PF_SUPPORT_TRACE
			Level--;
#endif
			endcase;
						
		case ID_1MINUS:  TOS--; endcase;
			
		case ID_1PLUS:   TOS++; endcase;
			
#ifndef PF_NO_SHELL
		case ID_2LITERAL:
			ff2Literal( TOS, M_POP );
			M_DROP;
			endcase;
#endif  /* !PF_NO_SHELL */

		case ID_2LITERAL_P:
/* hi part stored first, put on top of stack */
			PUSH_TOS;
			TOS = READ_LONG_DIC(InsPtr++);
			M_PUSH(READ_LONG_DIC(InsPtr++));
			endcase;
			
		case ID_2MINUS:  TOS -= 2; endcase;
			
		case ID_2PLUS:   TOS += 2; endcase;
		
		
		case ID_2OVER:  /* ( a b c d -- a b c d a b ) */
			PUSH_TOS;
			Scratch = M_STACK(3);
			M_PUSH(Scratch);
			TOS = M_STACK(3);
			endcase;
			
		case ID_2SWAP:  /* ( a b c d -- c d a b ) */
			Scratch = M_STACK(0);    /* c */
			M_STACK(0) = M_STACK(2); /* a */
			M_STACK(2) = Scratch;    /* c */
			Scratch = TOS;           /* d */
			TOS = M_STACK(1);        /* b */
			M_STACK(1) = Scratch;    /* d */
			endcase;
			
		case ID_2DUP:   /* ( a b -- a b a b ) */
			PUSH_TOS;
			Scratch = M_STACK(1);
			M_PUSH(Scratch);
			endcase;
			
		case ID_2_R_FETCH:
			PUSH_TOS;
			M_PUSH( (*(TORPTR+1)) );
			TOS = (*(TORPTR));
			endcase;

		case ID_2_R_FROM:
			PUSH_TOS;
			TOS = M_R_POP;
			M_PUSH( M_R_POP );
			endcase;

		case ID_2_TO_R:
			M_R_PUSH( M_POP );
			M_R_PUSH( TOS );
			M_DROP;
			endcase;
		
		case ID_ACCEPT_P: /* ( c-addr +n1 -- +n2 ) */
			CharPtr = (char *) M_POP;
			TOS = ioAccept( CharPtr, TOS );
			endcase;
			
#ifndef PF_NO_SHELL
		case ID_ALITERAL:
			ffALiteral( ABS_TO_CODEREL(TOS) );
			M_DROP;
			endcase;
#endif  /* !PF_NO_SHELL */

		case ID_ALITERAL_P:
			PUSH_TOS;
			TOS = (cell) LOCAL_CODEREL_TO_ABS( READ_LONG_DIC(InsPtr++) );
			endcase;
			
/* Allocate some extra and put validation identifier at base */
#define PF_MEMORY_VALIDATOR  (0xA81B4D69)
		case ID_ALLOCATE:
			/* Allocate at least one cell's worth because we clobber first cell. */
			if ( TOS < sizeof(cell) )
			{
				Temp = sizeof(cell);
			}
			else
			{
				Temp = TOS;
			}
			/* Allocate extra cells worth because we store validation info. */
			CellPtr = (cell *) pfAllocMem( Temp + sizeof(cell) );
			if( CellPtr )
			{
/* This was broken into two steps because different compilers incremented
** CellPtr before or after the XOR step. */
				Temp = (int32)CellPtr ^ PF_MEMORY_VALIDATOR;
				*CellPtr++ = Temp;
				M_PUSH( (cell) CellPtr );
				TOS = 0;
			}
			else
			{
				M_PUSH( 0 );
				TOS = -1;  /* FIXME Fix error code. */
			}
			endcase;

		case ID_AND:     BINARY_OP( & ); endcase;
		
		case ID_ARSHIFT:     BINARY_OP( >> ); endcase;  /* Arithmetic right shift */
		
		case ID_BODY_OFFSET:
			PUSH_TOS;
			TOS = CREATE_BODY_OFFSET;
			endcase;
			
/* Branch is followed by an offset relative to address of offset. */
		case ID_BRANCH:
DBUGX(("Before Branch: IP = 0x%x\n", InsPtr ));
			M_BRANCH;
DBUGX(("After Branch: IP = 0x%x\n", InsPtr ));
			endcase;

		case ID_BYE:
			M_THROW( THROW_BYE );
			endcase;

		case ID_BAIL:
			MSG("Emergency exit.\n");
			EXIT(1);
			endcase;
		
		case ID_CATCH:
			Scratch = TOS;
			TOS = M_POP;
			SAVE_REGISTERS;
			Scratch = pfCatch( Scratch );
			LOAD_REGISTERS;
			M_PUSH( TOS );
			TOS = Scratch;
			endcase;

		case ID_CALL_C:
			SAVE_REGISTERS;
			Scratch = READ_LONG_DIC(InsPtr++);
			CallUserFunction( Scratch & 0xFFFF,
				(Scratch >> 31) & 1,
				(Scratch >> 24) & 0x7F );
			LOAD_REGISTERS;
			endcase;

		case ID_CFETCH:   TOS = *((uint8 *) TOS); endcase;

		case ID_CMOVE: /* ( src dst n -- ) */
			{
				register char *DstPtr = (char *) M_POP; /* dst */
				CharPtr = (char *) M_POP;    /* src */
				for( Scratch=0; (uint32) Scratch < (uint32) TOS ; Scratch++ )
				{
					*DstPtr++ = *CharPtr++;
				}
				M_DROP;
			}
			endcase;
			
		case ID_CMOVE_UP: /* ( src dst n -- ) */
			{
				register char *DstPtr = ((char *) M_POP) + TOS; /* dst */
				CharPtr = ((char *) M_POP) + TOS;;    /* src */
				for( Scratch=0; (uint32) Scratch < (uint32) TOS ; Scratch++ )
				{
					*(--DstPtr) = *(--CharPtr);
				}
				M_DROP;
			}
			endcase;
		
#ifndef PF_NO_SHELL
		case ID_COLON:
			SAVE_REGISTERS;
			ffColon( );
			LOAD_REGISTERS;
			endcase;
		case ID_COLON_P:  /* ( $name xt -- ) */
			CreateDicEntry( TOS, (char *) M_POP, 0 );
			M_DROP;
			endcase;
#endif  /* !PF_NO_SHELL */

		case ID_COMPARE:
			{
				const char *s1, *s2;
				int32 len1;
				s2 = (const char *) M_POP;
				len1 = M_POP;
				s1 = (const char *) M_POP;
				TOS = ffCompare( s1, len1, s2, TOS );
			}
			endcase;
			
/* ( a b -- flag , Comparisons ) */
		case ID_COMP_EQUAL:
			TOS = ( TOS == M_POP ) ? FTRUE : FFALSE ;
			endcase;
		case ID_COMP_NOT_EQUAL:
			TOS = ( TOS != M_POP ) ? FTRUE : FFALSE ;
			endcase;
		case ID_COMP_GREATERTHAN:
			TOS = ( M_POP > TOS ) ? FTRUE : FFALSE ;
			endcase;
		case ID_COMP_LESSTHAN:
			TOS = (  M_POP < TOS ) ? FTRUE : FFALSE ;
			endcase;
		case ID_COMP_U_GREATERTHAN:
			TOS = ( ((uint32)M_POP) > ((uint32)TOS) ) ? FTRUE : FFALSE ;
			endcase;
		case ID_COMP_U_LESSTHAN:
			TOS = ( ((uint32)M_POP) < ((uint32)TOS) ) ? FTRUE : FFALSE ;
			endcase;
		case ID_COMP_ZERO_EQUAL:
			TOS = ( TOS == 0 ) ? FTRUE : FFALSE ;
			endcase;
		case ID_COMP_ZERO_NOT_EQUAL:
			TOS = ( TOS != 0 ) ? FTRUE : FALSE ;
			endcase;
		case ID_COMP_ZERO_GREATERTHAN:
			TOS = ( TOS > 0 ) ? FTRUE : FFALSE ;
			endcase;
		case ID_COMP_ZERO_LESSTHAN:
			TOS = ( TOS < 0 ) ? FTRUE : FFALSE ;
			endcase;
			
		case ID_CR:
			EMIT_CR;
			endcase;
		
#ifndef PF_NO_SHELL
		case ID_CREATE:
			SAVE_REGISTERS;
			ffCreate();
			LOAD_REGISTERS;
			endcase;
#endif  /* !PF_NO_SHELL */

		case ID_CREATE_P:
			PUSH_TOS;
/* Put address of body on stack.  Insptr points after code start. */
			TOS = (cell) ((char *)InsPtr - sizeof(cell) + CREATE_BODY_OFFSET );
			endcase;
	
		case ID_CSTORE: /* ( c caddr -- ) */
			*((uint8 *) TOS) = (uint8) M_POP;
			M_DROP;
			endcase;

/* Double precision add. */
		case ID_D_PLUS:  /* D+ ( al ah bl bh -- sl sh ) */ 
			{
				register ucell ah,al,bl,sh,sl;
#define bh TOS
				bl = M_POP;
				ah = M_POP;
				al = M_POP;
				sh = 0;
				sl = al + bl;
				if( sl < bl ) sh = 1; /* Carry */
				sh += ah + bh;
				M_PUSH( sl );
				TOS = sh;
#undef bh
			}
			endcase;
	
/* Double precision subtract. */
		case ID_D_MINUS:  /* D- ( al ah bl bh -- sl sh ) */ 
			{
				register ucell ah,al,bl,sh,sl;
#define bh TOS
				bl = M_POP;
				ah = M_POP;
				al = M_POP;
				sh = 0;
				sl = al - bl;
				if( al < bl ) sh = 1; /* Borrow */
				sh = ah - bh - sh;
				M_PUSH( sl );
				TOS = sh;
#undef bh
			}
			endcase;
			
/* Perform 32*32 bit multiply for 64 bit result, by factoring into 16 bit quantities. */
/* Using an improved algorithm suggested by Steve Green. */
		case ID_D_UMTIMES:  /* M* ( a b -- pl ph ) */ 
			{
				ucell ahi, alo, bhi, blo, temp;
				ucell pl, ph;
/* Get values from stack. */
				ahi = M_POP;
				bhi = TOS;
/* Break into hi and lo 16 bit parts. */
				alo = ahi & 0xFFFF;
				ahi = ahi>>16;
				blo = bhi & 0xFFFF;
				bhi = bhi>>16;
				ph = 0;
/* ahi * bhi */
				pl = ahi * bhi;
				ph = pl >> 16;  /* shift 64 bit value by 16 */
				pl = pl << 16;
/* ahi * blo */
				temp = ahi * blo;
				pl += temp;
				if( pl < temp ) ph += 1; /* Carry */
/* alo * bhi  */
				temp = alo * bhi;
				pl += temp;
				if( pl < temp ) ph += 1; /* Carry */
				ph = (ph << 16) | (pl >> 16); /* shift 64 bit value by 16 */
				pl = pl << 16;
/* alo * blo */
				temp = alo * blo;
				pl += temp;
				if( pl < temp ) ph += 1; /* Carry */

				M_PUSH( pl );
				TOS = ph;
			}
			endcase;
			
/* Perform 32*32 bit multiply for 64 bit result, using shift and add. */
		case ID_D_MTIMES:  /* M* ( a b -- pl ph ) */ 
			{
				cell a,b;
				ucell ap,bp, ahi, alo, bhi, blo, temp;
				ucell pl, ph;
/* Get values from stack. */
				a = M_POP;
				b = TOS;
				ap = (a < 0) ? -a : a ; /* Positive A */
				bp = (b < 0) ? -b : b ; /* Positive B */
/* Break into hi and lo 16 bit parts. */
				alo = ap & 0xFFFF;
				ahi = ap>>16;
				blo = bp & 0xFFFF;
				bhi = bp>>16;
				ph = 0;
/* ahi * bhi */
				pl = ahi * bhi;
				ph = pl >> 16;  /* shift 64 bit value by 16 */
				pl = pl << 16;
/* ahi * blo */
				temp = ahi * blo;
				pl += temp;
				if( pl < temp ) ph += 1; /* Carry */
/* alo * bhi  */
				temp = alo * bhi;
				pl += temp;
				if( pl < temp ) ph += 1; /* Carry */
				ph = (ph << 16) | (pl >> 16); /* shift 64 bit value by 16 */
				pl = pl << 16;
/* alo * blo */
				temp = alo * blo;
				pl += temp;
				if( pl < temp ) ph += 1; /* Carry */

/* Negate product if one operand negative. */
				if( ((a ^ b) & 0x80000000) )
				{
					pl = 0-pl;
					if( pl & 0x80000000 )
					{
						ph = -1 - ph;   /* Borrow */
					}
					else
					{
						ph = 0 - ph;
					}
				}

				M_PUSH( pl );
				TOS = ph;
			}
			endcase;

#define DULT(du1l,du1h,du2l,du2h) ( (du2h<du1h) ? FALSE : ( (du2h==du1h) ? (du1l<du2l) : TRUE) )
/* Perform 64/32 bit divide for 32 bit result, using shift and subtract. */
		case ID_D_UMSMOD:  /* UM/MOD ( al ah bdiv -- rem q ) */ 
			{
				ucell ah,al, q,di, bl,bh, sl,sh;
				ah = M_POP;
				al = M_POP;
				bh = TOS;
				bl = 0;
				q = 0;
				for( di=0; di<32; di++ )
				{
					if( !DULT(al,ah,bl,bh) )
					{
						sh = 0;
						sl = al - bl;
						if( al < bl ) sh = 1; /* Borrow */
						sh = ah - bh - sh;
						ah = sh;
						al = sl;
						q |= 1;
					}
					q = q << 1;
					bl = (bl >> 1) | (bh << 31);
					bh = bh >> 1;
				}
				if( !DULT(al,ah,bl,bh) )
				{
				
					al = al - bl;
					q |= 1;
				}
				M_PUSH( al );  /* rem */
				TOS = q;
			}
			endcase;

/* Perform 64/32 bit divide for 64 bit result, using shift and subtract. */
		case ID_D_MUSMOD:  /* MU/MOD ( al am bdiv -- rem ql qh ) */ 
			{
				register ucell ah,am,al,ql,qh,di;
#define bdiv ((ucell)TOS)
				ah = 0;
				am = M_POP;
				al = M_POP;
				qh = ql = 0;
				for( di=0; di<64; di++ )
				{
					if( bdiv <= ah )
					{
						ah = ah - bdiv;
						ql |= 1;
					}
					qh = (qh << 1) | (ql >> 31);
					ql = ql << 1;
					ah = (ah << 1) | (am >> 31);
					am = (am << 1) | (al >> 31);
					al = al << 1;
DBUG(("XX ah,m,l = 0x%8x,%8x,%8x - qh,l = 0x%8x,%8x\n", ah,am,al, qh,ql ));
				}
				if( bdiv <= ah )
				{
					ah = ah - bdiv;
					ql |= 1;
				}
				M_PUSH( ah ); /* rem */
				M_PUSH( ql );
				TOS = qh;
#undef bdiv
			}
			endcase;

#ifndef PF_NO_SHELL
		case ID_DEFER:
			ffDefer( );
			endcase;
#endif  /* !PF_NO_SHELL */

		case ID_DEFER_P:
			endcase;

		case ID_DEPTH:
			PUSH_TOS;
			TOS = gCurrentTask->td_StackBase - STKPTR;
			endcase;
			
		case ID_DIVIDE:     BINARY_OP( / ); endcase;
			
		case ID_DOT:
			ffDot( TOS );
			M_DROP;
			endcase;
			
		case ID_DOTS:
			M_DOTS;
			endcase;
			
		case ID_DROP:  M_DROP; endcase;
			
		case ID_DUMP:
			Scratch = M_POP;
			DumpMemory( (char *) Scratch, TOS );
			M_DROP;
			endcase;

		case ID_DUP:   M_DUP; endcase;
		
		case ID_DO_P: /* ( limit start -- ) ( R: -- start limit ) */
			M_R_PUSH( TOS );
			M_R_PUSH( M_POP );
			M_DROP;
			endcase;
			
		case ID_EOL:    /* ( -- end_of_line_char ) */
			PUSH_TOS;
			TOS = (cell) '\n';
			endcase;
			
		case ID_ERRORQ_P:  /* ( flag num -- , quit if flag true ) */
			Scratch = TOS;
			M_DROP;
			if(TOS)
			{
				M_THROW(Scratch);
			}
			else
			{
				M_DROP;
			}
			endcase;
			
		case ID_EMIT_P:
			EMIT( (char) TOS );
			M_DROP;
			endcase;
			
		case ID_EXECUTE:
/* Save IP on return stack like a JSR. */
			M_R_PUSH( InsPtr );
#ifdef PF_SUPPORT_TRACE
/* Bump level for trace. */
			Level++;
#endif
			if( IsTokenPrimitive( TOS ) )
			{
				WRITE_LONG_DIC( (cell *) &FakeSecondary[0], TOS);   /* Build a fake secondary and execute it. */
				InsPtr = &FakeSecondary[0];
			}
			else
			{
				InsPtr = (cell *) LOCAL_CODEREL_TO_ABS(TOS);
			}
			M_DROP;
			endcase;
			
		case ID_FETCH:
#if (defined(PF_BIG_ENDIAN_DIC) || defined(PF_LITTLE_ENDIAN_DIC))
			if( IN_DICS( TOS ) )
			{
				TOS = (cell) READ_LONG_DIC((cell *)TOS);
			}
			else
			{
				TOS = *((cell *)TOS);
			}
#else
			TOS = *((cell *)TOS);
#endif
			endcase;
		
		case ID_FILE_CREATE: /* ( c-addr u fam -- fid ior ) */
/* Build NUL terminated name string. */
			Scratch = M_POP; /* u */
			Temp = M_POP;    /* caddr */
			if( Scratch < TIB_SIZE-2 )
			{
				const char *famText = pfSelectFileModeCreate( TOS );
				pfCopyMemory( gScratch, (char *) Temp, (uint32) Scratch );
				gScratch[Scratch] = '\0';
				PRT(("Create file = %s with famTxt %s\n", gScratch, famText ));
				FileID = sdOpenFile( gScratch, famText );
				TOS = ( FileID == NULL ) ? -1 : 0 ;
				M_PUSH( (cell) FileID );
			}
			else
			{
				ERR("Filename too large for name buffer.\n");
				M_PUSH( 0 );
				TOS = -2;
			}
			endcase;

		case ID_FILE_OPEN: /* ( c-addr u fam -- fid ior ) */
/* Build NUL terminated name string. */
			Scratch = M_POP; /* u */
			Temp = M_POP;    /* caddr */
			if( Scratch < TIB_SIZE-2 )
			{
				const char *famText = pfSelectFileModeOpen( TOS );
				pfCopyMemory( gScratch, (char *) Temp, (uint32) Scratch );
				gScratch[Scratch] = '\0';
				DBUG(("Open file = %s\n", gScratch ));
				FileID = sdOpenFile( gScratch, famText );

				TOS = ( FileID == NULL ) ? -1 : 0 ;
				M_PUSH( (cell) FileID );
			}
			else
			{
				ERR("Filename too large for name buffer.\n");
				M_PUSH( 0 );
				TOS = -2;
			}
			endcase;
			
		case ID_FILE_CLOSE: /* ( fid -- ior ) */
			TOS = sdCloseFile( (FileStream *) TOS );
			endcase;
				
		case ID_FILE_READ: /* ( addr len fid -- u2 ior ) */
			FileID = (FileStream *) TOS;
			Scratch = M_POP;
			CharPtr = (char *) M_POP;
			Temp = sdReadFile( CharPtr, 1, Scratch, FileID );
			M_PUSH(Temp);
			TOS = 0;
			endcase;
				
		case ID_FILE_SIZE: /* ( fid -- ud ior ) */
/* Determine file size by seeking to end and returning position. */
			FileID = (FileStream *) TOS;
			Scratch = sdTellFile( FileID );
			sdSeekFile( FileID, 0, PF_SEEK_END );
			M_PUSH( sdTellFile( FileID ));
			sdSeekFile( FileID, Scratch, PF_SEEK_SET );
			TOS = (Scratch < 0) ? -4 : 0 ; /* !!! err num */
			endcase;

		case ID_FILE_WRITE: /* ( addr len fid -- ior ) */
			FileID = (FileStream *) TOS;
			Scratch = M_POP;
			CharPtr = (char *) M_POP;
			Temp = sdWriteFile( CharPtr, 1, Scratch, FileID );
			TOS = (Temp != Scratch) ? -3 : 0;
			endcase;

		case ID_FILE_REPOSITION: /* ( pos fid -- ior ) */
			FileID = (FileStream *) TOS;
			Scratch = M_POP;
			TOS = sdSeekFile( FileID, Scratch, PF_SEEK_SET );
			endcase;

		case ID_FILE_POSITION: /* ( pos fid -- ior ) */
			M_PUSH( sdTellFile( (FileStream *) TOS ));
			TOS = 0;
			endcase;

		case ID_FILE_RO: /* (  -- fam ) */
			PUSH_TOS;
			TOS = PF_FAM_READ_ONLY;
			endcase;
				
		case ID_FILE_RW: /* ( -- fam ) */
			PUSH_TOS;
			TOS = PF_FAM_READ_WRITE;
			endcase;

		case ID_FILE_WO: /* ( -- fam ) */
			PUSH_TOS;
			TOS = PF_FAM_WRITE_ONLY;
			endcase;

		case ID_FILE_BIN: /* ( -- fam ) */
			TOS = TOS | PF_FAM_BINARY_FLAG;
			endcase;
				
		case ID_FILL: /* ( caddr num charval -- ) */
			{
				register char *DstPtr;
				Temp = M_POP;    /* num */
				DstPtr = (char *) M_POP; /* dst */
				for( Scratch=0; (uint32) Scratch < (uint32) Temp ; Scratch++ )
				{
					*DstPtr++ = (char) TOS;
				}
				M_DROP;
			}
			endcase;
			
#ifndef PF_NO_SHELL
		case ID_FIND:  /* ( $addr -- $addr 0 | xt +-1 ) */
			TOS = ffFind( (char *) TOS, (ExecToken *) &Temp );
			M_PUSH( Temp );
			endcase;
			
		case ID_FINDNFA:
			TOS = ffFindNFA( (const ForthString *) TOS, (const ForthString **) &Temp );
			M_PUSH( (cell) Temp );
			endcase;
#endif  /* !PF_NO_SHELL */

		case ID_FLUSHEMIT:
			sdTerminalFlush();
			endcase;
			
/* Validate memory before freeing. Clobber validator and first word. */
		case ID_FREE:   /* ( addr -- result ) */
			if( TOS == 0 )
			{
				ERR("FREE passed NULL!\n");
				TOS = -2; /* FIXME error code */
			}
			else
			{
				CellPtr = (cell *) TOS;
				CellPtr--;
				if( ((uint32)*CellPtr) != ((uint32)CellPtr ^ PF_MEMORY_VALIDATOR))
				{
					TOS = -2; /* FIXME error code */
				}
				else
				{
					CellPtr[0] = 0xDeadBeef;
					pfFreeMem((char *)CellPtr);
					TOS = 0;
				}
			}
			endcase;
			
#include "pfinnrfp.h"

		case ID_HERE:
			PUSH_TOS;
			TOS = (cell)CODE_HERE;
			endcase;
		
		case ID_NUMBERQ_P:   /* ( addr -- 0 | n 1 ) */
/* Convert using number converter in 'C'.
** Only supports single precision for bootstrap.
*/
			TOS = (cell) ffNumberQ( (char *) TOS, &Temp );
			if( TOS == NUM_TYPE_SINGLE)
			{
				M_PUSH( Temp );   /* Push single number */
			}
			endcase;
			
		case ID_I:  /* ( -- i , DO LOOP index ) */
			PUSH_TOS;
			TOS = M_R_PICK(1);
			endcase;

#ifndef PF_NO_SHELL
		case ID_INCLUDE_FILE:
			FileID = (FileStream *) TOS;
			M_DROP;    /* Drop now so that INCLUDE has a clean stack. */
			SAVE_REGISTERS;
			Scratch = ffIncludeFile( FileID );
			LOAD_REGISTERS;
			if( Scratch ) M_THROW(Scratch)
			endcase;
#endif  /* !PF_NO_SHELL */
			
#ifndef PF_NO_SHELL
		case ID_INTERPRET:
			SAVE_REGISTERS;
			Scratch = ffInterpret();
			LOAD_REGISTERS;
			if( Scratch ) M_THROW(Scratch)
			endcase;
#endif  /* !PF_NO_SHELL */

		case ID_J:  /* ( -- j , second DO LOOP index ) */
			PUSH_TOS;
			TOS = M_R_PICK(3);
			endcase;

		case ID_KEY:
			PUSH_TOS;
			TOS = ioKey();
			endcase;
			
#ifndef PF_NO_SHELL
		case ID_LITERAL:
			ffLiteral( TOS );
			M_DROP;
			endcase;
#endif /* !PF_NO_SHELL */

		case ID_LITERAL_P:
			DBUG(("ID_LITERAL_P: InsPtr = 0x%x, *InsPtr = 0x%x\n", InsPtr, *InsPtr ));
			PUSH_TOS;
			TOS = READ_LONG_DIC(InsPtr++);
			endcase;
	
#ifndef PF_NO_SHELL
		case ID_LOCAL_COMPILER: DO_VAR(gLocalCompiler_XT); endcase;
#endif /* !PF_NO_SHELL */

		case ID_LOCAL_FETCH: /* ( i <local> -- n , fetch from local ) */
			TOS = *(LocalsPtr - TOS);
			endcase;

#define LOCAL_FETCH_N(num) \
		case ID_LOCAL_FETCH_##num: /* ( <local> -- n , fetch from local ) */ \
			PUSH_TOS; \
			TOS = *(LocalsPtr -(num)); \
			endcase;

		LOCAL_FETCH_N(1);
		LOCAL_FETCH_N(2);
		LOCAL_FETCH_N(3);
		LOCAL_FETCH_N(4);
		LOCAL_FETCH_N(5);
		LOCAL_FETCH_N(6);
		LOCAL_FETCH_N(7);
		LOCAL_FETCH_N(8);
		
		case ID_LOCAL_STORE:  /* ( n i <local> -- , store n in local ) */
			*(LocalsPtr - TOS) = M_POP;
			M_DROP;
			endcase;

#define LOCAL_STORE_N(num) \
		case ID_LOCAL_STORE_##num:  /* ( n <local> -- , store n in local ) */ \
			*(LocalsPtr - (num)) = TOS; \
			M_DROP; \
			endcase;

		LOCAL_STORE_N(1);
		LOCAL_STORE_N(2);
		LOCAL_STORE_N(3);
		LOCAL_STORE_N(4);
		LOCAL_STORE_N(5);
		LOCAL_STORE_N(6);
		LOCAL_STORE_N(7);
		LOCAL_STORE_N(8);
		
		case ID_LOCAL_PLUSSTORE:  /* ( n i <local> -- , add n to local ) */
			*(LocalsPtr - TOS) += M_POP;
			M_DROP;
			endcase;
			
		case ID_LOCAL_ENTRY: /* ( x0 x1 ... xn n -- ) */
		/* create local stack frame */
			{
				int32 i = TOS;
				cell *lp;
				DBUG(("LocalEntry: n = %d\n", TOS));
				/* End of locals. Create stack frame */
				DBUG(("LocalEntry: before RP@ = 0x%x, LP = 0x%x\n",
					TORPTR, LocalsPtr));
				M_R_PUSH(LocalsPtr);
				LocalsPtr = TORPTR;
				TORPTR -= TOS;
				DBUG(("LocalEntry: after RP@ = 0x%x, LP = 0x%x\n",
					TORPTR, LocalsPtr));
				lp = TORPTR;
				while(i-- > 0)
				{
					*lp++ = M_POP;    /* Load local vars from stack */
				}
				M_DROP;
			}
			endcase;

		case ID_LOCAL_EXIT: /* cleanup up local stack frame */
			DBUG(("LocalExit: before RP@ = 0x%x, LP = 0x%x\n",
				TORPTR, LocalsPtr));
			TORPTR = LocalsPtr;
			LocalsPtr = (cell *) M_R_POP;
			DBUG(("LocalExit: after RP@ = 0x%x, LP = 0x%x\n",
				TORPTR, LocalsPtr));
			endcase;
			
#ifndef PF_NO_SHELL
		case ID_LOADSYS:
			MSG("Load "); MSG(SYSTEM_LOAD_FILE); EMIT_CR;
			FileID = sdOpenFile(SYSTEM_LOAD_FILE, "r");
			if( FileID )
			{
				SAVE_REGISTERS;
				Scratch = ffIncludeFile( FileID );
				LOAD_REGISTERS;
				sdCloseFile( FileID );
				if( Scratch ) M_THROW(Scratch);
			}
			else
			{
				 ERR(SYSTEM_LOAD_FILE); ERR(" could not be opened!\n");
			}
			endcase;
#endif  /* !PF_NO_SHELL */

		case ID_LEAVE_P: /* ( R: index limit --  ) */
			M_R_DROP;
			M_R_DROP;
			M_BRANCH;
			endcase;

		case ID_LOOP_P: /* ( R: index limit -- | index limit ) */
			Temp = M_R_POP; /* limit */
			Scratch = M_R_POP + 1; /* index */
			if( Scratch == Temp )
			{
				InsPtr++;   /* skip branch offset, exit loop */
			}
			else
			{
/* Push index and limit back to R */
				M_R_PUSH( Scratch );
				M_R_PUSH( Temp );
/* Branch back to just after (DO) */
				M_BRANCH;
			}
			endcase;
				
		case ID_LSHIFT:     BINARY_OP( << ); endcase;
		
		case ID_MAX:
			Scratch = M_POP;
			TOS = ( TOS > Scratch ) ? TOS : Scratch ;
			endcase;
			
		case ID_MIN:
			Scratch = M_POP;
			TOS = ( TOS < Scratch ) ? TOS : Scratch ;
			endcase;
			
		case ID_MINUS:     BINARY_OP( - ); endcase;
			
#ifndef PF_NO_SHELL
		case ID_NAME_TO_TOKEN:
			TOS = (cell) NameToToken((ForthString *)TOS);
			endcase;
			
		case ID_NAME_TO_PREVIOUS:
			TOS = (cell) NameToPrevious((ForthString *)TOS);
			endcase;
#endif
			
		case ID_NOOP:
			endcase;
			
		case ID_OR:     BINARY_OP( | ); endcase;
		
		case ID_OVER:
			PUSH_TOS;
			TOS = M_STACK(1);
			endcase;
			
		case ID_PICK: /* ( ... n -- sp(n) ) */
			TOS = M_STACK(TOS);
			endcase;

		case ID_PLUS:     BINARY_OP( + ); endcase;
		
		case ID_PLUS_STORE:   /* ( n addr -- , add n to *addr ) */
#if (defined(PF_BIG_ENDIAN_DIC) || defined(PF_LITTLE_ENDIAN_DIC))
			if( IN_DICS( TOS ) )
			{
				Scratch = READ_LONG_DIC((cell *)TOS);
				Scratch += M_POP;
				WRITE_LONG_DIC((cell *)TOS,Scratch);
			}
			else
			{
				*((cell *)TOS) += M_POP;
			}
#else
			*((cell *)TOS) += M_POP;
#endif
			M_DROP;
			endcase;

		case ID_PLUSLOOP_P: /* ( delta -- ) ( R: index limit -- | index limit ) */
			{
				ucell OldIndex, NewIndex, Limit;

				Limit = M_R_POP;
				OldIndex = M_R_POP;
				NewIndex = OldIndex + TOS; /* add TOS to index, not 1 */
/* Do indices cross boundary between LIMIT-1 and LIMIT ? */
				if( ( (OldIndex - Limit) & ((Limit-1) - NewIndex) & 0x80000000 ) ||
				    ( (NewIndex - Limit) & ((Limit-1) - OldIndex) & 0x80000000 ) )
				{
					InsPtr++;   /* skip branch offset, exit loop */
				}
				else
				{
/* Push index and limit back to R */
					M_R_PUSH( NewIndex );
					M_R_PUSH( Limit );
/* Branch back to just after (DO) */
					M_BRANCH;
				}
				M_DROP;
			}
			endcase;

		case ID_QDO_P: /* (?DO) ( limit start -- ) ( R: -- start limit ) */
			Scratch = M_POP;  /* limit */
			if( Scratch == TOS )
			{
/* Branch to just after (LOOP) */
				M_BRANCH;
			}
			else
			{
				M_R_PUSH( TOS );
				M_R_PUSH( Scratch );
				InsPtr++;   /* skip branch offset, enter loop */
			}
			M_DROP;
			endcase;

		case ID_QDUP:     if( TOS ) M_DUP; endcase;

		case ID_QTERMINAL:  /* WARNING: Typically not fully implemented! */
			PUSH_TOS;
			TOS = sdQueryTerminal();
			endcase;
		
		case ID_QUIT_P: /* Stop inner interpreter, go back to user. */
#ifdef PF_SUPPORT_TRACE
			Level = 0;
#endif
			M_THROW(THROW_QUIT);
			endcase;
			
		case ID_R_DROP:
			M_R_DROP;
			endcase;

		case ID_R_FETCH:
			PUSH_TOS;
			TOS = (*(TORPTR));
			endcase;
		
		case ID_R_FROM:
			PUSH_TOS;
			TOS = M_R_POP;
			endcase;
			
		case ID_REFILL:
			PUSH_TOS;
			TOS = (ffRefill() > 0) ? FTRUE : FFALSE;
			endcase;
			
/* Resize memory allocated by ALLOCATE. */
		case ID_RESIZE:  /* ( addr1 u -- addr2 result ) */
			{
				cell *FreePtr;
				
				FreePtr = (cell *) ( M_POP - sizeof(cell) );
				if( ((uint32)*FreePtr) != ((uint32)FreePtr ^ PF_MEMORY_VALIDATOR))
				{
					M_PUSH( 0 );
					TOS = -3;
				}
				else
				{
					/* Try to allocate. */
					CellPtr = (cell *) pfAllocMem( TOS + sizeof(cell) );
					if( CellPtr )
					{
						/* Copy memory including validation. */
						pfCopyMemory( (char *) CellPtr, (char *) FreePtr, TOS + sizeof(cell) );
						*CellPtr = (cell)(((uint32)CellPtr) ^ (uint32)PF_MEMORY_VALIDATOR);
						CellPtr++;
			            M_PUSH( (cell) ++CellPtr );
						TOS = 0;
						FreePtr[0] = 0xDeadBeef;
						pfFreeMem((char *) FreePtr);
					}
					else
					{
						M_PUSH( 0 );
						TOS = -4;  /* FIXME Fix error code. */
					}
				}
			}
			endcase;
				
/*
** RP@ and RP! are called secondaries so we must
** account for the return address pushed before calling.
*/
		case ID_RP_FETCH:    /* ( -- rp , address of top of return stack ) */
			PUSH_TOS;
			TOS = (cell)TORPTR;  /* value before calling RP@ */
			endcase;
			
		case ID_RP_STORE:    /* ( rp -- , address of top of return stack ) */
			TORPTR = (cell *) TOS;
			M_DROP;
			endcase;
			
		case ID_ROLL: /* ( xu xu-1 xu-1 ... x0 u -- xu-1 xu-1 ... x0 xu ) */
			{
				int32 ri;
				cell *srcPtr, *dstPtr;
				Scratch = M_STACK(TOS);
				srcPtr = &M_STACK(TOS-1);
				dstPtr = &M_STACK(TOS);
				for( ri=0; ri<TOS; ri++ )
				{
					*dstPtr-- = *srcPtr--;
				}
				TOS = Scratch;
				STKPTR++;
			}
			endcase;

		case ID_ROT:  /* ( a b c -- b c a ) */
			Scratch = M_POP;    /* b */
			Temp = M_POP;       /* a */
			M_PUSH( Scratch );  /* b */
			PUSH_TOS;           /* c */
			TOS = Temp;         /* a */
			endcase;

/* Logical right shift */
		case ID_RSHIFT:     { TOS = ((uint32)M_POP) >> TOS; } endcase;   
		
#ifndef PF_NO_SHELL
		case ID_SAVE_FORTH_P:   /* ( $name Entry NameSize CodeSize -- err ) */
			{
				int32 NameSize, CodeSize, EntryPoint;
				CodeSize = TOS;
				NameSize = M_POP;
				EntryPoint = M_POP;
				ForthStringToC( gScratch, (char *) M_POP );
				TOS =  ffSaveForth( gScratch, EntryPoint, NameSize, CodeSize );
			}
			endcase;
#endif

/* Source     Stack
** EVALUATE    >IN  SourceID=(-1)  1111
** keyboard    >IN  SourceID=(0)   2222
** file        >IN  lineNumber filePos  SourceID=(fileID)
*/
		case ID_SAVE_INPUT:  /* FIXME - finish */
			{
			}
			endcase;

		case ID_SP_FETCH:    /* ( -- sp , address of top of stack, sorta ) */
			PUSH_TOS;
			TOS = (cell)STKPTR;
			endcase;
			
		case ID_SP_STORE:    /* ( sp -- , address of top of stack, sorta ) */
			STKPTR = (cell *) TOS;
			M_DROP;
			endcase;
			
		case ID_STORE: /* ( n addr -- , write n to addr ) */
#if (defined(PF_BIG_ENDIAN_DIC) || defined(PF_LITTLE_ENDIAN_DIC))
			if( IN_DICS( TOS ) )
			{
				WRITE_LONG_DIC((cell *)TOS,M_POP);
			}
			else
			{
				*((cell *)TOS) = M_POP;
			}
#else
			*((cell *)TOS) = M_POP;
#endif
			M_DROP;
			endcase;

		case ID_SCAN: /* ( addr cnt char -- addr' cnt' ) */
			Scratch = M_POP; /* cnt */
			Temp = M_POP;    /* addr */
			TOS = ffScan( (char *) Temp, Scratch, (char) TOS, &CharPtr );
			M_PUSH((cell) CharPtr);
			endcase;
			
#ifndef PF_NO_SHELL
		case ID_SEMICOLON:
			SAVE_REGISTERS;
			Scratch = ffSemiColon();
			LOAD_REGISTERS;
			if( Scratch ) M_THROW( Scratch );
			endcase;
#endif /* !PF_NO_SHELL */
			
		case ID_SKIP: /* ( addr cnt char -- addr' cnt' ) */
			Scratch = M_POP; /* cnt */
			Temp = M_POP;    /* addr */
			TOS = ffSkip( (char *) Temp, Scratch, (char) TOS, &CharPtr );
			M_PUSH((cell) CharPtr);
			endcase;

		case ID_SOURCE:  /* ( -- c-addr num ) */
			PUSH_TOS;
			M_PUSH( (cell) gCurrentTask->td_SourcePtr );
			TOS = (cell) gCurrentTask->td_SourceNum;
			endcase;
			
		case ID_SOURCE_SET: /* ( c-addr num -- ) */
			gCurrentTask->td_SourcePtr = (char *) M_POP;
			gCurrentTask->td_SourceNum = TOS;
			M_DROP;
			endcase;
			
		case ID_SOURCE_ID:
			PUSH_TOS;
			TOS = ffConvertStreamToSourceID( gCurrentTask->td_InputStream ) ;
			endcase;
			
		case ID_SOURCE_ID_POP:
			PUSH_TOS;
			TOS = ffConvertStreamToSourceID( ffPopInputStream() ) ;
			endcase;
			
		case ID_SOURCE_ID_PUSH:  /* ( source-id -- ) */
			TOS = (cell)ffConvertSourceIDToStream( TOS );
			Scratch = ffPushInputStream((FileStream *) TOS );
			if( Scratch )
			{
				M_THROW(Scratch);
			}
			else M_DROP;
			endcase;
			
		case ID_SWAP:
			Scratch = TOS;
			TOS = *STKPTR;
			*STKPTR = Scratch;
			endcase;
			
		case ID_TEST1:
			PUSH_TOS;
			M_PUSH( 0x11 );
			M_PUSH( 0x22 );
			TOS = 0x33;
			endcase;

		case ID_TEST2:
			endcase;

		case ID_THROW:  /* ( k*x err -- k*x | i*x err , jump to where CATCH was called ) */
			if(TOS)
			{
				M_THROW(TOS);
			}
			else M_DROP;
			endcase;

#ifndef PF_NO_SHELL
		case ID_TICK:
			PUSH_TOS;
			CharPtr = (char *) ffWord( (char) ' ' );
			TOS = ffFind( CharPtr, (ExecToken *) &Temp );
			if( TOS == 0 )
			{
				ERR("' could not find ");
				ioType( (char *) CharPtr+1, *CharPtr );
				M_THROW(-13);
			}
			else
			{
				TOS = Temp;
			}
			endcase;
#endif  /* !PF_NO_SHELL */
			
		case ID_TIMES: BINARY_OP( * ); endcase;
			
		case ID_TYPE:
			Scratch = M_POP; /* addr */
			ioType( (char *) Scratch, TOS );
			M_DROP;
			endcase;

		case ID_TO_R:
			M_R_PUSH( TOS );
			M_DROP;
			endcase;

		case ID_VAR_BASE: DO_VAR(gVarBase); endcase;
		case ID_VAR_CODE_BASE: DO_VAR(gCurrentDictionary->dic_CodeBase); endcase;
		case ID_VAR_CODE_LIMIT: DO_VAR(gCurrentDictionary->dic_CodeLimit); endcase;
		case ID_VAR_CONTEXT: DO_VAR(gVarContext); endcase;
		case ID_VAR_DP: DO_VAR(gCurrentDictionary->dic_CodePtr.Cell); endcase;
		case ID_VAR_ECHO: DO_VAR(gVarEcho); endcase;
		case ID_VAR_HEADERS_BASE: DO_VAR(gCurrentDictionary->dic_HeaderBase); endcase;
		case ID_VAR_HEADERS_LIMIT: DO_VAR(gCurrentDictionary->dic_HeaderLimit); endcase;
		case ID_VAR_HEADERS_PTR: DO_VAR(gCurrentDictionary->dic_HeaderPtr.Cell); endcase;
		case ID_VAR_NUM_TIB: DO_VAR(gCurrentTask->td_SourceNum); endcase;
		case ID_VAR_OUT: DO_VAR(gCurrentTask->td_OUT); endcase;
		case ID_VAR_STATE: DO_VAR(gVarState); endcase;
		case ID_VAR_TO_IN: DO_VAR(gCurrentTask->td_IN); endcase;
		case ID_VAR_TRACE_FLAGS: DO_VAR(gVarTraceFlags); endcase;
		case ID_VAR_TRACE_LEVEL: DO_VAR(gVarTraceLevel); endcase;
		case ID_VAR_TRACE_STACK: DO_VAR(gVarTraceStack); endcase;
		case ID_VAR_RETURN_CODE: DO_VAR(gVarReturnCode); endcase;

		case ID_WORD:
			TOS = (cell) ffWord( (char) TOS );
			endcase;

		case ID_WORD_FETCH: /* ( waddr -- w ) */
#if (defined(PF_BIG_ENDIAN_DIC) || defined(PF_LITTLE_ENDIAN_DIC))
			if( IN_DICS( TOS ) )
			{
				TOS = (uint16) READ_SHORT_DIC((uint16 *)TOS);
			}
			else
			{
				TOS = *((uint16 *)TOS);
			}
#else
			TOS = *((uint16 *)TOS);
#endif
			endcase;

		case ID_WORD_STORE: /* ( w waddr -- ) */
			
#if (defined(PF_BIG_ENDIAN_DIC) || defined(PF_LITTLE_ENDIAN_DIC))
			if( IN_DICS( TOS ) )
			{
				WRITE_SHORT_DIC((uint16 *)TOS,(uint16)M_POP);
			}
			else
			{
				*((uint16 *)TOS) = (uint16) M_POP;
			}
#else
			*((uint16 *)TOS) = (uint16) M_POP;
#endif
			M_DROP;
			endcase;

		case ID_XOR: BINARY_OP( ^ ); endcase;
				
				
/* Branch is followed by an offset relative to address of offset. */
		case ID_ZERO_BRANCH:
DBUGX(("Before 0Branch: IP = 0x%x\n", InsPtr ));
			if( TOS == 0 )
			{
				M_BRANCH;
			}
			else
			{
				InsPtr++;      /* skip over offset */
			}
			M_DROP;
DBUGX(("After 0Branch: IP = 0x%x\n", InsPtr ));
			endcase;
			
		default:
			ERR("pfCatch: Unrecognised token = 0x");
			ffDotHex(Token);
			ERR(" at 0x");
			ffDotHex((int32) InsPtr);
			EMIT_CR;
			InsPtr = 0;
			endcase;
		}
		
		if(InsPtr) Token = READ_LONG_DIC(InsPtr++);   /* Traverse to next token in secondary. */
		
#ifdef PF_DEBUG
		M_DOTS;
#endif

#if 0
		if( _CrtCheckMemory() == 0 )
		{
			ERR("_CrtCheckMemory abort: InsPtr = 0x");
			ffDotHex((int)InsPtr);
			ERR("\n");
		}
#endif

	} while( (InitialReturnStack - TORPTR) > 0 );

	SAVE_REGISTERS;
	
	return ExceptionReturnCode;
}
