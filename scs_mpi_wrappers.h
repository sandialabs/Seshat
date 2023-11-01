/*
** Copyright (2007) Sandia Corporation. Under the terms of Contract DE-AC04-94AL85000
** with Sandia Corporation, the U.S. Government retains certain rights in this software.
**
** Seshat
** $Id: scs_mpi_wrappers.h,v 1.3 2007/11/09 23:48:16 rolf Exp $
** Rolf Riesen (rolf@sandia.gov), November 2007, Sandia National Laboratories
*/

#ifndef _SCS_MPI_WRAPPERS_H_
#define _SCS_MPI_WRAPPERS_H_

#include <mpi.h>



/*
** MPI-1.1 from http://www.mpi-forum.org/docs/mpi-11-html/mpi-report.html
*/
int wrap_MPI_Init(int *argc, char **argv[]);
int wrap_MPI_Finalize(void);
int wrap_MPI_Abort(MPI_Comm comm, int errorcode);
double wrap_MPI_Wtime(void);


/* Point-to-Point */
int wrap_MPI_Send(const void *data, int len, MPI_Datatype dt, int dest, int tag, MPI_Comm comm);
int wrap_MPI_Recv(void *data, int len, MPI_Datatype dt, int src, int tag, MPI_Comm comm, MPI_Status *status);
int wrap_MPI_Isend(const void *data, int len, MPI_Datatype dt, int dest, int tag, MPI_Comm comm, MPI_Request *req);
int wrap_MPI_Irecv(void *data, int len, MPI_Datatype dt, int src, int tag, MPI_Comm comm, MPI_Request *req);
int wrap_MPI_Rsend(const void *data, int len, MPI_Datatype dt, int dest, int tag, MPI_Comm comm);
/* no wrapper needed: int MPI_Get_count(MPI_Status *status, MPI_Datatype datatype, int *count) */
int wrap_MPI_Bsend(void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm);
int wrap_MPI_Ssend(void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm);
/* no wrapper needed: int MPI_Buffer_attach(void *buffer, int size) */
/* no wrapper needed: int MPI_Buffer_detach(void *buffer, int *size) */
int wrap_MPI_Ibsend(void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request);
int wrap_MPI_Issend(void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request);
int wrap_MPI_Irsend(void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request);
/* no wrapper needed: int MPI_Test(MPI_Request *request, int *flag, MPI_Status *status) */
int wrap_MPI_Request_free(MPI_Request *request);
int wrap_MPI_Waitall(int count, MPI_Request array_of_requests[], MPI_Status array_of_statuses[]);
int wrap_MPI_Wait(MPI_Request *request, MPI_Status *status);
int wrap_MPI_Waitany(int count, MPI_Request array_of_requests[], int *index, MPI_Status *status);
/* no wrapper needed: int MPI_Testany(int count, MPI_Request array_of_requests[], int *index, int *flag, MPI_Status *status) */
int wrap_MPI_Testall(int count, MPI_Request array_of_requests[], int *flag, MPI_Status array_of_statuses[]);
int wrap_MPI_Waitsome(int incount, MPI_Request array_of_requests[], int *outcount, int array_of_indices[], MPI_Status array_of_statuses[]);
int wrap_MPI_Testsome(int incount, MPI_Request array_of_requests[], int *outcount, int array_of_indices[], MPI_Status array_of_statuses[]);
int wrap_MPI_Probe(int source, int tag, MPI_Comm comm, MPI_Status *status);
int wrap_MPI_Iprobe(int source, int tag, MPI_Comm comm, int *flag, MPI_Status *status);
int wrap_MPI_Test_cancelled(MPI_Status *status, int *flag);
int wrap_MPI_Send_init(void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request);
int wrap_MPI_Bsend_init(void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request);
int wrap_MPI_Ssend_init(void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request);
int wrap_MPI_Rsend_init(void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request);
int wrap_MPI_Recv_init(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Request *request);
int wrap_MPI_Start(MPI_Request *request);
int wrap_MPI_Startall(int count, MPI_Request *array_of_requests);
int wrap_MPI_Sendrecv(const void *sendbuf, int sendcount, MPI_Datatype sendtype, int dest, int sendtag, void *recvbuf, int recvcount, MPI_Datatype recvtype, int source, int recvtag, MPI_Comm comm, MPI_Status *status);
int wrap_MPI_Sendrecv_replace(void *buf, int count, MPI_Datatype datatype, int dest, int sendtag, int source, int recvtag, MPI_Comm comm, MPI_Status *status);
int wrap_MPI_Type_contiguous(int count, MPI_Datatype oldtype, MPI_Datatype *newtype);
int wrap_MPI_Type_free(MPI_Datatype *datatype);
int wrap_MPI_Type_struct(int count, int *array_of_blocklengths, MPI_Aint *array_of_displacements, MPI_Datatype *array_of_types, MPI_Datatype *newtype);
int wrap_MPI_Type_vector(int count, int blocklength, int stride, MPI_Datatype oldtype, MPI_Datatype *newtype);
int wrap_MPI_Type_hvector(int count, int blocklength, MPI_Aint stride, MPI_Datatype oldtype, MPI_Datatype *newtype);
int wrap_MPI_Type_hindexed(int count, int *array_of_blocklengths, MPI_Aint *array_of_displacements, MPI_Datatype oldtype, MPI_Datatype *newtype);
/* no wrapper needed: int MPI_Address(void *location, MPI_Aint *address) */
int wrap_MPI_Type_extent(MPI_Datatype datatype, MPI_Aint *extent);
/* no wrapper needed: int MPI_Type_size(MPI_Datatype datatype, int *size) */
int wrap_MPI_Type_lb(MPI_Datatype datatype, MPI_Aint *displacement);
int wrap_MPI_Type_ub(MPI_Datatype datatype, MPI_Aint *displacement);
int wrap_MPI_Get_elements(MPI_Status *status, MPI_Datatype datatype, int *count);
int wrap_MPI_Pack_size(int incount, MPI_Datatype datatype, MPI_Comm comm, int *size);
int wrap_MPI_Pack(const void *inbuf, int incount, MPI_Datatype datatype, void *outbuf, int outsize, int *position, MPI_Comm comm);
int wrap_MPI_Unpack(const void *inbuf, int insize, int *position, void *outbuf, int outcount, MPI_Datatype datatype, MPI_Comm comm);
int wrap_MPI_Cancel(MPI_Request *request);

