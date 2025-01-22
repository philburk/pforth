# call via:
#   find csrc/ fth/ -type f | awk -f too-long-filenames.awk


BEGIN {
  FS="/"  # separate by directory 
}


{
  noParts = split( $NF, parts, "." )
  if ( noParts!=2 )
  {
    if ( noParts>2 )
      print "----> invalid noParts=" noParts " for file='" $0 "'"
  }
  else
  {
    if( length(parts[1]) > 8  ||  length(parts[2]) > 3 ) 
      print $0;
  }
}
