/*
** Copyright (2007) Sandia Corporation. Under the terms of Contract DE-AC04-94AL85000
** with Sandia Corporation, the U.S. Government retains certain rights in this software.
**
** Seshat
** $Id: scs_mpi_wrappersF.c,v 1.54 2008/05/23 16:50:59 rolf Exp $
** Rolf Riesen (rolf@sandia.gov), April 2006, Sandia National Laboratories
**
** FORTRAN has it's own PMPI interface....
*/

#include <stdio.h>
#include <stdlib.h>
#include "scs.h"
#include "scs_mpi_wrappers.h"
#include "scs_mpi_wrappersF.h"


/* Mark function we have not tried out yet */
#define MARK() \
    { \
        static int first= 1; \
        if (first)   { \
            fprintf(stderr, "[%3d] Function %s has been used!\n", _scs_rank, __func__); \
            first= 0; \
        } \
    }

#if defined(REDSTORM)
    /* Should be defined, but are not on Red Storm... */
    extern void *MPI_F_STATUS_IGNORE;
    extern void *MPI_F_STATUSES_IGNORE;
#endif

static int _scs_fake_argc= 1;
static char *_scs_fake_argv[2];
static char **_scs_fake_argv_ptr;



int 
mpi_init_(int *ierr)
{

    Debug(DBG_MPI3, "\n", "");
    /* This one is for Fortran programs on Red Storm */
    _scs_fake_argv[0]= "fake_argv";
    _scs_fake_argv[1]= NULL;
    _scs_fake_argv_ptr= _scs_fake_argv;
    *ierr= wrap_MPI_Init(&_scs_fake_argc, &_scs_fake_argv_ptr);
    return *ierr;

}  /* end of mpi_init_() */



int
mpi_finalize_(int *ierr)
{

    Debug(DBG_MPI3, "\n", "");
    *ierr= wrap_MPI_Finalize();
    return *ierr;

}  /* end of mpi_finalize_() */



int
mpi_wait_(MPI_RequestF *req, MPI_StatusF *status, MPI_Fint *ierr)
{

MPI_Status c_status;
MPI_Request c_req;


    Debug(DBG_MPI3, "\n", "");
    c_req= PMPI_Request_f2c(*req);
    if ((status != MPI_F_STATUS_IGNORE) && (status != MPI_F_STATUSES_IGNORE))   {
        *ierr= wrap_MPI_Wait(&c_req, &c_status);
        PMPI_Status_c2f(&c_status, status);
    } else   {
        *ierr= wrap_MPI_Wait(&c_req, MPI_STATUS_IGNORE);
    }
    *req= PMPI_Request_c2f(c_req);

    return *ierr;

}  /* end of mpi_wait_() */



int
mpi_waitall_(MPI_Fint *count, MPI_RequestF *array_of_requests, MPI_StatusF *array_of_statuses, int *ierr)
{

int i;
MPI_Request *c_reqs;
MPI_Status *c_statuses;


    Debug(DBG_MPI3, "count is %d\n", *count);
    c_reqs= (MPI_Request *)malloc(*count * sizeof(MPI_Request));
    c_statuses= (MPI_Status *)malloc(*count * sizeof(MPI_Status));
    if ((c_reqs == NULL) || (c_statuses == NULL))   {
	Err("Out of memory!\n", "");
	_scs_abort();
    }

    if (*count > 0)   {
        /* We need to convert the requests and statuses to C */
        for (i= 0; i < *count; i++)   {
            c_reqs[i]= PMPI_Request_f2c(array_of_requests[i]);
        }

        if ((array_of_statuses != MPI_F_STATUS_IGNORE) && (array_of_statuses != MPI_F_STATUSES_IGNORE))   {
            *ierr= wrap_MPI_Waitall(*count, c_reqs, c_statuses);
        } else {
            *ierr= wrap_MPI_Waitall(*count, c_reqs, MPI_STATUSES_IGNORE);
        }

        for (i= 0; i < *count; i++)   {
            array_of_requests[i]= PMPI_Request_c2f(c_reqs[i]);
        }
    } else   {
        *ierr= wrap_MPI_Waitall(*count, NULL, c_statuses);
    }

    for (i= 0; i < *count; i++)   {
        PMPI_Status_c2f(&(c_statuses[i]), &(array_of_statuses[i]));
    }

    free(c_reqs);
    free(c_statuses);

    return *ierr;

}  /* end of mpi_waitall_() */



int
mpi_send_(void *data, int *len, MPI_DatatypeF *dt, int *dest, int *tag, MPI_CommF *comm, int *ierr)
{

MPI_Comm c_comm;
MPI_Datatype c_dt;


    Debug(DBG_MPI3, "buf %p, len %d objects, dest %d, tag 0x%x\n", data, *len, *dest, *tag);
    c_dt= PMPI_Type_f2c(*dt);
    c_comm= PMPI_Comm_f2c(*comm);
    *ierr= wrap_MPI_Send(data, *len, c_dt, *dest, *tag, c_comm);

    return *ierr;

}  /* end of mpi_send_() */



int
mpi_rsend_(void *data, int *len, MPI_DatatypeF *dt, int *dest, int *tag, MPI_CommF *comm, int *ierr)
{

MPI_Comm c_comm;
MPI_Datatype c_dt;


    MARK();
    Debug(DBG_MPI3, "buf %p, len %d objects, dest %d, tag 0x%x\n", data, *len, *dest, *tag);
    c_dt= PMPI_Type_f2c(*dt);
    c_comm= PMPI_Comm_f2c(*comm);
    *ierr= wrap_MPI_Rsend(data, *len, c_dt, *dest, *tag, c_comm);

    return *ierr;

}  /* end of mpi_rsend_() */



int
mpi_isend_(void *data, int *len, MPI_DatatypeF *dt, int *dest, int *tag, MPI_CommF *comm,
    MPI_RequestF *req, int *ierr)
{

MPI_Comm c_comm;
MPI_Datatype c_dt;
MPI_Request c_req;


    Debug(DBG_MPI3, "buf %p, len %d objects, dest %d, tag 0x%x\n", data, *len, *dest, *tag);
    c_dt= PMPI_Type_f2c(*dt);
    c_comm= PMPI_Comm_f2c(*comm);
    c_req= PMPI_Request_f2c(*req);

    *ierr= wrap_MPI_Isend(data, *len, c_dt, *dest, *tag, c_comm, &c_req);
    *req= PMPI_Request_c2f(c_req);

    return *ierr;

}  /* end of mpi_isend_() */



int
mpi_recv_(void *data, int *len, MPI_DatatypeF *dt, int *src, int *tag, MPI_CommF *comm,
    MPI_StatusF *status, int *ierr)
{

MPI_Datatype c_dt;
MPI_Comm c_comm;
MPI_Status c_status;


    Debug(DBG_MPI3, "buf %p, len %d objects, src %d, tag 0x%x\n", data, *len, *src, *tag);
    c_dt= PMPI_Type_f2c(*dt);
    c_comm= PMPI_Comm_f2c(*comm);
    if ((status != MPI_F_STATUS_IGNORE) && (status != MPI_F_STATUSES_IGNORE))   {
        PMPI_Status_f2c(status, &c_status);
        *ierr= wrap_MPI_Recv(data, *len, c_dt, *src, *tag, c_comm, &c_status);
        PMPI_Status_c2f(&c_status, status);
    } else   {
        *ierr= wrap_MPI_Recv(data, *len, c_dt, *src, *tag, c_comm, MPI_STATUS_IGNORE);
    }

    return *ierr;

}  /* end of mpi_recv_() */



int
mpi_irecv_(void *data, int *len, MPI_DatatypeF *dt, int *src, int *tag, MPI_CommF *comm,
    MPI_RequestF *req, int *ierr)
{

MPI_Comm c_comm;
MPI_Datatype c_dt;
MPI_Request c_req;


    Debug(DBG_MPI3, "buf %p, len %d objects, src %d, tag 0x%x\n", data, *len, *src, *tag);
    c_dt= PMPI_Type_f2c(*dt);
    c_comm= PMPI_Comm_f2c(*comm);
    c_req= PMPI_Request_f2c(*req);

    *ierr= wrap_MPI_Irecv(data, *len, c_dt, *src, *tag, c_comm, &c_req);
    *req= PMPI_Request_c2f(c_req);

    return *ierr;

}  /* end of mpi_irecv_() */



