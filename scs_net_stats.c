/*
** Copyright (2007) Sandia Corporation. Under the terms of Contract DE-AC04-94AL85000
** with Sandia Corporation, the U.S. Government retains certain rights in this software.
**
** Seshat
** $Id: scs_net_stats.c,v 1.12 2008/05/09 17:28:16 rolf Exp $
** Rolf Riesen (rolf@sandia.gov), June 2006, Sandia National Laboratories
** Collect and print performance data
*/
#include <stdio.h>
#include <stdlib.h>
#include "scs.h"


FILE *_scs_mdd_out;      /* File pointer for message density info */
FILE *_scs_ddd_out;      /* File pointer for data density info */
FILE *_scs_col_out;      /* File pointer for collectives info */
FILE *_scs_p2p_out;      /* File pointer for point-to-point versus collectives info */
FILE *_scs_msize_out;    /* File pointer for message size info */



/*
** Our counters
*/
static long **msg_density= NULL;
static long long **dat_density= NULL;
static long cnt_MPI_Reduce;
static long cnt_MPI_Allreduce;
static long cnt_MPI_Alltoall;
static long cnt_MPI_Alltoallv;
static long cnt_MPI_Barrier;
static long cnt_MPI_Bcast;
static long cnt_MPI_Scatterv;
static long cnt_MPI_Allgather;
static long cnt_MPI_Allgatherv;
static long cnt_MPI_Gatherv;
static long cnt_MPI_Reduce_scatter;
static long cnt_MPI_Gather;
static long cnt_MPI_Scan;

static long msize16;
static long msize64;
static long msize256;
static long msize1k;
static long msize4k;
static long msize16k;
static long msize64k;
static long msize256k;
static long msize1M;
static long msize4M;
static long msize16M;
static long msize64M;
static long msize256M;
static long msizeHuge;



void
_scs_init_records(int nnodes)
{

int src, dst;
int i;


    /*
    ** Allocate and reset a message density array
    */

    /* Allocate the rows */
    msg_density=  malloc(nnodes * sizeof(long *));
    if (msg_density == NULL)   {
	Err("Out of memory!\n", "");
	_scs_abort();
    }

    /* Allocate the columns */
    for (i= 0; i < nnodes; i++)   {
	msg_density[i]=  malloc(nnodes * sizeof(long));
	if (msg_density[i] == NULL)   {
	    Err("Out of memory!\n", "");
	    _scs_abort();
	}
    }

    for (src= 0; src < nnodes; src++)   {
	for (dst= 0; dst < nnodes; dst++)   {
	    msg_density[src][dst]= 0;
	}
    }



    /*
    ** Allocate and reset a data density array
    */

    /* Allocate the rows */
    dat_density=  malloc(nnodes * sizeof(long long *));
    if (dat_density == NULL)   {
	Err("Out of memory!\n", "");
	_scs_abort();
    }

    /* Allocate the columns */
    for (i= 0; i < nnodes; i++)   {
	dat_density[i]=  malloc(nnodes * sizeof(long long));
	if (dat_density[i] == NULL)   {
	    Err("Out of memory!\n", "");
	    _scs_abort();
	}
    }

    for (src= 0; src < nnodes; src++)   {
	for (dst= 0; dst < nnodes; dst++)   {
	    dat_density[src][dst]= 0;
	}
    }



    cnt_MPI_Reduce= 0;
    cnt_MPI_Allreduce= 0;
    cnt_MPI_Alltoall= 0;
    cnt_MPI_Alltoallv= 0;
    cnt_MPI_Barrier= 0;
    cnt_MPI_Bcast= 0;
    cnt_MPI_Scatterv= 0;
    cnt_MPI_Allgather= 0;
    cnt_MPI_Allgatherv= 0;
    cnt_MPI_Gatherv= 0;
    cnt_MPI_Reduce_scatter= 0;
    cnt_MPI_Gather= 0;
    cnt_MPI_Scan= 0;

    msize16= 0;
    msize64= 0;
    msize256= 0;
    msize1k= 0;
    msize4k= 0;
    msize16k= 0;
    msize64k= 0;
    msize256k= 0;
    msize1M= 0;
    msize4M= 0;
    msize16M= 0;
    msize64M= 0;
    msize256M= 0;
    msizeHuge= 0;

}  /* end of _scs_init_record() */



