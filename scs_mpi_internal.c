/*
** Copyright (2007) Sandia Corporation. Under the terms of Contract DE-AC04-94AL85000
** with Sandia Corporation, the U.S. Government retains certain rights in this software.
**
** Seshat
** $Id: scs_mpi_internal.c,v 1.15 2007/10/22 23:12:50 rolf Exp $
** Rolf Riesen (rolf@sandia.gov), June 2006, Sandia National Laboratories
**
** When SCS needs to use MPI internally, it needs to bypass the wrapped
** MPI functions that change the communicator and check for call back
** functions. The functions in this file do that.
*/

#include <stdio.h>
#include <mpi.h>
#include "scs.h"



/*
** The functions
*/
static int
_scs_MPI_Type_size(MPI_Datatype datatype, int *size)
{
    return PMPI_Type_size(datatype, size);
}  /* end of _scs_MPI_Type_size() */


__inline__ int
_scs_bytelen(MPI_Datatype dt, int len)
{

int rc;
int dt_size;


    /*
    fprintf(stderr, ">>> _scs_bytelen() dt %p, len %d\n", dt, len);
    if (dt == NULL) {int *bad; bad= (int *)0x0; rc= *bad; return rc;}
    */
    rc= _scs_MPI_Type_size(dt, &dt_size);
    if (rc != MPI_SUCCESS)   {
        Warn("Warning!!! _scs_MPI_Type_size() is wrong! dt_size %d\n", dt_size);
    }

    return len * dt_size;

}  /* end of _scs_bytelen() */


int
_scs_MPI_Init(int *argc, char **argv[])
{
    return PMPI_Init(argc, argv);
}  /* end of _scs_MPI_Init() */


int
_scs_MPI_Finalize(void)
{

int rc;


    Debug(DBG_MPI_INTERNAL, "Calling PMPI_Finalize\n", "");
    rc= PMPI_Finalize();
    Debug(DBG_MPI_INTERNAL, "Back from PMPI_Finalize\n", "");
    return rc;

}  /* end of _scs_MPI_Finalize() */


int
_scs_MPI_Send(void *data, int len, MPI_Datatype dt, int dest, int tag, MPI_Comm comm)
{
    Debug(DBG_MPI_INTERNAL, "dest %d, len %d, tag 0x%x, ...\n", dest, len, tag);
    return PMPI_Send(data, len, dt, dest, tag, comm);
}  /* end of _scs_MPI_Send() */


int
_scs_MPI_Recv(void *data, int len, MPI_Datatype dt, int src, int tag, MPI_Comm comm, MPI_Status *stat)
{
    Debug(DBG_MPI_INTERNAL, "src %d, len %d, tag 0x%x, ...\n", src, len, tag);
    return PMPI_Recv(data, len, dt, src, tag, comm, stat);
}  /* end of _scs_MPI_Recv() */


int
_scs_MPI_Iprobe(int src, int tag, MPI_Comm comm, int *flag, MPI_Status *stat)
{
    return PMPI_Iprobe(src, tag, comm, flag, stat);
}  /* end of _scs_MPI_Iprobe() */


int
_scs_MPI_Bcast(void *buffer, int count, MPI_Datatype dt, int root, MPI_Comm comm)
{
    Debug(DBG_MPI_INTERNAL, "root %d, len %d, ...\n", root, count);
    return PMPI_Bcast(buffer, count, dt, root, comm);
}  /* end of _scs_MPI_Bcast() */


int
_scs_MPI_Barrier(MPI_Comm comm)
{
    Debug(DBG_MPI_INTERNAL, "Calling PMPI_Barrier\n", "");
    return PMPI_Barrier(comm);
}  /* end of _scs_MPI_Barrier() */


double
_scs_MPI_Wtime(void)
{
    return _scs_systime();
}  /* end of _scs_MPI_Wtime() */


int
_scs_MPI_Comm_dup(MPI_Comm d1, MPI_Comm *d2)
{
    return PMPI_Comm_dup(d1, d2);
}  /* end of _scs_MPI_Comm_dup() */


int
_scs_MPI_Comm_size(MPI_Comm comm, int *size)
{
    return PMPI_Comm_size(comm, size);
}  /* end of _scs_MPI_Comm_size() */


int
_scs_MPI_Comm_rank(MPI_Comm comm, int *rank)
{
    return PMPI_Comm_rank(comm, rank);
}  /* end of _scs_MPI_Comm_rank() */

int
_scs_MPI_Comm_group(MPI_Comm d1, MPI_Group *d2)
{
    return PMPI_Comm_group(d1, d2);
}  /* end of _scs_MPI_Comm_group() */


int
_scs_MPI_Group_incl(MPI_Group group, int size, int *nodes, MPI_Group *new_group)
{
    return PMPI_Group_incl(group, size, nodes, new_group);
}  /* end of _scs_MPI_Group_incl() */


int
_scs_MPI_Comm_create(MPI_Comm comm, MPI_Group group, MPI_Comm *new_comm)
{
    return PMPI_Comm_create(comm, group, new_comm);
}  /* end of _scs_MPI_Comm_create() */


int
_scs_MPI_Group_size(MPI_Group group, int *size)
{
    return PMPI_Group_size(group, size);
}  /* end of _scs_MPI_Group_size() */


int
_scs_MPI_Group_rank(MPI_Group group, int *rank)
{
    return PMPI_Group_rank(group, rank);
}  /* end of _scs_MPI_Group_rank() */