int
mpi_barrier_(MPI_CommF *comm, int *ierr)
{

MPI_Comm c_comm;


    Debug(DBG_MPI3, "\n", "");
    c_comm= PMPI_Comm_f2c(*comm);
    *ierr= wrap_MPI_Barrier(c_comm);
    return *ierr;

}  /* end of mpi_barrier_() */



int
mpi_allreduce_(void *sendbuf, void *recvbuf, int *count, MPI_DatatypeF *dt, MPI_OpF *op,
    MPI_CommF *comm, int *ierr)
{

MPI_Comm c_comm;
MPI_Op c_op;
MPI_Datatype c_dt;


    Debug(DBG_MPI3, "sbuf %p, rbuf %p, count %d, op %d\n", sendbuf, recvbuf, *count, *op);
    c_dt= PMPI_Type_f2c(*dt);
    c_comm= PMPI_Comm_f2c(*comm);
    c_op= PMPI_Op_f2c(*op);
    *ierr= wrap_MPI_Allreduce(sendbuf, recvbuf, *count, c_dt, c_op, c_comm);
    return *ierr;

}  /* end of MPI_Allreduce_() */



int
mpi_alltoall_(void *sendbuf, int *sendcnt, MPI_DatatypeF *dt1, void *recvbuf, int *recvcnt,
    MPI_DatatypeF *dt2, MPI_CommF *comm, int *ierr)
{

MPI_Comm c_comm;
MPI_Datatype c_dt1;
MPI_Datatype c_dt2;


    Debug(DBG_MPI3, "sbuf %p, scnt %d, rbuf %p, rcnt %d\n", sendbuf, *sendcnt, recvbuf, *recvcnt);
    c_dt1= PMPI_Type_f2c(*dt1);
    c_dt2= PMPI_Type_f2c(*dt2);
    c_comm= PMPI_Comm_f2c(*comm);
    *ierr= wrap_MPI_Alltoall(sendbuf, *sendcnt, c_dt1, recvbuf, *recvcnt, c_dt2, c_comm);
    return *ierr;

}  /* end of MPI_Alltoall_() */



int
mpi_alltoallv_(void *sendbuf, int **sendcnts, int **sdispls, MPI_DatatypeF *dt1, void *recvbuf,
    int **recvcnts, int **rdispls, MPI_DatatypeF *dt2, MPI_CommF *comm, int *ierr)
{

MPI_Comm c_comm;
MPI_Datatype c_dt1;
MPI_Datatype c_dt2;


    MARK();
    Debug(DBG_MPI3, "sbuf %p, rbuf %p\n", sendbuf, recvbuf);
    c_dt1= PMPI_Type_f2c(*dt1);
    c_dt2= PMPI_Type_f2c(*dt2);
    c_comm= PMPI_Comm_f2c(*comm);
    *ierr= wrap_MPI_Alltoallv(sendbuf, *sendcnts, *sdispls, c_dt1, recvbuf, *recvcnts, *rdispls, c_dt2, c_comm);
    return *ierr;

}  /* end of MPI_Alltoallv_() */



int
mpi_reduce_(void *sendbuf, void *recvbuf, int *count, MPI_DatatypeF *dt, MPI_OpF *op, int *root,
    MPI_CommF *comm, int *ierr)
{

MPI_Comm c_comm;
MPI_Op c_op;
MPI_Datatype c_dt;


    Debug(DBG_MPI3, "sbuf %p, rbuf %p, count %d, op %d, root %d\n", sendbuf, recvbuf, *count, *op, root);
    c_dt= PMPI_Type_f2c(*dt);
    c_comm= PMPI_Comm_f2c(*comm);
    c_op= PMPI_Op_f2c(*op);
    *ierr= wrap_MPI_Reduce(sendbuf, recvbuf, *count, c_dt, c_op, *root, c_comm);
    return *ierr;

}  /* end of MPI_Reduce_() */



int
mpi_bcast_(void *buffer, int *count, MPI_DatatypeF *dt, int *root, MPI_CommF *comm, int *ierr)
{

MPI_Comm c_comm;
MPI_Datatype c_dt;


    Debug(DBG_MPI3, "buf %p, count %d, root %d\n", buffer, *count, root);
    c_dt= PMPI_Type_f2c(*dt);
    c_comm= PMPI_Comm_f2c(*comm);
    *ierr= wrap_MPI_Bcast(buffer, *count, c_dt, *root, c_comm);
    return *ierr;

}  /* end of MPI_Bcast_() */



int
mpi_comm_size_(MPI_CommF *comm, int *size, int *ierr)
{

MPI_Comm c_comm;


    Debug(DBG_MPI3, "\n", "");
    c_comm= PMPI_Comm_f2c(*comm);
    *ierr= wrap_MPI_Comm_size(c_comm, size);
    return *ierr;

}  /* end of MPI_Comm_size_() */



int
mpi_comm_rank_(MPI_CommF *comm, int *rank, int *ierr)
{

MPI_Comm c_comm;


    Debug(DBG_MPI3, "\n", "");
    c_comm= PMPI_Comm_f2c(*comm);
    *ierr= wrap_MPI_Comm_rank(c_comm, rank);
    return *ierr;

}  /* end of MPI_Comm_rank_() */



int
mpi_comm_dup_(MPI_CommF *comm, MPI_CommF *comm_out, int *ierr)
{

MPI_Comm c_comm;
MPI_Comm c_comm_out;


    Debug(DBG_MPI3, "\n", "");
    c_comm= PMPI_Comm_f2c(*comm);
    *ierr= wrap_MPI_Comm_dup(c_comm, &c_comm_out);
    *comm_out= PMPI_Comm_c2f(c_comm_out);
    return *ierr;

}  /* end of MPI_Comm_dup_() */



int
mpi_comm_split_(MPI_CommF *comm, int *color, int *key, MPI_CommF *comm_out, int *ierr)
{

MPI_Comm c_comm;
MPI_Comm c_comm_out;


    Debug(DBG_MPI3, "color %d\n", *color);
    c_comm= PMPI_Comm_f2c(*comm);
    *ierr= wrap_MPI_Comm_split(c_comm, *color, *key, &c_comm_out);
    *comm_out= PMPI_Comm_c2f(c_comm_out);
    return *ierr;

}  /* end of MPI_Comm_split_() */



int
mpi_abort_(MPI_CommF *comm, int *errorcode, int *ierr)
{

MPI_Comm c_comm;


    Debug(DBG_MPI3, "error code %d\n", *errorcode);
    c_comm= PMPI_Comm_f2c(*comm);
    *ierr= wrap_MPI_Abort(c_comm, *errorcode);
    return *ierr;

}  /* end of MPI_Abort_() */



double
mpi_wtime_(void)
{
    return MPI_Wtime();
}  /* end of MPI_Wtime_() */



int
mpi_file_open_(MPI_CommF *comm, char **filename, int *amode, MPI_InfoF *info, MPI_FileF **fh, int *ierr)
{

MPI_Comm c_comm;
MPI_Info c_info;
MPI_File c_file;


    MARK();
    Debug(DBG_MPI3, "fname %s, amode %d\n", *filename, *amode);

    c_comm= PMPI_Comm_f2c(*comm);
    c_info= PMPI_Info_f2c(*info);
    *ierr= wrap_MPI_File_open(c_comm, *filename, *amode, c_info, &c_file);
    **fh= PMPI_File_c2f(c_file);
    return *ierr;

}  /* end of MPI_File_open_() */



int
mpi_scatterv_(void *sendbuf, int **sendcnts, int **sdispls, MPI_DatatypeF *dt1, void *recvbuf,
    int *recvcnt, MPI_DatatypeF *dt2, int *root, MPI_CommF *comm, int *ierr)
{

MPI_Comm c_comm;
MPI_Datatype c_dt1;
MPI_Datatype c_dt2;


    MARK();
    Debug(DBG_MPI3, "sbuf %p, rbuf %p, root %d\n", sendbuf, recvbuf, root);
    c_dt1= PMPI_Type_f2c(*dt1);
    c_dt2= PMPI_Type_f2c(*dt2);
    c_comm= PMPI_Comm_f2c(*comm);
    *ierr= wrap_MPI_Scatterv(sendbuf, *sendcnts, *sdispls, c_dt1, recvbuf, *recvcnt, c_dt2, *root, c_comm);
    return *ierr;

}  /* end of MPI_scatterv_() */