void
_scs_record_event(_scs_event_t *e)
{


    /*
    ** For a reduce, I'll get one event for each message sent to the root
    ** node. For a bcast I'll get one event for each destination.
    */
    if (e->collective == MPI_Reduce_TAG)   {
	cnt_MPI_Reduce++;
    } else if (e->collective == MPI_Allreduce_TAG)   {
	cnt_MPI_Allreduce++;
    } else if (e->collective == MPI_Alltoall_TAG)   {
	cnt_MPI_Alltoall++;
    } else if (e->collective == MPI_Alltoallv_TAG)   {
	cnt_MPI_Alltoallv++;
    } else if (e->collective == MPI_Barrier_TAG)   {
	cnt_MPI_Barrier++;
    } else if (e->collective == MPI_Bcast_TAG)   {
	cnt_MPI_Bcast++;
    } else if (e->collective == MPI_Scatterv_TAG)   {
	cnt_MPI_Scatterv++;
    } else if (e->collective == MPI_Allgather_TAG)   {
	cnt_MPI_Allgather++;
    } else if (e->collective == MPI_Allgatherv_TAG)   {
	cnt_MPI_Allgatherv++;
    } else if (e->collective == MPI_Gatherv_TAG)   {
	cnt_MPI_Gatherv++;
    } else if (e->collective == MPI_Reduce_scatter_TAG)   {
	cnt_MPI_Reduce_scatter++;
    } else if (e->collective == MPI_Gather_TAG)   {
	cnt_MPI_Gather++;
    } else if (e->collective == MPI_Scan_TAG)   {
	cnt_MPI_Scan++;
    }

    msg_density[e->src][e->dest]++;
    dat_density[e->src][e->dest]+= e->len;

    if (e->len <= 16)   {
	msize16++;
    } else if (e->len <= 64)   {
	msize64++;
    } else if (e->len <= 256)   {
	msize256++;
    } else if (e->len <= 1024)   {
	msize1k++;
    } else if (e->len <= 4 * 1024)   {
	msize4k++;
    } else if (e->len <= 16 * 1024)   {
	msize16k++;
    } else if (e->len <= 64 * 1024)   {
	msize64k++;
    } else if (e->len <= 256 * 1024)   {
	msize256k++;
    } else if (e->len <= 1024 * 1024)   {
	msize1M++;
    } else if (e->len <= 4 * 1024 * 1024)   {
	msize4M++;
    } else if (e->len <= 16 * 1024 * 1024)   {
	msize16M++;
    } else if (e->len <= 64 * 1024 * 1024)   {
	msize64M++;
    } else if (e->len <= 256 * 1024 * 1024)   {
	msize256M++;
    } else    {
	msizeHuge++;
    }

}  /* end of _scs_record_event() */



