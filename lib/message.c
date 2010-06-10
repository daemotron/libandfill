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

#define MSG_NAMES
#include "landfill/message.h"
#undef MSG_NAMES

#include "config.h"

#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef HAVE_PTHREAD_H
#include <pthread.h>
#endif

#include "landfill/error.h"


void
__msg_loglevel(char *buffer, size_t bufsize, int loglevel)
{
	msg_code_t *n = msg_prioritynames;
	while ((n->c_name) && (n->c_val != loglevel))
		n++;
	
	memset(buffer, (int)'\0', bufsize);
	if (n->c_name)
		strncpy(buffer, (char *)n->c_name, (strlen((char *)n->c_name) > (bufsize - 1) ? (bufsize -1) : strlen((char *)n->c_name)));
	else
		strncpy(buffer, "none", 5 > (bufsize - 1) ? (bufsize - 1) : 5);
}


void
__msg_strmcap(char *buffer, size_t bufsize, const char *string)
{
	int i;
	
	memset(buffer, (int)'\0', bufsize);
	
	for (i = 0; i < (int)strlen(string) && i < bufsize - 1; i++)
	{
		if ((string[i] >= 97) && (string[i] <= 122))
			buffer[i] = string[i] - 32;
		else
			buffer[i] = string[i];
	}
}


/* The following code only works on systems providing a syslog facility */
#ifdef HAVE_SYSLOG_H

#include <syslog.h>

int
__msg_init_syslog(msg_ctx_t *ctx, const char *ident, int option, int facility)
{
	int rval = 0;
	
	ctx->destination = MSG_SYSLOG;
	memset(ctx->syslog_ident, (int)'\0', MSG_IDENT_MAXLEN);
	strncpy(ctx->syslog_ident, ident, (strlen(ident) > (MSG_IDENT_MAXLEN - 1) ? (MSG_IDENT_MAXLEN -1) : strlen(ident)));
	ctx->syslog_option = option;
	ctx->syslog_facility = facility;
	
	openlog(ident, option, facility);
	
	ctx->status = INITIALIZED;
	
	return rval;
}


int
__msg_close_syslog(msg_ctx_t *ctx)
{
	int rval = 0;
	
	if (ctx->destination == MSG_SYSLOG)
	{
		closelog();
		ctx->destination = MSG_NONE;
		memset(ctx->syslog_ident, (int)'\0', MSG_IDENT_MAXLEN);
		ctx->syslog_facility = 0;
		ctx->syslog_option = 0;
		ctx->status = NONE;
	}
	else
	{
		rval = E_LF_MSG_UNEXDEST;
	}

	return rval;
}

#endif /* HAVE_SYSLOG_H */


int
__msg_init_file(msg_ctx_t *ctx, const char *filename)
{
	int rval = 0;
	
	ctx->destination = MSG_FILE;
	ctx->file_fp = NULL;
	
	memset(ctx->file_name, (int)'\0', MSG_FNAME_MAXLEN);
	strncpy(ctx->file_name, filename, (strlen(filename) > (MSG_FNAME_MAXLEN - 1) ? (MSG_FNAME_MAXLEN -1) : strlen(filename)));
	
	ctx->file_fp = fopen(filename, "a");
	if (NULL == ctx->file_fp)
	{
		rval = errno;
		ctx->destination = NONE;
		ctx->status = FAIL;
	}
	else
	{
		setvbuf(ctx->file_fp, NULL, _IONBF, 0);
		ctx->status = INITIALIZED;
	}
	return rval;
}


int
__msg_close_file(msg_ctx_t *ctx)
{
	int rval = 0;
	if (ctx->destination == MSG_FILE)
	{
		if (ctx->file_fp && (ctx->file_fp != stderr) && (ctx->file_fp != stdout))
			fclose(ctx->file_fp);
		
		ctx->destination = MSG_NONE;
		ctx->status = NONE;
		memset(ctx->file_name, (int)'\0', MSG_FNAME_MAXLEN);
	}
	else
	{
		rval = E_LF_MSG_UNEXDEST;
	}
	return rval;
}


int
__msg_init_term(msg_ctx_t *ctx)
{
	int rval = 0;
	ctx->destination = MSG_TERM;
	ctx->status = INITIALIZED;
	return rval;
}


int
__msg_close_term(msg_ctx_t *ctx)
{
	int rval = 0;
	if (ctx->destination == MSG_TERM)
	{
		ctx->destination = MSG_NONE;
		ctx->status = NONE;
	}
	else
	{
		rval = E_LF_MSG_UNEXDEST;
	}
	return rval;
}