int
mpi_allgather_(void *sendbuf, int *sendcnt, MPI_DatatypeF *dt1, void *recvbuf,
    int *recvcnt, MPI_DatatypeF *dt2, MPI_CommF *comm, int *ierr)
{

MPI_Comm c_comm;
MPI_Datatype c_dt1;
MPI_Datatype c_dt2;


    MARK();
    Debug(DBG_MPI3, "sbuf %p, rbuf %p\n", sendbuf, recvbuf);
    c_dt1= PMPI_Type_f2c(*dt1);
    c_dt2= PMPI_Type_f2c(*dt2);
    c_comm= PMPI_Comm_f2c(*comm);
    *ierr= wrap_MPI_Allgather(sendbuf, *sendcnt, c_dt1, recvbuf, *recvcnt, c_dt2, c_comm);
    return *ierr;

}  /* end of MPI_Allgather_() */



int
mpi_allgatherv_(void *sendbuf, int *sendcnt, MPI_DatatypeF *dt1, void *recvbuf,
    int **recvcounts, int **rdispls, MPI_DatatypeF *dt2, MPI_CommF *comm, int *ierr)
{

MPI_Comm c_comm;
MPI_Datatype c_dt1;
MPI_Datatype c_dt2;


    MARK();
    Debug(DBG_MPI3, "sbuf %p, scnt %d, rbuf %p\n", sendbuf, *sendcnt, recvbuf);
    c_dt1= PMPI_Type_f2c(*dt1);
    c_dt2= PMPI_Type_f2c(*dt2);
    c_comm= PMPI_Comm_f2c(*comm);
    *ierr= wrap_MPI_Allgatherv(sendbuf, *sendcnt, c_dt1, recvbuf, *recvcounts, *rdispls, c_dt2, c_comm);
    return *ierr;

}  /* end of MPI_Allgatherv_() */



int
mpi_gatherv_(void *sendbuf, int *sendcnt, MPI_DatatypeF *dt1, void *recvbuf,
    int **recvcounts, int **rdispls, MPI_DatatypeF *dt2, int *root, MPI_CommF *comm, int *ierr)
{

MPI_Comm c_comm;
MPI_Datatype c_dt1;
MPI_Datatype c_dt2;


    MARK();
    Debug(DBG_MPI3, "sbuf %p, scnt %d, rbuf %p, root %d\n", sendbuf, *sendcnt, recvbuf, *root);
    c_dt1= PMPI_Type_f2c(*dt1);
    c_dt2= PMPI_Type_f2c(*dt2);
    c_comm= PMPI_Comm_f2c(*comm);
    *ierr= wrap_MPI_Gatherv(sendbuf, *sendcnt, c_dt1, recvbuf, *recvcounts, *rdispls, c_dt2, *root, c_comm);
    return *ierr;

}  /* end of MPI_gatherv_() */


int
mpi_iprobe_(int *source, int *tag, MPI_CommF *comm, int *flag, MPI_StatusF *status, int *ierr)
{

MPI_Comm c_comm;
MPI_Status c_status;


    MARK();
    Debug(DBG_MPI3, "src %d, tag 0x%x\n", *source, &tag);
    c_comm= PMPI_Comm_f2c(*comm);

    if ((status != MPI_F_STATUS_IGNORE) && (status != MPI_F_STATUSES_IGNORE))   {
        PMPI_Status_f2c(status, &c_status);
        *ierr= wrap_MPI_Iprobe(*source, *tag, c_comm, flag, &c_status);
        PMPI_Status_c2f(&c_status, status);
    } else   {
        *ierr= wrap_MPI_Iprobe(*source, *tag, c_comm, flag, MPI_STATUS_IGNORE);
    }

    return *ierr;

}  /* end of mpi_iprobe_() */



int 
mpi_type_contiguous_(int *count, MPI_DatatypeF *oldtype, MPI_DatatypeF *newtype, int *ierr)
{

MPI_Datatype c_dt1;
MPI_Datatype c_dt2;


    MARK();
    Debug(DBG_MPI3, "count %d\n", *count);
    c_dt1= PMPI_Type_f2c(*oldtype);
    *ierr= wrap_MPI_Type_contiguous(*count, c_dt1, &c_dt2);
    *newtype= PMPI_Type_c2f(c_dt2);
    return *ierr;

} /* end of mpi_type_contiguous_() */


int 
mpi_type_free_(MPI_DatatypeF *datatype, int *ierr)
{

MPI_Datatype c_dt;


    MARK();
    Debug(DBG_MPI3, "\n", "");
    c_dt= PMPI_Type_f2c(*datatype);
    *ierr= wrap_MPI_Type_free(&c_dt);
    *datatype= PMPI_Type_c2f(c_dt);
    return *ierr;

} /* end of mpi_type_free_() */


int 
mpi_type_struct_(int *count, int *array_of_blocklengths, MPI_Aint *array_of_displacements, MPI_DatatypeF *array_of_types[], MPI_DatatypeF *newtype, int *ierr)
{
    Err("Not implemented yet\n", ""); _scs_abort();
} /* end of mpi_type_struct_() */


int 
mpi_type_vector_(int *count, int *blocklength, int *stride, MPI_DatatypeF *oldtype, MPI_DatatypeF *newtype, int *ierr)
{

MPI_Datatype c_dt1;
MPI_Datatype c_dt2;


    MARK();
    Debug(DBG_MPI3, "count %d, blocklength %d, stride %d\n", *count, *blocklength, *stride);
    c_dt1= PMPI_Type_f2c(*oldtype);
    *ierr= wrap_MPI_Type_vector(*count, *blocklength, *stride, c_dt1, &c_dt2);
    *newtype= PMPI_Type_c2f(c_dt2);
    return *ierr;

} /* end of mpi_type_vector_() */


int
mpi_comm_create_(MPI_CommF *comm, MPI_GroupF *group, MPI_CommF *comm_out, int *ierr)
{

MPI_Comm c_comm1;
MPI_Comm c_comm2;
MPI_Group c_group;


    MARK();
    Debug(DBG_MPI3, "\n", "");
    c_comm1= PMPI_Comm_f2c(*comm);
    c_group= PMPI_Group_f2c(*group);
    *ierr= wrap_MPI_Comm_create(c_comm1, c_group, &c_comm2);
    *comm_out= PMPI_Comm_c2f(c_comm2);
    return *ierr;

}  /* end of mpi_comm_create_() */



int
mpi_comm_free_(MPI_CommF *comm, int *ierr)
{

MPI_Comm c_comm;


    MARK();
    Debug(DBG_MPI3, "\n", "");
    c_comm= PMPI_Comm_f2c(*comm);
    *ierr= wrap_MPI_Comm_free(&c_comm);
    *comm= PMPI_Comm_c2f(c_comm);       /* FIXME: I don't know if this is necessary */
    return *ierr;

}  /* end of mpi_comm_free_() */



int
mpi_comm_group_(MPI_CommF *comm, MPI_GroupF *group, int *ierr)
{

MPI_Comm c_comm;
MPI_Group c_group;


    MARK();
    Debug(DBG_MPI3, "\n", "");
    c_comm= PMPI_Comm_f2c(*comm);
    *ierr= wrap_MPI_Comm_group(c_comm, &c_group);
    *group= PMPI_Group_c2f(c_group);
    return *ierr;

}  /* end of mpi_comm_group_() */



int
mpi_group_free_(MPI_GroupF *group, int *ierr)
{

MPI_Group c_group;


    MARK();
    Debug(DBG_MPI3, "\n", "");
    c_group= PMPI_Group_f2c(*group);
    *ierr= wrap_MPI_Group_free(&c_group);
    *group= PMPI_Group_c2f(c_group);
    return *ierr;

}  /* end of mpi_group_free_() */



