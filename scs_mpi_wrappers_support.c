/*
** Copyright (2007) Sandia Corporation. Under the terms of Contract DE-AC04-94AL85000
** with Sandia Corporation, the U.S. Government retains certain rights in this software.
**
** Seshat
** $Id: scs_mpi_wrappers_support.c,v 1.7 2008/05/12 21:17:29 rolf Exp $
** Rolf Riesen (rolf@sandia.gov), June 2006, Sandia National Laboratories
**
** These are functions that provide support for our MPI wrappers.
*/

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <unistd.h>
#include "scs.h"
#include "scs_mpi_wrappers.h"


/*
** Locals and Globals
*/
static int my_app_rank= -1;
static int app_nnodes= -1;
int _scs_MPI_initialized= FALSE;


/*
** =================================================================================================
** FIXME: This needs to be dynamic and much more efficient!
** We need to keep track of requests that MPI hands out so we can tell
** inside MPI_Wait() whether they are for receives or sends.
*/
#define MAX_OPEN_REQ	(10 * 1024)
typedef struct open_req_t   {
    MPI_Request req;
    int send;			/* Is this for a send or a receive? */
    _scs_vtime_t tr;            /* When request was posted */
    int len;                    /* Size of requested message */
    int token;                  /* A way to match posts & receives and also sends & matched */
    int available;              /* Is this entry available? */
} open_req_t;


static open_req_t open_req[MAX_OPEN_REQ];
static long open_req_cnt;
static int high_water;



/*
** Find an open slot and store the request
*/
void
_scs_req_insert(MPI_Request request, int send, _scs_vtime_t tr, int len, unsigned int token)
{

int i;
static int first_time= 1;


    if (first_time)   {
	for (i= 0; i < MAX_OPEN_REQ; i++)   {
	    open_req[i].available= TRUE;
	    open_req[i].req= (MPI_Request)0;
	}
	open_req_cnt= 0;
	first_time= 0;
        high_water= 0;
    }

    /* This is highly inefficient, but useful to track bugs in applications... */
    for (i= 0; i < MAX_OPEN_REQ; i++)   {
        if (!open_req[i].available && (open_req[i].req == request))   {
	    Err("Request %p already in system!\n", request);
	    Err("    This is an MPI bug in the node program!\n", "");
            _scs_abort();
	}
    }

    for (i= 0; i < MAX_OPEN_REQ; i++)   {
	if (open_req[i].available)   {
	    open_req[i].req= request;
	    open_req[i].send= send;
            open_req[i].tr= tr;
            open_req[i].len= len;
            open_req[i].token= token;
	    open_req[i].available= FALSE;
	    Debug(DBG_MPI2, "Inserted request %p, token 0x%08x, send %d\n", request, token, send);
	    open_req_cnt++;
	    if (i > high_water)   {
		high_water= i;
	    }
	    return;
	}
    }

    Err("Out of request slots!\n", "");
    _scs_abort();

}  /* end of _scs_req_insert() */



/*
** Find a given request and remove it
*/
int
_scs_req_find(MPI_Request request, int *send, _scs_vtime_t *tr, int *len, unsigned int *token)
{

int i;


    for (i= 0; i < MAX_OPEN_REQ; i++)   {
	if (open_req[i].req == request)   {
	    *send= open_req[i].send;
            *tr= open_req[i].tr;
            *len= open_req[i].len;
            *token= open_req[i].token;
	    open_req[i].available= TRUE;
	    Debug(DBG_MPI2, "Found request %p, token  0x%08x, send %d\n", request, *token, *send);
	    open_req_cnt--;
	    return 1;
	}
    }

    Debug(DBG_MPI2, "Request %p not found\n", request);
    return 0;

}  /* end of _scs_req_find() */



int
_scs_req_dump(void)
{

int i;


    for (i= 0; i < high_water; i++)   {
        if (open_req[i].available)   {
            Debug(DBG_MPI2, "Deleted request %3d: req %p, token 0x%08x, send %d, len %d, in list %d\n", i, open_req[i].req, open_req[i].token,
                open_req[i].send, open_req[i].len, open_req_cnt);
        } else   {
            Debug(DBG_MPI2, "Active request  %3d: req %p, token 0x%08x, send %d, len %d, in list %d\n", i, open_req[i].req, open_req[i].token,
                open_req[i].send, open_req[i].len, open_req_cnt);
        }
    }

    return 0;

}  /* end of _scs_req_dump() */




/*
** =================================================================================================
** Initialize the Wrapper routines for use by the application
**
*/
void
_scs_init_MPI_wrappers(void)
{

    _scs_MPI_Comm_rank(_scs_app_comm, &my_app_rank);
    _scs_MPI_Comm_size(_scs_app_comm, &app_nnodes);

}   /* end of _scs_init_MPI_wrappers() */



/*
** =================================================================================================
** Init and Finalize
**
*/
void
_scs_normal_exit(void)
{
    Debug(DBG_MPI2, "Calling wrap_MPI_Finalize from exit handler\n", "");
    wrap_MPI_Finalize();
}  /* end of _scs_normal_exit() */


