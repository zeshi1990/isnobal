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

/*
** NAME
**	usage -- print usage message and exit
**
** SYNOPSIS
**	#include "getargs.h"
**
**	void
**	usage(void)
**
** DESCRIPTION
**	usage prints a usage message on the standard error output, then exits
**	with a nonzero status.  The usage message contains the command name, a
**	one line description of the command, and a synopsis of the command's
**	options.
**
** RETURN VALUE
**	This function does not return; it terminates the program by calling
**	the ipwexit function.
**
** GLOBAL VARIABLES READ
**	_argv
**	_descrip
**	_optv
*/

#include <string.h>

#include "ipw.h"
#include "_ipwmain.h"
#include "../../libipw/error/_error.h"	/* for "_usrerr" */

/*
 *  A local routine to print the arguments for an option.
 */
static void pr_opt_args(OPTION_T *optp);

void
usage(void)
{

	REG_2 int       i;		/* _optv index of current option */
	REG_1 OPTION_T *optp;		/* -> current option		 */

	assert(_argv != NULL && _argv[0] != NULL);
	assert(_descrip != NULL);

	(void) fflush(stdout);

 /*
  * print error string if set, else program description
  */
	if (_usrerr[0] != EOS) {
		(void) fprintf(stderr, "\nERROR: %s\n", _usrerr);
	}
	else {
		(void) fprintf(stderr, "\n%s -- %s\n", _argv[0], _descrip);
	}

 /*
  * print program synopsis
  * 
  * - name
  */
	(void) fprintf(stderr, "\nUsage: %s", _argv[0]);

	if (_optv == NULL) {
		(void) fprintf(stderr, "\n\n");
		ipwexit(EXIT_FAILURE);
	}

 /*
  * - options
  */
	i = 0;

	for (; (optp = _optv[i]) != NULL && optp->option != OPERAND; ++i) {
		pr_opt_args(optp);
	}

 /*
  * - operands
  */
	for (; (optp = _optv[i]) != NULL; ++i) {
		pr_opt_args(optp);
	}

	(void) fprintf(stderr, "\n");

 /*
  * print option descriptions
  */
	i = 0;

	if ((optp = _optv[i]) != NULL && optp->option != OPERAND) {
		(void) fprintf(stderr, "\nOptions:\n");

		do {
			assert(optp->descrip != NULL);

			(void) fprintf(stderr, "\t%s%s\t%s\n",
				       optp->option, 
				       (strlen(optp->option) > 7) ? "\n\t" : "",
				       optp->descrip);

			++i;
		} while ((optp = _optv[i]) != NULL
			 && optp->option != OPERAND);
	}

 /*
  * print operand descriptions
  */
	if ((optp = _optv[i]) != NULL) {
		(void) fprintf(stderr, "\nOperands:\n");

		do {
			assert(optp->descrip != NULL);

			(void) fprintf(stderr, "\t%s%s\t%s\n",
				       optp->arg_descrip,
				       (strlen(optp->arg_descrip) > 7) ? "\n\t"
								       : "",
				       optp->descrip);

			++i;
		} while ((optp = _optv[i]) != NULL);
	}

 /*
  * all done
  */
	(void) fprintf(stderr, "\n");
	ipwexit(EXIT_FAILURE);
}

/* ------------------------------------------------------------------------ */

static void
pr_opt_args(
	REG_1 OPTION_T *optp)		/* -> current option descriptor	 */
{
	REG_2 int       j;		/* loop counter			 */
	REG_6 char      sep;		/* optarg/operand separators	 */
	REG_3 const char *placeholder;	/* placeholder for opt's args    */
	REG_4 char     *end;		/* separator following placeholder */
	REG_5 int	length;		/* length of placeholder	 */

	(void) fprintf(stderr, " ");

	if (!optp->required) {
		(void) fprintf(stderr, "[ ");
	}

	if (optp->option != OPERAND) {
		(void) fprintf(stderr, "-%s", optp->option);
	}

	if (optp->type != NO_OPTARGS) {
		assert(optp->arg_descrip != NULL);

		if (optp->option == OPERAND) {
			sep = ' ';
		}
		else {
			(void) fprintf(stderr, " ");
			sep = ',';
		}

		/*
		 *  If the argument descriptor contains the separator
		 *  character (',' for options, ' ' (space) for operands),
		 *  then each substring between separators is a separate
		 *  argument placeholder.
		 */
		placeholder = optp->arg_descrip;
		end = strchr(placeholder, sep);
		if (end == NULL) {
			length = strlen(placeholder);
		} else {
			length = end - placeholder;
		}
		(void) fprintf(stderr, "%.*s", length, placeholder);

		for (j = 1; j < optp->min_nargs; ++j) {
			/*
			 *  Find the next placeholder.  If none, we'll just
			 *  use the previous placeholder.	
			 */
			if (end != NULL) {
				placeholder = end + 1;
				end = strchr(placeholder, sep);
				if (end == NULL) {
					length = strlen(placeholder);
				} else {
					length = end - placeholder;
				}
			}
			(void) fprintf(stderr, "%c%.*s", sep, length,
							 placeholder);
		}

		if (optp->max_nargs == 0) {
			(void) fprintf(stderr, "%c...", sep);
		}
		else if (j < optp->max_nargs) {
			(void) fprintf(stderr, "[");

			do {
				/*
				 *  Find the next placeholder.  If none, we'll
				 *  just use the previous placeholder.	
				 */
				if (end != NULL) {
					placeholder = end + 1;
					end = strchr(placeholder, sep);
					if (end == NULL) {
						length = strlen(placeholder);
					} else {
						length = end - placeholder;
					}
				}
				(void) fprintf(stderr, "%c%.*s",
					       sep, length, placeholder);
			} while (++j < optp->max_nargs);

			(void) fprintf(stderr, "]");
		}
	}

	if (!optp->required) {
		(void) fprintf(stderr, " ]");
	}
}
