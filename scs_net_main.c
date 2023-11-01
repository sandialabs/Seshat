/*
** Copyright (2007) Sandia Corporation. Under the terms of Contract DE-AC04-94AL85000
** with Sandia Corporation, the U.S. Government retains certain rights in this software.
**
** Seshat
** $Id: scs_net_main.c,v 1.34 2008/05/11 18:27:46 rolf Exp $
** main() of the simple network simulator
** Rolf Riesen (rolf@sandia.gov), June 2006, Sandia National Laboratories
** It initializes the statistics functions. Then it enters a loop and
** services events.
*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include "scs.h"


void
_scs_main_net(void)
{

int app_nnodes;
int client_cnt;

int src, len;
unsigned int token;
_scs_event_t *send_event;
_scs_event_t *post_event;
_scs_event_t *e;

_scs_event_type_t type;
_scs_eventQ_t *incomingQ;
_scs_eventQ_t *sentQ;
_scs_eventQ_t *postedQ;

double lat_factor, bw_factor;
double coll_factor;
int same_board;
int report;
int tracing;
long int sim_sleep;


    /* FIXME: This asumes the network sim runs on 1 node! */
    client_cnt= _scs_nnodes - 1;	/* Number of app nodes still talking to us */
    app_nnodes= _scs_nnodes - 1;	/* The number of nodes the app is running on */


    lat_factor= _scs_config.net_lat_factor;
    bw_factor= _scs_config.net_bw_factor;
    coll_factor= _scs_config.net_coll_factor;
    report= _scs_config.net_reporting;
    sim_sleep= _scs_config.sim_sleep;

    Debug(DBG_CONFIG, "Using lat  factor %f\n", lat_factor);
    Debug(DBG_CONFIG, "      bw   factor %f\n", bw_factor);
    Debug(DBG_CONFIG, "      coll factor %f\n", coll_factor);
    Debug(DBG_CONFIG, "      cpu  factor %f\n", _scs_config.net_cpu_factor);

    if (report)
	_scs_init_records(app_nnodes);

    tracing= _scs_init_trace(app_nnodes);

    Debug(DBG_GENERAL, "Simulating \"%s\"\n", simsystem);

    /* Create the queues we need */
    _scs_create_eventQ(&incomingQ);
    _scs_create_eventQ(&postedQ);
    _scs_create_eventQ(&sentQ);

    while (client_cnt > 0)   {
        check_for_remote_events(incomingQ);
	type= _scs_pop_eventQ(incomingQ, &e);
	switch (type)    {
	    case E_MATCHED:
                    Err("Network sim should not get DELIVERED events!\n", "");
                    _scs_delete_event(e);
                    _scs_abort();
		    break;

	    case E_RECV:
                    Err("Network sim should not get RECEIVE events!\n", "");
                    _scs_delete_event(e);
                    _scs_abort();
		    break;

	    case E_POST:
                    /*
                    ** A node posted a receive. See if we already have a matching send for it.
                    ** If not, we enqueue it in the posted receive queue.
                    */
		    Debug(DBG_NET, "Net sim got a post event from %d! Len %d, Token 0x%08x\n", e->dest, e->len, e->token);

                    if (_scs_match_eventQ(sentQ, &send_event, e->src, e->dest, e->tag, NO_TOKEN) != E_NONE)   {
                        /* A matching send is already here */

                        /* Forward the send event to the receiver */
                        token= send_event->token;
                        src= send_event->src;
                        len= send_event->len;
                        send_event->token= e->token;
                        send_event->type= E_RECV;
                        Debug(DBG_NET, "Forwarding recv event to %d! Token 0x%08x\n", send_event->dest, send_event->token);
                        _scs_event_to_node(send_event, send_event->dest);

                        /* For long messages, send the post info to the sender */
                        if (len >= _scs_config.mpi_long_proto)   {
                            e->token= token;
                            e->type= E_MATCHED;
                            _scs_event_to_node(e, src);
                            Debug(DBG_NET, "Forwarding matched event from sent Q to %d! Token 0x%08x. %d >= %d bytes\n", src, token, len, _scs_config.mpi_long_proto);
                        }

                    } else   {
                        /* Enqueue this post for later */
                        _scs_push_eventQ(postedQ, e);
                        Debug(DBG_NET, "Pushing into recv queue Token 0x%08x\n", e->token);
                    }
		    break;

	    case E_SEND:
		    Debug(DBG_NET, "Net sim got a send event from %d! Len %d, Token 0x%08x\n", e->src, e->len, e->token);
		    /*if (e.src < e.dest)   {
			if (((e.src + 1) == e.dest) && ((e.src & 0x01) == 0))   {
			    same_board= TRUE;
			} else   {
			    same_board= FALSE;
			}
		    } else if (e.src == e.dest)   {
			same_board= TRUE;
		    } else   {
			if (((e.dest + 1) == e.src) && ((e.dest & 0x01) == 0))   {
			    same_board= TRUE;
			} else   {
			    same_board= FALSE;
			}
		    }*/

                    /* FIXME: Assuming all communications are offboard*/
                    same_board= FALSE;

                    e->delay= _scs_model_ptop(e->len, bw_factor, lat_factor, same_board);

		    if (report)   {
			_scs_record_event(e);
		    }

		    if (tracing)   {
			_scs_trace_event(e);
		    }

		    if (sim_sleep)   {
			/* Test the simulator by delaying it a little while */
			struct timespec req, rem;
			req.tv_sec= sim_sleep / 1000000000;
			req.tv_nsec= sim_sleep % 1000000000;

			while (nanosleep(&req, &rem) != 0)   {
			    if (errno == EINTR)   {
				req.tv_sec= rem.tv_sec;
				req.tv_nsec= rem.tv_nsec;
				continue;
			    } else   {
				break;
			    }
			}
		    }

                    /*
                    ** A node sent a message. See if we already have a posted receive for it.
                    ** If not, we enqueue it in the sent queue.
                    */
                    if (_scs_match_eventQ(postedQ, &post_event, e->src, e->dest, e->tag, NO_TOKEN) != E_NONE)   {
                        /* A matching post is already here */

                        /* Forward the send event to the receiver */
                        token= e->token;
                        src= e->src;
                        len= e->len;
                        e->token= post_event->token;
                        e->type= E_RECV;
                        Debug(DBG_NET, "Forwarding recv event to %d! Token 0x%08x\n", e->dest, e->token);
                        _scs_event_to_node(e, e->dest);

                        /* For long messages, send the post info to the sender */
                        if (len >= _scs_config.mpi_long_proto)   {
                            post_event->token= token;
                            post_event->type= E_MATCHED;
                            _scs_event_to_node(post_event, src);
                            Debug(DBG_NET, "Forwarding matched event from posted Q to %d! Token 0x%08x. %d >= %d bytes\n", src, token, len, _scs_config.mpi_long_proto);
                        }

                    } else   {
                        /* Enqueue this send for later */
                        _scs_push_eventQ(sentQ, e);
                        Debug(DBG_NET, "Pushing onto sent queue, Token 0x%08x\n", e->token);
                    }
		    break;

	    case E_COLLECTIVE:
		    Debug(DBG_NET, "Net sim %d/%d [%d/%d] got a collective (0x%x) event! Token 0x%08x, Forwarding to [%d/%d]\n",
			_scs_net_sim_node, app_nnodes, _scs_rank, _scs_nnodes, e->collective, e->token, e->dest, _scs_nnodes);

		    if (e->collective == MPI_Reduce_TAG)   {
			e->delay= _scs_model_col(e->len, app_nnodes, coll_factor);
		    } else if (e->collective == MPI_Allreduce_TAG)   {
			e->delay= _scs_model_col(e->len, app_nnodes, coll_factor);
		    } else if (e->collective == MPI_Alltoall_TAG)   {
			e->delay= _scs_model_col(e->len, app_nnodes, coll_factor);
		    } else if (e->collective == MPI_Alltoallv_TAG)   {
			e->delay= _scs_model_col(e->len, app_nnodes, coll_factor);
		    } else if (e->collective == MPI_Barrier_TAG)   {
			e->delay= _scs_model_col(e->len, app_nnodes, coll_factor);
		    } else if (e->collective == MPI_Bcast_TAG)   {
			e->delay= _scs_model_col(e->len, app_nnodes, coll_factor);
		    } else if (e->collective == MPI_Scatterv_TAG)   {
			e->delay= _scs_model_col(e->len, app_nnodes, coll_factor);
		    } else if (e->collective == MPI_Allgather_TAG)   {
			e->delay= _scs_model_col(e->len, app_nnodes, coll_factor);
		    } else if (e->collective == MPI_Allgatherv_TAG)   {
			e->delay= _scs_model_col(e->len, app_nnodes, coll_factor);
		    } else if (e->collective == MPI_Gatherv_TAG)   {
			e->delay= _scs_model_col(e->len, app_nnodes, coll_factor);
		    } else if (e->collective == MPI_Reduce_scatter_TAG)   {
			e->delay= _scs_model_col(e->len, app_nnodes, coll_factor);
		    } else if (e->collective == MPI_Gather_TAG)   {
			e->delay= _scs_model_col(e->len, app_nnodes, coll_factor);
		    } else if (e->collective == MPI_Scan_TAG)   {
			e->delay= _scs_model_col(e->len, app_nnodes, coll_factor);
		    } else   {
			Err("Unknown collective %d\n", e->collective);
		    }

		    if (report)   {
			_scs_record_event(e);
		    }

		    if (tracing)   {
			_scs_trace_event(e);
		    }

		    if (sim_sleep)   {
			/* Test the simulator by delaying it a little while */
			struct timespec req, rem;
			req.tv_sec= sim_sleep / 1000000000;
			req.tv_nsec= sim_sleep % 1000000000;

			while (nanosleep(&req, &rem) != 0)   {
			    if (errno == EINTR)   {
				req.tv_sec= rem.tv_sec;
				req.tv_nsec= rem.tv_nsec;
				continue;
			    } else   {
				break;
			    }
			}
		    }

		    e->type= E_RECV;
		    _scs_event_to_node(e, e->dest);
		    break;

	    case E_DONE:
		    client_cnt--;
		    Debug(DBG_NET, "Net sim %d/%d [%d/%d] got a done event. client_cnt is %d\n",
                        _scs_net_sim_node, app_nnodes, _scs_rank, _scs_nnodes, client_cnt);
                    _scs_delete_event(e);
		    break;

	    case E_NONE:
		    /* We get these when there are no events available */
		    break;

	    default:
		    Debug(DBG_NET, "Net sim %d/%d [%d/%d] got an unknown event %d\n", _scs_net_sim_node,
			app_nnodes, _scs_rank, _scs_nnodes, e->type);
                    _scs_delete_event(e);
	}

    }

    Debug(DBG_NET, "Net sim %d/%d [%d/%d] exiting\n", _scs_net_sim_node, app_nnodes, _scs_rank, _scs_nnodes);
    if (report) _scs_print_report(app_nnodes);

}  /* end of _scs_main_net() */
