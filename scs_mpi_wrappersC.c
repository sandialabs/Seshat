/*
** Copyright (2007) Sandia Corporation. Under the terms of Contract DE-AC04-94AL85000
** with Sandia Corporation, the U.S. Government retains certain rights in this software.
**
** Seshat
** $Id: scs_mpi_wrappersC.c,v 1.55 2008/05/29 18:55:42 rolf Exp $
** Rolf Riesen (rolf@sandia.gov), April 2006, Sandia National Laboratories
**
** These are the C function that call out wrappers
*/

#include <stdio.h>
#include "scs_mpi_wrappers.h"

int 
MPI_Init(int *argc, char **argv[])
{
    return wrap_MPI_Init(argc, argv);
}  /* end of MPI_Init() */


int
MPI_Finalize(void)
{
    return wrap_MPI_Finalize();
}  /* end of MPI_Finalize() */


int
MPI_Wait(MPI_Request *request, MPI_Status *status)
{
    return wrap_MPI_Wait(request, status);
}  /* end of MPI_Wait() */


int
MPI_Waitall(int count, MPI_Request array_of_requests[], MPI_Status array_of_statuses[])
{
    return wrap_MPI_Waitall(count, array_of_requests, array_of_statuses);
}  /* end of MPI_Waitall() */


int
MPI_Send(const void *data, int len, MPI_Datatype dt, int dest, int tag, MPI_Comm comm)
{
    return wrap_MPI_Send(data, len, dt, dest, tag, comm);
}  /* end of MPI_Send() */


int
MPI_Rsend(const void *data, int len, MPI_Datatype dt, int dest, int tag, MPI_Comm comm)
{
    return wrap_MPI_Rsend(data, len, dt, dest, tag, comm);
}  /* end of MPI_Rsend() */


int
MPI_Isend(const void *data, int len, MPI_Datatype dt, int dest, int tag, MPI_Comm comm, MPI_Request *req)
{
    return wrap_MPI_Isend(data, len, dt, dest, tag, comm, req);
}  /* end of MPI_Isend() */


int
MPI_Recv(void *data, int len, MPI_Datatype dt, int src, int tag, MPI_Comm comm, MPI_Status *status)
{
    return wrap_MPI_Recv(data, len, dt, src, tag, comm, status);
}  /* end of MPI_Recv() */


int
MPI_Irecv(void *data, int len, MPI_Datatype dt, int src, int tag, MPI_Comm comm, MPI_Request *req)
{
    return wrap_MPI_Irecv(data, len, dt, src, tag, comm, req);
}  /* end of MPI_Irecv() */


int
MPI_Barrier(MPI_Comm comm)
{
    return wrap_MPI_Barrier(comm);
}  /* end of MPI_Barrier() */


int
MPI_Allreduce(const void *sendbuf, void *recvbuf, int count, MPI_Datatype dt, MPI_Op op, MPI_Comm comm)
{
    return wrap_MPI_Allreduce(sendbuf, recvbuf, count, dt, op, comm);
}  /* end of MPI_Allreduce() */


int
MPI_Alltoall(const void *sendbuf, int sendcnt, MPI_Datatype dt1, void *recvbuf, int recvcnt, MPI_Datatype dt2, MPI_Comm comm)
{
    return wrap_MPI_Alltoall(sendbuf, sendcnt, dt1, recvbuf, recvcnt, dt2, comm);
}  /* end of MPI_Alltoall() */


int
MPI_Alltoallv(const void *sendbuf, const int *sendcnts, const int *sdispls, MPI_Datatype dt1, void *recvbuf, const int *recvcnts,
	const int *rdispls, MPI_Datatype dt2, MPI_Comm comm)
{
    return wrap_MPI_Alltoallv(sendbuf, sendcnts, sdispls, dt1, recvbuf, recvcnts, rdispls, dt2, comm);
}  /* end of MPI_Alltoallv() */


int
MPI_Reduce(const void *sendbuf, void *recvbuf, int count, MPI_Datatype dt, MPI_Op op, int root, MPI_Comm comm)
{
    return wrap_MPI_Reduce(sendbuf, recvbuf, count, dt, op, root, comm);
}  /* end of MPI_Reduce() */


int
MPI_Bcast(void *buffer, int count, MPI_Datatype dt, int root, MPI_Comm comm) 
{
    return wrap_MPI_Bcast(buffer, count, dt, root, comm);
}  /* end of MPI_Bcast() */


int
MPI_Comm_size(MPI_Comm comm, int *size)
{
    return wrap_MPI_Comm_size(comm, size);
}  /* end of MPI_Comm_size() */


int
MPI_Comm_rank(MPI_Comm comm, int *rank)
{
    return wrap_MPI_Comm_rank(comm, rank);
}  /* end of MPI_Comm_rank() */


int
MPI_Comm_dup(MPI_Comm comm, MPI_Comm *comm_out)
{
    return wrap_MPI_Comm_dup(comm, comm_out);
}  /* end of MPI_Comm_dup() */


int
MPI_Comm_split(MPI_Comm comm, int color, int key, MPI_Comm *comm_out)
{
    return wrap_MPI_Comm_split(comm, color, key, comm_out);
}  /* end of MPI_Comm_split() */


int
MPI_Abort(MPI_Comm comm, int errorcode)
{
    return wrap_MPI_Abort(comm, errorcode);
}  /* end of MPI_Abort() */


double
MPI_Wtime(void)
{
    return wrap_MPI_Wtime();
}  /* end of MPI_Wtime() */