/* Collective Communications */
int wrap_MPI_Barrier(MPI_Comm comm);
int wrap_MPI_Allreduce(const void *sendbuf, void *recvbuf, int count, MPI_Datatype dt, MPI_Op op, MPI_Comm comm);
int wrap_MPI_Alltoall(const void *sendbuf, int sendcnt, MPI_Datatype dt1, void *recvbuf, int recvcnt, MPI_Datatype dt2, MPI_Comm comm);
int wrap_MPI_Alltoallv(const void *sendbuf, const int *sendcnts, const int *sdispls, MPI_Datatype dt1, void *recvbuf, const int *recvcnts, const int *rdispls, MPI_Datatype dt2, MPI_Comm comm);
int wrap_MPI_Reduce(const void *sendbuf, void *recvbuf, int count, MPI_Datatype dt, MPI_Op op, int root, MPI_Comm comm);
int wrap_MPI_Bcast(void *buffer, int count, MPI_Datatype dt, int root, MPI_Comm comm);
int wrap_MPI_Allgather(const void *sendbuf, int sendcount, MPI_Datatype dt1, void *recvbuf, int recvcnt, MPI_Datatype dt2, MPI_Comm comm);
int wrap_MPI_Allgatherv(const void *sendbuf, int sendcnt, MPI_Datatype dt1, void *recvbuf, const int *recvcounts, const int *rdispls, MPI_Datatype dt2, MPI_Comm comm);
int wrap_MPI_Gatherv(const void *sendbuf, int sendcnt, MPI_Datatype dt1, void *recvbuf, const int *recvcounts, const int *rdispls, MPI_Datatype dt2, int root, MPI_Comm comm);
int wrap_MPI_Gather(const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm);
int wrap_MPI_Scatter(const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm);
int wrap_MPI_Scatterv(const void *sendbuf, const int *sendcnts, const int *sdispls, MPI_Datatype dt1, void *recvbuf, int recvcount, MPI_Datatype dt2, int root, MPI_Comm comm);
int wrap_MPI_Op_create(MPI_User_function *function, int commute, MPI_Op *op);
int wrap_MPI_Op_free(MPI_Op *op);
int wrap_MPI_Reduce_scatter(const void *sendbuf, void *recvbuf, const int *recvcounts, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm);
int wrap_MPI_Scan(const void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm);

