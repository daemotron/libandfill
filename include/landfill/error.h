/**
 * Copyright (c) 2010 Jesco Freund.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of Backtory nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

#ifndef __LF_ERROR_H
#define __LF_ERROR_H

#define E_PSEUDOBASE 1024

#include <errno.h>
#include <stddef.h>

#define E_LF_OK				0
#define E_MSGBASE			(E_PSEUDOBASE + 128)	/* base for message errors */
#define E_SYSBASE			(E_PSEUDOBASE + 256)	/* base for system errors */

/* message */
#define E_LF_MSG_OK			E_LF_OK
#define E_LF_MSG_UNEXDEST	(E_MSGBASE + 1)			/* unexpected destination in context */
#define E_LF_MSG_UNINICTX	(E_MSGBASE + 2)			/* uninitialized context */
#define E_LF_MSG_INITCTX	(E_MSGBASE + 3)			/* initialized context where uninitialized context expected */

/* system */
#define E_LF_SYS_NOGROUP	(E_SYSBASE + 1)			/* no group found */
#define E_LF_SYS_NOUSER		(E_SYSBASE + 2)			/* no user found */


extern int lf_strerror_r(int errnum, char *buffer, size_t bufsize);

/* This part is not for public use and should only be included
 * by error handling functions of libbacktory
 */
#ifdef __LF_ERROR_NAMES

typedef struct
{
	int code;
	char *description;
} lf_edesc_t;

lf_edesc_t lf_errlist[] =
{
	{ E_LF_OK,				"Undefined error: 0" 											},
	{ E_LF_MSG_UNEXDEST,	"Unexpected destination in context"								},
	{ E_LF_MSG_UNINICTX,	"Uninitialized context object, expected initialized ctx object"	},
	{ E_LF_MSG_INITCTX,		"Initialized context object, expected uninitialized ctx object"	},
	{ E_LF_SYS_NOGROUP,		"Group does not exist" 											},
	{ E_LF_SYS_NOUSER,		"User does not exist" 											},
	{ -1,					NULL															}	/* list terminator */
};

#endif /* __LF_ERROR_NAMES */

#endif /* __LF_ERROR_H */