int
MPI_File_open(MPI_Comm comm, const char *filename, int amode, MPI_Info info, MPI_File *fh)
{
    return wrap_MPI_File_open(comm, filename, amode, info, fh);
}  /* end of MPI_File_open() */


int
MPI_Scatterv(const void *sendbuf, const int *sendcnts, const int *sdispls, MPI_Datatype dt1, void *recvbuf,
	int recvcount, MPI_Datatype dt2, int root, MPI_Comm comm)
{
    return wrap_MPI_Scatterv(sendbuf, sendcnts, sdispls, dt1, recvbuf, recvcount, dt2, root, comm);
}  /* end of MPI_Scatterv() */


int
MPI_Reduce_scatter(const void *sendbuf, void *recvbuf, const int *recvcounts, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm)
{
	return wrap_MPI_Reduce_scatter(sendbuf, recvbuf, recvcounts, datatype, op, comm);
}  /* end of MPI_Reduce_scatter() */


int
MPI_Allgather(const void *sendbuf, int sendcount, MPI_Datatype dt1, void *recvbuf, int recvcnt, MPI_Datatype dt2,
	MPI_Comm comm)
{
    return wrap_MPI_Allgather(sendbuf, sendcount, dt1, recvbuf, recvcnt, dt2, comm);
}  /* end of MPI_Allgather() */


int
MPI_Allgatherv(const void *sendbuf, int sendcnt, MPI_Datatype dt1, void *recvbuf, const int *recvcounts,
	const int *rdispls, MPI_Datatype dt2, MPI_Comm comm)
{
    return wrap_MPI_Allgatherv(sendbuf, sendcnt, dt1, recvbuf, recvcounts, rdispls, dt2, comm);
}  /* end of MPI_Allgatherv() */


int
MPI_Gather(const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm)
{
	return wrap_MPI_Gather(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, root, comm);
}  /* end of MPI_Gather() */


int
MPI_Gatherv(const void *sendbuf, int sendcnt, MPI_Datatype dt1, void *recvbuf, const int *recvcounts,
	const int *rdispls, MPI_Datatype dt2, int root, MPI_Comm comm)
{
    return wrap_MPI_Gatherv(sendbuf, sendcnt, dt1, recvbuf, recvcounts, rdispls, dt2, root, comm);
}  /* end of MPI_Gatherv() */


int
MPI_Scan(const void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm)
{
    return wrap_MPI_Scan(sendbuf, recvbuf, count, datatype, op, comm);
}  /* end of MPI_Scan() */


int
MPI_Iprobe(int source, int tag, MPI_Comm comm, int *flag, MPI_Status *status)
{
    return wrap_MPI_Iprobe(source, tag, comm, flag, status);
}  /* end of MPI_Iprobe() */


int 
MPI_Type_contiguous(int count, MPI_Datatype oldtype, MPI_Datatype *newtype)
{
    return wrap_MPI_Type_contiguous(count, oldtype, newtype);
} /* end of MPI_Type_contiguous() */


int 
MPI_Type_free(MPI_Datatype *datatype)
{
    return wrap_MPI_Type_free(datatype);
} /* end of MPI_Type_free() */


int 
MPI_Type_struct(int count, int *array_of_blocklengths, MPI_Aint *array_of_displacements, MPI_Datatype *array_of_types, MPI_Datatype *newtype)
{
    return wrap_MPI_Type_struct(count, array_of_blocklengths, array_of_displacements, array_of_types, newtype);
} /* end of MPI_Type_struct() */


int 
MPI_Type_vector(int count, int blocklength, int stride, MPI_Datatype oldtype, MPI_Datatype *newtype)
{
    return wrap_MPI_Type_vector(count, blocklength, stride, oldtype, newtype);
} /* end of MPI_Type_vector() */


int
MPI_Comm_create(MPI_Comm comm, MPI_Group group, MPI_Comm *comm_out)
{
    return wrap_MPI_Comm_create(comm, group, comm_out);
}  /* end of MPI_Comm_create() */


int
MPI_Comm_free(MPI_Comm *commp)
{
    return wrap_MPI_Comm_free(commp);
}  /* end of MPI_Comm_free() */


int
MPI_Comm_group(MPI_Comm comm, MPI_Group *group)
{
    return wrap_MPI_Comm_group(comm, group);
}  /* end of MPI_Comm_group() */


int
MPI_Pack_size(int incount, MPI_Datatype datatype, MPI_Comm comm, int *size)
{
    return wrap_MPI_Pack_size(incount, datatype, comm, size);
}  /* end of MPI_Pack_size() */


int
MPI_Pack(const void *inbuf, int incount, MPI_Datatype datatype, void *outbuf, int outsize, int *position, MPI_Comm comm)
{
    return wrap_MPI_Pack(inbuf, incount, datatype, outbuf, outsize, position, comm);
}  /* end of MPI_Pack() */


int
MPI_Unpack(const void *inbuf, int insize, int *position, void *outbuf, int outcount, MPI_Datatype datatype, MPI_Comm comm)
{
    return wrap_MPI_Unpack(inbuf, insize, position, outbuf, outcount, datatype, comm);
}  /* end of MPI_Unpack() */


int
MPI_Group_free(MPI_Group *group)
{
    return wrap_MPI_Group_free(group);
}  /* end of MPI_Group_free() */


int
MPI_Group_incl(MPI_Group group, int n, const int *ranks, MPI_Group *group_out)
{
    return wrap_MPI_Group_incl(group, n, ranks, group_out);
}  /* end of MPI_Group_incl() */


