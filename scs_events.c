/*
** Copyright (2007) Sandia Corporation. Under the terms of Contract DE-AC04-94AL85000
** with Sandia Corporation, the U.S. Government retains certain rights in this software.
**
** Seshat
** $Id: scs_events.c,v 1.40 2008/01/18 17:05:27 rolf Exp $
** Rolf Riesen (rolf@sandia.gov), June 2006, Sandia National Laboratories
**
** Function to deal with events
*/

#include <stdio.h>
#include <stdlib.h>
#include "scs.h"


/*
** Local Globals
*/
_scs_eventQ_t *NodeQ;



/*
** Local functions
*/
static void _scs_event_to_sim(_scs_event_t *e);


/*
** Initialize the node side event handling system
*/
void
_scs_node_event_init(void)
{

    _scs_create_eventQ(&NodeQ);

}  /* end of _scs_event_init() */



/*
** Generate a system-wide unique token
*/
unsigned int
_scs_gen_token(void)
{

static int last= 0;
unsigned int token;


    last= (last + 1) & 0x0000ffff;

    /* Token 0 should never be used! */
    if (last == NO_TOKEN)   {
        last++;
    }

    /* Token 1 is reserved for collectives! */
    if (last == COLL_TOKEN)   {
        last++;
    }

    token= (_scs_rank << 16) + last;
    Debug(DBG_EVENT, "Created token 0x%08x\n", token);
    return token;

}  /* end of _scs_gen_token() */



/*
** Generate a send event.
** A node sends this event to the network sim when it sends an MPI message
*/
void
_scs_event_send(_scs_vtime_t t, int len, MPI_Datatype dt, int dest, int tag, unsigned int token,
        int collective, long int pos)
{

_scs_event_t *e;


    e= _scs_create_event();
    if (collective)   {
        e->type= E_COLLECTIVE;
    } else   {
        e->type= E_SEND;
    }

    e->time= t;
    e->dest= dest;
    e->src= _scs_rank;
    e->dt= dt;
    e->mpi_count= len;
    e->len= _scs_bytelen(dt, len);
    e->tag= tag;
    e->token= token;
    e->collective= collective;
    e->msg_data_pos= pos;
    e->next= NULL;

    Debug(DBG_EVENT, "%10.6f   Token 0x%08x Send %d bytes to %d\n", t, token, e->len, dest);

    _scs_event_to_sim(e);

}  /* end of _scs_event_send() */



/*
** Generate a done event.
** Nodes send this to the simulator when they leave MPI_Finalize
*/
void
_scs_event_done(void)
{

_scs_event_t *e;


    Debug(DBG_EVENT, "%10.6f   DONE event\n", _scs_get_lvtime());

    e= _scs_create_event();
    e->type= E_DONE;
    e->src= _scs_rank;
    e->next= NULL;

    _scs_event_to_sim(e);

}  /* end of _scs_event_done() */



/*
** Generate a post event
** A node sends this event to the network sim when it posts an MPI recv
*/
void
_scs_event_post(_scs_vtime_t t, int src, int tag, unsigned int token)
{

_scs_event_t *e;


    e= _scs_create_event();
    e->time= t;
    e->type= E_POST;
    e->dest= _scs_rank;
    e->src= src;
    e->tag= tag;
    e->token= token;
    e->len= -1; /* Posted length is not used */
    e->next= NULL;

    Debug(DBG_EVENT, "%10.6f   Posted src %d, tag %d, token 0x%08x\n", t, src, tag, token);

    _scs_event_to_sim(e);

}  /* end of _scs_event_post() */



