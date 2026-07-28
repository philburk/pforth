/* @(#) pf_core.c 98/01/28 1.5 */
/***************************************************************
** Forth based on 'C'
**
** This file has the main entry points to the pForth library.
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
****************************************************************
** 940502 PLB Creation.
** 940505 PLB More macros.
** 940509 PLB Moved all stack handling into inner interpreter.
**        Added Create, Colon, Semicolon, HNumberQ, etc.
** 940510 PLB Got inner interpreter working with secondaries.
**        Added (LITERAL).   Compiles colon definitions.
** 940511 PLB Added conditionals, LITERAL, CREATE DOES>
** 940512 PLB Added DO LOOP DEFER, fixed R>
** 940520 PLB Added INCLUDE
** 940521 PLB Added NUMBER?
** 940930 PLB Outer Interpreter now uses deferred NUMBER?
** 941005 PLB Added ANSI locals, LEAVE, modularised
** 950320 RDG Added underflow checking for FP stack
** 970702 PLB Added STACK_SAFETY to FP stack size.
***************************************************************/

#include "pf_all.h"
#include "paging/unittest.h"
#include "paging/qadmpage.h"

/***************************************************************
** Global Data
***************************************************************/

char            gScratch[TIB_SIZE];
pfTaskData_t   *gCurrentTask = NULL;
pfDictionary_t *gCurrentDictionary;
cell_t          gNumPrimitives;

ExecToken       gLocalCompiler_XT;   /* custom compiler for local variables */
ExecToken       gNumberQ_XT;         /* XT of NUMBER? */
ExecToken       gQuitP_XT;           /* XT of (QUIT) */
ExecToken       gAcceptP_XT;         /* XT of ACCEPT */
cell_t          gPfAssertEnabled = PF_ASSERT_ENABLED;

/* Depth of data stack when colon called. */
cell_t          gDepthAtColon;

/* Global Forth variables.
* These must be initialized in pfInit below.
*/
cell_t          gVarContext;      /* Points to last name field. */
cell_t          gVarState;        /* 1 if compiling. */
cell_t          gVarBase;         /* Numeric Base. */
cell_t          gVarByeCode;      /* Echo input. */
cell_t          gVarEcho;         /* Echo input. */
cell_t          gVarTraceLevel;   /* Trace Level for Inner Interpreter. */
cell_t          gVarTraceStack;   /* Dump Stack each time if true. */
cell_t          gVarTraceFlags;   /* Enable various internal debug messages. */
cell_t          gVarQuiet;        /* Suppress unnecessary messages, OK, etc. */
cell_t          gVarReturnCode;   /* Returned to caller of Forth, eg. UNIX shell. */

/* data for INCLUDE that allows multiple nested files. */
IncludeFrame    gIncludeStack[MAX_INCLUDE_DEPTH];
cell_t          gIncludeIndex;

PFQA_INSTANTIATE_GLOBALS;

static void pfResetForthTask( void );
static void pfInitSystem( void );
static void pfTermSystem( void );

#define DEFAULT_RETURN_DEPTH (512)
#define DEFAULT_USER_DEPTH (512)

#ifndef PF_DEFAULT_HEADER_SIZE
#define PF_DEFAULT_HEADER_SIZE (120000)
#endif

#ifndef PF_DEFAULT_CODE_SIZE
#define PF_DEFAULT_CODE_SIZE (300000)
#endif

/* Initialize globals in a function to simplify loading on
 * embedded systems which may not support initialization of data section.
 */