int
MPI_Group_range_incl(MPI_Group group, int n, int ranges[][3], MPI_Group *newgroup)
{
    return wrap_MPI_Group_range_incl(group, n, ranges, newgroup);
}  /* end of MPI_Group_range_incl() */


int
MPI_Cancel(MPI_Request *request)
{
    return wrap_MPI_Cancel(request);
}  /* end of MPI_Cancel() */



int
MPI_Cart_create(MPI_Comm comm, int ndims, const int *dims, const int *periods, int reorder, MPI_Comm *comm_cart)
{
    return wrap_MPI_Cart_create(comm, ndims, dims, periods, reorder, comm_cart);
}  /* end of MPI_Cart_create() */



int
MPI_Cart_get(MPI_Comm comm, int maxdims, int *dims, int *periods, int *coords)
{
    return wrap_MPI_Cart_get(comm, maxdims, dims, periods, coords);
}  /* end of MPI_Cart_get() */



int
MPI_Cart_rank(MPI_Comm comm, const int *coords, int *rank)
{
    return wrap_MPI_Cart_rank(comm, coords, rank);
}  /* end of MPI_Cart_rank() */



int
MPI_Cart_shift(MPI_Comm comm, int direction, int disp, int *rank_source, int *rank_dest)
{
    return wrap_MPI_Cart_shift(comm, direction, disp, rank_source, rank_dest);
}  /* end of MPI_Cart_shift() */



int
MPI_Sendrecv(const void *sendbuf, int sendcount, MPI_Datatype sendtype, int dest, int sendtag,
        void *recvbuf, int recvcount, MPI_Datatype recvtype, int source, int recvtag,
        MPI_Comm comm, MPI_Status *status)
{
    return wrap_MPI_Sendrecv(sendbuf, sendcount, sendtype, dest, sendtag,
                recvbuf, recvcount, recvtype, source, recvtag, comm, status);
}  /* end of MPI_Sendrecv() */




/*
** -----------------------------------------------------------------------------------------------------------
** Below here are functions for which we do not need to do anything
*/
#ifdef EASY_FUNCTIONS

/* point-to-point */
int MPI_Type_indexed(int count, int *array_of_blocklengths, int *array_of_displacements, MPI_Datatype oldtype, MPI_Datatype *newtype)NOOP
int MPI_Type_commit(MPI_Datatype *datatype)NOOP

/* miscellany */
int MPI_Status_f2c(MPI_Fint *f_status, MPI_Status *c_status)NOOP
MPI_Fint MPI_Comm_c2f(MPI_Comm comm)NOOP
MPI_Op MPI_Op_f2c(MPI_Fint op)NOOP_OP
int MPI_Status_c2f(MPI_Status *c_status, MPI_Fint *f_status)NOOP
MPI_Request MPI_Request_f2c(MPI_Fint request)NOOP_REQUEST
MPI_Fint MPI_Request_c2f(MPI_Request request)NOOP
MPI_Fint MPI_Win_c2f(MPI_Win win)NOOP
int MPI_Win_create_errhandler(MPI_Win_errhandler_fn *function, MPI_Errhandler *errhandler)NOOP
MPI_Win MPI_Win_f2c(MPI_Fint win)NOOP_WIN
int MPI_Win_get_errhandler(MPI_Win win, MPI_Errhandler *errhandler)NOOP
int MPI_Win_set_errhandler(MPI_Win win, MPI_Errhandler errhandler)NOOP

/* Collective Communications */
int MPI_Op_create(MPI_User_function *function, int commute, MPI_Op *op)NOOP

/* Groups, Contexts, and Communicators */
int MPI_Comm_test_inter(MPI_Comm comm, int *flag)NOOP

/* Environmental Enquiry */
int MPI_Get_processor_name(char *name, int *resultlen)NOOP
int MPI_Initialized(int *flag)NOOP

#endif /* EASY_FUNCTIONS */




/*
** -----------------------------------------------------------------------------------------------------------
** Below are the MPI functions we have not written wrappers for. Some of them will not
** need a wrapper. We only need wrappers for those that take a communicator or
** a group. (Plus a few special MPI functions like MPI_Init() which we already have.)
*/
#define NOOP		{fprintf(stderr, "No wrapper exists yet for function %s! Aborting...\n", __func__); \
			return MPI_ERR_UNSUPPORTED_OPERATION;}

#define NOOP_COMM	{fprintf(stderr, "No wrapper exists yet for function %s! Aborting...\n", __func__); \
			return MPI_COMM_NULL;}


#define NOOP_FILE	{fprintf(stderr, "No wrapper exists yet for function %s! Aborting...\n", __func__); \
			return MPI_FILE_NULL;}

#define NOOP_GROUP	{fprintf(stderr, "No wrapper exists yet for function %s! Aborting...\n", __func__); \
			return MPI_GROUP_NULL;}

#define NOOP_INFO	{fprintf(stderr, "No wrapper exists yet for function %s! Aborting...\n", __func__); \
			return MPI_INFO_NULL;}

#define NOOP_OP		{fprintf(stderr, "No wrapper exists yet for function %s! Aborting...\n", __func__); \
			return MPI_OP_NULL;}

#define NOOP_REQUEST	{fprintf(stderr, "No wrapper exists yet for function %s! Aborting...\n", __func__); \
			return MPI_REQUEST_NULL;}

#define NOOP_DT		{fprintf(stderr, "No wrapper exists yet for function %s! Aborting...\n", __func__); \
			return (MPI_Datatype)NULL;}

#define NOOP_WIN	{fprintf(stderr, "No wrapper exists yet for function %s! Aborting...\n", __func__); \
			return MPI_WIN_NULL;}