/*
** Wait (block) for an event
** If token != 0, then we look for an event with that token
*/
void
_scs_event_wait(_scs_vtime_t *t, _scs_vtime_t *delay, unsigned int token)
{

_scs_event_t *e;
_scs_event_type_t type;
int done= 0;
int ignore= 0;


    Debug(DBG_EMU, "Waiting for token 0x%08x\n", token);

    /* We need to wait for the event; this is a blocking call. */
    while (!done)   {

        /* FIXME: _scs_rank is not right. It should be my_node within the right MPI context */
        check_for_remote_events(NodeQ);
	type= _scs_match_eventQ(NodeQ, &e, ignore, ignore, ignore, token);
	switch (type)    {
	    case E_RECV:
		    Debug(DBG_EMU, "Node %d got a recv event! Token 0x%08x\n", _scs_rank, e->token);
		    done= TRUE;
		    *t= e->time;
		    *delay= e->delay;
		    break;

	    case E_SEND:
		    /* A node sim should not get this event type! */
		    Err("Node emu [%d/%d] Got a send event!\n", _scs_rank, _scs_nnodes);
		    _scs_abort();
		    break;

            case E_POST:
                    Err("Got E_POST on a client node!\n", "");
                    _scs_abort();
		    break;

            case E_MATCHED:
		    Debug(DBG_EMU, "Node %d got a matched event! Token 0x%08x\n", _scs_rank, e->token);
		    done= TRUE;
		    *t= e->time;
		    *delay= e->delay;
		    break;

            case E_COLLECTIVE:
                    Err("Got E_COLLECTIVE on a client node!\n", "");
                    _scs_abort();
		    break;

	    case E_DONE:
		    /* This should probably not happen... */
                    Err("Got E_DONE on a client node!\n", "");
                    _scs_abort();
		    break;

	    case E_NONE:
		    /* No matching event in the queue */
		    break;
	}
    }

}  /* end of _scs_event_wait() */



/*
** Forward an event from the network sim to a node
*/
void
_scs_event_to_node(_scs_event_t *e, int where)
{
    Debug(DBG_EVENT, "%10.6f   Forwarding event type %d to node %d. Token 0x%08x\n", _scs_get_lvtime(),
	e->type, where, e->token);
    _scs_MPI_Send(e, sizeof(_scs_event_t), MPI_CHAR, where, NEW_EVENT, _scs_comm_world);
    _scs_delete_event(e);

}  /* end of _scs_event_to_node() */




/*
** Check for remote events and push them (it) onto the provided queue
*/
void
check_for_remote_events(_scs_eventQ_t *queue)
{

MPI_Status status;
_scs_event_t *e;
int flag;


    while (TRUE)   {
        flag= FALSE;
        /* FIXME: Preposting a receive and using MPI_Test() might be more efficient. */
        _scs_MPI_Iprobe(MPI_ANY_SOURCE, NEW_EVENT, _scs_comm_world, &flag, &status);

        if (!flag)   {
            /* Nothing here */
            break;
        }

        e= _scs_create_event();
        _scs_MPI_Recv(e, sizeof(_scs_event_t), MPI_CHAR, MPI_ANY_SOURCE, NEW_EVENT, _scs_comm_world, &status);
        Debug(DBG_EVENT, "%10.6f   Received event type %d from %d, token 0x%08x\n", _scs_get_lvtime(),
            e->type, status.MPI_SOURCE, e->token);
        _scs_push_eventQ(queue, e);
    }

}  /* end of check_for_remote_events() */



/*
** -------------------------------------------------------------------------------
** Local (Private) functions 
*/


/*
** Send an event to the network simulator
*/
static void
_scs_event_to_sim(_scs_event_t *e)
{

int where;


    /*
    ** FIXME: For now we send to the well known network simulator node.
    ** FIXME: This has to change in the future...
    */
    where= _scs_net_sim_node;

    Debug(DBG_EVENT, "%10.6f   Inject event type %d into the network to netsim, token 0x%08x\n", _scs_get_lvtime(),
	e->type, e->token);

    /* The simulator doesn't need to send an event to itself... */
    if (where != _scs_rank)   {
	_scs_MPI_Send(e, sizeof(_scs_event_t), MPI_CHAR, where, NEW_EVENT, _scs_comm_world);
        _scs_delete_event(e);
    } else   {
	Err("Why would this happen?\n", "");
    }

}  /* end of _scs_event_to_sim() */