int
mpi_group_incl_(MPI_GroupF *group, int *n, int **ranks, MPI_GroupF *group_out, int *ierr)
{

MPI_Group c_group1;
MPI_Group c_group2;


    MARK();
    Debug(DBG_MPI3, "n %d\n", n);
    c_group1= PMPI_Group_f2c(*group);
    *ierr= wrap_MPI_Group_incl(c_group1, *n, *ranks, &c_group2);
    *group= PMPI_Group_c2f(c_group2);
    return *ierr;

}  /* end of mpi_group_incl_() */



int
mpi_group_range_incl_(MPI_GroupF *group, int *n, int ranges[][3], MPI_GroupF *newgroup, int *ierr)
{

MPI_Group c_group1;
MPI_Group c_group2;


    MARK();
    Debug(DBG_MPI3, "n %d\n", n);
    c_group1= PMPI_Group_f2c(*group);
    *ierr= wrap_MPI_Group_range_incl(c_group1, *n, ranges, &c_group2);
    *newgroup= PMPI_Group_c2f(c_group2);
    return *ierr;

}  /* end of mpi_group_rankge_incl_() */


int
mpi_cancel_(int *req, int *ierr)
{

MPI_Request c_req;


    MARK();
    Debug(DBG_MPI3, "\n", "");
    c_req= PMPI_Request_f2c(*req);
    *ierr= wrap_MPI_Cancel(&c_req);
    *req= PMPI_Request_c2f(c_req);      /* FIXME: I don't think I need this, but am not sure */

    return *ierr;

}  /* end of mpi_cancel_() */


int 
mpi_unpack_(void *inbuf, int insize, int *position, void *outbuf, int outcount, MPI_DatatypeF *dt, MPI_CommF *comm, int *ierr)
{

MPI_Comm c_comm;
MPI_Datatype c_dt;


    MARK();
    Debug(DBG_MPI3, "\n", "");
    c_comm= PMPI_Comm_f2c(*comm);
    c_dt= PMPI_Type_f2c(*dt);
    *ierr= wrap_MPI_Unpack(inbuf, insize, position, outbuf, outcount, c_dt, c_comm);
    return *ierr;

}  /* end of MPI_Unpack() */



/*
** -----------------------------------------------------------------------------------------------------------
** Below here are functions for which we do not need to do anything
*/
#ifdef EASY_FUNCTIONS
/* point-to-point */
int mpi_type_indexed_(int count, int *array_of_blocklengths, int *array_of_displacements, MPI_DatatypeF *oldtype, MPI_DatatypeF *newtype, int *ierr)NOOP
int mpi_type_commit_(MPI_DatatypeF *datatype)NOOP

/* miscellany */
int mpi_status_f2c_(MPI_Fint *f_status, MPI_StatusF *c_status, int *ierr)NOOP
MPI_Fint mpi_comm_c2f_(MPI_CommF comm, int *ierr)NOOP
MPI_OpF mpi_op_f2c_(MPI_Fint op, int *ierr)NOOP_OP
int mpi_status_c2f_(MPI_StatusF *c_status, MPI_Fint *f_status, int *ierr)NOOP
MPI_RequestF mpi_request_f2c_(MPI_Fint request, int *ierr)NOOP_REQUEST
MPI_Fint mpi_request_c2f_(MPI_RequestF request, int *ierr)NOOP
MPI_Fint mpi_win_c2f_(MPI_Win win, int *ierr)NOOP
int mpi_win_create_errhandler_(MPI_Win_errhandler_fn *function, MPI_Errhandler *errhandler, int *ierr)NOOP
MPI_Win mpi_win_f2c_(MPI_Fint win, int *ierr)NOOP_WIN
int mpi_win_get_errhandler_(MPI_Win win, MPI_Errhandler *errhandler, int *ierr)NOOP
int mpi_win_set_errhandler_(MPI_Win win, MPI_Errhandler errhandler, int *ierr)NOOP

/* collective communications */
int mpi_op_create_(MPI_User_function *function, int commute, MPI_OpF *op, int *ierr)NOOP

/* groups, contexts, and communicators */
int mpi_comm_test_inter_(MPI_CommF comm, int *flag, int *ierr)NOOP

/* environmental enquiry */
int mpi_get_processor_name_(char *name, int *resultlen, int *ierr)NOOP
int mpi_initialized_(int *flag, int *ierr)NOOP

#endif /* EASY_FUNCTIONS */



/*
** -----------------------------------------------------------------------------------------------------------
** Below are the MPI functions we have not written wrappers for. Some of them will not
** need a wrapper. We only need wrappers for those that take a communicator or
** a group. (Plus a few special MPI functions like MPI_Init() which we already have.)
*/
#define NOOP		{fprintf(stderr, "No wrapper exists yet for function %s! Aborting...\n", __func__); \
			_scs_abort(); return MPI_ERR_UNSUPPORTED_OPERATION;}

#define NOOP_COMM	{fprintf(stderr, "No wrapper exists yet for function %s! Aborting...\n", __func__); \
			_scs_abort(); return 0;}


#define NOOP_FILE	{fprintf(stderr, "No wrapper exists yet for function %s! Aborting...\n", __func__); \
			_scs_abort(); return 0;}

#define NOOP_GROUP	{fprintf(stderr, "No wrapper exists yet for function %s! Aborting...\n", __func__); \
			_scs_abort(); return 0;}

#define NOOP_INFO	{fprintf(stderr, "No wrapper exists yet for function %s! Aborting...\n", __func__); \
			_scs_abort(); return 0;}

#define NOOP_OP		{fprintf(stderr, "No wrapper exists yet for function %s! Aborting...\n", __func__); \
			_scs_abort(); return 0;}

#define NOOP_REQUEST	{fprintf(stderr, "No wrapper exists yet for function %s! Aborting...\n", __func__); \
			_scs_abort(); return 0;}

#define NOOP_DT		{fprintf(stderr, "No wrapper exists yet for function %s! Aborting...\n", __func__); \
			_scs_abort(); return 0;}

#define NOOP_WIN	{fprintf(stderr, "No wrapper exists yet for function %s! Aborting...\n", __func__); \
			_scs_abort(); return MPI_WIN_NULL;}

#define NOOP_ERR	{fprintf(stderr, "No wrapper exists yet for function %s! Aborting...\n", __func__); \
			_scs_abort(); return MPI_ERRHANDLER_NULL;}
/*
    FIXME: All those "no wrapper needed" below probably need to go into the EASY part above.
*/

/*
** mpi-1.1 from http://www.mpi-forum.org/docs/mpi-11-html/mpi-report.html
*/

