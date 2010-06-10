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


#ifndef __LF_MESSAGE_H
#define __LF_MESSAGE_H

#include "config.h"

#include <stdio.h>

#ifdef HAVE_PTHREAD_H
#include <pthread.h>
#endif


#ifdef HAVE_SYSLOG_H
#include <syslog.h>
#else
#define LOG_EMERG       0       /* system is unusable */
#define LOG_ALERT       1       /* action must be taken immediately */
#define LOG_CRIT        2       /* critical conditions */
#define LOG_ERR         3       /* error conditions */
#define LOG_WARNING     4       /* warning conditions */
#define LOG_NOTICE      5       /* normal but significant condition */
#define LOG_INFO        6       /* informational */
#define LOG_DEBUG       7       /* debug-level messages */
#endif /* HAVE_SYSLOG_H */

#define MSG_EUNEXPDEST	1
#define MSG_EUNINITCTX	2

#ifdef MSG_NAMES

typedef struct
{
	char *c_name;
	int c_val;
} msg_code_t;

msg_code_t msg_prioritynames[] =
{
#ifdef LOG_EMERG
	{ "emerg", LOG_EMERG,      },
#endif
#ifdef LOG_ALERT
	{ "alert", LOG_ALERT,      },
#endif
#ifdef LOG_CRIT
	{ "crit ", LOG_CRIT,       },
#endif
#ifdef LOG_ERR
	{ "error", LOG_ERR,        },
#endif
#ifdef LOG_WARNING
	{ "warn ", LOG_WARNING,    },
#endif
#ifdef LOG_NOTICE
	{ "note ", LOG_NOTICE,     },
#endif
#ifdef LOG_INFO
	{ "info ", LOG_INFO,       },
#endif
#ifdef LOG_DEBUG
	{ "debug", LOG_DEBUG,      },
#endif
	{ NULL, -1,                }
};

#endif /* MSG_NAMES */

#define MSG_IDENT_MAXLEN 1024
#define MSG_FNAME_MAXLEN 1024

typedef struct
{
	enum { NONE, READY, INITIALIZED, FAIL } status;
	enum { 
		MSG_NONE, 
#ifdef HAVE_SYSLOG_H
		MSG_SYSLOG,
#endif
		MSG_FILE, 
		MSG_TERM 
	} destination;
	FILE *file_fp;
#ifdef HAVE_PTHREAD_H
	pthread_mutex_t mutex;
#endif
	char file_name[MSG_FNAME_MAXLEN];
	char syslog_ident[MSG_IDENT_MAXLEN];
	int syslog_option;
	int syslog_facility;
} msg_ctx_t;


extern int msg_ctx_create(msg_ctx_t *ctx);
extern int msg_ctx_destroy(msg_ctx_t *ctx);

#ifdef HAVE_SYSLOG_H
extern int msg_init_syslog(msg_ctx_t *ctx, const char *ident, int option, int facility);
extern int msg_close_syslog(msg_ctx_t *ctx);
#endif /* HAVE_SYSLOG_H */

extern int msg_init_file(msg_ctx_t *ctx, const char *filename);
extern int msg_close_file(msg_ctx_t *ctx);

extern int msg_init_term(msg_ctx_t *ctx);
extern int msg_close_term(msg_ctx_t *ctx);

extern int msg_close(msg_ctx_t *ctx);
extern int msg_log(msg_ctx_t *ctx, int loglevel, const char *format, ...);

#endif /* __LF_MESSAGE_H */