/* Groups, Contexts, and Communicators */
int wrap_MPI_Group_free(MPI_Group *group);
int wrap_MPI_Group_incl(MPI_Group group, int n, const int *ranks, MPI_Group *group_out);
int wrap_MPI_Group_range_incl(MPI_Group group, int n, int ranges[][3], MPI_Group *newgroup);
int wrap_MPI_Group_size(MPI_Group group, int *size);
int wrap_MPI_Group_rank(MPI_Group group, int *rank);
int wrap_MPI_Group_translate_ranks(MPI_Group group1, int n, int *ranks1, MPI_Group group2, int *ranks2);
int wrap_MPI_Group_compare(MPI_Group group1, MPI_Group group2, int *result);
int wrap_MPI_Group_union(MPI_Group group1, MPI_Group group2, MPI_Group *newgroup);
int wrap_MPI_Group_intersection(MPI_Group group1, MPI_Group group2, MPI_Group *newgroup);
int wrap_MPI_Group_difference(MPI_Group group1, MPI_Group group2, MPI_Group *newgroup);
int wrap_MPI_Group_excl(MPI_Group group, int n, int *ranks, MPI_Group *newgroup);
int wrap_MPI_Group_range_excl(MPI_Group group, int n, int ranges[][3], MPI_Group *newgroup);
int wrap_MPI_Comm_size(MPI_Comm comm, int *size);
int wrap_MPI_Comm_rank(MPI_Comm comm, int *rank);
int wrap_MPI_Comm_dup(MPI_Comm comm, MPI_Comm *comm_out);
int wrap_MPI_Comm_split(MPI_Comm comm, int color, int key, MPI_Comm *comm_out);
int wrap_MPI_Comm_create(MPI_Comm comm, MPI_Group group, MPI_Comm *comm_out);
int wrap_MPI_Comm_free(MPI_Comm *commp);
int wrap_MPI_Comm_group(MPI_Comm comm, MPI_Group *group);
int wrap_MPI_Comm_compare(MPI_Comm comm1, MPI_Comm comm2, int *result);
int wrap_MPI_Comm_test_inter(MPI_Comm comm, int *flag);
int wrap_MPI_Comm_remote_size(MPI_Comm comm, int *size);
int wrap_MPI_Comm_remote_group(MPI_Comm comm, MPI_Group *group);
int wrap_MPI_Intercomm_create(MPI_Comm local_comm, int local_leader, MPI_Comm peer_comm, int remote_leader, int tag, MPI_Comm *newintercomm);
int wrap_MPI_Intercomm_merge(MPI_Comm intercomm, int high, MPI_Comm *newintracomm);
int wrap_MPI_Keyval_create(MPI_Copy_function *copy_fn, MPI_Delete_function *delete_fn, int *keyval, void *extra_state);
int wrap_MPI_Keyval_free(int *keyval);
int wrap_MPI_Attr_put(MPI_Comm comm, int keyval, void *attribute_val);
int wrap_MPI_Attr_get(MPI_Comm comm, int keyval, void *attribute_val, int *flag);
int wrap_MPI_Attr_delete(MPI_Comm comm, int keyval);

