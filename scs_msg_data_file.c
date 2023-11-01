/*
** Copyright (2007) Sandia Corporation. Under the terms of Contract DE-AC04-94AL85000
** with Sandia Corporation, the U.S. Government retains certain rights in this software.
**
** Seshat
** $Id: scs_msg_data_file.c,v 1.7 2007/10/22 20:37:57 rolf Exp $
** Rolf Riesen (rolf@sandia.gov), September 2007, Sandia National Laboratories
** Write message data to a file
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scs.h"
#include <errno.h>


FILE *_scs_msg_data_out= NULL;	/* File pointer for message data file */


/*
** Position to write to
*/
static long data_pos= 0;



/*
** Return TRUE, if we are tracing. FALSE otherwise.
*/
int
_scs_init_msg_data(void)
{

char local_fname[2*MAX_LINE];


    Debug(DBG_CONFIG, "Message data file name is \"%s\"\n", _scs_config.msg_data_out);
    if (_scs_config.msg_data_out[0] == 0)   {
        /* We don't need to open a message data file */
        _scs_msg_data_out= NULL;
        return TRUE;
    }

    sprintf(local_fname, "%s.node_%d", _scs_config.msg_data_out, _scs_rank);
    _scs_open_out(local_fname, KEY_MSG_DATA_FILE, &_scs_msg_data_out);
    return TRUE;

}  /* end of _scs_init_msg_data() */



long int
_scs_msg_data_write(const void *buf, int len)
{

long int old_pos;
int rc;


    old_pos= data_pos;
    if (len <= 0)   {
	return old_pos;
    }

    rc= fwrite(buf, len, 1, _scs_msg_data_out);
    if (rc != 1)   {
	Err("Msg data file write error (buf %p, len %d): %s\n", buf, len, strerror(errno));
	_scs_abort();
    }
    data_pos= data_pos + len;
    return old_pos;


}  /* end of _scs_trace_event() */
