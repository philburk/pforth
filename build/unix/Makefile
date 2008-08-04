# makefile for pForth
# Portable Forth written in 'C'
# by Phil Burk
# For more info visit http://www.softsynth.com/pforth/

.SUFFIXES: .c .o

# Options include: PF_SUPPORT_FP PF_NO_MALLOC PF_NO_INIT PF_DEBUG
# See "docs/pf_ref.htm" file for more info.

# Note for Mac OS X, use cc instead of gcc
COMPILER = gcc

PFORTHDIR   := $(shell cd ../../; pwd)
CSRCDIR      = $(PFORTHDIR)/csrc
FTHDIR       = $(PFORTHDIR)/fth

PFDICAPP     = $(FTHDIR)/pforth
PFORTHDIC    = $(FTHDIR)/pforth.dic
PFDICDAT     = ${CSRCDIR}/pfdicdat.h
PFORTHAPP    = $(PFORTHDIR)/pforth_standalone
OBJECTDIR    = $(PFORTHDIR)/objects
TEMPOBJECTDIR = $(PFORTHDIR)/tempobjects

FULL_WARNINGS =  \
        -fsigned-char \
        -fno-builtin \
        -fno-unroll-loops \
        -fpeephole \
        -fno-keep-inline-functions \
        -Wcast-qual \
        -Wall \
        -Wwrite-strings \
        -Winline  \
        -Wmissing-prototypes \
        -Wmissing-declarations
        
CCOPTS = -DPF_SUPPORT_FP -DPF_POSIX_IO -O2 $(FULL_WARNINGS) $(EXTRA_CCOPTS)

EMBCCOPTS = -DPF_STATIC_DIC

#######################################
# Build file lists from wildcards.
PFITEMP    = ${wildcard ${CSRCDIR}/*.h}
PFINCLUDES = ${PFITEMP:${CSRCDIR}/pfdicdat.h=}
PFSOURCE   = ${wildcard ${CSRCDIR}/*.c} ${CSRCDIR}/posix/pf_io_posix.c
PFTEMP     = ${PFSOURCE:%.c=%.o}
PFOBJS     = ${PFTEMP:${CSRCDIR}/%=${TEMPOBJECTDIR}/%}
PFEMBOBJS  = ${PFTEMP:${CSRCDIR}/%=${OBJECTDIR}/%}

COMPILE = $(COMPILER) $(CCOPTS) $(CDEFS)
	
${TEMPOBJECTDIR}/%.o:  ${TEMPOBJECTDIR}/posix $(PFINCLUDES)  ${CSRCDIR}/%.c
	$(COMPILE) -O -o ${TEMPOBJECTDIR}/$*.o -c ${CSRCDIR}/$*.c
	
${OBJECTDIR}/%.o:  ${OBJECTDIR}/posix $(PFINCLUDES) ${CSRCDIR}/%.c ${CSRCDIR}/pfdicdat.h 
	$(COMPILE) -O -o ${OBJECTDIR}/$*.o -c ${CSRCDIR}/$*.c $(EMBCCOPTS)

all: $(PFORTHAPP)

pffiles:
	@echo "ITEMP FILES -----------------"
	@echo ${PFITEMP}
	@echo "INCLUDE FILES -----------------"
	@echo ${PFINCLUDES}
	@echo "'C' FILES ---------------------"
	@echo ${PFSOURCE}
	@echo "OBJECT FILES ------------------"
	@echo ${PFOBJS}
	@echo "EMBEDDED OBJECT FILES ------------------"
	@echo ${PFEMBOBJS}
	
${TEMPOBJECTDIR}/posix: 
	mkdir -p ${TEMPOBJECTDIR}/posix

${OBJECTDIR}/posix: 
	mkdir -p ${OBJECTDIR}/posix

# build pforth by compiling 'C' source
$(PFDICAPP): $(PFINCLUDES) $(PFOBJS)
	$(COMPILER) $(PFOBJS) -lm -o $(PFDICAPP)

pfdicapp: $(PFDICAPP)

# build basic dictionary by running newly built pforth and including system.fth
$(PFDICDAT): $(PFDICAPP)
	cd $(FTHDIR); $(PFDICAPP) -i system.fth ; mv pfdicdat.h $(PFDICDAT)

$(PFORTHAPP): ${TEMPOBJECTDIR}/posix $(PFDICDAT) $(PFINCLUDES) $(PFEMBOBJS)
	$(COMPILER) $(PFEMBOBJS) -lm -o $(PFORTHAPP)
	@echo ""
	@echo "Standalone pForth executable written to $(PFORTHAPP)"
	
clean:
	-rm -f $(PFOBJS) $(PFEMBOBJS)
	-rm $(PFORTHAPP)
	-rm $(PFDICDAT)
	-rm $(PFORTHDIC)
	-rm $(PFDICAPP)