int
__msg_close(msg_ctx_t *ctx)
{
	switch (ctx->destination)
	{
#ifdef HAVE_SYSLOG_H
		case MSG_SYSLOG:
			return __msg_close_syslog(ctx);
			break;
#endif
		case MSG_FILE:
			return __msg_close_file(ctx);
			break;
			
		case MSG_TERM:
			return __msg_close_term(ctx);
			break;
			
		default:
			return E_LF_MSG_UNEXDEST;
	}
}


#ifdef HAVE_SYSLOG_H

extern int
msg_init_syslog(msg_ctx_t *ctx, const char *ident, int option, int facility)
{
	int rval = 0;
#ifdef HAVE_PTHREAD_H
	int stat;
	stat = pthread_mutex_lock(&ctx->mutex);
	if (stat == 0)
	{
#endif
	if (ctx->status == INITIALIZED)
	{
		rval = __msg_close(ctx);
		if (rval)
			return rval;
	}
	rval = __msg_init_syslog(ctx, ident, option, facility);
#ifdef HAVE_PTHREAD_H
		stat = pthread_mutex_unlock(&ctx->mutex);
		if (stat != 0)
			rval = stat;
	}
	else
		rval = stat;
#endif
	return rval;
}


extern int
msg_close_syslog(msg_ctx_t *ctx)
{
	int rval = 0;
#ifdef HAVE_PTHREAD_H
	int stat;
	stat = pthread_mutex_lock(&ctx->mutex);
	if (stat == 0)
	{
#endif
		rval = __msg_close_syslog(ctx);
#ifdef HAVE_PTHREAD_H
		stat = pthread_mutex_unlock(&ctx->mutex);
		if (stat != 0)
			rval = stat;
	}
	else
		rval = stat;
#endif
	return rval;
}

#endif /* HAVE_SYSLOG_H */


extern int
msg_init_file(msg_ctx_t *ctx, const char *filename)
{
	int rval = 0;
#ifdef HAVE_PTHREAD_H
	int stat;
	stat = pthread_mutex_lock(&ctx->mutex);
	if (stat == 0)
	{
#endif
		if (ctx->status == INITIALIZED)
		{
			rval = __msg_close(ctx);
			if (rval)
				return rval;
		}
		rval = __msg_init_file(ctx, filename);
#ifdef HAVE_PTHREAD_H
		stat = pthread_mutex_unlock(&ctx->mutex);
		if (stat != 0)
			rval = stat;
	}
	else
		rval = stat;
#endif
	return rval;
}


extern int
msg_close_file(msg_ctx_t *ctx)
{
	int rval = 0;
#ifdef HAVE_PTHREAD_H
	int stat;
	stat = pthread_mutex_lock(&ctx->mutex);
	if (stat == 0)
	{
#endif
		rval = __msg_close_file(ctx);
#ifdef HAVE_PTHREAD_H
		stat = pthread_mutex_unlock(&ctx->mutex);
		if (stat != 0)
			rval = stat;
	}
	else
		rval = stat;
#endif
	return rval;
}


extern int
msg_init_term(msg_ctx_t *ctx)
{
	int rval = 0;
#ifdef HAVE_PTHREAD_H
	int stat;
	stat = pthread_mutex_lock(&ctx->mutex);
	if (stat == 0)
	{
#endif
		if (ctx->status == INITIALIZED)
		{
			rval = __msg_close(ctx);
			if (rval)
				return rval;
		}
		rval = __msg_init_term(ctx);
#ifdef HAVE_PTHREAD_H
		stat = pthread_mutex_unlock(&ctx->mutex);
		if (stat != 0)
			rval = stat;
	}
	else
		rval = stat;
#endif
	return rval;
}


extern int
msg_close_term(msg_ctx_t *ctx)
{
	int rval = 0;
#ifdef HAVE_PTHREAD_H
	int stat;
	stat = pthread_mutex_lock(&ctx->mutex);
	if (stat == 0)
	{
#endif
		rval = __msg_close_term(ctx);
#ifdef HAVE_PTHREAD_H
		stat = pthread_mutex_unlock(&ctx->mutex);
		if (stat != 0)
			rval = stat;
	}
	else
		rval = stat;
#endif
	return rval;
}