/* point-to-point */
/* no wrapper needed: int mpi_get_count_(MPI_StatusF *status, MPI_DatatypeF *datatype, int *count, int *ierr) */
int mpi_bsend_(void *buf, int count, MPI_DatatypeF *datatype, int dest, int tag, MPI_CommF comm, int *ierr)NOOP
int mpi_ssend_(void *buf, int count, MPI_DatatypeF *datatype, int dest, int tag, MPI_CommF comm, int *ierr)NOOP
/* no wrapper needed: int mpi_buffer_attach_(void *buffer, int size, int *ierr) */
/* no wrapper needed: int mpi_buffer_detach_(void *buffer, int *size, int *ierr) */
int mpi_ibsend_(void *buf, int count, MPI_DatatypeF *datatype, int dest, int tag, MPI_CommF comm, MPI_RequestF *request, int *ierr)NOOP
int mpi_issend_(void *buf, int count, MPI_DatatypeF *datatype, int dest, int tag, MPI_CommF comm, MPI_RequestF *request, int *ierr)NOOP
int mpi_irsend_(void *buf, int count, MPI_DatatypeF *datatype, int dest, int tag, MPI_CommF comm, MPI_RequestF *request, int *ierr)NOOP
/* no wrapper needed: int mpi_test_(MPI_RequestF *request, int *flag, MPI_StatusF *status, int *ierr) */
int mpi_request_free_(MPI_RequestF *request, int *ierr)NOOP
int mpi_waitany_(int count, MPI_RequestF array_of_requests[], int *index, MPI_StatusF *status, int *ierr)NOOP
/* no wrapper needed: int mpi_testany_(int count, MPI_RequestF array_of_requests[], int *index, int *flag, MPI_StatusF *status, int *ierr) */
int mpi_testall_(int count, MPI_RequestF array_of_requests[], int *flag, MPI_StatusF array_of_statuses[], int *ierr)NOOP
int mpi_waitsome_(int incount, MPI_RequestF array_of_requests[], int *outcount, int array_of_indices[], MPI_StatusF array_of_statuses[], int *ierr)NOOP
int mpi_testsome_(int incount, MPI_RequestF array_of_requests[], int *outcount, int array_of_indices[], MPI_StatusF array_of_statuses[], int *ierr)NOOP
int mpi_probe_(int source, int tag, MPI_CommF comm, MPI_StatusF *status, int *ierr)NOOP
int mpi_test_cancelled_(MPI_StatusF *status, int *flag, int *ierr)NOOP
int mpi_send_init_(void *buf, int count, MPI_DatatypeF *datatype, int dest, int tag, MPI_CommF comm, MPI_RequestF *request, int *ierr)NOOP
int mpi_bsend_init_(void *buf, int count, MPI_DatatypeF *datatype, int dest, int tag, MPI_CommF comm, MPI_RequestF *request, int *ierr)NOOP
int mpi_ssend_init_(void *buf, int count, MPI_DatatypeF *datatype, int dest, int tag, MPI_CommF comm, MPI_RequestF *request, int *ierr)NOOP
int mpi_rsend_init_(void *buf, int count, MPI_DatatypeF *datatype, int dest, int tag, MPI_CommF comm, MPI_RequestF *request, int *ierr)NOOP
int mpi_recv_init_(void *buf, int count, MPI_DatatypeF *datatype, int source, int tag, MPI_CommF comm, MPI_RequestF *request, int *ierr)NOOP
int mpi_start_(MPI_RequestF *request, int *ierr)NOOP
int mpi_startall_(int count, MPI_RequestF *array_of_requests, int *ierr)NOOP
int mpi_sendrecv_(void *sendbuf, int sendcount, MPI_DatatypeF *sendtype, int dest, int sendtag, void *recvbuf, int recvcount, MPI_DatatypeF *recvtype, int source, int recvtag, MPI_CommF comm, MPI_StatusF *status, int *ierr)NOOP
int mpi_sendrecv_replace_(void *buf, int count, MPI_DatatypeF *datatype, int dest, int sendtag, int source, int recvtag, MPI_CommF comm, MPI_StatusF *status, int *ierr)NOOP
int mpi_type_hvector_(int count, int blocklength, MPI_Aint stride, MPI_DatatypeF *oldtype, MPI_DatatypeF *newtype, int *ierr)NOOP
int mpi_type_hindexed_(int count, int *array_of_blocklengths, MPI_Aint *array_of_displacements, MPI_DatatypeF *oldtype, MPI_DatatypeF *newtype, int *ierr)NOOP
/* no wrapper needed: int mpi_address_(void *location, MPI_Aint *address, int *ierr) */
int mpi_type_extent_(MPI_DatatypeF *datatype, MPI_Aint *extent, int *ierr)NOOP
/* no wrapper needed: int mpi_type_size_(MPI_DatatypeF *datatype, int *size, int *ierr) */
int mpi_type_lb_(MPI_DatatypeF *datatype, MPI_Aint *displacement, int *ierr)NOOP
int mpi_type_ub_(MPI_DatatypeF *datatype, MPI_Aint *displacement, int *ierr)NOOP
int mpi_get_elements_(MPI_StatusF *status, MPI_DatatypeF *datatype, int *count, int *ierr)NOOP
int mpi_pack_(void *inbuf, int incount, MPI_DatatypeF *datatype, void *outbuf, int outsize, int *position, MPI_CommF comm, int *ierr)NOOP
int mpi_pack_size_(int incount, MPI_DatatypeF *datatype, MPI_CommF comm, int *size, int *ierr)NOOP

/* collective communications */
int mpi_gather_(void *sendbuf, int sendcount, MPI_DatatypeF *sendtype, void *recvbuf, int recvcount, MPI_DatatypeF *recvtype, int root, MPI_CommF comm, int *ierr)NOOP
int mpi_scatter_(void *sendbuf, int sendcount, MPI_DatatypeF *sendtype, void *recvbuf, int recvcount, MPI_DatatypeF *recvtype, int root, MPI_CommF comm, int *ierr)NOOP
int mpi_op_free_(MPI_OpF *op, int *ierr)NOOP
int mpi_reduce_scatter_(void *sendbuf, void *recvbuf, int *recvcounts, MPI_DatatypeF *datatype, MPI_OpF op, MPI_CommF comm, int *ierr)NOOP
int mpi_scan_(void *sendbuf, void *recvbuf, int count, MPI_DatatypeF *datatype, MPI_OpF op, MPI_CommF comm, int *ierr)NOOP

/* groups, contexts, and communicators */
int mpi_group_size_(MPI_GroupF group, int *size, int *ierr)NOOP
int mpi_group_rank_(MPI_GroupF group, int *rank, int *ierr)NOOP
int mpi_group_translate_ranks_(MPI_GroupF group1, int n, int *ranks1, MPI_GroupF group2, int *ranks2, int *ierr)NOOP
int mpi_group_compare_(MPI_GroupF group1, MPI_GroupF group2, int *result, int *ierr)NOOP
int mpi_group_union_(MPI_GroupF group1, MPI_GroupF group2, MPI_GroupF *newgroup, int *ierr)NOOP
int mpi_group_intersection_(MPI_GroupF group1, MPI_GroupF group2, MPI_GroupF *newgroup, int *ierr)NOOP
int mpi_group_difference_(MPI_GroupF group1, MPI_GroupF group2, MPI_GroupF *newgroup, int *ierr)NOOP
int mpi_group_excl_(MPI_GroupF group, int n, int *ranks, MPI_GroupF *newgroup, int *ierr)NOOP
int mpi_group_range_excl_(MPI_GroupF group, int n, int ranges[][3], MPI_GroupF *newgroup, int *ierr)NOOP
int mpi_comm_compare_(MPI_CommF comm1, MPI_CommF comm2, int *result, int *ierr)NOOP
int mpi_comm_remote_size_(MPI_CommF comm, int *size, int *ierr)NOOP
int mpi_comm_remote_group_(MPI_CommF comm, MPI_GroupF *group, int *ierr)NOOP
int mpi_intercomm_create_(MPI_CommF local_comm, int local_leader, MPI_CommF peer_comm, int remote_leader, int tag, MPI_CommF *newintercomm, int *ierr)NOOP
int mpi_intercomm_merge_(MPI_CommF intercomm, int high, MPI_CommF *newintracomm, int *ierr)NOOP
int mpi_keyval_create_(MPI_Copy_function *copy_fn, MPI_Delete_function *delete_fn, int *keyval, void *extra_state, int *ierr)NOOP
int mpi_keyval_free_(int *keyval, int *ierr)NOOP
int mpi_attr_put_(MPI_CommF comm, int keyval, void *attribute_val, int *ierr)NOOP
int mpi_attr_get_(MPI_CommF comm, int keyval, void *attribute_val, int *flag, int *ierr)NOOP
int mpi_attr_delete_(MPI_CommF comm, int keyval, int *ierr)NOOP

/* process tolpologies */
int mpi_cart_create_(MPI_CommF comm_old, int ndims, int *dims, int *periods, int reorder, MPI_CommF *comm_cart, int *ierr)NOOP
int mpi_dims_create_(int nnodes, int ndims, int *dims, int *ierr)NOOP
int mpi_graph_create_(MPI_CommF comm_old, int nnodes, int *index, int *edges, int reorder, MPI_CommF *comm_graph, int *ierr)NOOP
int mpi_topo_test_(MPI_CommF comm, int *status, int *ierr)NOOP
int mpi_graphdims_get_(MPI_CommF comm, int *nnodes, int *nedges, int *ierr)NOOP
int mpi_graph_get_(MPI_CommF comm, int maxindex, int maxedges, int *index, int *edges, int *ierr)NOOP
int mpi_cartdim_get_(MPI_CommF comm, int *ndims, int *ierr)NOOP
int mpi_cart_get_(MPI_CommF comm, int maxdims, int *dims, int *periods, int *coords, int *ierr)NOOP
int mpi_cart_rank_(MPI_CommF comm, int *coords, int *rank, int *ierr)NOOP
int mpi_cart_coords_(MPI_CommF comm, int rank, int maxdims, int *coords, int *ierr)NOOP
int mpi_graph_neighbors_count_(MPI_CommF comm, int rank, int *nneighbors, int *ierr)NOOP
int mpi_graph_neighbors_(MPI_CommF comm, int rank, int maxneighbors, int *neighbors, int *ierr)NOOP
int mpi_cart_shift_(MPI_CommF comm, int direction, int disp, int *rank_source, int *rank_dest, int *ierr)NOOP
int mpi_cart_sub_(MPI_CommF comm, int *remain_dims, MPI_CommF *newcomm, int *ierr)NOOP
int mpi_cart_map_(MPI_CommF comm, int ndims, int *dims, int *periods, int *newrank, int *ierr)NOOP
int mpi_graph_map_(MPI_CommF comm, int nnodes, int *index, int *edges, int *newrank, int *ierr)NOOP

