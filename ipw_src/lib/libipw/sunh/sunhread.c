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
**	sunhread -- read an IPW SUNH header
**
** SYNOPSIS
**	#include "sunh.h"
**
**	SUNH_T **
**	sunhread(
**		int             fd)	|* image file descriptor	 *|
**
** DESCRIPTION
**	Sunhread reads a SUNH image header from file descriptor fd.  An
**	array of SUNH_T pointers is allocated, one per band.  If a band
**	has a SUNH header, then an SUNH_T header is allocated and its
**	address is placed in the corresponding array element;
**	otherwise, the corresponding array element is NULL.
**
**	Before calling sunhread, the caller must verify (by calling
**	hrname()) that a SUNH header is available for ingesting.
**
** RETURN VALUE
**	pointer to array of SUNH_T pointers; else NULL if EOF or error
*/

#include "ipw.h"
#include "hdrio.h"
#include "bih.h"
#include "sunh.h"
#include "_sunh.h"

SUNH_T **
sunhread(
	int             fd)		/* image file descriptor	 */
{
	char           *hname;		/* current header name		 */
	int             nbands;		/* # bands / pixel		 */
	SUNH_T        **sunhpp;		/* -> array of SUNH pointers	 */

 /*
  * allocate array of header pointers
  */
	nbands = hnbands(fd);
	assert(nbands > 0);

 /* NOSTRICT */
	sunhpp = (SUNH_T **) hdralloc(nbands, sizeof(SUNH_T *), fd,
				      SUNH_HNAME);
	if (sunhpp == NULL) {
		return (NULL);
	}
 /*
  * loop through per-band headers
  */
	while ((hname = hrname(fd)) != NULL && streq(hname, SUNH_HNAME)) {
		int             band;	/* current header band #	 */
		int             err;	/* hgetrec return value		 */
		SUNH_T         *sunhp;	/* -> current SUNH		 */

		char            key[HREC_MAX + 1];	/* keyword	 */
		char            value[HREC_MAX + 1];	/* value string	 */

 /*
  * get header band #
  */
		band = hrband(fd);
		if (band < 0 || band >= nbands) {
			uferr(fd);
			usrerr("\"%s\" header: bad band \"%d\"",
			       SUNH_HNAME, band);
			return (NULL);
		}
 /*
  * allocate header
  */
 /* NOSTRICT */
		sunhp = (SUNH_T *) hdralloc(1, sizeof(SUNH_T), fd,
					    SUNH_HNAME);
		if (sunhp == NULL) {
			return (NULL);
		}

		sunhpp[band] = sunhp;
 /*
  * ingest records
  */
		while ((err = hgetrec(fd, (char *) NULL, key, value))
		       == HGOT_DATA) {
 /*
  * ignore all-comment records
  */
			if (key[0] == EOS) {
				continue;
			}
 /*
  * barf if missing value
  */
			if (value[0] == EOS) {
				uferr(fd);
				usrerr("\"%s\" header, key \"%s\": no value",
				       SUNH_HNAME, key);
				return (NULL);
			}
 /*
  * match key to header field, ingest value
  */
			if (streq(key, SUNH_COS)) {
				sunhp->cos_sun = atof(value);
			}
			else if (streq(key, SUNH_ZEN)) {
				sunhp->zen_sun = atof(value);
			}
			else if (streq(key, SUNH_AZM)) {
				sunhp->azm_sun = atof(value);
			}
		}

		if (err == ERROR) {
			return (NULL);
		}
	}

	if (hname == NULL) {
		return (NULL);
	}
 /*
  * verify the header
  */
	if (!_sunhcheck(sunhpp, nbands)) {
		uferr(fd);
		return (NULL);
	}

	return (sunhpp);
}
