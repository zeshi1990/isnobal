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
**	_ok_sv -- validate a strvec
**
** SYNOPSIS
**	#include "ipw.h"
**
**	bool_t
**	_ok_sv(
**		REG_1 STRVEC_T *p)
**
** DESCRIPTION
**	_ok_sv checks whether p points to a valid strvec.
**
** RETURN VALUE
**	true if p points to a valid strvec, else false.
*/

#include "ipw.h"
#include "_strvec.h"

bool_t
_ok_sv(
	REG_1 STRVEC_T *p)
{
	return (p != NULL &&
		p->n > 0 &&
		p->curr >= 0 && p->curr < p->n &&
		p->v != NULL);
}