static void pfInitSystem( void )
{
/* all zero */
    gCurrentTask = NULL;
    gCurrentDictionary = NULL;
    gNumPrimitives = 0;
    gLocalCompiler_XT = 0;
    gVarContext = PF_VM_NULL;   /* Points to last name field. */
    gVarState = 0;        /* 1 if compiling. */
    gVarByeCode = 0;      /* BYE-CODE */
    gVarEcho = 0;         /* Echo input. */
    gVarTraceLevel = 0;   /* Trace Level for Inner Interpreter. */
    gVarTraceFlags = 0;   /* Enable various internal debug messages. */
    gVarReturnCode = 0;   /* Returned to caller of Forth, eg. UNIX shell. */
    gIncludeIndex = 0;

/* non-zero */
    gVarBase = 10;        /* Numeric Base. */
    gDepthAtColon = DEPTH_AT_COLON_INVALID;
    gVarTraceStack = 1;

    pfInitMemoryAllocator();
    pfResetLockedMemory();
    ioInit();

}

static void pfTermSystem( void )
{
    ioTerm();
}

/***************************************************************
** Task Management
***************************************************************/

void pfDeleteTask( PForthTask task )
{
    pfTaskData_t *cftd = (pfTaskData_t *)task;
    if (cftd == NULL) return;
#ifdef PF_SUPPORT_FP
    FREE_VAR( cftd->td_FloatStackLimit );
#endif
    FREE_VAR( cftd->td_ReturnLimit );
    FREE_VAR( cftd->td_StackLimit );
    pfFreeMem( cftd );
}

/* Allocate some extra cells to protect against mild stack underflows. */
#define STACK_SAFETY  (8)
PForthTask pfCreateTask( cell_t UserStackDepth, cell_t ReturnStackDepth )
{
    pfTaskData_t *cftd;

    cftd = ( pfTaskData_t * ) pfAllocMem( sizeof( pfTaskData_t ) );
    if( !cftd ) goto nomem;
    pfSetMemory( cftd, 0, sizeof( pfTaskData_t ));

/* Allocate User Stack */
    cftd->td_StackLimit = (cell_t *) pfAllocMem((ucell_t)(sizeof(cell_t) *
                (UserStackDepth + STACK_SAFETY)));
    if( !cftd->td_StackLimit ) goto nomem;
    cftd->td_StackBase = cftd->td_StackLimit + UserStackDepth;
    cftd->td_StackPtr = cftd->td_StackBase;

/* Allocate Return Stack */
    cftd->td_ReturnLimit = (cell_t *) pfAllocMem((ucell_t)(sizeof(cell_t) * ReturnStackDepth) );
    if( !cftd->td_ReturnLimit ) goto nomem;
    cftd->td_ReturnBase = cftd->td_ReturnLimit + ReturnStackDepth;
    cftd->td_ReturnPtr = cftd->td_ReturnBase;

/* Allocate Float Stack */
#ifdef PF_SUPPORT_FP
/* Allocate room for as many Floats as we do regular data. */
    cftd->td_FloatStackLimit = (PF_FLOAT *) pfAllocMem((ucell_t)(sizeof(PF_FLOAT) *
                (UserStackDepth + STACK_SAFETY)));
    if( !cftd->td_FloatStackLimit ) goto nomem;
    cftd->td_FloatStackBase = cftd->td_FloatStackLimit + UserStackDepth;
    cftd->td_FloatStackPtr = cftd->td_FloatStackBase;
#endif

    cftd->td_InputStream = PF_STDIN;

    cftd->td_SourcePtr = PTR_TO_VMA(&cftd->td_TIB[0]);
    cftd->td_SourceNum = 0;

    return (PForthTask) cftd;

nomem:
    ERR("CreateTaskContext: insufficient memory.\n");
    if( cftd ) pfDeleteTask( (PForthTask) cftd );
    return NULL;
}

/***************************************************************
** Dictionary Management
***************************************************************/

ThrowCode pfExecIfDefined( const char *CString )
{
    ThrowCode result = 0;
    if( NAME_BASE != PF_VM_NULL)
    {
        ExecToken  XT;
        if( ffFindC( CString, &XT ) )
        {
            result = pfCatch( XT );
        }
    }
    return result;
}

