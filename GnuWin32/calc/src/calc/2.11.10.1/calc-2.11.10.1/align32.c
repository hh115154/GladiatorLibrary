/*
 * align32 - determine if 32 bit accesses must be aligned
 *
 * Copyright (C) 1999  Landon Curt Noll
 *
 * Calc is open software; you can redistribute it and/or modify it under
 * the terms of the version 2.1 of the GNU Lesser General Public License
 * as published by the Free Software Foundation.
 *
 * Calc is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU Lesser General
 * Public License for more details.
 *
 * A copy of version 2.1 of the GNU Lesser General Public License is
 * distributed with calc under the filename COPYING-LGPL.  You should have
 * received a copy with calc; if not, write to Free Software Foundation, Inc.
 * 59 Temple Place, Suite 330, Boston, MA  02111-1307, USA.
 *
 * @(#) $Revision: 29.2 $
 * @(#) $Id: align32.c,v 29.2 2000/06/07 14:02:13 chongo Exp $
 * @(#) $Source: /usr/local/src/cmd/calc/RCS/align32.c,v $
 *
 * Under source code control:	1995/11/23 05:18:06
 * File existed as early as:	1995
 *
 * chongo <was here> /\oo/\	http://www.isthe.com/chongo/
 * Share and enjoy!  :-)	http://www.isthe.com/chongo/tech/comp/calc/
 */


#include <stdio.h>
#include <signal.h>
#include "longbits.h"

#include "have_unistd.h"
#if defined(HAVE_UNISTD_H)
#include <unistd.h>
#endif

static void buserr(void);	/* catch alignment errors */


int
main(void)
{
	char byte[2*sizeof(USB32)];	/* mis-alignment buffer */
	USB32 *p;			/* mis-alignment pointer */
	int i;

#if defined(MUST_ALIGN32)
	/* force alignment */
	printf("#define MUST_ALIGN32\t%c* forced to align 32 bit values *%c\n",
	   '/', '/');
#else
	/* setup to catch alignment bus errors */
	signal(SIGBUS, buserr);
	signal(SIGSEGV, buserr);  /* some systems will generate SEGV instead! */

	/* mis-align our long fetches */
	for (i=0; i < sizeof(USB32); ++i) {
		p = (USB32 *)(byte+i);
		*p = i;
		*p += 1;
	}

	/* if we got here, then we can mis-align longs */
	printf("#undef MUST_ALIGN32\t%c* can mis-align 32 bit values *%c\n",
	   '/', '/');

#endif
	/* exit(0); */
	return 0;
}


/*
 * buserr - catch an alignment error
 *
 * given:
 *	arg		to keep ANSI C happy
 */
/*ARGSUSED*/
static void
buserr(int arg)
{
	/* alignment is required */
	printf("#define MUST_ALIGN32\t%c* must align 32 bit values *%c\n",
	  '/', '/');
	exit(0);
}
