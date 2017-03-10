/*
 * Copyright (c) 1990 The Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that: (1) source distributions retain this entire copyright
 * notice and comment, and (2) distributions including binaries display
 * the following acknowledgement:  ``This product includes software
 * developed by the Computer Systems Laboratory, University of
 * California, Santa Barbara and its contributors'' in the documentation
 * or other materials provided with the distribution and in all
 * advertising materials mentioning features or use of this software.
 *
 * Neither the name of the University nor the names of its contributors
 * may be used to endorse or promote products derived from this software
 * without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

/* LINTLIBRARY */

#include <string.h>

#include "ipw.h"

/*
** NAME
**	opt_check -- check for conflicting command-line options
**
** SYNOPSIS
**	#include "ipw.h"
**
**	void opt_check(int n_min, int n_max, int n_opts, OPTION_T *opt, ...)
**
** DESCRIPTION
**	opt_check checks that at least n_min and at most n_max of the
**	following n_opts option descriptors point to options that were
**	specified on the command line.  If the check fails, an error message
**	is printed and execution terminates.
**
** RESTRICTIONS
**
** GLOBALS ACCESSED
**
** ERRORS
**	must specify at least {n_min} of: {options}
**	may specify no more than {n_max} of: {options}
**
** WARNINGS
**	This function causes program termination if an error occurs.
**
**	This function accepts a variable number of arguments; thus, its actual
**	calling sequence cannot be verified by lint.
**
** APPLICATION USAGE
**	opt_check is typically called immediately after ipwenter to ensure
**	that conflicting options were not specified.  For example:
**
**		opt_check(1, 2, 2, &opt_c, &opt_i);
**
**	will check that at least 1 of the options described by opt_c and opt_i
**	was specified.
**
** FUTURE DIRECTIONS
**
** HISTORY
**	7/1/90	Written by James Frew, UCSB.
**	4/27/93	Modified for ANSI C.  Dana Jacobsen, ERL-C.
**	Apr 97	Updated for IPW version 2.0  J. Domingo, OSU
**
** BUGS
*/

#ifdef STANDARD_C

#include <stdarg.h>

void
opt_check(int n_min, int n_max, int n_opts, OPTION_T *opt, ...)
{
  va_list      ap;
  int          n_got;
  char       * opts;
  OPTION_T   * p;

  assert(n_min >= 0);
  assert(n_max >= n_min);
  assert(n_opts > 1);

     /*
      * allocate space for error message string.  3 chars per option, for "-x,"
      * or "-x\0"
      */
  opts = ecalloc(n_opts, 3);
  if (opts == NULL) {
    error("can't allocate opt_check error message string");
  }

  n_got = 0;
  va_start(ap, opt);

  for (  p = opt  ;  --n_opts >= 0  ;  p = va_arg(ap, OPTION_T *) ) {

    assert(p != NULL);
     /*
      * count # of set options
      */
    if (p->nargs > 0) {
      ++n_got;
    }
     /*
      * accumulate option letters in error message string
      */
    if (opts[0] != 0) {
      (void) strcat(opts, ",");
    }

    (void) strcat(opts, "-");
    (void) strncat(opts, &p->option, 1);
  }

  va_end(ap);
     /*
      * check # options against bounds
      */
  if (n_got < n_min) {
    error("must specify at least %d of: %s", n_min, opts);
  }

  if (n_got > n_max) {
    error("may specify no more than %d of: %s", n_max, opts);
  }
     /*
      * OK: discard error message string
      */
  SAFE_FREE(opts);
}


#else	/* non standard C */


/* VARARGS */
void
opt_check(va_alist)
#ifdef	lint
	int             va_alist;

#else
                va_dcl			/* varargs arg list		 */
#endif
{
	va_list         ap;		/* -> current arg		 */
	int             n_got;		/* # options set		 */
	int             n_max;		/* max # options that may be set */
	int             n_min;		/* min # options that may be set */
	int             n_opts;		/* # option descriptors supplied */
	char           *opts;		/* option names			 */

 /* NOSTRICT */
	va_start(ap);

 /* NOSTRICT */
	n_min = va_arg(ap, int);
	assert(n_min >= 0);
 /* NOSTRICT */
	n_max = va_arg(ap, int);
	assert(n_max >= n_min);
 /* NOSTRICT */
	n_opts = va_arg(ap, int);
	assert(n_opts > 1);
 /*
  * allocate space for error message string.  3 chars per option, for "-x,"
  * or "-x\0"
  */
 /* NOSTRICT */
	opts = ecalloc(n_opts, 3);
	if (opts == NULL) {
		error("can't allocate opt_check error message string");
	}

	n_got = 0;

	do {
		OPTION_T       *p;	/* -> current option descriptor	 */

 /* NOSTRICT */
		p = va_arg(ap, OPTION_T *);
		assert(p != NULL);
 /*
  * count # of set options
  */
		if (p->nargs > 0) {
			++n_got;
		}
 /*
  * accumulate option letters in error message string
  */
		if (opts[0] != 0) {
			(void) strcat(opts, ",");
		}

		(void) strcat(opts, "-");
		(void) strncat(opts, &p->option, 1);
	} while (--n_opts > 0);

	va_end(ap);
 /*
  * check # options against bounds
  */
	if (n_got < n_min) {
		error("must specify at least %d of: %s", n_min, opts);
	}

	if (n_got > n_max) {
		error("may specify no more than %d of: %s", n_max, opts);
	}
 /*
  * OK: discard error message string
  */
	free(opts);
}

#endif
