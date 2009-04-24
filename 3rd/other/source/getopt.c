/*	$NetBSD: getopt.c,v 1.27 2005/11/29 03:12:00 christos Exp $	*/

/*
 * Copyright (c) 1987, 1993, 1994
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#if defined(LIBC_SCCS) && !defined(lint)
#if 0
static char sccsid[] = "@(#)getopt.c	8.3 (Berkeley) 4/27/95";
#else
__RCSID("$NetBSD: getopt.c,v 1.27 2005/11/29 03:12:00 christos Exp $");
#endif
#endif /* LIBC_SCCS and not lint */

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <other/getopt.h>

#ifdef __weak_alias
__weak_alias(getopt,_getopt)
#endif

#define	BADCH	(int)'?'
#define	BADARG	(int)':'
#define	EMSG	""

/*
 * getopt --
 *	Parse argc/argv argument vector.
 */
int getopt(int nargc, char * const nargv[], const char *ostr, GetOptContext *ctx)
{
	static const char *place = EMSG;	/* option letter processing */
	char *oli;				/* option letter list index */

	//_DIAGASSERT(nargv != NULL);
	//_DIAGASSERT(ostr != NULL);

	if (ctx->optreset || *place == 0) /* update scanning pointer */ 
	{
		ctx->optreset = 0;
		place = nargv[ctx->optind];
		if (ctx->optind >= nargc || *place++ != '-') {
			/* Argument is absent or is not an option */
			place = EMSG;
			return (-1);
		}
		ctx->optopt = *place++;
		if (ctx->optopt == '-' && *place == 0) {
			/* "--" => end of options */
			++ctx->optind;
			place = EMSG;
			return (-1);
		}
		if (ctx->optopt == 0) {
			/* Solitary '-', treat as a '-' option
			   if the program (eg su) is looking for it. */
			place = EMSG;
			if (strchr(ostr, '-') == NULL)
				return -1;
			ctx->optopt = '-';
		}
	} else
		ctx->optopt = *place++;

	/* See if option letter is one the caller wanted... */
	if (ctx->optopt == ':' || (oli = (char *) strchr(ostr, ctx->optopt)) == NULL) {
		if (*place == 0)
			++ctx->optind;
		if (ctx->opterr && *ostr != ':')
			(void)fprintf(stderr,
			    "%s: unknown option -- %c\n", nargv[0],
			    ctx->optopt);
		return (BADCH);
	}

	/* Does this option need an argument? */
	if (oli[1] != ':') {
		/* don't need argument */
		ctx->optarg = NULL;
		if (*place == 0)
			++ctx->optind;
	} else {
		/* Option-argument is either the rest of this argument or the
		   entire next argument. */
		if (*place)
			ctx->optarg = (char *) place;
		else if (nargc > ++ctx->optind)
			ctx->optarg = nargv[ctx->optind];
		else {
			/* option-argument absent */
			place = EMSG;
			if (*ostr == ':')
				return (BADARG);
			if (ctx->opterr)
				(void)fprintf(stderr,
				    "%s: option requires an argument -- %c\n",
				    nargv[0], ctx->optopt);
			return (BADCH);
		}
		place = EMSG;
		++ctx->optind;
	}
	return (ctx->optopt);			/* return option letter */
}

void init_getopt_context( GetOptContext *ctx )
{
	ctx->optarg = 0;
	ctx->optind = 1;
	ctx->opterr = 1;
	ctx->optopt = -1;	
	ctx->optreset = 0;	
}


