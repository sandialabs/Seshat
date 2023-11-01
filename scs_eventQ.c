/*
** Copyright (2007) Sandia Corporation. Under the terms of Contract DE-AC04-94AL85000
** with Sandia Corporation, the U.S. Government retains certain rights in this software.
**
** Seshat
** $Id: scs_eventQ.c,v 1.6 2007/11/20 23:51:13 rolf Exp $
** Rolf Riesen (rolf@sandia.gov), September 2007, Sandia National Laboratories
**
** Functions to manage event queues
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scs.h"




/*
** Create a new event
*/
_scs_event_t *
_scs_create_event(void)
{

_scs_event_t *tmp;


    tmp= (_scs_event_t *)malloc(sizeof(_scs_event_t));
    if (tmp == NULL)   {
        Err("Out of memory!\n", "");
        _scs_abort();
    }

    memset(tmp, 0, sizeof(_scs_event_t));
    tmp->next= NULL;

    return tmp;

}  /* end of _scs_create_event() */



/*
** Free an event
*/
void
_scs_delete_event(_scs_event_t *e)
{
    if (e->next != NULL)   {
        /* This indicates a bug */
        Err("Event still points to something!\n", "");
        _scs_abort();
    }

    memset(e, 0, sizeof(_scs_event_t));
    free(e);

}  /* end of _scs_delete_event() */



/*
** Create a new, empty queue. User is expected to call _scs_delete_eventQ
** to free data structures and delete it.
*/
void
_scs_create_eventQ(_scs_eventQ_t **queue)
{
    *queue= (_scs_eventQ_t *)malloc(sizeof(_scs_eventQ_t));
    if (*queue == NULL)   {
        Err("Out of memory!\n", "");
        _scs_abort();
    }

    (*queue)->head= NULL;
    (*queue)->tail= NULL;

}  /* end of _scs_create_eventQ() */



/*
** Free the resources used by a queue. The queue must be empty!
*/
void
_scs_delete_eventQ(_scs_eventQ_t *queue)
{
    if ((queue->head != NULL) || (queue->tail != NULL))   {
        Err("Queue not empty!\n", "");
        _scs_abort();
    }

    free(queue);

}  /* end of _scs_delete_eventQ() */



/*
** Push an event at the end of a queue
*/
void
_scs_push_eventQ(_scs_eventQ_t *queue, _scs_event_t *event)
{
    if (queue->tail)   {
        /* Append the entry at the tail */
        queue->tail->next= event;
        event->next= NULL;
        queue->tail= event;
    } else   {
        /* Queue must be empty */
        queue->head= event;
        event->next= NULL;
        queue->tail= event;
    }

}  /* end of _scs_push_eventQ() */



/*
** Find the first (oldest) event that matches, unlink it, and return it
*/
_scs_event_type_t
_scs_match_eventQ(_scs_eventQ_t *queue, _scs_event_t **event, int src, int dest, int tag, unsigned int token)
{

_scs_event_type_t type;
_scs_event_t *current;
_scs_event_t *prev;


    type= E_NONE;
    current= queue->head;
    prev= NULL;

    while (current)    {
        if (token)   {
            /* Only search for the token */
            if (current->token == token)   {

                Debug(DBG_EVENT, "Matched an event by its token 0x%08x (src %d, dest %d, tag 0x%x)\n", token,
                    current->src, current->dest, current->tag);
                *event= current;

                if (prev)   {
                    /* current is not the first entry in the list */
                    prev->next= current->next;
                    if (current == queue->tail)   {
                        /* current is the last entry in the list */
                        queue->tail= prev;
                    }
                } else   {
                    /* current is the first entry in the list */
                    queue->head= current->next;
                    if (current == queue->tail)   {
                        /* current is also the last entry in the list */
                        queue->tail= NULL;
                    }
                }

                (*event)->next= NULL;
                return (*event)->type;
            }

        } else   {

            if ((current->dest == dest) &&
                /*
                ** The posted receive in the queue could MPI_ANY_SOURCE,
                ** or the receive we are comparing to could be MPI_ANY_SOURCE
                */
                ((current->src == src) || (current->src == MPI_ANY_SOURCE) || (src == MPI_ANY_SOURCE)) &&
                ((current->tag == tag) || (current->tag == MPI_ANY_TAG) || (tag == MPI_ANY_TAG)))   {

                Debug(DBG_EVENT, "Matched an event using dest %d, src %d, and tag 0x%x (Token 0x%08x)\n", dest, src, tag, current->token);
                *event= current;

                if (prev)   {
                    /* current is not the first entry in the list */
                    prev->next= current->next;
                    if (current == queue->tail)   {
                        /* current is the last entry in the list */
                        queue->tail= prev;
                    }
                } else   {
                    /* current is the first entry in the list */
                    queue->head= current->next;
                    if (current == queue->tail)   {
                        /* current is also the last entry in the list */
                        queue->tail= NULL;
                    }
                }

                (*event)->next= NULL;
                return (*event)->type;
            }
        }

        prev= current;
        current= current->next;
    }

    return type;

}  /* end of _scs_match_eventQ() */



/*
** Unlink and return the first (oldest) event on the queue
*/
_scs_event_type_t
_scs_pop_eventQ(_scs_eventQ_t *queue, _scs_event_t **event)
{

    if (queue->head)   {
        *event= queue->head;
        queue->head= queue->head->next;
        if (queue->tail == *event)   {
            queue->tail= NULL;
        }
        (*event)->next= NULL;
        return (*event)->type;
    }

    return E_NONE;

}  /* end of _scs_pop_eventQ() */