/* environmental enquiry */
int mpi_errhandler_create_(MPI_Handler_function *function, MPI_Errhandler *errhandler, int *ierr)NOOP
int mpi_errhandler_set_(MPI_CommF comm, MPI_Errhandler errhandler, int *ierr)NOOP
int mpi_errhandler_get_(MPI_CommF comm, MPI_Errhandler *errhandler, int *ierr)NOOP
int mpi_errhandler_free_(MPI_Errhandler *errhandler, int *ierr)NOOP
int mpi_error_string_(int errorcode, char *string, int *resultlen, int *ierr)NOOP
int mpi_error_class_(int errorcode, int *errorclass, int *ierr)NOOP
double mpi_wtick_(void)NOOP

/* profiling */
int mpi_pcontrol_(const int level, ...)NOOP


/*
** mpi-1.2 from http://www-unix.mcs.anl.gov/mpi/mpi-standard/mpi-report-2.0/mpi2-report.htm#node0
*/
int mpi_get_version_(int *version, int *subversion, int *ierr)NOOP



/*
** mpi-2 from http://www-unix.mcs.anl.gov/mpi/mpi-standard/mpi-report-2.0/mpi2-report.htm#node0
*/

/* miscellany */
int mpi_alloc_mem_(MPI_Aint size, MPI_InfoF info, void *baseptr, int *ierr)NOOP
int mpi_comm_create_errhandler_(MPI_Comm_errhandler_fn *function, MPI_Errhandler *errhandler, int *ierr)NOOP
MPI_CommF mpi_comm_f2c_(MPI_Fint comm, int *ierr)NOOP_COMM
int mpi_comm_get_errhandler_(MPI_CommF comm, MPI_Errhandler *errhandler, int *ierr)NOOP
int mpi_comm_set_errhandler_(MPI_CommF comm, MPI_Errhandler errhandler, int *ierr)NOOP
MPI_Fint mpi_file_c2f_(MPI_FileF file, int *ierr)NOOP
int mpi_file_create_errhandler_(MPI_File_errhandler_fn *function, MPI_Errhandler *errhandler, int *ierr)NOOP
MPI_FileF mpi_file_f2c_(MPI_Fint file, int *ierr)NOOP_FILE
int mpi_file_get_errhandler_(MPI_FileF file, MPI_Errhandler *errhandler, int *ierr)NOOP
int mpi_file_set_errhandler_(MPI_FileF file, MPI_Errhandler errhandler, int *ierr)NOOP
int mpi_finalized_(int *flag, int *ierr)NOOP
int mpi_free_mem_(void *base, int *ierr)NOOP
int mpi_get_address_(void *location, MPI_Aint *address, int *ierr)NOOP
MPI_Fint mpi_group_c2f_(MPI_GroupF group, int *ierr)NOOP
MPI_GroupF mpi_group_f2c_(MPI_Fint group, int *ierr)NOOP_GROUP
MPI_Fint mpi_info_c2f_(MPI_InfoF info, int *ierr)NOOP
int mpi_info_create_(MPI_InfoF *info, int *ierr)NOOP
int mpi_info_delete_(MPI_InfoF info, char *key, int *ierr)NOOP
int mpi_info_dup_(MPI_InfoF info, MPI_InfoF *newinfo, int *ierr)NOOP
MPI_InfoF mpi_info_f2c_(MPI_Fint info, int *ierr)NOOP_INFO
int mpi_info_free_(MPI_InfoF *info, int *ierr)NOOP
int mpi_info_get_(MPI_InfoF info, char *key, int valuelen, char *value, int *flag, int *ierr)NOOP
int mpi_info_get_nkeys_(MPI_InfoF info, int *nkeys, int *ierr)NOOP
int mpi_info_get_nthkey_(MPI_InfoF info, int n, char *key, int *ierr)NOOP
int mpi_info_get_valuelen_(MPI_InfoF info, char *key, int *valuelen, int *flag, int *ierr)NOOP
int mpi_info_set_(MPI_InfoF info, char *key, char *value, int *ierr)NOOP
MPI_Fint mpi_op_c2f_(MPI_OpF op, int *ierr)NOOP
int mpi_pack_external_(char *datarep, void *inbuf, int incount, MPI_DatatypeF *datatype, void *outbuf, MPI_Aint outsize, MPI_Aint *position, int *ierr)NOOP
int mpi_pack_external_size_(char *datarep, int incount, MPI_DatatypeF *datatype, MPI_Aint *size, int *ierr)NOOP
int mpi_request_get_status_(MPI_RequestF request, int *flag, MPI_StatusF *status, int *ierr)NOOP
MPI_Fint mpi_type_c2f_(MPI_DatatypeF *datatype, int *ierr)NOOP
int mpi_type_create_darray_(int size, int rank, int ndims, int array_of_gsizes[], int array_of_distribs[], int array_of_dargs[], int array_of_psizes[], int order, MPI_DatatypeF *oldtype, MPI_DatatypeF *newtype, int *ierr)NOOP
int mpi_type_create_hindexed_(int count, int array_of_blocklengths[], MPI_Aint array_of_displacements[], MPI_DatatypeF *oldtype, MPI_DatatypeF *newtype, int *ierr)NOOP
int mpi_type_create_hvector_(int count, int blocklength, MPI_Aint stride, MPI_DatatypeF *oldtype, MPI_DatatypeF *newtype, int *ierr)NOOP
int mpi_type_create_indexed_block_(int count, int blocklength, int array_of_displacements[], MPI_DatatypeF *oldtype, MPI_DatatypeF *newtype, int *ierr)NOOP
int mpi_type_create_resized_(MPI_DatatypeF *oldtype, MPI_Aint lb, MPI_Aint extent, MPI_DatatypeF *newtype, int *ierr)NOOP
int mpi_type_create_struct_(int count, int array_of_blocklengths[], MPI_Aint array_of_displacements[], MPI_DatatypeF *array_of_types[], MPI_DatatypeF *newtype, int *ierr)NOOP
int mpi_type_create_subarray_(int ndims, int array_of_sizes[], int array_of_subsizes[], int array_of_starts[], int order, MPI_DatatypeF *oldtype, MPI_DatatypeF *newtype, int *ierr)NOOP
MPI_DatatypeF mpi_type_f2c_(MPI_Fint datatype, int *ierr)NOOP_DT
int mpi_type_get_extent_(MPI_DatatypeF *datatype, MPI_Aint *lb, MPI_Aint *extent, int *ierr)NOOP
int mpi_type_get_true_extent_(MPI_DatatypeF *datatype, MPI_Aint *true_lb, MPI_Aint *true_extent, int *ierr)NOOP
int mpi_unpack_external_(char *datarep, void *inbuf, MPI_Aint insize, MPI_Aint *position, void *outbuf, int outcount, MPI_DatatypeF *datatype, int *ierr)NOOP