/***************************************************************
** Delete a dictionary created by pfCreateDictionary()
*/
void pfDeleteDictionary( PForthDictionary dictionary )
{
    pfDictionary_t *dic = (pfDictionary_t *) dictionary;
    if( !dic ) return;

    if( dic->dic_Flags & PF_DICF_ALLOCATED_SEGMENTS )
    {
        FREE_VM_VAR( dic->dic_HeaderBaseUnaligned );
        FREE_VM_VAR( dic->dic_CodeBaseUnaligned );
    }
    pfFreeMem( dic );
}

/***************************************************************
** Create a complete dictionary.
** The dictionary consists of two parts, the header with the names,
** and the code portion.
** Delete using pfDeleteDictionary().
** Return pointer to dictionary management structure.
*/
PForthDictionary pfCreateDictionary( cell_t HeaderSize, cell_t CodeSize )
{
/* Allocate memory for initial dictionary. */
    pfDictionary_t *dic;

    dic = ( pfDictionary_t * ) pfAllocMem( sizeof( pfDictionary_t ) );
    if( !dic ) goto nomem;
    pfSetMemory( dic, 0, sizeof( pfDictionary_t ));

    dic->dic_Flags |= PF_DICF_ALLOCATED_SEGMENTS;

/* Align dictionary segments to preserve alignment of floats across hosts.
 * Thank you Helmut Proelss for pointing out that this needs to be cast
 * to (ucell_t) on 16 bit systems.
 */
#define DIC_ALIGNMENT_SIZE  ((ucell_t)(0x10))
#define DIC_ALIGN(addr)  ((((ucell_t)(addr)) + DIC_ALIGNMENT_SIZE - 1) & ~(DIC_ALIGNMENT_SIZE - 1))

/* Allocate memory for header. */
    if( HeaderSize > 0 )
    {

#if PF_DEMAND_PAGING
        dic->dic_HeaderBaseUnaligned = pfAllocatePagedMemory( (ucell_t) HeaderSize + DIC_ALIGNMENT_SIZE);
#else

        dic->dic_HeaderBaseUnaligned = PTR_TO_VMA(pfAllocMem((ucell_t) HeaderSize + DIC_ALIGNMENT_SIZE ));
#endif
        if( !dic->dic_HeaderBaseUnaligned ) goto nomem;
/* Align header base. */
        dic->dic_HeaderBase = DIC_ALIGN(dic->dic_HeaderBaseUnaligned);
        pfSetVirtualMemory((vm_address_t) dic->dic_HeaderBase, 0xA5, (uint32_t) HeaderSize);
        dic->dic_HeaderLimit = dic->dic_HeaderBase + HeaderSize;
        dic->dic_HeaderPtr = dic->dic_HeaderBase;
    }
    else
    {
        dic->dic_HeaderBase = 0;
    }

/* Allocate memory for code. */
#if PF_DEMAND_PAGING
    dic->dic_CodeBaseUnaligned = pfAllocatePagedMemory( (ucell_t) CodeSize + DIC_ALIGNMENT_SIZE );
#else
    dic->dic_CodeBaseUnaligned = PTR_TO_VMA(pfAllocMem( (ucell_t) CodeSize + DIC_ALIGNMENT_SIZE ));
#endif
    if( !dic->dic_CodeBaseUnaligned ) goto nomem;
    dic->dic_CodeBase = DIC_ALIGN(dic->dic_CodeBaseUnaligned);
    pfSetVirtualMemory((vm_address_t) dic->dic_CodeBase, 0x5A, (uint32_t) CodeSize);

    dic->dic_CodeLimit = dic->dic_CodeBase + CodeSize;
    dic->dic_CodePtr = (vm_address_t) (dic->dic_CodeBase + (QUADUP(NUM_PRIMITIVES) * PF_CELL_SIZE));

    return (PForthDictionary) dic;
nomem:
    pfDeleteDictionary( dic );
    return NULL;
}

/***************************************************************
** Used by Quit and other routines to restore system.
***************************************************************/

