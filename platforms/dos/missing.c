 #include "missing.h"
 
double round(double x)  {
  double i, s;
  if( x>=0 ) {
    i = floor(x);
    s = 1.0;
  } else {
    i = ceil(x);
    s = -1.0;
  }
  if( fabs(x - i) < 0.5 )
    return i;      /* round down */
  else
    return i + s;  /* round up   */
}

double log1p(double x)  {
  if( -1.0 < x  &&  x <= 1.0 )
    return x - 0.5*x*x + 1.0/3.0*x*x*x - 0.25*x*x*x*x + 0.20*x*x*x*x*x;
  else
    return log( 1.0 + x );
}