/* process creation and management */
int mpi_close_port_(char *port_name, int *ierr)NOOP
int mpi_comm_accept_(char *port_name, MPI_InfoF info, int root, MPI_CommF comm, MPI_CommF *newcomm, int *ierr)NOOP
int mpi_comm_connect_(char *port_name, MPI_InfoF info, int root, MPI_CommF comm, MPI_CommF *newcomm, int *ierr)NOOP
int mpi_comm_disconnect_(MPI_CommF *comm, int *ierr)NOOP
int mpi_comm_get_parent_(MPI_CommF *parent, int *ierr)NOOP
int mpi_comm_join_(int fd, MPI_CommF *intercomm, int *ierr)NOOP
int mpi_comm_spawn_(char *command, char *argv[], int maxprocs, MPI_InfoF info, int root, MPI_CommF comm, MPI_CommF *intercomm, int array_of_errcodes[], int *ierr)NOOP
int mpi_comm_spawn_multiple_(int count, char *array_of_commands[], char **array_of_argv[], int array_of_maxprocs[], MPI_InfoF array_of_info[], int root, MPI_CommF comm, MPI_CommF *intercomm, int array_of_errcodes[], int *ierr)NOOP
int mpi_lookup_name_(char *service_name, MPI_InfoF info, char *port_name, int *ierr)NOOP
int mpi_open_port_(MPI_InfoF info, char *port_name, int *ierr)NOOP
int mpi_publish_name_(char *service_name, MPI_InfoF info, char *port_name, int *ierr)NOOP
int mpi_unpublish_name_(char *service_name, MPI_InfoF info, char *port_name, int *ierr)NOOP

/* one-sided communications */
int mpi_accumulate_(void *origin_addr, int origin_count, MPI_DatatypeF *origin_datatype, int target_rank, MPI_Aint target_disp, int target_count, MPI_DatatypeF *target_datatype, MPI_OpF op, MPI_Win win, int *ierr)NOOP
int mpi_get_(void *origin_addr, int origin_count, MPI_DatatypeF *origin_datatype, int target_rank, MPI_Aint target_disp, int target_count, MPI_DatatypeF *target_datatype, MPI_Win win, int *ierr)NOOP
int mpi_put_(void *origin_addr, int origin_count, MPI_DatatypeF *origin_datatype, int target_rank, MPI_Aint target_disp, int target_count, MPI_DatatypeF *target_datatype, MPI_Win win, int *ierr)NOOP
int mpi_win_complete_(MPI_Win win, int *ierr)NOOP
int mpi_win_create_(void *base, MPI_Aint size, int disp_unit, MPI_InfoF info, MPI_CommF comm, MPI_Win *win, int *ierr)NOOP
int mpi_win_fence_(int assert, MPI_Win win, int *ierr)NOOP
int mpi_win_free_(MPI_Win *win, int *ierr)NOOP
int mpi_win_get_group_(MPI_Win win, MPI_GroupF *group, int *ierr)NOOP
int mpi_win_lock_(int lock_type, int rank, int assert, MPI_Win win, int *ierr)NOOP
int mpi_win_post_(MPI_GroupF group, int assert, MPI_Win win, int *ierr)NOOP
int mpi_win_start_(MPI_GroupF group, int assert, MPI_Win win, int *ierr)NOOP
int mpi_win_test_(MPI_Win win, int *flag, int *ierr)NOOP
int mpi_win_unlock_(int rank, MPI_Win win, int *ierr)NOOP
int mpi_win_wait_(MPI_Win win, int *ierr)NOOP

/* extended collective operations */
int mpi_alltoallw_(void *sendbuf, int sendcounts[], int sdispls[], MPI_DatatypeF *sendtypes[], void *recvbuf, int recvcounts[], int rdispls[], MPI_DatatypeF *recvtypes[], MPI_CommF comm, int *ierr)NOOP
int mpi_exscan_(void *sendbuf, void *recvbuf, int count, MPI_DatatypeF *datatype, MPI_OpF op, MPI_CommF comm, int *ierr)NOOP

/* external interfaces */
int mpi_add_error_class_(int *errorclass, int *ierr)NOOP
int mpi_add_error_code_(int errorclass, int *errorcode, int *ierr)NOOP
int mpi_add_error_string_(int errorcode, char *string, int *ierr)NOOP
int mpi_comm_call_errhandler_(MPI_CommF comm, int errorcode, int *ierr)NOOP
int mpi_comm_create_keyval_(MPI_Comm_copy_attr_function *comm_copy_attr_fn, MPI_Comm_delete_attr_function *comm_delete_attr_fn, int *comm_keyval, void *extra_state, int *ierr)NOOP
int mpi_comm_delete_attr_(MPI_CommF comm, int comm_keyval, int *ierr)NOOP
int mpi_comm_free_keyval_(int *comm_keyval, int *ierr)NOOP
int mpi_comm_get_attr_(MPI_CommF comm, int comm_keyval, void *attribute_val, int *flag, int *ierr)NOOP
int mpi_comm_get_name_(MPI_CommF comm, char *comm_name, int *resultlen, int *ierr)NOOP
int mpi_comm_set_attr_(MPI_CommF comm, int comm_keyval, void *attribute_val, int *ierr)NOOP
int mpi_comm_set_name_(MPI_CommF comm, char *comm_name, int *ierr)NOOP
int mpi_file_call_errhandler_(MPI_FileF fh, int errorcode, int *ierr)NOOP
int mpi_grequest_complete_(MPI_RequestF request, int *ierr)NOOP
int mpi_grequest_start_(MPI_Grequest_query_function *query_fn, MPI_Grequest_free_function *free_fn, MPI_Grequest_cancel_function *cancel_fn, void *extra_state, MPI_RequestF *request, int *ierr)NOOP
int mpi_init_thread_(int *argc, char **argv[], int required, int *provided, int *ierr)NOOP
int mpi_is_thread_main_(int *flag, int *ierr)NOOP
int mpi_query_thread_(int *provided, int *ierr)NOOP
int mpi_status_set_cancelled_(MPI_StatusF *status, int flag, int *ierr)NOOP
int mpi_status_set_elements_(MPI_StatusF *status, MPI_DatatypeF *datatype, int count, int *ierr)NOOP
int mpi_type_create_keyval_(MPI_Type_copy_attr_function *type_copy_attr_fn, MPI_Type_delete_attr_function *type_delete_attr_fn, int *type_keyval, void *extra_state, int *ierr)NOOP
int mpi_type_delete_attr_(MPI_DatatypeF *type, int type_keyval, int *ierr)NOOP
int mpi_type_dup_(MPI_DatatypeF *type, MPI_DatatypeF *newtype, int *ierr)NOOP
int mpi_type_free_keyval_(int *type_keyval, int *ierr)NOOP
int mpi_type_get_attr_(MPI_DatatypeF *type, int type_keyval, void *attribute_val, int *flag, int *ierr)NOOP
int mpi_type_get_contents_(MPI_DatatypeF *datatype, int max_integers, int max_addresses, int max_datatypes, int array_of_integers[], MPI_Aint array_of_addresses[], MPI_DatatypeF *array_of_datatypes[], int *ierr)NOOP
int mpi_type_get_envelope_(MPI_DatatypeF *datatype, int *num_integers, int *num_addresses, int *num_datatypes, int *combiner, int *ierr)NOOP
int mpi_type_get_name_(MPI_DatatypeF *type, char *type_name, int *resultlen, int *ierr)NOOP
int mpi_type_set_attr_(MPI_DatatypeF *type, int type_keyval, void *attribute_val, int *ierr)NOOP
int mpi_type_set_name_(MPI_DatatypeF *type, char *type_name, int *ierr)NOOP
int mpi_win_call_errhandler_(MPI_Win win, int errorcode, int *ierr)NOOP
int mpi_win_create_keyval_(MPI_Win_copy_attr_function *win_copy_attr_fn, MPI_Win_delete_attr_function *win_delete_attr_fn, int *win_keyval, void *extra_state, int *ierr)NOOP
int mpi_win_delete_attr_(MPI_Win win, int win_keyval, int *ierr)NOOP
int mpi_win_free_keyval_(int *win_keyval, int *ierr)NOOP
int mpi_win_get_attr_(MPI_Win win, int win_keyval, void *attribute_val, int *flag, int *ierr)NOOP
int mpi_win_get_name_(MPI_Win win, char *win_name, int *resultlen, int *ierr)NOOP
int mpi_win_set_attr_(MPI_Win win, int win_keyval, void *attribute_val, int *ierr)NOOP
int mpi_win_set_name_(MPI_Win win, char *win_name, int *ierr)NOOP