/*
** =================================================================================================
** Support Function
** We need to make sure we use _scs_app_comm instead of MPI_COMM_WORLD
** And we need to generate and wait for the appropriate events.
*/
void
_scs_event_reduce(_scs_vtime_t t1, void *data, int dest, int len, int collective, MPI_Datatype dt)
{

_scs_vtime_t tmax= 0.0;
_scs_vtime_t tx, delay= 0.0;
int real_len;
int src;
long int pos= 0;
int ignore= 0;


    dest= 0;
    if (my_app_rank != dest)   {
        /* Write message data if necessary */
        real_len= _scs_bytelen(dt, len);
        if (_scs_msg_data_out && len > 0)   {
            pos= _scs_msg_data_write(data, real_len);
        }

	_scs_event_send(t1, len, dt, dest, ignore, COLL_TOKEN, collective, pos);
	_scs_set_lvtime(t1);	/* Set time back to beginning of collective */
    } else   {
	/* We're the receiver. */
	tmax= 0.0;
        /* FIXME: app_nnodes is wrong, this depends on the communicator size */
	for (src= 0; src < app_nnodes; src++)   {
	    if (src == dest)   {
		continue;
	    }
	    _scs_event_wait(&tx, &delay, COLL_TOKEN);
	    if (tx > tmax)   {
		tmax= tx;
	    }
	}

	_scs_set_lvtime(MAX(tmax + delay, t1));
    }

}  /* end of _scs_event_reduce() */



void
_scs_event_bcast(_scs_vtime_t t1, void *data, int root, int len, int collective, MPI_Datatype dt)
{

_scs_vtime_t tx, delay;
int real_len;
int dst;
long int pos= 0;
int ignore= 0;


    if (my_app_rank != root)   {
	/* We're a receiver. */
	_scs_event_wait(&tx, &delay, COLL_TOKEN);
	_scs_set_lvtime(MAX(tx + delay, t1));
    } else   {

        /* Write message data if necessary, but only on the root */
        real_len= _scs_bytelen(dt, len);
        if (_scs_msg_data_out && len > 0)   {
            pos= _scs_msg_data_write(data, real_len);
        }

	for (dst= 0; dst < app_nnodes; dst++)   {
	    if (dst == root)   {
		continue;
	    }
	    _scs_event_send(t1, len, dt, dst, ignore, COLL_TOKEN, collective, pos);
	}

	                        /* FIXME: This is probably not correct */
	_scs_set_lvtime(t1);	/* Set time back to beginning of collective */
    }

}  /* end of _scs_event_bcast() */



/*
** The combination for event_reduce() and event_bcast()
*/
void
_scs_event_exchange(_scs_vtime_t t1, void *sendbuf, void *recvbuf, int root, int len,
        int collective, MPI_Datatype dt)
{

_scs_vtime_t tmin, t3;
_scs_vtime_t tx, delay;
int real_len;
int src;
int dst;
long int pos= 0;
int ignore= 0;


    /* Reduce */
    if (my_app_rank != root)   {
        /* Write message data if necessary */
        real_len= _scs_bytelen(dt, len);
        if (_scs_msg_data_out && len > 0)   {
            if (sendbuf == MPI_IN_PLACE)   {
                /* Use the data from the receive buffer instead */
                pos= _scs_msg_data_write(recvbuf, real_len);
            } else   {
                pos= _scs_msg_data_write(sendbuf, real_len);
            }
        }

	/* Send to the root */
	_scs_event_send(t1, len, dt, root, ignore, COLL_TOKEN, collective, pos);

	/* Now wait for the message to come back */
	_scs_event_wait(&tx, &delay, COLL_TOKEN);
	_scs_set_lvtime(MAX(tx + delay, t1));

    } else   { /* I'm the root node. */

	/* Receive from everybody */
	tmin= 999999999.9;
	/* FIXME: This assumes all nodes participate in collective ops! */
	for (src= 0; src < app_nnodes; src++)   {
	    if (src == root)   {
		continue;
	    }
	    _scs_event_wait(&tx, &delay, COLL_TOKEN);
	    if (tx < tmin)   {
		tmin= tx;
	    }
	}
	t3= MAX(tmin + delay, t1);

	/* Send to everybody */

        /* Write message data if necessary */
        real_len= _scs_bytelen(dt, len);
        if (_scs_msg_data_out && len > 0)   {
            pos= _scs_msg_data_write(recvbuf, real_len);
        }

	for (dst= 0; dst < app_nnodes; dst++)   {
	    if (dst == root)   {
		continue;
	    }

	    _scs_event_send(t3, len, dt, dst, ignore, COLL_TOKEN, collective, pos);
	}

	if (app_nnodes <= 1)   {
	    /* Running on a single node. No comm cost */
	    t3= t1;
	}
	_scs_set_lvtime(t3);
    }

}  /* end of _scs_event_exchange() */