/* Process Tolpologies */
int wrap_MPI_Cart_create(MPI_Comm comm_old, int ndims, const int *dims, const int *periods, int reorder, MPI_Comm *comm_cart);
int wrap_MPI_Dims_create(int nnodes, int ndims, int *dims);
int wrap_MPI_Graph_create(MPI_Comm comm_old, int nnodes, int *index, int *edges, int reorder, MPI_Comm *comm_graph);
int wrap_MPI_Topo_test(MPI_Comm comm, int *status);
int wrap_MPI_Graphdims_get(MPI_Comm comm, int *nnodes, int *nedges);
int wrap_MPI_Graph_get(MPI_Comm comm, int maxindex, int maxedges, int *index, int *edges);
int wrap_MPI_Cartdim_get(MPI_Comm comm, int *ndims);
int wrap_MPI_Cart_get(MPI_Comm comm, int maxdims, int *dims, int *periods, int *coords);
int wrap_MPI_Cart_rank(MPI_Comm comm, const int *coords, int *rank);
int wrap_MPI_Cart_coords(MPI_Comm comm, int rank, int maxdims, int *coords);
int wrap_MPI_Graph_neighbors_count(MPI_Comm comm, int rank, int *nneighbors);
int wrap_MPI_Graph_neighbors(MPI_Comm comm, int rank, int maxneighbors, int *neighbors);
int wrap_MPI_Cart_shift(MPI_Comm comm, int direction, int disp, int *rank_source, int *rank_dest);
int wrap_MPI_Cart_sub(MPI_Comm comm, int *remain_dims, MPI_Comm *newcomm);
int wrap_MPI_Cart_map(MPI_Comm comm, int ndims, int *dims, int *periods, int *newrank);
int wrap_MPI_Graph_map(MPI_Comm comm, int nnodes, int *index, int *edges, int *newrank);

/* Environmental Enquiry */
int wrap_MPI_Get_processor_name(char *name, int *resultlen);
int wrap_MPI_Errhandler_create(MPI_Handler_function *function, MPI_Errhandler *errhandler);
int wrap_MPI_Errhandler_set(MPI_Comm comm, MPI_Errhandler errhandler);
int wrap_MPI_Errhandler_get(MPI_Comm comm, MPI_Errhandler *errhandler);
int wrap_MPI_Errhandler_free(MPI_Errhandler *errhandler);
int wrap_MPI_Error_class(int errorcode, int *errorclass);
double wrap_MPI_Wtick(void);
int wrap_MPI_Initialized(int *flag);

/* Profiling */
int wrap_MPI_Pcontrol(const int level, ...);


/*
** MPI-1.2 from http://www-unix.mcs.anl.gov/mpi/mpi-standard/mpi-report-2.0/mpi2-report.htm#Node0
*/
int wrap_MPI_Get_version(int *version, int *subversion);



/*
** MPI-2 from http://www-unix.mcs.anl.gov/mpi/mpi-standard/mpi-report-2.0/mpi2-report.htm#Node0
*/

/* Miscellany */
int wrap_MPI_Alloc_mem(MPI_Aint size, MPI_Info info, void *baseptr);
int wrap_MPI_Comm_create_errhandler(MPI_Comm_errhandler_fn *function, MPI_Errhandler *errhandler);
#if !defined (MPI_Comm_f2c)
    /* Some implementations define this one as a macro. Luckily, we don't need to do anything with it. */
/* no wrapper needed:     MPI_Comm MPI_Comm_f2c(MPI_Fint comm) */
#endif
int wrap_MPI_Comm_get_errhandler(MPI_Comm comm, MPI_Errhandler *errhandler);
int wrap_MPI_Comm_set_errhandler(MPI_Comm comm, MPI_Errhandler errhandler);
MPI_Fint wrap_MPI_File_c2f(MPI_File file);
int wrap_MPI_File_create_errhandler(MPI_File_errhandler_fn *function, MPI_Errhandler *errhandler);
MPI_File wrap_MPI_File_f2c(MPI_Fint file);
int wrap_MPI_File_get_errhandler(MPI_File file, MPI_Errhandler *errhandler);
int wrap_MPI_File_set_errhandler(MPI_File file, MPI_Errhandler errhandler);
int wrap_MPI_Finalized(int *flag);
int wrap_MPI_Free_mem(void *base);
int wrap_MPI_Get_address(void *location, MPI_Aint *address);
#if !defined (MPI_Group_c2f)
    /* Some implementations define this one as a macro. Luckily, we don't need to do anything with it. */
    MPI_Fint wrap_MPI_Group_c2f(MPI_Group group);
#endif
#if !defined (MPI_Group_f2c)
    /* Some implementations define this one as a macro. Luckily, we don't need to do anything with it. */
    MPI_Group wrap_MPI_Group_f2c(MPI_Fint group);
#endif
#if !defined (MPI_Info_c2f)
    /* Some implementations define this one as a macro. Luckily, we don't need to do anything with it. */
    MPI_Fint wrap_MPI_Info_c2f(MPI_Info info);