#define NOOP_ERR	{fprintf(stderr, "No wrapper exists yet for function %s! Aborting...\n", __func__); \
			return MPI_ERRHANDLER_NULL;}


/* I need to define this, but I don't know why!!! */
int MPI_Error_string(int errorcode, char *string, int *resultlen)NOOP

/*
** MPI-1.1 from http://www.mpi-forum.org/docs/mpi-11-html/mpi-report.html
*/

/*
	FIXME: All those "no wrapper needed" below probably need to go into the EASY part above.
*/
/* Point-to-Point */
/* no wrapper needed: int MPI_Get_count(MPI_Status *status, MPI_Datatype datatype, int *count) */
int MPI_Bsend(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm)NOOP
int MPI_Ssend(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm)NOOP
/* no wrapper needed: int MPI_Buffer_attach(void *buffer, int size) */
/* no wrapper needed: int MPI_Buffer_detach(void *buffer, int *size) */
int MPI_Ibsend(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request)NOOP
int MPI_Issend(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request)NOOP
int MPI_Irsend(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request)NOOP
/* no wrapper needed: int MPI_Test(MPI_Request *request, int *flag, MPI_Status *status) */
int MPI_Request_free(MPI_Request *request)NOOP
int MPI_Waitany(int count, MPI_Request array_of_requests[], int *index, MPI_Status *status)NOOP
/* no wrapper needed: int MPI_Testany(int count, MPI_Request array_of_requests[], int *index, int *flag, MPI_Status *status) */
int MPI_Testall(int count, MPI_Request array_of_requests[], int *flag, MPI_Status array_of_statuses[])NOOP
int MPI_Waitsome(int incount, MPI_Request array_of_requests[], int *outcount, int array_of_indices[], MPI_Status array_of_statuses[])NOOP
int MPI_Testsome(int incount, MPI_Request array_of_requests[], int *outcount, int array_of_indices[], MPI_Status array_of_statuses[])NOOP
int MPI_Probe(int source, int tag, MPI_Comm comm, MPI_Status *status)NOOP
int MPI_Test_cancelled(const MPI_Status *status, int *flag)NOOP
int MPI_Send_init(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request)NOOP
int MPI_Bsend_init(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request)NOOP
int MPI_Ssend_init(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request)NOOP
int MPI_Rsend_init(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request)NOOP
int MPI_Recv_init(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Request *request)NOOP
int MPI_Start(MPI_Request *request)NOOP
int MPI_Startall(int count, MPI_Request *array_of_requests)NOOP
int MPI_Sendrecv_replace(void *buf, int count, MPI_Datatype datatype, int dest, int sendtag, int source, int recvtag, MPI_Comm comm, MPI_Status *status)NOOP
int MPI_Type_hvector(int count, int blocklength, MPI_Aint stride, MPI_Datatype oldtype, MPI_Datatype *newtype)NOOP
int MPI_Type_hindexed(int count, int *array_of_blocklengths, MPI_Aint *array_of_displacements, MPI_Datatype oldtype, MPI_Datatype *newtype)NOOP
/* no wrapper needed: int MPI_Address(void *location, MPI_Aint *address) */
int MPI_Type_extent(MPI_Datatype datatype, MPI_Aint *extent)NOOP
/* no wrapper needed: int MPI_Type_size(MPI_Datatype datatype, int *size) */
int MPI_Type_lb(MPI_Datatype datatype, MPI_Aint *displacement)NOOP
int MPI_Type_ub(MPI_Datatype datatype, MPI_Aint *displacement)NOOP
int MPI_Get_elements(const MPI_Status *status, MPI_Datatype datatype, int *count)NOOP

/* Collective Communications */
int MPI_Scatter(const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm)NOOP
int MPI_Op_free(MPI_Op *op)NOOP

/* Groups, Contexts, and Communicators */
int MPI_Group_size(MPI_Group group, int *size)NOOP
int MPI_Group_rank(MPI_Group group, int *rank)NOOP
int MPI_Group_translate_ranks(MPI_Group group1, int n, const int *ranks1, MPI_Group group2, int *ranks2)NOOP
int MPI_Group_compare(MPI_Group group1, MPI_Group group2, int *result)NOOP
int MPI_Group_union(MPI_Group group1, MPI_Group group2, MPI_Group *newgroup)NOOP
int MPI_Group_intersection(MPI_Group group1, MPI_Group group2, MPI_Group *newgroup)NOOP
int MPI_Group_difference(MPI_Group group1, MPI_Group group2, MPI_Group *newgroup)NOOP
int MPI_Group_excl(MPI_Group group, int n, const int *ranks, MPI_Group *newgroup)NOOP
int MPI_Group_range_excl(MPI_Group group, int n, int ranges[][3], MPI_Group *newgroup)NOOP
int MPI_Comm_compare(MPI_Comm comm1, MPI_Comm comm2, int *result)NOOP
int MPI_Comm_remote_size(MPI_Comm comm, int *size)NOOP
int MPI_Comm_remote_group(MPI_Comm comm, MPI_Group *group)NOOP
int MPI_Intercomm_create(MPI_Comm local_comm, int local_leader, MPI_Comm peer_comm, int remote_leader, int tag, MPI_Comm *newintercomm)NOOP
int MPI_Intercomm_merge(MPI_Comm intercomm, int high, MPI_Comm *newintracomm)NOOP
int MPI_Keyval_create(MPI_Copy_function *copy_fn, MPI_Delete_function *delete_fn, int *keyval, void *extra_state)NOOP
int MPI_Keyval_free(int *keyval)NOOP
int MPI_Attr_put(MPI_Comm comm, int keyval, void *attribute_val)NOOP
int MPI_Attr_get(MPI_Comm comm, int keyval, void *attribute_val, int *flag)NOOP
int MPI_Attr_delete(MPI_Comm comm, int keyval)NOOP