static void pfResetForthTask( void )
{
/* Go back to terminal input. */
    gCurrentTask->td_InputStream = PF_STDIN;

/* Reset stacks. */
    gCurrentTask->td_StackPtr = gCurrentTask->td_StackBase;
    gCurrentTask->td_ReturnPtr = gCurrentTask->td_ReturnBase;
#ifdef PF_SUPPORT_FP  /* Reset Floating Point stack too! */
    gCurrentTask->td_FloatStackPtr = gCurrentTask->td_FloatStackBase;
#endif

/* Advance >IN to end of input. */
    gCurrentTask->td_IN = gCurrentTask->td_SourceNum;
    gVarState = 0;
}

/***************************************************************
** Set current task context.
***************************************************************/

void pfSetCurrentTask( PForthTask task )
{
    gCurrentTask = (pfTaskData_t *) task;
}

PForthTask pfGetCurrentTask(void)
{
    return (PForthTask) gCurrentTask;
}

/***************************************************************
** Set Quiet Flag.
***************************************************************/

void pfSetQuiet( cell_t IfQuiet )
{
    gVarQuiet = (cell_t) IfQuiet;
}

/***************************************************************
** Query message status.
***************************************************************/

cell_t  pfQueryQuiet( void )
{
    return gVarQuiet;
}

/***************************************************************
** Top level interpreter.
***************************************************************/
ThrowCode pfQuit( void )
{
    ThrowCode exception;
    int go = 1;

    while(go)
    {
        exception = ffOuterInterpreterLoop();
        if( exception == 0 )
        {
            exception = ffOK();
        }

        switch( exception )
        {
        case 0:
            break;

        case THROW_BYE:
            go = 0;
            break;

        case THROW_ABORT:
        default:
            ffDotS();
            pfReportThrow( exception );
            pfHandleIncludeError();
            pfResetForthTask();
            break;
        }
    }

    return gVarReturnCode;
}

/***************************************************************
** Include file based on 'C' name.
***************************************************************/

cell_t pfIncludeFile( const char *FileName )
{
    FileStream *fid;
    cell_t Result;
    char  buffer[32];
    cell_t numChars, len;

/* Open file. */
    fid = sdOpenFile( FileName, PF_FAM_OPEN_RO );
    if( fid == NULL )
    {
        ERR("pfIncludeFile could not open ");
        ERR(FileName);
        EMIT_CR;
        return -1;
    }

/* Create a dictionary word named ::::FileName for FILE? */
    pfCopyMemory( &buffer[0], "::::", 4);
    len = (cell_t) pfCStringLength(FileName);
    numChars = ( len > (32-4-1) ) ? (32-4-1) : len;
    pfCopyMemory( &buffer[4], &FileName[len-numChars], numChars+1 );
    CreateDicEntryC( ID_NOOP, buffer, 0 );

    Result = ffIncludeFile( fid ); /* Also close the file. */

/* Create a dictionary word named ;;;; for FILE? */
    CreateDicEntryC( ID_NOOP, ";;;;", 0 );

    return Result;
}

/***************************************************************
** Output 'C' string message.
** Use sdTerminalOut which works before initializing gCurrentTask.
***************************************************************/
void pfDebugMessage( const char *CString )
{
#ifdef PF_DEBUG
    while( *CString )
    {
        char c = *CString++;
        if( c == '\n' )
        {
            sdTerminalOut( 0x0D );
            sdTerminalOut( 0x0A );
            pfDebugMessage( "DBG: " );
        }
        else
        {
            sdTerminalOut( c );
        }
    }
#else /* PF_DEBUG */
    (void)CString;
#endif /* PF_DEBUG */
}

/***************************************************************
** Print a decimal number to debug output.
*/
void pfDebugPrintDecimalNumber( cell_t n )
{
    pfDebugMessage( ConvertNumberToText( n, 10, TRUE, 1 ) );
}

