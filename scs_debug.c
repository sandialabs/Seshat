/*
** Copyright (2007) Sandia Corporation. Under the terms of Contract DE-AC04-94AL85000
** with Sandia Corporation, the U.S. Government retains certain rights in this software.
**
** Seshat
** $Id: scs_debug.c,v 1.16 2008/05/30 21:52:42 rolf Exp $
** Routines that help with debugging the simulator
** Rolf Riesen (rolf@sandia.gov), June 2006, Sandia National Laboratories
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include "scs.h"

/* The debug file pointer */
FILE *_scs_dbg_out= NULL;



/*
** Write a debug message
*/
void
_scs_Debug(const char *func, int line, int dbg, char *fmt, ...)
{

va_list ap;
char new_fmt[MAX_LINE];
char log_str[MAX_LINE];
char *debug;


    if (_scs_dbg_out == NULL)   {
	/* No configuration file set (yet). Use stderr for now. */
	_scs_dbg_out= stderr;
    }

    if (_scs_verbose & dbg)   {
	switch (dbg)   {
	    case DBG_CONFIG:		/* Config file processing */	debug= "CFG"; break;
	    case DBG_PROCESSMGMT:	/* Process management */	debug= "PROC"; break;
	    case DBG_MPI:		/* MPI related things */	debug= "MPI"; break;
	    case DBG_MPI2:		/* MPI verbose */		debug= "MPI2"; break;
	    case DBG_MPI3:		/* MPI non-C */		        debug= "MPI3"; break;
	    case DBG_EVENT:		/* events */			debug= "EVNT"; break;
	    case DBG_EMU:		/* Emulator */			debug= "EMU"; break;
	    case DBG_NET:		/* Network sim */		debug= "NET"; break;
	    case DBG_TIME:		/* (virtual) time */		debug= "TIME"; break;
	    case DBG_MPI_INTERNAL:	/* Internal MPI usage */	debug= "MPI int"; break;
	    case DBG_GENERAL:		/* General information */	debug= "MISC"; break;
	    case DBG_REQ:		/* Non-blocking tracing */	debug= "REQ"; break;
	    default:			/* Don't know */		debug= "UNKNOWN";
	}

	if (_scs_rank == _scs_net_sim_node)   {
	    sprintf(new_fmt, "[sim] %-7s %3d %-18s   %s", debug, line, func, fmt);
	} else   {
	    sprintf(new_fmt, "[%3d] %-7s %3d %-18s   %s", _scs_rank, debug, line, func, fmt);
	}
	va_start(ap, fmt);
	vsnprintf(log_str, MAX_LINE, new_fmt, ap);
	va_end(ap);
	fprintf(_scs_dbg_out, "%s", log_str);
        fflush(_scs_dbg_out);
    }

}  /* end of _scs_Debug() */



/*
** Write information to a file
*/
void
_scs_file_print(FILE *file, char *leader, char *fmt, ...)
{

va_list ap;
char new_fmt[MAX_LINE];
char log_str[MAX_LINE];


    if (file == NULL)   {
	/* No configuration file set. */
	return;
    }

    sprintf(new_fmt, "%s%s", leader, fmt);
    va_start(ap, fmt);
    vsnprintf(log_str, MAX_LINE, new_fmt, ap);
    va_end(ap);
    fprintf(file, "%s", log_str);
    fflush(file);

}  /* end of _scs_file_print() */



/*
** Used for Warnings and errors to stderr.
*/
void
_scs_Warn(const char *func, int line, char *type, char *fmt, ...)
{

va_list ap;
char new_fmt[MAX_LINE];
char log_str[MAX_LINE];


	if (_scs_rank == _scs_net_sim_node)   {
	    sprintf(new_fmt, "[sim] %s %3d %-18s   %s", type, line, func, fmt);
	} else   {
	    sprintf(new_fmt, "[%3d] %s %3d %-18s   %s", _scs_rank, type, line, func, fmt);
	}
	va_start(ap, fmt);
        vsnprintf(log_str, MAX_LINE, new_fmt, ap);
	va_end(ap);
        fprintf(stderr, "%s", log_str);

}  /* end of _scs_Warn() */



/*
** Node 0 creates and truncates the file
*/
int
_scs_create_out(char *fname, char *type)
{

FILE *tmp;


    if (strcmp(fname, "stderr") == 0)   {
	return 0;
    } else if (strcmp(fname, "stdout") == 0)   {
	return 0;
    }

    tmp= fopen(fname, "w");
    if (tmp == NULL)   {
	Err("Creation of %s output file \"%s\" failed: %s\n", type, fname, strerror(errno));
	return -1;
    }
    fclose(tmp);
    fflush(tmp);
    return 0;

}  /* end of _scs_create_out() */



/*
** All nodes open the file in append mode before using it
** The file might be "", in which case we set the file pointer
** to NULL, so no output is generated
*/
void
_scs_open_out(char *fname, char *type, FILE **file)
{

char *file_buf;


    if (strcmp(fname, "") == 0)   {
	*file= NULL;
	return;
    } else if (strcmp(fname, "stderr") == 0)   {
	*file= stderr;
	return;
    } else if (strcmp(fname, "stdout") == 0)   {
	*file= stdout;
	return;
    }

    *file= fopen(fname, "a");
    if (*file == NULL)   {
	Err("Opening of %s output file \"%s\" failed: %s\n", type, fname, strerror(errno));
	*file= stderr;
	_scs_abort();
    }

    file_buf= (char *)malloc(MAX_LINE * 100);
    if (file_buf == NULL)   {
	Err("Out of memory for IO buffer\n", "");
	_scs_abort();
    }
    setvbuf(*file, file_buf, _IOLBF, MAX_LINE * 100);

}  /* end of _scs_open_out() */