/* Process Tolpologies */
int MPI_Dims_create(int nnodes, int ndims, int *dims)NOOP
int MPI_Graph_create(MPI_Comm comm_old, int nnodes, const int *index, const int *edges, int reorder, MPI_Comm *comm_graph)NOOP
int MPI_Topo_test(MPI_Comm comm, int *status)NOOP
int MPI_Graphdims_get(MPI_Comm comm, int *nnodes, int *nedges)NOOP
int MPI_Graph_get(MPI_Comm comm, int maxindex, int maxedges, int *index, int *edges)NOOP
int MPI_Cartdim_get(MPI_Comm comm, int *ndims)NOOP
int MPI_Cart_coords(MPI_Comm comm, int rank, int maxdims, int *coords)NOOP
int MPI_Graph_neighbors_count(MPI_Comm comm, int rank, int *nneighbors)NOOP
int MPI_Graph_neighbors(MPI_Comm comm, int rank, int maxneighbors, int *neighbors)NOOP
int MPI_Cart_sub(MPI_Comm comm, const int *remain_dims, MPI_Comm *newcomm)NOOP
int MPI_Cart_map(MPI_Comm comm, int ndims, const int *dims, const int *periods, int *newrank)NOOP
int MPI_Graph_map(MPI_Comm comm, int nnodes, const int *index, const int *edges, int *newrank)NOOP

/* Environmental Enquiry */
int MPI_Errhandler_create(MPI_Handler_function *function, MPI_Errhandler *errhandler)NOOP
int MPI_Errhandler_set(MPI_Comm comm, MPI_Errhandler errhandler)NOOP
int MPI_Errhandler_get(MPI_Comm comm, MPI_Errhandler *errhandler)NOOP
int MPI_Errhandler_free(MPI_Errhandler *errhandler)NOOP
int MPI_Error_class(int errorcode, int *errorclass)NOOP
double MPI_Wtick(void)NOOP

/* Profiling */
int MPI_Pcontrol(const int level, ...)NOOP


/*
** MPI-1.2 from http://www-unix.mcs.anl.gov/mpi/mpi-standard/mpi-report-2.0/mpi2-report.htm#Node0
*/
int MPI_Get_version(int *version, int *subversion)NOOP



/*
** MPI-2 from http://www-unix.mcs.anl.gov/mpi/mpi-standard/mpi-report-2.0/mpi2-report.htm#Node0
*/

/* Miscellany */
int MPI_Alloc_mem(MPI_Aint size, MPI_Info info, void *baseptr)NOOP
int MPI_Comm_create_errhandler(MPI_Comm_errhandler_fn *function, MPI_Errhandler *errhandler)NOOP
#if !defined (MPI_Comm_f2c)
    /* Some implementations define this one as a macro. Luckily, we don't need to do anything with it. */
/* no wrapper needed:     MPI_Comm MPI_Comm_f2c(MPI_Fint comm) */
#endif
int MPI_Comm_get_errhandler(MPI_Comm comm, MPI_Errhandler *errhandler)NOOP
int MPI_Comm_set_errhandler(MPI_Comm comm, MPI_Errhandler errhandler)NOOP
MPI_Fint MPI_File_c2f(MPI_File file)NOOP
int MPI_File_create_errhandler(MPI_File_errhandler_fn *function, MPI_Errhandler *errhandler)NOOP
MPI_File MPI_File_f2c(MPI_Fint file)NOOP_FILE
int MPI_File_get_errhandler(MPI_File file, MPI_Errhandler *errhandler)NOOP
int MPI_File_set_errhandler(MPI_File file, MPI_Errhandler errhandler)NOOP
int MPI_Finalized(int *flag)NOOP
int MPI_Free_mem(void *base)NOOP
int MPI_Get_address(const void *location, MPI_Aint *address)NOOP
#if !defined (MPI_Group_c2f)
    /* Some implementations define this one as a macro. Luckily, we don't need to do anything with it. */
    MPI_Fint MPI_Group_c2f(MPI_Group group)NOOP
#endif
#if !defined (MPI_Group_f2c)
    /* Some implementations define this one as a macro. Luckily, we don't need to do anything with it. */
    MPI_Group MPI_Group_f2c(MPI_Fint group)NOOP_GROUP
#endif
#if !defined (MPI_Info_c2f)
    /* Some implementations define this one as a macro. Luckily, we don't need to do anything with it. */
    MPI_Fint MPI_Info_c2f(MPI_Info info)NOOP
#endif
int MPI_Info_create(MPI_Info *info)NOOP
int MPI_Info_delete(MPI_Info info, const char *key)NOOP
int MPI_Info_dup(MPI_Info info, MPI_Info *newinfo)NOOP
#if !defined (MPI_Info_f2c)
    /* Some implementations define this one as a macro. Luckily, we don't need to do anything with it. */
    MPI_Info MPI_Info_f2c(MPI_Fint info)NOOP_INFO