/***************************************************************
** Output 'C' string message.
** This is provided to help avoid the use of printf() and other I/O
** which may not be present on a small embedded system.
** Uses ioType & ioEmit so requires that gCurrentTask has been initialized.
***************************************************************/
void pfMessage( const char *CString )
{
    ioType( CString, (cell_t) pfCStringLength(CString) );
}

/**
 * Interprets the Forth in the text.
 * The text length cannot exceed TIB_SIZE.
 * @param text the Forth code to be executed
 * @return 0 if successful or throw code.
 */
ThrowCode pfInterpretText(char *text) {
    ThrowCode Result = 0;
    cell_t length = strlen(text);
    if (length > TIB_SIZE) {
        MSG("pfInterpretText: text bigger than TIB\n");
        Result = THROW_ABORT;
        goto error;
    }
    gCurrentTask->td_IN = 0;
    gCurrentTask->td_SourceNum = length;
    vm_address_t savedSource = gCurrentTask->td_SourcePtr;
    pfCopyMemory(gCurrentTask->td_TIB, text, length);
    gCurrentTask->td_SourcePtr = savedSource;
    Result = ffInterpret();
error:
    return Result;
}

void pfPushToStack(cell_t value) {
    PUSH_DATA_STACK(value);
}

cell_t pfPopFromStack(void) {
    return POP_DATA_STACK;
}

cell_t pfGetStackDepth(void) {
    return DATA_STACK_DEPTH;
}

#ifdef PF_UNIT_TEST
static int pfQaInterpret(void) {
    printf("pfQaInterpret() called\n");
    int savedNumFailed = pfQaNumFailed;

    ASSERT_EQ(0, pfInterpretText("0sp 123"));
    ASSERT_EQ(1, pfGetStackDepth());
    ASSERT_EQ(123, pfPopFromStack());
    ASSERT_EQ(0, pfGetStackDepth());

    pfPushToStack(5000);
    ASSERT_EQ(0, pfInterpretText("678 +"));
    ASSERT_EQ(5678, pfPopFromStack());

    ASSERT_EQ(THROW_UNDEFINED_WORD, pfInterpretText("567 BADWORD 5432"));

    ASSERT_EQ(0, pfInterpretText("0sp"));
    ASSERT_EQ(0, pfGetStackDepth());

    printf("pfQaInterpret() ended\n");

error:
    PFQA_PRINT_RESULT;
    return pfQaNumFailed - savedNumFailed;
}
#endif