#endif
int wrap_MPI_Info_create(MPI_Info *info);
int wrap_MPI_Info_delete(MPI_Info info, char *key);
int wrap_MPI_Info_dup(MPI_Info info, MPI_Info *newinfo);
#if !defined (MPI_Info_f2c)
    /* Some implementations define this one as a macro. Luckily, we don't need to do anything with it. */
    MPI_Info wrap_MPI_Info_f2c(MPI_Fint info);
#endif
int wrap_MPI_Info_free(MPI_Info *info);
int wrap_MPI_Info_get(MPI_Info info, char *key, int valuelen, char *value, int *flag);
int wrap_MPI_Info_get_nkeys(MPI_Info info, int *nkeys);
int wrap_MPI_Info_get_nthkey(MPI_Info info, int n, char *key);
int wrap_MPI_Info_get_valuelen(MPI_Info info, char *key, int *valuelen, int *flag);
int wrap_MPI_Info_set(MPI_Info info, char *key, char *value);
#if !defined (MPI_Op_c2f)
    /* Some implementations define this one as a macro. Luckily, we don't need to do anything with it. */
    MPI_Fint wrap_MPI_Op_c2f(MPI_Op op);
#endif
int wrap_MPI_Pack_external(char *datarep, void *inbuf, int incount, MPI_Datatype datatype, void *outbuf, MPI_Aint outsize, MPI_Aint *position);
int wrap_MPI_Pack_external_size(char *datarep, int incount, MPI_Datatype datatype, MPI_Aint *size);
int wrap_MPI_Request_get_status(MPI_Request request, int *flag, MPI_Status *status);
#if !defined (MPI_Type_c2f)
    /* Some implementations define this one as a macro. Luckily, we don't need to do anything with it. */
    MPI_Fint wrap_MPI_Type_c2f(MPI_Datatype datatype);
#endif
int wrap_MPI_Type_create_darray(int size, int rank, int ndims, int array_of_gsizes[], int array_of_distribs[], int array_of_dargs[], int array_of_psizes[], int order, MPI_Datatype oldtype, MPI_Datatype *newtype);
int wrap_MPI_Type_create_hindexed(int count, int array_of_blocklengths[], MPI_Aint array_of_displacements[], MPI_Datatype oldtype, MPI_Datatype *newtype);
int wrap_MPI_Type_create_hvector(int count, int blocklength, MPI_Aint stride, MPI_Datatype oldtype, MPI_Datatype *newtype);
int wrap_MPI_Type_create_indexed_block(int count, int blocklength, int array_of_displacements[], MPI_Datatype oldtype, MPI_Datatype *newtype);
int wrap_MPI_Type_create_resized(MPI_Datatype oldtype, MPI_Aint lb, MPI_Aint extent, MPI_Datatype *newtype);
int wrap_MPI_Type_create_struct(int count, int array_of_blocklengths[], MPI_Aint array_of_displacements[], MPI_Datatype array_of_types[], MPI_Datatype *newtype);
int wrap_MPI_Type_create_subarray(int ndims, int array_of_sizes[], int array_of_subsizes[], int array_of_starts[], int order, MPI_Datatype oldtype, MPI_Datatype *newtype);
#if !defined (MPI_Type_f2c)
    /* Some implementations define this one as a macro. Luckily, we don't need to do anything with it. */
/* no wrapper needed:     MPI_Datatype MPI_Type_f2c(MPI_Fint datatype) */
#endif
int wrap_MPI_Type_get_extent(MPI_Datatype datatype, MPI_Aint *lb, MPI_Aint *extent);
int wrap_MPI_Type_get_true_extent(MPI_Datatype datatype, MPI_Aint *true_lb, MPI_Aint *true_extent);
int wrap_MPI_Unpack_external(char *datarep, void *inbuf, MPI_Aint insize, MPI_Aint *position, void *outbuf, int outcount, MPI_Datatype datatype);