#endif
int MPI_Info_free(MPI_Info *info)NOOP
int MPI_Info_get(MPI_Info info, const char *key, int valuelen, char *value, int *flag)NOOP
int MPI_Info_get_nkeys(MPI_Info info, int *nkeys)NOOP
int MPI_Info_get_nthkey(MPI_Info info, int n, char *key)NOOP
int MPI_Info_get_valuelen(MPI_Info info, const char *key, int *valuelen, int *flag)NOOP
int MPI_Info_set(MPI_Info info, const char *key, const char *value)NOOP
#if !defined (MPI_Op_c2f)
    /* Some implementations define this one as a macro. Luckily, we don't need to do anything with it. */
    MPI_Fint MPI_Op_c2f(MPI_Op op)NOOP
#endif
int MPI_Pack_external(const char *datarep, const void *inbuf, int incount, MPI_Datatype datatype, void *outbuf, MPI_Aint outsize, MPI_Aint *position)NOOP
int MPI_Pack_external_size(const char *datarep, int incount, MPI_Datatype datatype, MPI_Aint *size)NOOP
int MPI_Request_get_status(MPI_Request request, int *flag, MPI_Status *status)NOOP
#if !defined (MPI_Type_c2f)
    /* Some implementations define this one as a macro. Luckily, we don't need to do anything with it. */
    MPI_Fint MPI_Type_c2f(MPI_Datatype datatype)NOOP
#endif
int MPI_Type_create_darray(int size, int rank, int ndims, const int array_of_gsizes[], const int array_of_distribs[], const int array_of_dargs[], const int array_of_psizes[], int order, MPI_Datatype oldtype, MPI_Datatype *newtype)NOOP
int MPI_Type_create_hindexed(int count, const int array_of_blocklengths[], const MPI_Aint array_of_displacements[], MPI_Datatype oldtype, MPI_Datatype *newtype)NOOP
int MPI_Type_create_hvector(int count, int blocklength, MPI_Aint stride, MPI_Datatype oldtype, MPI_Datatype *newtype)NOOP
int MPI_Type_create_indexed_block(int count, int blocklength, const int array_of_displacements[], MPI_Datatype oldtype, MPI_Datatype *newtype)NOOP
int MPI_Type_create_resized(MPI_Datatype oldtype, MPI_Aint lb, MPI_Aint extent, MPI_Datatype *newtype)NOOP
int MPI_Type_create_struct(int count, const int array_of_blocklengths[], const MPI_Aint array_of_displacements[], const MPI_Datatype array_of_types[], MPI_Datatype *newtype)NOOP
int MPI_Type_create_subarray(int ndims, const int array_of_sizes[], const int array_of_subsizes[], const int array_of_starts[], int order, MPI_Datatype oldtype, MPI_Datatype *newtype)NOOP
#if !defined (MPI_Type_f2c)
    /* Some implementations define this one as a macro. Luckily, we don't need to do anything with it. */
/* no wrapper needed:     MPI_Datatype MPI_Type_f2c(MPI_Fint datatype) */
#endif
int MPI_Type_get_extent(MPI_Datatype datatype, MPI_Aint *lb, MPI_Aint *extent)NOOP
int MPI_Type_get_true_extent(MPI_Datatype datatype, MPI_Aint *true_lb, MPI_Aint *true_extent)NOOP
int MPI_Unpack_external(const char *datarep, const void *inbuf, MPI_Aint insize, MPI_Aint *position, void *outbuf, int outcount, MPI_Datatype datatype)NOOP


/* Process Creation and Management */
int MPI_Close_port(const char *port_name)NOOP
int MPI_Comm_accept(const char *port_name, MPI_Info info, int root, MPI_Comm comm, MPI_Comm *newcomm)NOOP
int MPI_Comm_connect(const char *port_name, MPI_Info info, int root, MPI_Comm comm, MPI_Comm *newcomm)NOOP
int MPI_Comm_disconnect(MPI_Comm *comm)NOOP
int MPI_Comm_get_parent(MPI_Comm *parent)NOOP
int MPI_Comm_join(int fd, MPI_Comm *intercomm)NOOP
int MPI_Comm_spawn(const char *command, char *argv[], int maxprocs, MPI_Info info, int root, MPI_Comm comm, MPI_Comm *intercomm, int array_of_errcodes[])NOOP
int MPI_Comm_spawn_multiple(int count, char *array_of_commands[], char **array_of_argv[], const int array_of_maxprocs[], const MPI_Info array_of_info[], int root, MPI_Comm comm, MPI_Comm *intercomm, int array_of_errcodes[])NOOP
int MPI_Lookup_name(const char *service_name, MPI_Info info, char *port_name)NOOP
int MPI_Open_port(MPI_Info info, char *port_name)NOOP
int MPI_Publish_name(const char *service_name, MPI_Info info, const char *port_name)NOOP
int MPI_Unpublish_name(const char *service_name, MPI_Info info, const char *port_name)NOOP

/* One-Sided Communications */
int MPI_Accumulate(const void *origin_addr, int origin_count, MPI_Datatype origin_datatype, int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Op op, MPI_Win win)NOOP
int MPI_Get(void *origin_addr, int origin_count, MPI_Datatype origin_datatype, int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Win win)NOOP
int MPI_Put(const void *origin_addr, int origin_count, MPI_Datatype origin_datatype, int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Win win)NOOP
int MPI_Win_complete(MPI_Win win)NOOP
int MPI_Win_create(void *base, MPI_Aint size, int disp_unit, MPI_Info info, MPI_Comm comm, MPI_Win *win)NOOP
int MPI_Win_fence(int assert, MPI_Win win)NOOP
int MPI_Win_free(MPI_Win *win)NOOP
int MPI_Win_get_group(MPI_Win win, MPI_Group *group)NOOP
int MPI_Win_lock(int lock_type, int rank, int assert, MPI_Win win)NOOP
int MPI_Win_post(MPI_Group group, int assert, MPI_Win win)NOOP
int MPI_Win_start(MPI_Group group, int assert, MPI_Win win)NOOP
int MPI_Win_test(MPI_Win win, int *flag)NOOP
int MPI_Win_unlock(int rank, MPI_Win win)NOOP
int MPI_Win_wait(MPI_Win win)NOOP