/*
** nnodes is the number of application nodes
*/
void
_scs_print_report(int nnodes)
{

int src, dst;
long tot_msg;
long tot_collectives;
long pt2pt;


    if ((msg_density == NULL) || (nnodes <= 0))   {
	return;
    }


    OUT_MDD("# ", "Message Density Array\n", "");
    OUT_MDD("# ", "--->         Destination\n", "");
    tot_msg= 0;
    for (src= 0; src < nnodes; src++)   {
	OUT_MDD("", "", "");
	for (dst= 0; dst < nnodes; dst++)   {
	    OUT_MDD("", "%6ld ", msg_density[src][dst]);
	    tot_msg += msg_density[src][dst];
	}
	OUT_MDD("", "\n", "");
    }
    OUT_MDD("", "\n", "");

    OUT_DDD("#", "Data Density Array\n", "");
    OUT_DDD("#", "  in mega (10^6) Bytes\n", "");
    for (src= 0; src < nnodes; src++)   {
	OUT_DDD("", "", "");
	for (dst= 0; dst < nnodes; dst++)   {
	    OUT_DDD("", "%9.3f ", dat_density[src][dst] / 1000000.0);
	}
	OUT_DDD("\n", "", "");
    }


    /*
    ** Process this info
    ** We see one event from each node (except the roo) for every reduce operation
    ** We see one event from the root node for each other node for bcast ops.
    ** Most collectives do a reduce first, and then a bcast.
    */
    tot_collectives= cnt_MPI_Reduce + cnt_MPI_Allreduce + cnt_MPI_Alltoall +
	cnt_MPI_Alltoallv + cnt_MPI_Barrier + cnt_MPI_Scatterv + cnt_MPI_Allgather +
	cnt_MPI_Allgatherv + cnt_MPI_Gatherv + cnt_MPI_Reduce_scatter + cnt_MPI_Gather +
	cnt_MPI_Scan;
    pt2pt= tot_msg - tot_collectives;

    cnt_MPI_Reduce= cnt_MPI_Reduce / nnodes;
    cnt_MPI_Allreduce= (cnt_MPI_Allreduce / nnodes) / 2;
    cnt_MPI_Alltoall= (cnt_MPI_Alltoall / nnodes) / 2;
    cnt_MPI_Alltoallv= (cnt_MPI_Alltoallv / nnodes) / 2;
    cnt_MPI_Barrier= (cnt_MPI_Barrier / nnodes) / 2;
    cnt_MPI_Scatterv= (cnt_MPI_Scatterv / nnodes) / 2;
    cnt_MPI_Allgather= (cnt_MPI_Allgather / nnodes) / 2;
    cnt_MPI_Allgatherv= (cnt_MPI_Allgatherv / nnodes) / 2;
    cnt_MPI_Gatherv= (cnt_MPI_Gatherv / nnodes) / 2;
    cnt_MPI_Reduce_scatter= (cnt_MPI_Reduce_scatter / nnodes) / 2;
    cnt_MPI_Gather= (cnt_MPI_Gather / nnodes) / 2;
    cnt_MPI_Scan= (cnt_MPI_Scan / nnodes) / 2;


    OUT_COL("#", "Number of collectives per node\n", "");
    OUT_COL("#", "Reduce   Allreduce   Alltoall   Alltoallv   Barrier   Bcast   Scatterv   Allgather   Allgatherv Gatherv Reduce_scatter Gather Scan\n", "");
    OUT_COL("", "%8ld   %9ld   %8ld   %9ld   %7ld   %7ld  %7ld   %7ld   %7ld   %7ld   %7ld   %7ld   %7ld\n",
	cnt_MPI_Reduce, cnt_MPI_Allreduce, cnt_MPI_Alltoall, cnt_MPI_Alltoallv,
	cnt_MPI_Barrier, cnt_MPI_Bcast, cnt_MPI_Scatterv, cnt_MPI_Allgather,
	cnt_MPI_Allgatherv, cnt_MPI_Gatherv, cnt_MPI_Reduce_scatter, cnt_MPI_Gather,
	cnt_MPI_Scan);

    OUT_P2P("#", "Tot Coll   Tot Msgs   Pt2pt   Pt2pt/node   Ratio p/c\n", "");
    OUT_P2P("", "%10ld   %8ld  %6ld   %10ld   %2.0f %2.0f\n",
	cnt_MPI_Reduce + cnt_MPI_Allreduce + cnt_MPI_Alltoall + cnt_MPI_Alltoallv +
	    cnt_MPI_Barrier + cnt_MPI_Scatterv + cnt_MPI_Allgather + cnt_MPI_Allgatherv + cnt_MPI_Gatherv +
		cnt_MPI_Reduce_scatter + cnt_MPI_Gather + cnt_MPI_Scan,
	tot_msg, pt2pt, pt2pt / nnodes, 100.0 / tot_msg * pt2pt, 100.0 / tot_msg * tot_collectives);

    OUT_MSIZE("#", "Message Size Distribution\n", "");
    OUT_MSIZE("#", "<= 16   <= 64   <= 256   <= 1k   <= 4k   <= 16k   <= 64k   <= 256k   <= 1M   <= 4M   <= 16M   <= 64M   <= 256M   Huge\n", "");
    OUT_MSIZE("", "%8ld %8ld %8ld %8ld %8ld %8ld %8ld %8ld %8ld %8ld %8ld %8ld %8ld %8ld\n",
	msize16,
	msize64,
	msize256,
	msize1k,
	msize4k,
	msize16k,
	msize64k,
	msize256k,
	msize1M,
	msize4M,
	msize16M,
	msize64M,
	msize256M,
	msizeHuge);

    free(msg_density);
    free(dat_density);

}  /* end of _scs_print_report() */