/* Process Creation and Management */
int wrap_MPI_Close_port(char *port_name);
int wrap_MPI_Comm_accept(char *port_name, MPI_Info info, int root, MPI_Comm comm, MPI_Comm *newcomm);
int wrap_MPI_Comm_connect(char *port_name, MPI_Info info, int root, MPI_Comm comm, MPI_Comm *newcomm);
int wrap_MPI_Comm_disconnect(MPI_Comm *comm);
int wrap_MPI_Comm_get_parent(MPI_Comm *parent);
int wrap_MPI_Comm_join(int fd, MPI_Comm *intercomm);
int wrap_MPI_Comm_spawn(char *command, char *argv[], int maxprocs, MPI_Info info, int root, MPI_Comm comm, MPI_Comm *intercomm, int array_of_errcodes[]);
int wrap_MPI_Comm_spawn_multiple(int count, char *array_of_commands[], char **array_of_argv[], int array_of_maxprocs[], MPI_Info array_of_info[], int root, MPI_Comm comm, MPI_Comm *intercomm, int array_of_errcodes[]);
int wrap_MPI_Lookup_name(char *service_name, MPI_Info info, char *port_name);
int wrap_MPI_Open_port(MPI_Info info, char *port_name);
int wrap_MPI_Publish_name(char *service_name, MPI_Info info, char *port_name);
int wrap_MPI_Unpublish_name(char *service_name, MPI_Info info, char *port_name);

/* One-Sided Communications */
int wrap_MPI_Accumulate(void *origin_addr, int origin_count, MPI_Datatype origin_datatype, int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Op op, MPI_Win win);
int wrap_MPI_Get(void *origin_addr, int origin_count, MPI_Datatype origin_datatype, int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Win win);
int wrap_MPI_Put(void *origin_addr, int origin_count, MPI_Datatype origin_datatype, int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Win win);
int wrap_MPI_Win_complete(MPI_Win win);
int wrap_MPI_Win_create(void *base, MPI_Aint size, int disp_unit, MPI_Info info, MPI_Comm comm, MPI_Win *win);
int wrap_MPI_Win_fence(int assert, MPI_Win win);
int wrap_MPI_Win_free(MPI_Win *win);
int wrap_MPI_Win_get_group(MPI_Win win, MPI_Group *group);
int wrap_MPI_Win_lock(int lock_type, int rank, int assert, MPI_Win win);
int wrap_MPI_Win_post(MPI_Group group, int assert, MPI_Win win);
int wrap_MPI_Win_start(MPI_Group group, int assert, MPI_Win win);
int wrap_MPI_Win_test(MPI_Win win, int *flag);
int wrap_MPI_Win_unlock(int rank, MPI_Win win);
int wrap_MPI_Win_wait(MPI_Win win);

/* Extended Collective Operations */
int wrap_MPI_Alltoallw(const void *sendbuf, int sendcounts[], int sdispls[], MPI_Datatype sendtypes[], void *recvbuf, int recvcounts[], int rdispls[], MPI_Datatype recvtypes[], MPI_Comm comm);
int wrap_MPI_Exscan(const void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm);

