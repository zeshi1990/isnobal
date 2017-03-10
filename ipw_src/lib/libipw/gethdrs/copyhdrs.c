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
**	copyhdrs -- copy IPW image headers
**
** SYNOPSIS
**	#include "gethdrs.h"
**
**	void
**	copyhdrs(
**		int      fdi,		|* input image file descriptor	 *|
**		int      nbands,	|* # bands in output image	 *|
**		int      fdo)		|* output image file descriptor	 *|
**
** DESCRIPTION
**	copyhdrs copies all IPW image headers from file descriptor fdi to
**	file descriptor fdo.  All per-image headers, and all per-band headers
**	pertaining to band numbers less than nbands, are copied.
**
**	copyhdrs is a simple way to copy all remaining headers from an input
**	to an output image, once any desired headers have been ingested.
**
** GLOBALS ACCESSED
**
** ERRORS
**	can't copy {header-name} header
**	can't skip {header-name} header
**	header read error
*/

#include "ipw.h"
#include "hdrio.h"
#include "gethdrs.h"

void
copyhdrs(
	int             fdi,		/* input image file descriptor	 */
	int             nbands,		/* # bands in output image	 */
	int             fdo)		/* output image file descriptor	 */
{
	REG_1 char     *hname;		/* name of current header	 */

	assert(nbands > 0);

	while ((hname = hrname(fdi)) != NULL && strdiff(hname, BOIMAGE)) {
		if (hrband(fdi) < nbands) {
			if (hcopy(fdi, fdo) == ERROR) {
				error("can't copy \"%s\" header", hname);
			}
		}
		else {
			if (hrskip(fdi) == ERROR) {
				error("can't skip \"%s\" header", hname);
			}
		}
	}

	if (hname == NULL) {
		error("header read error");
	}
}