/* Extended Collective Operations */
int MPI_Alltoallw(const void *sendbuf, const int sendcounts[], const int sdispls[], const MPI_Datatype sendtypes[], void *recvbuf, const int recvcounts[], const int rdispls[], const MPI_Datatype recvtypes[], MPI_Comm comm)NOOP
int MPI_Exscan(const void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm)NOOP

/* External Interfaces */
int MPI_Add_error_class(int *errorclass)NOOP
int MPI_Add_error_code(int errorclass, int *errorcode)NOOP
int MPI_Add_error_string(int errorcode, const char *string)NOOP
int MPI_Comm_call_errhandler(MPI_Comm comm, int errorcode)NOOP
int MPI_Comm_create_keyval(MPI_Comm_copy_attr_function *comm_copy_attr_fn, MPI_Comm_delete_attr_function *comm_delete_attr_fn, int *comm_keyval, void *extra_state)NOOP
int MPI_Comm_delete_attr(MPI_Comm comm, int comm_keyval)NOOP
int MPI_Comm_free_keyval(int *comm_keyval)NOOP
int MPI_Comm_get_attr(MPI_Comm comm, int comm_keyval, void *attribute_val, int *flag)NOOP
int MPI_Comm_get_name(MPI_Comm comm, char *comm_name, int *resultlen)NOOP
int MPI_Comm_set_attr(MPI_Comm comm, int comm_keyval, void *attribute_val)NOOP
int MPI_Comm_set_name(MPI_Comm comm, const char *comm_name)NOOP
int MPI_File_call_errhandler(MPI_File fh, int errorcode)NOOP
int MPI_Grequest_complete(MPI_Request request)NOOP
int MPI_Grequest_start(MPI_Grequest_query_function *query_fn, MPI_Grequest_free_function *free_fn, MPI_Grequest_cancel_function *cancel_fn, void *extra_state, MPI_Request *request)NOOP
int MPI_Init_thread(int *argc, char **argv[], int required, int *provided)NOOP
int MPI_Is_thread_main(int *flag)NOOP
int MPI_Query_thread(int *provided)NOOP
int MPI_Status_set_cancelled(MPI_Status *status, int flag)NOOP
int MPI_Status_set_elements(MPI_Status *status, MPI_Datatype datatype, int count)NOOP
int MPI_Type_create_keyval(MPI_Type_copy_attr_function *type_copy_attr_fn, MPI_Type_delete_attr_function *type_delete_attr_fn, int *type_keyval, void *extra_state)NOOP
int MPI_Type_delete_attr(MPI_Datatype type, int type_keyval)NOOP
int MPI_Type_dup(MPI_Datatype type, MPI_Datatype *newtype)NOOP
int MPI_Type_free_keyval(int *type_keyval)NOOP
int MPI_Type_get_attr(MPI_Datatype type, int type_keyval, void *attribute_val, int *flag)NOOP
int MPI_Type_get_contents(MPI_Datatype datatype, int max_integers, int max_addresses, int max_datatypes, int array_of_integers[], MPI_Aint array_of_addresses[], MPI_Datatype array_of_datatypes[])NOOP
int MPI_Type_get_envelope(MPI_Datatype datatype, int *num_integers, int *num_addresses, int *num_datatypes, int *combiner)NOOP
int MPI_Type_get_name(MPI_Datatype type, char *type_name, int *resultlen)NOOP
int MPI_Type_set_attr(MPI_Datatype type, int type_keyval, void *attribute_val)NOOP
int MPI_Type_set_name(MPI_Datatype type, const char *type_name)NOOP
int MPI_Win_call_errhandler(MPI_Win win, int errorcode)NOOP
int MPI_Win_create_keyval(MPI_Win_copy_attr_function *win_copy_attr_fn, MPI_Win_delete_attr_function *win_delete_attr_fn, int *win_keyval, void *extra_state)NOOP
int MPI_Win_delete_attr(MPI_Win win, int win_keyval)NOOP
int MPI_Win_free_keyval(int *win_keyval)NOOP
int MPI_Win_get_attr(MPI_Win win, int win_keyval, void *attribute_val, int *flag)NOOP
int MPI_Win_get_name(MPI_Win win, char *win_name, int *resultlen)NOOP
int MPI_Win_set_attr(MPI_Win win, int win_keyval, void *attribute_val)NOOP
int MPI_Win_set_name(MPI_Win win, const char *win_name)NOOP

