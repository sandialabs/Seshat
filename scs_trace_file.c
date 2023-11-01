/*
** Copyright (2007) Sandia Corporation. Under the terms of Contract DE-AC04-94AL85000
** with Sandia Corporation, the U.S. Government retains certain rights in this software.
**
** Seshat
** $Id: scs_trace_file.c,v 1.9 2007/11/16 17:48:25 rolf Exp $
** Rolf Riesen (rolf@sandia.gov), May 2007, Sandia National Laboratories
** Write trace data to a file
*/
#include <stdio.h>
#include <stdlib.h>
#include "scs.h"


FILE *_scs_trace_out;	/* File pointer for trace file */



/*
** Our counters
*/
static long trace_entries= 0;



/*
** Return TRUE, if we are tracing. FALSE otherwise.
*/
int
_scs_init_trace(int nnodes)
{
    if (_scs_trace_out == NULL)   {
	/* Nothing to do */
	return FALSE;
    }

    OUT_TRACE("# ", "Seshat Trace file\n", "");
    OUT_TRACE("# ", "-----------------\n", "");
    OUT_TRACE("# ", "    Net Time:    Local time on simulator when event was written\n", "");
    OUT_TRACE("# ", "    Source:      Sender of message (MPI application rank)\n", "");
    OUT_TRACE("# ", "    Dest:        Destination of message (MPI application rank)\n", "");
    OUT_TRACE("# ", "    Virt Time:   Sender virtual time when message was sent\n", "");
    OUT_TRACE("# ", "    Delay:       Simulated time in network\n", "");
    OUT_TRACE("# ", "    Tag:         Tag used by sender\n", "");
    OUT_TRACE("# ", "    Function:    Point-to-point or type of collective\n", "");
    OUT_TRACE("# ", "    Lenght:      of message in bytes\n", "");
    OUT_TRACE("# ", "    DT:          MPI data type\n", "");
    OUT_TRACE("# ", "    MPI len:     Number of MPI units in message\n", "");
    OUT_TRACE("# ", "    Offset:      Offset into msg data file\n", "");
    OUT_TRACE("# ", "\n", "");
    OUT_TRACE("# ", "\n", "");
    OUT_TRACE("", "%s %d.%d\n", TRACE_S, TRACE_V_MAJOR, TRACE_V_MAJOR);
    OUT_TRACE("# ", "\n", "");
    OUT_TRACE("# ", "\n", "");
    OUT_TRACE("# ", "Event  Net Time  Source Dest   Virt Time    Delay     Tag        Function     Length       DT      MPI len      Offset\n", "");

    return TRUE;

}  /* end of _scs_init_trace() */



void
_scs_trace_event(_scs_event_t *e)
{

char *t_str;
char *c_str;


    switch (e->type)   {
	case E_NONE:		t_str= "NONE "; break;
	case E_DONE:		t_str= "DONE "; break;
	case E_SEND:		t_str= "SEND "; break;
	case E_RECV:		t_str= "RECV "; break;
	case E_POST:		t_str= "POST "; break;
	case E_MATCHED:		t_str= "MATCH"; break;
	case E_COLLECTIVE:	t_str= "COLL "; break;
	default:    		t_str= "???? "; break;
    }

    switch (e->collective)   {
	case MPI_Reduce_TAG:		c_str= "Reduce"; break;
	case MPI_Allreduce_TAG:		c_str= "Allreduce"; break;
	case MPI_Alltoall_TAG:		c_str= "Alltoall"; break;
	case MPI_Alltoallv_TAG:		c_str= "Alltoallv"; break;
	case MPI_Barrier_TAG:		c_str= "Barrier"; break;
	case MPI_Bcast_TAG:		c_str= "Bcast"; break;
	case MPI_Scatterv_TAG:		c_str= "Scatterv"; break;
	case MPI_Allgather_TAG:		c_str= "Allgather"; break;
	case MPI_Allgatherv_TAG:	c_str= "Allgatherv"; break;
	case MPI_Gatherv_TAG:		c_str= "Gatherv"; break;
	case MPI_Reduce_scatter_TAG:	c_str= "Reduce_scatter"; break;
	case MPI_Gather_TAG:		c_str= "Gather"; break;
	case MPI_Scan_TAG:		c_str= "Scan"; break;
	default:			c_str= "Pt2pt";
    }

    OUT_TRACE("", "%s %12.6f %5d %5d %12.6f %12.6f 0x%08x %12s %12d %4d %12d %12d\n",
	t_str, _scs_systime(), e->src, e->dest, e->time, e->delay, e->tag, c_str, e->len, e->dt,
        e->mpi_count, e->msg_data_pos);
    trace_entries++;

}  /* end of _scs_trace_event() */
