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
**	itoa -- convert integer to string
**
** SYNOPSIS
**	#include "ipw.h"
**
**	char *
**	itoa(
**		char    *s,		|* -> output string		 *|
**		int      i)		|* input integer		 *|
**
** DESCRIPTION
**	Itoa stores the ASCII representation of "i", followed by an EOS, into
**	the character array pointed to by "s".
**
**	The array pointed to by "s" must be large enough to hold the ASCII
**	representation of "i", plus a trailing EOS.
**
** RETURN VALUE
**	Itoa returns the value of the pointer "s".
*/

#include "ipw.h"

char *
itoa(
	char           *s,		/* -> output string		 */
	int             i)		/* input integer		 */
{
	assert(s != NULL);

	(void) sprintf(s, "%d", i);
	return (s);
}