/* I/O */
int MPI_File_close(MPI_File *fh)NOOP
int MPI_File_delete(const char *filename, MPI_Info info)NOOP
int MPI_File_get_amode(MPI_File fh, int *amode)NOOP
int MPI_File_get_atomicity(MPI_File fh, int *flag)NOOP
int MPI_File_get_byte_offset(MPI_File fh, MPI_Offset offset, MPI_Offset *disp)NOOP
int MPI_File_get_group(MPI_File fh, MPI_Group *group)NOOP
int MPI_File_get_info(MPI_File fh, MPI_Info *info_used)NOOP
int MPI_File_get_position(MPI_File fh, MPI_Offset *offset)NOOP
int MPI_File_get_position_shared(MPI_File fh, MPI_Offset *offset)NOOP
int MPI_File_get_size(MPI_File fh, MPI_Offset *size)NOOP
int MPI_File_get_type_extent(MPI_File fh, MPI_Datatype datatype, MPI_Aint *extent)NOOP
int MPI_File_get_view(MPI_File fh, MPI_Offset *disp, MPI_Datatype *etype, MPI_Datatype *filetype, char *datarep)NOOP
int MPI_File_iread(MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Request *request)NOOP
int MPI_File_iread_at(MPI_File fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPI_Request *request)NOOP
int MPI_File_iread_shared(MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Request *request)NOOP
int MPI_File_iwrite(MPI_File fh, const void *buf, int count, MPI_Datatype datatype, MPI_Request *request)NOOP
int MPI_File_iwrite_at(MPI_File fh, MPI_Offset offset, const void *buf, int count, MPI_Datatype datatype, MPI_Request *request)NOOP
int MPI_File_iwrite_shared(MPI_File fh, const void *buf, int count, MPI_Datatype datatype, MPI_Request *request)NOOP
int MPI_File_preallocate(MPI_File fh, MPI_Offset size)NOOP
int MPI_File_read(MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status)NOOP
int MPI_File_read_all(MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status)NOOP
int MPI_File_read_all_begin(MPI_File fh, void *buf, int count, MPI_Datatype datatype)NOOP
int MPI_File_read_all_end(MPI_File fh, void *buf, MPI_Status *status)NOOP
int MPI_File_read_at(MPI_File fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPI_Status *status)NOOP
int MPI_File_read_at_all(MPI_File fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPI_Status *status)NOOP
int MPI_File_read_at_all_begin(MPI_File fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype)NOOP
int MPI_File_read_at_all_end(MPI_File fh, void *buf, MPI_Status *status)NOOP
int MPI_File_read_ordered(MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status)NOOP
int MPI_File_read_ordered_begin(MPI_File fh, void *buf, int count, MPI_Datatype datatype)NOOP
int MPI_File_read_ordered_end(MPI_File fh, void *buf, MPI_Status *status)NOOP
int MPI_File_read_shared(MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status)NOOP
int MPI_File_seek(MPI_File fh, MPI_Offset offset, int whence)NOOP
int MPI_File_seek_shared(MPI_File fh, MPI_Offset offset, int whence)NOOP
int MPI_File_set_atomicity(MPI_File fh, int flag)NOOP
int MPI_File_set_info(MPI_File fh, MPI_Info info)NOOP
int MPI_File_set_size(MPI_File fh, MPI_Offset size)NOOP
int MPI_File_set_view(MPI_File fh, MPI_Offset disp, MPI_Datatype etype, MPI_Datatype filetype, const char *datarep, MPI_Info info)NOOP
int MPI_File_sync(MPI_File fh)NOOP
int MPI_File_write(MPI_File fh, const void *buf, int count, MPI_Datatype datatype, MPI_Status *status)NOOP
int MPI_File_write_all(MPI_File fh, const void *buf, int count, MPI_Datatype datatype, MPI_Status *status)NOOP
int MPI_File_write_all_begin(MPI_File fh, const void *buf, int count, MPI_Datatype datatype)NOOP
int MPI_File_write_all_end(MPI_File fh, const void *buf, MPI_Status *status)NOOP
int MPI_File_write_at(MPI_File fh, MPI_Offset offset, const void *buf, int count, MPI_Datatype datatype, MPI_Status *status)NOOP
int MPI_File_write_at_all(MPI_File fh, MPI_Offset offset, const void *buf, int count, MPI_Datatype datatype, MPI_Status *status)NOOP
int MPI_File_write_at_all_begin(MPI_File fh, MPI_Offset offset, const void *buf, int count, MPI_Datatype datatype)NOOP
int MPI_File_write_at_all_end(MPI_File fh, const void *buf, MPI_Status *status)NOOP
int MPI_File_write_ordered(MPI_File fh, const void *buf, int count, MPI_Datatype datatype, MPI_Status *status)NOOP
int MPI_File_write_ordered_begin(MPI_File fh, const void *buf, int count, MPI_Datatype datatype)NOOP
int MPI_File_write_ordered_end(MPI_File fh, const void *buf, MPI_Status *status)NOOP
int MPI_File_write_shared(MPI_File fh, const void *buf, int count, MPI_Datatype datatype, MPI_Status *status)NOOP
int MPI_Register_datarep(const char *datarep, MPI_Datarep_conversion_function *read_conversion_fn, MPI_Datarep_conversion_function *write_conversion_fn, MPI_Datarep_extent_function *dtype_file_extent_fn, void *extra_state)NOOP

/* Language Bindings */
int MPI_Type_create_f90_complex(int p, int r, MPI_Datatype *newtype)NOOP
int MPI_Type_create_f90_integer(int r, MPI_Datatype *newtype)NOOP
int MPI_Type_create_f90_real(int p, int r, MPI_Datatype *newtype)NOOP
int MPI_Type_match_size(int typeclass, int size, MPI_Datatype *type)NOOP

/* ??? */
#if !defined (MPI_Errhandler_f2c)
    /* Some implementations define this one as a macro. Luckily, we don't need to do anything with it. */
    MPI_Errhandler MPI_Errhandler_f2c(MPI_Fint errhandler)NOOP_ERR
#endif
#if !defined (MPI_Errhandler_c2f)
    /* Some implementations define this one as a macro. Luckily, we don't need to do anything with it. */
    MPI_Fint MPI_Errhandler_c2f(MPI_Errhandler errhandler)NOOP
#endif
