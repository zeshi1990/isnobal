/* These functions taken from the f2c libf77 library.      10/15/90
 * C version and Fortran dependencies removed by Sean Dougherty and
 * Philip Thompson (phils@athena.mit.edu), 10/5/90.
 * Computer Resource Lab., Dept. Architecture and Urban Planning,
 * MIT, Cambridge MA  02139
 */

#include "ipw.h"

double d_sign(a, b)
double *a, *b;
{
    double x;

    x = (*a >= 0 ? *a : -*a);
    return (*b >= 0 ? x : -x);
}

double pow_dd(ap, bp)
double *ap, *bp;
{
    double pow();

    return (pow(*ap, *bp));
}

/* changed this to have IEEE_drem configured, September 1992, jacobsd */

#ifndef IEEE_drem
/* Assume we have IEEE_drem if they didn't tell us */
/*
 * Wrong.  Sept 1997: assume that if they didn't tell us, then we *don't*
 * have IEEE_drem.
 *
 * #define IEEE_drem 1
 */
#endif

double d_mod(x, y)
double *x, *y;
{
#if IEEE_drem
    double drem(), xa, ya, z;

    if ((ya = *y) < 0.0)
        ya = -ya;
    z = drem(xa = *x, ya);
    if (xa > 0) {
        if (z < 0)
            z += ya;
    } else if (z > 0)
        z -= ya;
    return z;
#else
    double floor(), quotient;

    if ((quotient = *x / *y) >= 0)
        quotient = floor(quotient);
    else
        quotient = -floor(-quotient);
    return (*x - (*y) * quotient);
#endif
}

/*** end xtra.c ***/
