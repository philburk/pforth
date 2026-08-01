# run with wmake from OpenWatcomC

coreObjFiles   = PFCOMPIL.OBJ PF_CGLUE.OBJ PF_CLIB.OBJ PF_CORE.OBJ PF_INNER.OBJ PF_IO.OBJ PF_MAIN.OBJ PF_MEM.OBJ PF_SAVE.OBJ PF_TEXT.OBJ PF_WORDS.OBJ
ioObjFiles     =
customObjFiles = PFCUSTOM.OBJ 
allObjFiles    = $(coreObjFiles) $(ioObjFiles) $(customObjFiles) missing.obj

.c.obj
	wcc386 -fp3 -fpi87 -I. -DPF_SUPPORT_FP $<
	
pforth.dic: pforth.exe
	cd ..\fth
	..\csrc\pforth -i system.fth
	cd ..\csrc
	move ..\fth\pforth.dic .

pforth.exe: $(allObjFiles)
	%write pforth.lnk NAME   $@
	%write pforth.lnk SYSTEM DOS4G
	%write pforth.lnk FILE   {$(allObjFiles)}
	wlink  @pforth.lnk

clean:  .symbolic
	del *.dic *.exe *.lnk *.obj *.err

test: pforth.dic .symbolic
    copy pforth.dic ..\fth\
	cd ..\fth
	..\csrc\pforth -q t_corex.fth
	..\csrc\pforth -q tstrings.fth
	..\csrc\pforth -q t_locals.fth
	..\csrc\pforth -q t_alloc.fth
	..\csrc\pforth -q t_floats.fth
# this will not succeed:	..\csrc\pforth -q t_file.fth
	del pforth.dic
	cd ..\csrc\
	echo
	echo "all tests PASSED"
