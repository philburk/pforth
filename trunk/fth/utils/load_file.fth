\ Load a file into an allocated memory image.
\
\ Author: Phil Burk
\ Copyright 3DO 1995

anew task-load_file.fth

: $LOAD.FILE { $filename | fid numbytes numread err data -- data-addr 0 | 0 err }
	0 -> data
\ open file
	$filename count r/o open-file -> err -> fid
	err
	IF
		." $LOAD.FILE - Could not open input file!" cr
	ELSE
\ determine size of file
		fid file-size -> err -> numbytes
		err
		IF
			 ." $LOAD.FILE - File size failed!" cr
		ELSE
			." File size = " numbytes . cr
\ allocate memory for sample, when done free memory using FREE
			numbytes allocate -> err -> data
			err
			IF
				." $LOAD.FILE - Memory allocation failed!" cr
			ELSE
\ read data
				data numbytes fid read-file -> err
				." Read " . ." bytes from file " $filename count type cr
			THEN
		THEN
		fid close-file drop
	THEN
	data err
;

\ Example:   c" myfile" $load.file   abort" Oops!"   free .