/* i/o */
int mpi_file_close_(MPI_FileF *fh, int *ierr)NOOP
int mpi_file_delete_(char *filename, MPI_InfoF info, int *ierr)NOOP
int mpi_file_get_amode_(MPI_FileF fh, int *amode, int *ierr)NOOP
int mpi_file_get_atomicity_(MPI_FileF fh, int *flag, int *ierr)NOOP
int mpi_file_get_byte_offset_(MPI_FileF fh, MPI_Offset offset, MPI_Offset *disp, int *ierr)NOOP
int mpi_file_get_group_(MPI_FileF fh, MPI_GroupF *group, int *ierr)NOOP
int mpi_file_get_info_(MPI_FileF fh, MPI_InfoF *info_used, int *ierr)NOOP
int mpi_file_get_position_(MPI_FileF fh, MPI_Offset *offset, int *ierr)NOOP
int mpi_file_get_position_shared_(MPI_FileF fh, MPI_Offset *offset, int *ierr)NOOP
int mpi_file_get_size_(MPI_FileF fh, MPI_Offset *size, int *ierr)NOOP
int mpi_file_get_type_extent_(MPI_FileF fh, MPI_DatatypeF *datatype, MPI_Aint *extent, int *ierr)NOOP
int mpi_file_get_view_(MPI_FileF fh, MPI_Offset *disp, MPI_DatatypeF *etype, MPI_DatatypeF *filetype, char *datarep, int *ierr)NOOP
int mpi_file_iread_(MPI_FileF fh, void *buf, int count, MPI_DatatypeF *datatype, MPI_RequestF *request, int *ierr)NOOP
int mpi_file_iread_at_(MPI_FileF fh, MPI_Offset offset, void *buf, int count, MPI_DatatypeF *datatype, MPI_RequestF *request, int *ierr)NOOP
int mpi_file_iread_shared_(MPI_FileF fh, void *buf, int count, MPI_DatatypeF *datatype, MPI_RequestF *request, int *ierr)NOOP
int mpi_file_iwrite_(MPI_FileF fh, void *buf, int count, MPI_DatatypeF *datatype, MPI_RequestF *request, int *ierr)NOOP
int mpi_file_iwrite_at_(MPI_FileF fh, MPI_Offset offset, void *buf, int count, MPI_DatatypeF *datatype, MPI_RequestF *request, int *ierr)NOOP
int mpi_file_iwrite_shared_(MPI_FileF fh, void *buf, int count, MPI_DatatypeF *datatype, MPI_RequestF *request, int *ierr)NOOP
int mpi_file_preallocate_(MPI_FileF fh, MPI_Offset size, int *ierr)NOOP
int mpi_file_read_(MPI_FileF fh, void *buf, int count, MPI_DatatypeF *datatype, MPI_StatusF *status, int *ierr)NOOP
int mpi_file_read_all_(MPI_FileF fh, void *buf, int count, MPI_DatatypeF *datatype, MPI_StatusF *status, int *ierr)NOOP
int mpi_file_read_all_begin_(MPI_FileF fh, void *buf, int count, MPI_DatatypeF *datatype, int *ierr)NOOP
int mpi_file_read_all_end_(MPI_FileF fh, void *buf, MPI_StatusF *status, int *ierr)NOOP
int mpi_file_read_at_(MPI_FileF fh, MPI_Offset offset, void *buf, int count, MPI_DatatypeF *datatype, MPI_StatusF *status, int *ierr)NOOP
int mpi_file_read_at_all_(MPI_FileF fh, MPI_Offset offset, void *buf, int count, MPI_DatatypeF *datatype, MPI_StatusF *status, int *ierr)NOOP
int mpi_file_read_at_all_begin_(MPI_FileF fh, MPI_Offset offset, void *buf, int count, MPI_DatatypeF *datatype, int *ierr)NOOP
int mpi_file_read_at_all_end_(MPI_FileF fh, void *buf, MPI_StatusF *status, int *ierr)NOOP
int mpi_file_read_ordered_(MPI_FileF fh, void *buf, int count, MPI_DatatypeF *datatype, MPI_StatusF *status, int *ierr)NOOP
int mpi_file_read_ordered_begin_(MPI_FileF fh, void *buf, int count, MPI_DatatypeF *datatype, int *ierr)NOOP
int mpi_file_read_ordered_end_(MPI_FileF fh, void *buf, MPI_StatusF *status, int *ierr)NOOP
int mpi_file_read_shared_(MPI_FileF fh, void *buf, int count, MPI_DatatypeF *datatype, MPI_StatusF *status, int *ierr)NOOP
int mpi_file_seek_(MPI_FileF fh, MPI_Offset offset, int whence, int *ierr)NOOP
int mpi_file_seek_shared_(MPI_FileF fh, MPI_Offset offset, int whence, int *ierr)NOOP
int mpi_file_set_atomicity_(MPI_FileF fh, int flag, int *ierr)NOOP
int mpi_file_set_info_(MPI_FileF fh, MPI_InfoF info, int *ierr)NOOP
int mpi_file_set_size_(MPI_FileF fh, MPI_Offset size, int *ierr)NOOP
int mpi_file_set_view_(MPI_FileF fh, MPI_Offset disp, MPI_DatatypeF *etype, MPI_DatatypeF *filetype, char *datarep, MPI_InfoF info, int *ierr)NOOP
int mpi_file_sync_(MPI_FileF fh, int *ierr)NOOP
int mpi_file_write_(MPI_FileF fh, void *buf, int count, MPI_DatatypeF *datatype, MPI_StatusF *status, int *ierr)NOOP
int mpi_file_write_all_(MPI_FileF fh, void *buf, int count, MPI_DatatypeF *datatype, MPI_StatusF *status, int *ierr)NOOP
int mpi_file_write_all_begin_(MPI_FileF fh, void *buf, int count, MPI_DatatypeF *datatype, int *ierr)NOOP
int mpi_file_write_all_end_(MPI_FileF fh, void *buf, MPI_StatusF *status, int *ierr)NOOP
int mpi_file_write_at_(MPI_FileF fh, MPI_Offset offset, void *buf, int count, MPI_DatatypeF *datatype, MPI_StatusF *status, int *ierr)NOOP
int mpi_file_write_at_all_(MPI_FileF fh, MPI_Offset offset, void *buf, int count, MPI_DatatypeF *datatype, MPI_StatusF *status, int *ierr)NOOP
int mpi_file_write_at_all_begin_(MPI_FileF fh, MPI_Offset offset, void *buf, int count, MPI_DatatypeF *datatype, int *ierr)NOOP
int mpi_file_write_at_all_end_(MPI_FileF fh, void *buf, MPI_StatusF *status, int *ierr)NOOP
int mpi_file_write_ordered_(MPI_FileF fh, void *buf, int count, MPI_DatatypeF *datatype, MPI_StatusF *status, int *ierr)NOOP
int mpi_file_write_ordered_begin_(MPI_FileF fh, void *buf, int count, MPI_DatatypeF *datatype, int *ierr)NOOP
int mpi_file_write_ordered_end_(MPI_FileF fh, void *buf, MPI_StatusF *status, int *ierr)NOOP
int mpi_file_write_shared_(MPI_FileF fh, void *buf, int count, MPI_DatatypeF *datatype, MPI_StatusF *status, int *ierr)NOOP
int mpi_register_datarep_(char *datarep, MPI_Datarep_conversion_function *read_conversion_fn, MPI_Datarep_conversion_function *write_conversion_fn, MPI_Datarep_extent_function *dtype_file_extent_fn, void *extra_state, int *ierr)NOOP

/* language bindings */
int mpi_type_create_f90_complex_(int p, int r, MPI_DatatypeF *newtype, int *ierr)NOOP
int mpi_type_create_f90_integer_(int r, MPI_DatatypeF *newtype, int *ierr)NOOP
int mpi_type_create_f90_real_(int p, int r, MPI_DatatypeF *newtype, int *ierr)NOOP
int mpi_type_match_size_(int typeclass, int size, MPI_DatatypeF *type, int *ierr)NOOP

/* ??? */
MPI_Errhandler mpi_errhandler_f2c_(MPI_Fint errhandler, int *ierr)NOOP_ERR
MPI_Fint mpi_errhandler_c2f_(MPI_Errhandler errhandler, int *ierr)NOOP