/* External Interfaces */
int wrap_MPI_Add_error_class(int *errorclass);
int wrap_MPI_Add_error_code(int errorclass, int *errorcode);
int wrap_MPI_Add_error_string(int errorcode, char *string);
int wrap_MPI_Comm_call_errhandler(MPI_Comm comm, int errorcode);
int wrap_MPI_Comm_create_keyval(MPI_Comm_copy_attr_function *comm_copy_attr_fn, MPI_Comm_delete_attr_function *comm_delete_attr_fn, int *comm_keyval, void *extra_state);
int wrap_MPI_Comm_delete_attr(MPI_Comm comm, int comm_keyval);
int wrap_MPI_Comm_free_keyval(int *comm_keyval);
int wrap_MPI_Comm_get_attr(MPI_Comm comm, int comm_keyval, void *attribute_val, int *flag);
int wrap_MPI_Comm_get_name(MPI_Comm comm, char *comm_name, int *resultlen);
int wrap_MPI_Comm_set_attr(MPI_Comm comm, int comm_keyval, void *attribute_val);
int wrap_MPI_Comm_set_name(MPI_Comm comm, char *comm_name);
int wrap_MPI_File_call_errhandler(MPI_File fh, int errorcode);
int wrap_MPI_Grequest_complete(MPI_Request request);
int wrap_MPI_Grequest_start(MPI_Grequest_query_function *query_fn, MPI_Grequest_free_function *free_fn, MPI_Grequest_cancel_function *cancel_fn, void *extra_state, MPI_Request *request);
int wrap_MPI_Init_thread(int *argc, char **argv[], int required, int *provided);
int wrap_MPI_Is_thread_main(int *flag);
int wrap_MPI_Query_thread(int *provided);
int wrap_MPI_Status_set_cancelled(MPI_Status *status, int flag);
int wrap_MPI_Status_set_elements(MPI_Status *status, MPI_Datatype datatype, int count);
int wrap_MPI_Type_create_keyval(MPI_Type_copy_attr_function *type_copy_attr_fn, MPI_Type_delete_attr_function *type_delete_attr_fn, int *type_keyval, void *extra_state);
int wrap_MPI_Type_delete_attr(MPI_Datatype type, int type_keyval);
int wrap_MPI_Type_dup(MPI_Datatype type, MPI_Datatype *newtype);
int wrap_MPI_Type_free_keyval(int *type_keyval);
int wrap_MPI_Type_get_attr(MPI_Datatype type, int type_keyval, void *attribute_val, int *flag);
int wrap_MPI_Type_get_contents(MPI_Datatype datatype, int max_integers, int max_addresses, int max_datatypes, int array_of_integers[], MPI_Aint array_of_addresses[], MPI_Datatype array_of_datatypes[]);
int wrap_MPI_Type_get_envelope(MPI_Datatype datatype, int *num_integers, int *num_addresses, int *num_datatypes, int *combiner);
int wrap_MPI_Type_get_name(MPI_Datatype type, char *type_name, int *resultlen);
int wrap_MPI_Type_set_attr(MPI_Datatype type, int type_keyval, void *attribute_val);
int wrap_MPI_Type_set_name(MPI_Datatype type, char *type_name);
int wrap_MPI_Win_call_errhandler(MPI_Win win, int errorcode);
int wrap_MPI_Win_create_keyval(MPI_Win_copy_attr_function *win_copy_attr_fn, MPI_Win_delete_attr_function *win_delete_attr_fn, int *win_keyval, void *extra_state);
int wrap_MPI_Win_delete_attr(MPI_Win win, int win_keyval);
int wrap_MPI_Win_free_keyval(int *win_keyval);
int wrap_MPI_Win_get_attr(MPI_Win win, int win_keyval, void *attribute_val, int *flag);
int wrap_MPI_Win_get_name(MPI_Win win, char *win_name, int *resultlen);
int wrap_MPI_Win_set_attr(MPI_Win win, int win_keyval, void *attribute_val);
int wrap_MPI_Win_set_name(MPI_Win win, char *win_name);