ThrowCode pfInitialize(const char *DicFileName,
                       cell_t IfInit,
                       ExecToken  *EntryPointPtr) {
    
    pfTaskData_t *cftd = NULL;
    pfDictionary_t *dic = NULL;
    ThrowCode Result = 0;
    
#ifdef PF_USER_INIT
    Result = PF_USER_INIT;
    if( Result < 0 ) goto error1;
#endif
    
    pfInitSystem();
    
    /* Allocate Task structure. */
    pfDebugMessage("pfDoForth: call pfCreateTask()\n");
    cftd = pfCreateTask( DEFAULT_USER_DEPTH, DEFAULT_RETURN_DEPTH );
    
    if( cftd )
    {
        pfSetCurrentTask( cftd );
        
        if( !gVarQuiet )
        {
            MSG( "PForth V"PFORTH_VERSION_NAME", " );
            
            if( IsHostLittleEndian() ) MSG("LE");
            else MSG("BE");
#if PF_BIG_ENDIAN_DIC
            MSG("/BE");
#elif PF_LITTLE_ENDIAN_DIC
            MSG("/LE");
#endif
            
#if (PF_SIZEOF_CELL == 8)
            MSG("/64");
#elif (PF_SIZEOF_CELL  == 4)
            MSG("/32");
#endif
            
            MSG( ", built "__DATE__" "__TIME__ );
        }

#ifdef PF_NO_GLOBAL_INIT
        if( LoadCustomFunctionTable() < 0 ) goto error2; /* Init custom 'C' call array. */
#endif
        
#if (!defined(PF_NO_INIT)) && (!defined(PF_NO_SHELL))
        if( IfInit )
        {
            pfDebugMessage("Build dictionary from scratch.\n");
            dic = pfBuildDictionary( PF_DEFAULT_HEADER_SIZE, PF_DEFAULT_CODE_SIZE );
        }
        else
#else
            TOUCH(IfInit);
#endif /* !PF_NO_INIT && !PF_NO_SHELL*/
        {
            if( DicFileName )
            {
                pfDebugMessage("DicFileName = "); pfDebugMessage(DicFileName); pfDebugMessage("\n");
                if( !gVarQuiet )
                {
                    EMIT_CR;
                }
                dic = pfLoadDictionary( DicFileName, EntryPointPtr );
            }
            else
            {
                if( !gVarQuiet )
                {
                    MSG(" (static)");
                    EMIT_CR;
                }
                dic = pfLoadStaticDictionary();
            }
        }
        if( dic == NULL ) goto error2;
        
        if( !gVarQuiet )
        {
            EMIT_CR;
        }
        
        pfDebugMessage("pfDoForth: try AUTO.INIT\n");
        Result = pfExecIfDefined("AUTO.INIT");
        if( Result != 0 )
        {
            MSG("Error in AUTO.INIT");
            goto error2;
        }
    }
    return 0;
    
error2:
    MSG("pfDoForth: Error occurred.\n");
    pfDeleteTask( cftd );
    /* Terminate so we restore normal shell tty mode. */
    pfTermSystem();

#ifdef PF_USER_INIT
error1:
#endif
    return -1;
}


void pfTerminate(void) {
    /* Clean up after running Forth. */
    if (gCurrentDictionary != NULL) {
        pfExecIfDefined("AUTO.TERM");
        pfDeleteDictionary( gCurrentDictionary );
        gCurrentDictionary = NULL;
    }
    if (pfGetCurrentTask() != NULL) {
        pfDeleteTask( pfGetCurrentTask() );
        pfSetCurrentTask(NULL);
    }

    pfTermSystem();

#if PF_DEMAND_PAGING
    pfCheckPagedMemory();
#endif

#ifdef PF_USER_TERM
    PF_USER_TERM;
#endif
}

/**************************************************************************
** Main entry point for pForth.
*/
ThrowCode pfDoForth(const char *DicFileName,
                    const char *SourceName,
                    cell_t IfInit )
{
    ExecToken  EntryPoint = 0;
    
    ThrowCode Result = pfInitialize(DicFileName, IfInit, &EntryPoint);
    
    if (Result == 0) {
#ifdef PF_UNIT_TEST
        if ((Result = pfQaInterpret()) != 0) goto error;
#endif

        if( EntryPoint != 0 )
        {
            /* Run a TURNKEY application. */
            Result = pfCatch( EntryPoint );
        }
#ifndef PF_NO_SHELL
        else
        {
            if( SourceName == NULL )
            {
                pfDebugMessage("pfDoForth: pfQuit\n");
                Result = pfQuit();
            }
            else
            {
                if( !gVarQuiet )
                {
                    MSG("Including: ");
                    MSG(SourceName);
                    MSG("\n");
                }
                Result = pfIncludeFile( SourceName );
            }
        }
#endif /* PF_NO_SHELL */
    }

#ifdef PF_UNIT_TEST
error:
#endif

    pfTerminate();

    return Result ? Result : gVarByeCode;
}

#ifdef PF_UNIT_TEST
cell_t pfUnitTest( void )
{
    cell_t numErrors = 0;
    MSG("pfUnitTest() called\n");
    numErrors += pfUnitTestText();

#if PF_DEMAND_PAGING
    numErrors += pfQaDemandPaging();
#endif

    numErrors += pfQaFileIO();
    numErrors += pfQaCompile();

    return numErrors;
}
#endif