extern int
msg_close(msg_ctx_t *ctx)
{
	int rval = 0;
#ifdef HAVE_PTHREAD_H
	int stat;
	stat = pthread_mutex_lock(&ctx->mutex);
	if (stat == 0)
	{
#endif
		rval = __msg_close(ctx);
#ifdef HAVE_PTHREAD_H
		stat = pthread_mutex_unlock(&ctx->mutex);
		if (stat != 0)
			rval = stat;
	}
	else
		rval = stat;
#endif
	return rval;
}



extern int
msg_ctx_create(msg_ctx_t *ctx)
{
	int rval = 0;
	ctx->destination = MSG_NONE;
	ctx->file_fp = NULL;
#ifdef HAVE_PTHREAD_H
	memset(ctx->file_name, (int)'\0', MSG_FNAME_MAXLEN);
	rval = pthread_mutex_init(&ctx->mutex, NULL);
	if (rval)
		return rval;
#endif	
	ctx->syslog_facility = 0;
	ctx->syslog_option = 0;
	memset(ctx->syslog_ident, (int)'\0', MSG_IDENT_MAXLEN);
	ctx->status = READY;
	return rval;
}


extern int
msg_ctx_destroy(msg_ctx_t *ctx)
{
	int rval = 0;
	
	if (ctx->status == INITIALIZED)
	{
		rval = msg_close(ctx);
		if (rval)
			return rval;
	}
	
	ctx->destination = MSG_NONE;
	ctx->file_fp = NULL;
	memset(ctx->file_name, (int)'\0', MSG_FNAME_MAXLEN);
#ifdef HAVE_PTHREAD_H
	rval = pthread_mutex_destroy(&ctx->mutex);
	if (rval)
		return rval;
#endif	
	ctx->syslog_facility = 0;
	ctx->syslog_option = 0;
	memset(ctx->syslog_ident, (int)'\0', MSG_IDENT_MAXLEN);
	ctx->status = NONE;
	return rval;
}


extern int
msg_log(msg_ctx_t *ctx, int loglevel, const char *format, ...)
{
	int rval = 0;
#ifdef HAVE_PTHREAD_H
	int stat;
	stat = pthread_mutex_lock(&ctx->mutex);
	if (stat == 0)
	{
#endif
		va_list vargptr;
		char level[16] = { '\0' };
		char level_name[16] = { '\0' };
		char level_cname[16] = { '\0' };
		char *message = NULL;
		time_t tstmp;
		struct tm *nicetime;
		char tbuf[21] = { '\0' };

		if (ctx->status != INITIALIZED)
		{
			rval = E_LF_MSG_UNINICTX;
		}
		else
		{
			va_start(vargptr, format);
			switch (ctx->destination)
			{
#ifdef HAVE_SYSLOG_H
				case MSG_SYSLOG:
					vsyslog(loglevel, format, vargptr);
					break;
#endif
				case MSG_FILE:
					time(&tstmp);
					nicetime = localtime(&tstmp);
					strftime(tbuf, 21, "%Y-%m-%d %H:%M:%S ", nicetime);
					__msg_loglevel(level_name, 16, loglevel);
					__msg_strmcap(level_cname, 16, level_name);
					snprintf(level, 15, "[%s] ", level_cname);
					message = (char *)malloc(strlen(tbuf) + strlen(level) + strlen(format) + 1);
					if (NULL == message)
					{
						rval = errno;
					}
					else
					{
						memset(message, '\0', strlen(tbuf) + strlen(level) + strlen(format) + 1);
						strncpy(message, tbuf, strlen(tbuf) + 1);
						strncat(message, level, strlen(level) + 1);
						strncat(message, format, strlen(format) + 1);
						vfprintf(ctx->file_fp, message, vargptr);
						fflush(ctx->file_fp);
					}
					break;
				
				case MSG_TERM:
					__msg_loglevel(level_name, 16, loglevel);
					__msg_strmcap(level_cname, 16, level_name);
					snprintf(level, 15, "[%s] ", level_cname);
					message = (char *)malloc(strlen(level) + strlen(format) + 1);
					if (NULL == message)
					{
						rval = errno;
					}
					else
					{
						strncpy(message, level, strlen(level)+1);
						strncat(message, format, strlen(format) + 1);
						if (loglevel > 3)
							vfprintf(stdout, message, vargptr);
						else
							vfprintf(stderr, message, vargptr);
					}
					break;
				
				default:
					break;
			}
			va_end(vargptr);
		}
#ifdef HAVE_PTHREAD_H
		stat = pthread_mutex_unlock(&ctx->mutex);
		if (stat != 0)
			rval = stat;
	}
	else
		rval = stat;
#endif
	return rval;
}