/* I/O */
int wrap_MPI_File_open(MPI_Comm comm, const char *filename, int amode, MPI_Info info, MPI_File *fh);
int wrap_MPI_File_close(MPI_File *fh);
int wrap_MPI_File_delete(char *filename, MPI_Info info);
int wrap_MPI_File_get_amode(MPI_File fh, int *amode);
int wrap_MPI_File_get_atomicity(MPI_File fh, int *flag);
int wrap_MPI_File_get_byte_offset(MPI_File fh, MPI_Offset offset, MPI_Offset *disp);
int wrap_MPI_File_get_group(MPI_File fh, MPI_Group *group);
int wrap_MPI_File_get_info(MPI_File fh, MPI_Info *info_used);
int wrap_MPI_File_get_position(MPI_File fh, MPI_Offset *offset);
int wrap_MPI_File_get_position_shared(MPI_File fh, MPI_Offset *offset);
int wrap_MPI_File_get_size(MPI_File fh, MPI_Offset *size);
int wrap_MPI_File_get_type_extent(MPI_File fh, MPI_Datatype datatype, MPI_Aint *extent);
int wrap_MPI_File_get_view(MPI_File fh, MPI_Offset *disp, MPI_Datatype *etype, MPI_Datatype *filetype, char *datarep);
int wrap_MPI_File_iread(MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Request *request);
int wrap_MPI_File_iread_at(MPI_File fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPI_Request *request);
int wrap_MPI_File_iread_shared(MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Request *request);
int wrap_MPI_File_iwrite(MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Request *request);
int wrap_MPI_File_iwrite_at(MPI_File fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPI_Request *request);
int wrap_MPI_File_iwrite_shared(MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Request *request);
int wrap_MPI_File_preallocate(MPI_File fh, MPI_Offset size);
int wrap_MPI_File_read(MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
int wrap_MPI_File_read_all(MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
int wrap_MPI_File_read_all_begin(MPI_File fh, void *buf, int count, MPI_Datatype datatype);
int wrap_MPI_File_read_all_end(MPI_File fh, void *buf, MPI_Status *status);
int wrap_MPI_File_read_at(MPI_File fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
int wrap_MPI_File_read_at_all(MPI_File fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
int wrap_MPI_File_read_at_all_begin(MPI_File fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype);
int wrap_MPI_File_read_at_all_end(MPI_File fh, void *buf, MPI_Status *status);
int wrap_MPI_File_read_ordered(MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
int wrap_MPI_File_read_ordered_begin(MPI_File fh, void *buf, int count, MPI_Datatype datatype);
int wrap_MPI_File_read_ordered_end(MPI_File fh, void *buf, MPI_Status *status);
int wrap_MPI_File_read_shared(MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
int wrap_MPI_File_seek(MPI_File fh, MPI_Offset offset, int whence);
int wrap_MPI_File_seek_shared(MPI_File fh, MPI_Offset offset, int whence);
int wrap_MPI_File_set_atomicity(MPI_File fh, int flag);
int wrap_MPI_File_set_info(MPI_File fh, MPI_Info info);
int wrap_MPI_File_set_size(MPI_File fh, MPI_Offset size);
int wrap_MPI_File_set_view(MPI_File fh, MPI_Offset disp, MPI_Datatype etype, MPI_Datatype filetype, char *datarep, MPI_Info info);
int wrap_MPI_File_sync(MPI_File fh);
int wrap_MPI_File_write(MPI_File fh, const void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
int wrap_MPI_File_write_all(MPI_File fh, const void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
int wrap_MPI_File_write_all_begin(MPI_File fh, const void *buf, int count, MPI_Datatype datatype);
int wrap_MPI_File_write_all_end(MPI_File fh, const void *buf, MPI_Status *status);
int wrap_MPI_File_write_at(MPI_File fh, MPI_Offset offset, const void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
int wrap_MPI_File_write_at_all(MPI_File fh, MPI_Offset offset, const void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
int wrap_MPI_File_write_at_all_begin(MPI_File fh, MPI_Offset offset, const void *buf, int count, MPI_Datatype datatype);
int wrap_MPI_File_write_at_all_end(MPI_File fh, const void *buf, MPI_Status *status);
int wrap_MPI_File_write_ordered(MPI_File fh, const void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
int wrap_MPI_File_write_ordered_begin(MPI_File fh, const void *buf, int count, MPI_Datatype datatype);
int wrap_MPI_File_write_ordered_end(MPI_File fh, void *buf, MPI_Status *status);
int wrap_MPI_File_write_shared(MPI_File fh, const void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
int wrap_MPI_Register_datarep(const char *datarep, MPI_Datarep_conversion_function *read_conversion_fn, MPI_Datarep_conversion_function *write_conversion_fn, MPI_Datarep_extent_function *dtype_file_extent_fn, void *extra_state);

/* Language Bindings */
int wrap_MPI_Type_create_f90_complex(int p, int r, MPI_Datatype *newtype);
int wrap_MPI_Type_create_f90_integer(int r, MPI_Datatype *newtype);
int wrap_MPI_Type_create_f90_real(int p, int r, MPI_Datatype *newtype);
int wrap_MPI_Type_match_size(int typeclass, int size, MPI_Datatype *type);

/* ??? */
#if !defined (MPI_Errhandler_f2c)
    /* Some implementations define this one as a macro. Luckily, we don't need to do anything with it. */
    MPI_Errhandler wrap_MPI_Errhandler_f2c(MPI_Fint errhandler);
#endif
#if !defined (MPI_Errhandler_c2f)
    /* Some implementations define this one as a macro. Luckily, we don't need to do anything with it. */
    MPI_Fint wrap_MPI_Errhandler_c2f(MPI_Errhandler errhandler);
#endif
#endif /* _SCS_MPI_WRAPPERS_H_ */
