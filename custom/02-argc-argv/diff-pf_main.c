*** old-pf_main.c	Wed Jan  1 21:13:11 2025
--- new-pf_main.c	Wed Jan  1 21:11:33 2025
***************
*** 58,63 ****
--- 58,64 ----
  }
  #else
  
+ void fillScriptParams( int startItem, int NoItems, char *argv[] );  /* forward declaration for cf_demo2.c */
  int main( int argc, char **argv )
  {
  #ifdef PF_STATIC_DIC
***************
*** 90,95 ****
--- 91,100 ----
              c = *s++;
              switch(c)
              {
+             case '-': /* call code in cf_demo2.c then skip remaining arguments */
+ 				fillScriptParams( i+1, argc, argv );
+                 i = argc; 
+                 break;
              case 'i':
                  IfInit = TRUE;
                  DicName = NULL;
