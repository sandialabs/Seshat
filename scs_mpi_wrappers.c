/*
** Copyright (2007) Sandia Corporation. Under the terms of Contract DE-AC04-94AL85000
** with Sandia Corporation, the U.S. Government retains certain rights in this software.
**
** Seshat
** $Id: scs_mpi_wrappers.c,v 1.87 2008/05/30 22:52:45 rolf Exp $
** Rolf Riesen (rolf@sandia.gov), April 2006, Sandia National Laboratories
**
** These are our C wrappers. All languages call these.
*/

#include "scs.h"
#include "scs_mpi_wrappers.h"
#include <stdlib.h>
#include <unistd.h>


/*
** Local Globals
*/
/* These are cirtual times, and should correspond to native time, if CPU factor is 1.0 */
static _scs_vtime_t mpi_start_time= 0.0;     /* time at exit of MPI_Init() */
static _scs_vtime_t mpi_exit_time= 0.0;      /* time of last mpi function exit */
static _scs_vtime_t mpi_entry_time= 0.0;     /* time of current mpi function entry */
static _scs_vtime_t total_mpi_time= 0.0;     /* Total time spent within mpi functions */
static _scs_vtime_t total_cpu_time= 0.0;     /* Total time spent outside mpi functions */

static int in_MPI_call= FALSE;



/*
** substitute our app communicator whenever they use MPI_COMM_WORLD
*/
#define SET_COMM() \
    if (MPI_COMM_WORLD == comm)    { \
	comm= _scs_app_comm; \
    }



/*
** Get the MPI function entry time, but adjust it if necessary
** according to the CPU multiplication factor.
*/
static __inline__ _scs_vtime_t
MPI_entry(const char *func)
{

_scs_vtime_t last_cpu_time;
_scs_vtime_t t0;


    t0= _scs_get_lvtime();
    Debug(DBG_MPI2, "%10.6f Entering %s()\n", t0, func);
    if (in_MPI_call)   {
	return t0;
    }

    in_MPI_call= TRUE;
    last_cpu_time= (t0 - mpi_exit_time) * _scs_config.net_cpu_factor;
    total_cpu_time= total_cpu_time + last_cpu_time;

    /* Adjust the CPU time */
    mpi_entry_time= last_cpu_time + mpi_exit_time;
    _scs_set_lvtime(mpi_entry_time);
    // OUT_VTIME("", "Entering %s() t0 %9.6f, lvtime %9.6f, last_cpu %9.6f, mpi exit %9.6f, factor %6.3f\n", __func__, t0, _scs_get_lvtime(), last_cpu_time, mpi_exit_time, _scs_config.net_cpu_factor);

    return mpi_entry_time;

}  /* end of MPI_entry() */


/*
** At exit, set mpi_exit_time
*/
#define MPI_exit() \
    mpi_exit_time= _scs_get_lvtime(); \
    if (in_MPI_call)   { \
	total_mpi_time= total_mpi_time + mpi_exit_time - mpi_entry_time; \
	in_MPI_call= FALSE; \
    } \
    Debug(DBG_MPI2, "%10.6f Exiting from %s. rc %d\n", mpi_exit_time, __func__, rc);



/*
** -------------------------------------------------------------------------------------------------
** MPI function wrappers
*/
int
wrap_MPI_Init(int *argc, char **argv[])
{

int rc;


    _scs_MPI_initialized= TRUE;
    rc= _scs_sim_start(argc, argv);


    /*
    ** The atexit() call has to be here. The net sim node will call _scs_normal_exit() on its own.
    ** Some MPI implementation; e.g. MPICH will have additional processes that would also call
    ** _scs_normal_exit(). These processes never make it here, though, so it is safe to call
    ** atexit() here: Only the application processes will do that.
    **
    ** The reason for this atexit() is for applications that do not call MPI_Finalize(). We need to
    ** run at the end to write stats and so on.
    */
    if (atexit(_scs_normal_exit) != 0)   {
	Err("Warning: Could not register exit function\n", "");
    }

    mpi_start_time= _scs_get_lvtime();
    mpi_exit_time= _scs_get_lvtime();
    return rc;

}  /* end of wrap_MPI_Init() */


int
wrap_MPI_Finalize(void)
{

int rc= MPI_SUCCESS;
static int sent_done_event= FALSE;


    Debug(DBG_MPI, "_scs_MPI_initialized is %d\n", _scs_MPI_initialized);

    if (_scs_MPI_initialized == TRUE)   {
	_scs_MPI_initialized= FALSE;

	if (!sent_done_event)   {
	    sent_done_event= TRUE;

	    if (_scs_rank != _scs_net_sim_node)   {
		if (_scs_rank == 0)   {
		    if (_scs_config.net_reporting)   {
			OUT_VTIME("# ", "Node   open req   max reqd\n", "");
		    }
		}

		if (_scs_config.net_reporting)   {
		    _scs_vtime_stats();
		}

                if (_scs_config.net_reporting)   {
                    if (_scs_rank == 0)   {
			OUT_VTIME("# ", "Node   MPI start time   CPU time       MPI time       Ratio          Sum            =   Current - start\n", "");
		    }
                    // OUT_VTIME("", "Is this my executable? (6)\n", "");
                    OUT_VTIME("# ", "[%3d] %12.6f   %12.6f   %12.6f   1:%0.2f   =   %12.6f\n", _scs_rank,
                        mpi_start_time, total_cpu_time, total_mpi_time, total_mpi_time / total_cpu_time,
                        total_mpi_time + total_cpu_time, _scs_get_lvtime() - mpi_start_time);
		}

		_scs_event_done();

	    }
	}

	/* FIXME: Do we really need to call this three times? */
	Debug(DBG_MPI, "Calling _scs_MPI_Barrier(%d)\n", _scs_comm_world);
	_scs_MPI_Barrier(_scs_comm_world);

	Debug(DBG_MPI, "Calling _scs_MPI_Barrier(%d)\n", _scs_comm_world);
	_scs_MPI_Barrier(_scs_comm_world);

	Debug(DBG_MPI, "Calling _scs_MPI_Barrier(%d)\n", _scs_comm_world);
	_scs_MPI_Barrier(_scs_comm_world);
	
	Debug(DBG_MPI, "Calling _scs_MPI_Finalize\n", "");
	rc= _scs_MPI_Finalize();

	Debug(DBG_MPI, "Back from _scs_MPI_Finalize\n", "");
    }

    Debug(DBG_MPI, "Leaving. _scs_MPI_initialized is %d, sent_done_event is %d\n", _scs_MPI_initialized, sent_done_event);
    return rc;

}  /* end of wrap_MPI_Finalize() */


int
wrap_MPI_Wait(MPI_Request *request, MPI_Status *status)
{

int rc, len;
unsigned int token;
_scs_vtime_t t1, tp;
_scs_vtime_t tx, delta, tr;
_scs_vtime_t new_vtime;
static char errstr[MPI_MAX_ERROR_STRING];
int errlen;
MPI_Request saved_req;
int send;


    t1= MPI_entry(__func__);
    saved_req= *request;
    Debug(DBG_MPI2, "\n", "");
    rc= PMPI_Wait(request, status);
    if (rc != MPI_SUCCESS)   {
	MPI_Error_string(rc, errstr, &errlen);
	Warn("MPI ERROR! %s\n", errstr);
	return rc;
    }

    if (_scs_req_find(saved_req, &send, &tr, &len, &token))   {
	if (!send)   {
	    /* We only need to do this for receives... */
	    _scs_event_wait(&tx, &delta, token);
            if (len >= _scs_config.mpi_long_proto)
	      _scs_set_lvtime(MAX(MAX(tx, tr) + delta, t1));
            else
	      _scs_set_lvtime(MAX(tx + delta, t1));
	} else   {
	    /* Complete a non-blocking send */

	    tx= tr;         /* It's really the start of the send */

	    if (len >= _scs_config.mpi_long_proto)   {
		_scs_event_wait(&tp, &delta, token);
		new_vtime= MAX(MAX(tx, tp) + delta, t1);
	    } else   {
		new_vtime= t1 + _scs_model_ptop(len, _scs_config.net_bw_factor, _scs_config.net_lat_factor, FALSE);
	    }

	    _scs_set_lvtime(new_vtime);   
        }
    } else   {
	Err("Error: C MPI_Wait Missing pre-post! %p\n", "(void *)saved_req");
	_scs_abort();
    }

    MPI_exit();
    return rc;

}  /* end of wrap_MPI_Wait() */


int
wrap_MPI_Waitall(int count, MPI_Request array_of_requests[], MPI_Status array_of_statuses[])
{

int rc;
int i;
_scs_vtime_t t1, tp;
_scs_vtime_t tx, delta, tr;
_scs_vtime_t new_vtime;
static char errstr[MPI_MAX_ERROR_STRING];
int errlen;
MPI_Request *saved_reqs;
int send, len;
unsigned int token;


    t1= MPI_entry(__func__);
    Debug(DBG_MPI2, "Waiting for %d requests\n", count);
    saved_reqs= (MPI_Request *)malloc(count * sizeof(MPI_Request));
    if (saved_reqs == NULL)   {
	Err("Out of memory!\n", "");
	_scs_abort();
    }

    /*
    ** We need to save the actual requests, because MPI_Waitall() clears them
    ** as it matches them.
    */
    for (i= 0; i < count; i++)   {
	saved_reqs[i]= array_of_requests[i];
    }

    rc= PMPI_Waitall(count, array_of_requests, array_of_statuses);
    if (rc != MPI_SUCCESS)   {
	MPI_Error_string(rc, errstr, &errlen);
	Warn("MPI ERROR! %s\n", errstr);
	return rc;
    }

    for (i= 0; i < count; i++)   {
	if (_scs_req_find(saved_reqs[i], &send, &tr, &len, &token))   {
            if (token == 0)   {
                Err("No token attached, when there should be!", "");
                _scs_abort();
            }

	    if (!send)   {
                /* Complete a non-blocking receive */
		_scs_event_wait(&tx, &delta, token);
                if (len >= _scs_config.mpi_long_proto)   {
                    _scs_set_lvtime(MAX(MAX(tx, tr) + delta, t1));
                } else   {
                    _scs_set_lvtime(MAX(tx + delta, t1));
                }
	    } else   {
                /* Complete a non-blocking send */

                tx= tr;         /* It's really the start of the send */

                if (len >= _scs_config.mpi_long_proto)   {
                    _scs_event_wait(&tp, &delta, token);
                    new_vtime= MAX(MAX(tx, tp) + delta, t1);
                } else   {
                    new_vtime= t1 + _scs_model_ptop(len, _scs_config.net_bw_factor, _scs_config.net_lat_factor, FALSE);
                }

                _scs_set_lvtime(new_vtime);   
            }
	} else   {
	    Err("Error: C MPI_Waitall Missing pre-post %d/%d!\n", i, count);
            _scs_req_dump();
	    _scs_abort();
	}
    }

    free(saved_reqs);
    MPI_exit();
    return rc;

}  /* end of wrap_MPI_Waitall() */


int
wrap_MPI_Send(const void *data, int len, MPI_Datatype dt, int dest, int tag, MPI_Comm comm)
{

int rc, real_len;
_scs_vtime_t tx, tp, new_vtime;
_scs_vtime_t delta;
unsigned int token;
int collective= FALSE;
long int pos= 0;


    tx= MPI_entry(__func__);
    Debug(DBG_MPI2, "dest %d, %d objects, tag 0x%x, ...\n", dest, len, tag);
    Debug(DBG_MPI2, "MPI_COMM_WORLD %p, comm %p, _scs_app_comm %p\n", MPI_COMM_WORLD, comm, _scs_app_comm);
    SET_COMM();
    rc= PMPI_Send(data, len, dt, dest, tag, comm);

    /* Write message data if necessary */
    real_len= _scs_bytelen(dt, len);
    Debug(DBG_MPI2, "real len %d\n", real_len);
    if (_scs_msg_data_out && len > 0)   {
	pos= _scs_msg_data_write(data, real_len);
    }

    token= _scs_gen_token();
    _scs_event_send(tx, len, dt, dest, tag, token, collective, pos); 
 
    if (real_len >= _scs_config.mpi_long_proto)   {
	/* get post time of respective receive */
	_scs_event_wait(&tp, &delta, token);
	new_vtime= MAX(tx, tp) + delta;
    } else   {
        /* assumes offboard comm */
	new_vtime= tx + _scs_model_ptop(real_len, _scs_config.net_bw_factor, _scs_config.net_lat_factor, FALSE);
    }

    _scs_set_lvtime(new_vtime);   

    MPI_exit();
    return rc;

}  /* end of wrap_MPI_Send() */


int
wrap_MPI_Rsend(const void *data, int len, MPI_Datatype dt, int dest, int tag, MPI_Comm comm)
{

int rc, real_len;
_scs_vtime_t tx, tp, new_vtime;
double delta;
unsigned int token;
int collective= FALSE;
long int pos= 0;


    tx= MPI_entry(__func__);
    Debug(DBG_MPI2, "dest %d, len %d objects, tag 0x%x, ...\n", dest, len, tag);
    SET_COMM();
    rc= PMPI_Rsend(data, len, dt, dest, tag, comm);

    /* Write message data if necessary */
    real_len= _scs_bytelen(dt, len);
    if (_scs_msg_data_out && len > 0)   {
	pos= _scs_msg_data_write(data, real_len);
    }

    token= _scs_gen_token();
    _scs_event_send(tx, len, dt, dest, tag, token, collective, pos);
 
    if (real_len >= _scs_config.mpi_long_proto)   {
	/* get post time of respective receive */
	_scs_event_wait(&tp, &delta, token);
	new_vtime= MAX(tx, tp) + delta;
    } else   {
        /* assumes offboard comm */
	new_vtime= tx + _scs_model_ptop(real_len, _scs_config.net_bw_factor, _scs_config.net_lat_factor, FALSE);
    }

    _scs_set_lvtime(new_vtime); 

    MPI_exit();
    return rc;

}  /* end of wrap_MPI_Rsend() */


int
wrap_MPI_Isend(const void *data, int len, MPI_Datatype dt, int dest, int tag, MPI_Comm comm, MPI_Request *req)
{

int rc, real_len;
_scs_vtime_t tx;
unsigned int token;
int send= TRUE;
int collective= FALSE;
long int pos= 0;


    tx= MPI_entry(__func__);
    Debug(DBG_MPI2, "dest %d, len %d objects, tag 0x%x, ...\n", dest, len, tag);
    SET_COMM();

    rc= PMPI_Isend(data, len, dt, dest, tag, comm, req);

    real_len= _scs_bytelen(dt, len);
    token= _scs_gen_token();
    Debug(DBG_REQ, "[%2d] %s inserting request %p\n", _scs_rank, __func__, *req);
    _scs_req_insert(*req, send, tx, real_len, token);

    /* Write message data if necessary */
    if (_scs_msg_data_out && len > 0)   {
	pos= _scs_msg_data_write(data, real_len);
    }

    _scs_event_send(tx, len, dt, dest, tag, token, collective, pos);
    
    MPI_exit();
    return rc;

}  /* end of wrap_MPI_Isend() */


int
wrap_MPI_Recv(void *data, int len, MPI_Datatype dt, int src, int tag, MPI_Comm comm, MPI_Status *status)
{

int rc;
int real_len;
_scs_vtime_t tp;
_scs_vtime_t tx, delay;
unsigned int token;


    tp= MPI_entry(__func__);
    Debug(DBG_MPI2, "src %d, len %d objects, tag 0x%x, ...\n", src, len, tag);
    SET_COMM();

    /*
    ** FIXME
    ** Based on a discussion with Marc Snir, for wildcard receives, I should probably look at the
    ** event list first and then decide which MPI message to wait for.
    ** Probably OK as long as the simulator is serialized
    */
    rc= PMPI_Recv(data, len, dt, src, tag, comm, status);


    /* send post event to sim */
    token= _scs_gen_token();
    _scs_event_post(tp, src, tag, token);

    if (rc == MPI_SUCCESS)   {
        _scs_event_wait(&tx, &delay, token);

        real_len= _scs_bytelen(dt, len);
        if (real_len >= _scs_config.mpi_long_proto)   {
            /* message is synchronous */
            _scs_set_lvtime(MAX(tp, tx) + delay);
        } else   {
            _scs_set_lvtime(MAX(tx + delay, tp));
        }
    }
    MPI_exit();
    return rc;

}  /* end of wrap_MPI_Recv() */


int
wrap_MPI_Irecv(void *data, int len, MPI_Datatype dt, int src, int tag, MPI_Comm comm, MPI_Request *req)
{

int rc, real_len;
_scs_vtime_t tp;
unsigned int token;
int send= FALSE;


    tp= MPI_entry(__func__);
    Debug(DBG_MPI2, "src %d, len %d objects, tag 0x%x, ...\n", src, len, tag);
    SET_COMM();

    rc= PMPI_Irecv(data, len, dt, src, tag, comm, req);

    /* Insert it in local request queue */
    real_len= _scs_bytelen(dt, len);
    token= _scs_gen_token();
    Debug(DBG_REQ, "[%2d] %s inserting request %p\n", _scs_rank, __func__, *req);
    _scs_req_insert(*req, send, tp, real_len, token);

    /* Send post event to sim */
    _scs_event_post(tp, src, tag, token);

    MPI_exit();
    return rc;

}  /* end of wrap_MPI_Irecv() */


/*
** -------------------------------------------------------------------------------------------------
** Collectives
** -------------------------------------------------------------------------------------------------
*/
int
wrap_MPI_Barrier(MPI_Comm comm)
{

int rc;
_scs_vtime_t t1;
int root= 0;
int len= 0;


    t1= MPI_entry(__func__);

    /*
    ** Some MPI implementations call MPI_Barrier from within PMPI_Finalize() (e.g. Red Storm)
    ** In that case we need to use the real MPI_COMM_WORLD; i.e. skip SET_COM()
    */
    if (_scs_MPI_initialized)   {
	SET_COMM();
	rc= PMPI_Barrier(comm);
	_scs_event_exchange(t1, NULL, NULL, root, len, MPI_Barrier_TAG, MPI_BYTE);
    } else   {
	rc= PMPI_Barrier(comm);
    }

    MPI_exit();
    return rc;

}  /* end of wrap_MPI_Barrier() */


int
wrap_MPI_Allreduce(const void *sendbuf, void *recvbuf, int count, MPI_Datatype dt, MPI_Op op, MPI_Comm comm)
{

int rc;
_scs_vtime_t t1;
int root= 0;


    t1= MPI_entry(__func__);
    Debug(DBG_MPI2, "sndp %p, rcvp %p, cnt %d\n", sendbuf, recvbuf, count);

    if (_scs_net_sim_node >= 0)   {
	SET_COMM();
	rc= PMPI_Allreduce(sendbuf, recvbuf, count, dt, op, comm);

	_scs_event_exchange(t1, sendbuf, recvbuf, root, count, MPI_Allreduce_TAG, dt);
    } else   {
	/*
	** Some implementations; e.g. mpich on Liberty call MPI_Allreduce()
	** when we are in the middle of setting up the group communicators
	** in scs_node_sim_init_mpi() via the call to MPI_Comm_create(). This
	** test here is annoying, but necessary.
	*/
	rc= PMPI_Allreduce(sendbuf, recvbuf, count, dt, op, comm);
    }
    MPI_exit();
    return rc;

}  /* end of wrap_MPI_Allreduce() */


int
wrap_MPI_Alltoall(const void *sendbuf, int sendcnt, MPI_Datatype dt1, void *recvbuf, int recvcnt, MPI_Datatype dt2,
	MPI_Comm comm)
{

int rc;
_scs_vtime_t t1;
int root= 0;


    t1= MPI_entry(__func__);
    Debug(DBG_MPI2, "sndp %p, send cnt %d, rcvp %p, recv cnt %d\n", sendbuf, sendcnt, recvbuf, recvcnt);
    SET_COMM();
    rc= PMPI_Alltoall(sendbuf, sendcnt, dt1, recvbuf, recvcnt, dt2, comm);

    _scs_event_exchange(t1, sendbuf, recvbuf, root, sendcnt, MPI_Alltoall_TAG, dt1);
    MPI_exit();
    return rc;

}  /* end of wrap_MPI_Alltoall() */


int
wrap_MPI_Alltoallv(const void *sendbuf, const int *sendcnts, const int *sdispls, MPI_Datatype dt1, void *recvbuf, const int *recvcnts,
	const int *rdispls, MPI_Datatype dt2, MPI_Comm comm)
{

int rc;
_scs_vtime_t t1;
int sendlength, i, n;
int root= 0;


    t1= MPI_entry(__func__);
    Debug(DBG_MPI2, "sndp %p, rcvp %p\n", sendbuf, recvbuf);
    SET_COMM();
    rc= PMPI_Alltoallv(sendbuf, sendcnts, sdispls, dt1, recvbuf, recvcnts, rdispls, dt2, comm);

    /* FIXME: I'm not sure this is the right way to calculate the number of bytes sent. */
    sendlength= 0;
    PMPI_Comm_size(comm, &n);
    for (i= 0; i < n; i++)   {
	sendlength= sendlength + sendcnts[i];
    }

    _scs_event_exchange(t1, sendbuf, recvbuf, root, sendlength, MPI_Alltoallv_TAG, dt1);
    MPI_exit();
    return rc;

}  /* end of wrap_MPI_Alltoallv() */


int
wrap_MPI_Reduce(const void *sendbuf, void *recvbuf, int count, MPI_Datatype dt, MPI_Op op, int root, MPI_Comm comm)
{

int rc;
_scs_vtime_t t1;


    t1= MPI_entry(__func__);
    Debug(DBG_MPI2, "root %d, ...\n", root);
    SET_COMM();
    rc= PMPI_Reduce(sendbuf, recvbuf, count, dt, op, root, comm);

    _scs_event_reduce(t1, sendbuf, root, count, MPI_Reduce_TAG, dt);
    MPI_exit();
    return rc;

}  /* end of wrap_MPI_Reduce() */


int
wrap_MPI_Bcast(void *buffer, int count, MPI_Datatype dt, int root, MPI_Comm comm)
{

int rc;
_scs_vtime_t t1;


    t1= MPI_entry(__func__);
    Debug(DBG_MPI2, "sndp %p, count %d, root %d\n", buffer, count, root);
    SET_COMM();
    rc= PMPI_Bcast(buffer, count, dt, root, comm);

    _scs_event_bcast(t1, buffer, root, count, MPI_Bcast_TAG, dt);
    MPI_exit();
    return rc;

}  /* end of wrap_MPI_Bcast() */


int
wrap_MPI_Scatterv(const void *sendbuf, const int *sendcnts, const int *sdispls, MPI_Datatype dt1, void *recvbuf,
	int recvcount, MPI_Datatype dt2, int root, MPI_Comm comm)
{

int rc;
_scs_vtime_t t1;
int sendlength, i, n;


    Err("Not tested yet!\n", ""); _scs_abort();
    t1= MPI_entry(__func__);
    Debug(DBG_MPI2, "sndp %p, rcvp %p, rcv cnt %d, root %d\n", sendbuf, recvbuf, recvcount, root);
    SET_COMM();
    rc= PMPI_Scatterv(sendbuf, sendcnts, sdispls, dt1, recvbuf, recvcount, dt2, root, comm);


    /* FIXME: I'm not sure this is the right way to calculate the number of bytes sent. */
    sendlength= 0;
    PMPI_Comm_size(comm, &n);
    for (i= 0; i < n; i++)   {
	sendlength= sendlength + sendcnts[i];
    }

    _scs_event_bcast(t1, sendbuf, root, sendlength, MPI_Scatterv_TAG, dt1);
    MPI_exit();
    return rc;

}  /* end of wrap_MPI_Scatterv() */


int
wrap_MPI_Allgather(const void *sendbuf, int sendcount, MPI_Datatype dt1, void *recvbuf, int recvcnt, MPI_Datatype dt2,
	MPI_Comm comm)
{

int rc;
_scs_vtime_t t1;
int root= 0;


    t1= MPI_entry(__func__);
    Debug(DBG_MPI2, "sndp %p, snd count %d, rcvp %p, rcv count %d\n", sendbuf, sendcount, recvbuf, recvcnt);
    SET_COMM();
    rc= PMPI_Allgather(sendbuf, sendcount, dt1, recvbuf, recvcnt, dt2, comm);

    _scs_event_exchange(t1, sendbuf, recvbuf, root, sendcount, MPI_Allgather_TAG, dt1);
    MPI_exit();
    return rc;

}  /* end of wrap_MPI_Allgather() */


int
wrap_MPI_Allgatherv(const void *sendbuf, int sendcnt, MPI_Datatype dt1, void *recvbuf, const int *recvcounts,
	const int *rdispls, MPI_Datatype dt2, MPI_Comm comm)
{

int rc;
_scs_vtime_t t1;
int sendlength, i, n;
int root= 0;


    Err("Not tested yet!\n", ""); _scs_abort();
    t1= MPI_entry(__func__);
    Debug(DBG_MPI2, "sndp %p, snd cnt %d, rcvp %p\n", sendbuf, sendcnt, recvbuf);
    SET_COMM();
    rc= PMPI_Allgatherv(sendbuf, sendcnt, dt1, recvbuf, recvcounts, rdispls, dt2, comm);


    /* FIXME: I'm not sure this is the right way to calculate the number of bytes sent. */
    sendlength= 0;
    PMPI_Comm_size(comm, &n);
    for (i= 0; i < n; i++)   {
	sendlength= sendlength + recvcounts[i];
    }

    _scs_event_exchange(t1, sendbuf, recvbuf, root, sendlength, MPI_Allgatherv_TAG, dt1);
    MPI_exit();
    return rc;

}  /* end of wrap_MPI_Allgatherv() */


int
wrap_MPI_Gather(const void *sendbuf, int sendcnt, MPI_Datatype dt1, void *recvbuf, int recvcnt,
	MPI_Datatype dt2, int root, MPI_Comm comm)
{

int rc;
_scs_vtime_t t1;


    t1= MPI_entry(__func__);
    Debug(DBG_MPI2, "sndp %p, snd cnt %d, rcvp %p, rcv cnt %d, root %d\n", sendbuf, sendcnt, recvbuf, recvcnt, root);
    SET_COMM();
    rc= PMPI_Gather(sendbuf, sendcnt, dt1, recvbuf, recvcnt, dt2, root, comm);

    _scs_event_reduce(t1, sendbuf, root, sendcnt, MPI_Gather_TAG, dt1);
    MPI_exit();
    return rc;

}  /* end of wrap_MPI_Gather() */


int
wrap_MPI_Gatherv(const void *sendbuf, int sendcnt, MPI_Datatype dt1, void *recvbuf, const int *recvcounts,
	const int *rdispls, MPI_Datatype dt2, int root, MPI_Comm comm)
{

int rc;
_scs_vtime_t t1;
int sendlength, i, n;


    t1= MPI_entry(__func__);
    Debug(DBG_MPI2, "sndp %p, snd cnt %d, rcvp %p, root %d\n", sendbuf, sendcnt, recvbuf, root);
    SET_COMM();
    rc= PMPI_Gatherv(sendbuf, sendcnt, dt1, recvbuf, recvcounts, rdispls, dt2, root, comm);


    /* FIXME: I'm not sure this is the right way to calculate the number of bytes sent. */
    sendlength= 0;
    PMPI_Comm_size(comm, &n);
    for (i= 0; i < n; i++)   {
	sendlength= sendlength + recvcounts[i];
    }

    _scs_event_reduce(t1, sendbuf, root, sendcnt, MPI_Gatherv_TAG, dt1);
    MPI_exit();
    return rc;

}  /* end of wrap_MPI_Gatherv() */


int
wrap_MPI_Reduce_scatter(const void *sendbuf, void *recvbuf, const int *recvcounts, MPI_Datatype dt, MPI_Op op, MPI_Comm comm)
{

int rc;
_scs_vtime_t t1;
int sendlength, i, n;
int root= 0;

    t1= MPI_entry(__func__);
    Debug(DBG_MPI2, "sndbuf %p, rcvbuf %p\n", sendbuf, recvbuf);
    SET_COMM();
    rc= PMPI_Reduce_scatter(sendbuf, recvbuf, recvcounts, dt, op, comm);


    /* FIXME: I'm not sure this is the right way to calculate the number of bytes sent. */
    sendlength= 0;
    PMPI_Comm_size(comm, &n);
    for (i= 0; i < n; i++)   {
	sendlength= sendlength + recvcounts[i];
    }

    _scs_event_exchange(t1, sendbuf, recvbuf, root, sendlength, MPI_Reduce_scatter_TAG, dt);
    MPI_exit();
    return rc;

}  /* end of wrap_MPI_Reduce_scatter() */


int
wrap_MPI_Scan(const void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm)
{

int rc;
_scs_vtime_t t1;


    t1= MPI_entry(__func__);
    Debug(DBG_MPI2, "sndp %p, rcvp %p, cnt %d\n", sendbuf, recvbuf, count);
    SET_COMM();
    rc= PMPI_Scan(sendbuf, recvbuf, count, datatype, op, comm);

    _scs_event_exchange(t1, sendbuf, recvbuf, 0, count, MPI_Scan_TAG, datatype);
    MPI_exit();
    return rc;

}  /* end of wrap_MPI_Scan() */
/*
** -------------------------------------------------------------------------------------------------
** End of Collectives
** -------------------------------------------------------------------------------------------------
*/


int
wrap_MPI_Comm_size(MPI_Comm comm, int *size)
{

_scs_vtime_t t1;
int rc;


    t1= MPI_entry(__func__);
    SET_COMM();
    rc= PMPI_Comm_size(comm, size);
    MPI_exit();
    return rc;

}  /* end of wrap_MPI_Comm_size() */


int
wrap_MPI_Comm_rank(MPI_Comm comm, int *rank)
{

_scs_vtime_t t1;
int rc;


    t1= MPI_entry(__func__);
    SET_COMM();
    rc= PMPI_Comm_rank(comm, rank);
    MPI_exit();
    return rc;

}  /* end of wrap_MPI_Comm_rank() */


int
wrap_MPI_Comm_dup(MPI_Comm comm, MPI_Comm *comm_out)
{

_scs_vtime_t t1;
int rc;


    t1= MPI_entry(__func__);
    SET_COMM();
    rc= PMPI_Comm_dup(comm, comm_out);
    MPI_exit();
    return rc;

}  /* end of wrap_MPI_Comm_dup() */


int
wrap_MPI_Comm_split(MPI_Comm comm, int color, int key, MPI_Comm *comm_out)
{

_scs_vtime_t t1;
int rc;


    t1= MPI_entry(__func__);
    SET_COMM();
    rc= PMPI_Comm_split(comm, color, key, comm_out);
    MPI_exit();
    return rc;

}  /* end of wrap_MPI_Comm_split() */


int
wrap_MPI_Abort(MPI_Comm comm, int errorcode)
{

    SET_COMM();
    return PMPI_Abort(comm, errorcode);

}  /* end of wrap_MPI_Abort() */


double
wrap_MPI_Wtime(void)
{

_scs_vtime_t t1;
double rc;


    t1= MPI_entry(__func__);
    rc= _scs_get_lvtime();
    MPI_exit();
    return rc;

}  /* end of wrap_MPI_Wtime() */


int
wrap_MPI_File_open(MPI_Comm comm, const char *filename, int amode, MPI_Info info, MPI_File *fh)
{

_scs_vtime_t t1;
int rc;


    t1= MPI_entry(__func__);
    Err("Not tested yet!\n", ""); _scs_abort();
    SET_COMM();
    rc= PMPI_File_open(comm, filename, amode, info, fh);
    MPI_exit();
    return rc;

}  /* end of wrap_MPI_File_open() */




int
wrap_MPI_Iprobe(int source, int tag, MPI_Comm comm, int *flag, MPI_Status *status)
{

_scs_vtime_t t1;
int rc;


    t1= MPI_entry(__func__);
    Err("Not tested yet!\n", ""); _scs_abort();
    SET_COMM();
    rc= PMPI_Iprobe(source, tag, comm, flag, status);
    MPI_exit();
    return rc;

}  /* end of wrap_MPI_Iprobe() */


int 
wrap_MPI_Type_contiguous(int count, MPI_Datatype oldtype, MPI_Datatype *newtype)
{

_scs_vtime_t t1;
int rc;


    t1= MPI_entry(__func__);
    Err("Not tested yet!\n", ""); _scs_abort();
    rc= PMPI_Type_contiguous(count, oldtype, newtype);
    MPI_exit();
    return rc;

} /* end of wrap_MPI_Type_contiguous() */


int 
wrap_MPI_Type_free(MPI_Datatype *datatype)
{

_scs_vtime_t t1;
int rc;


    t1= MPI_entry(__func__);
    rc= PMPI_Type_free(datatype);
    MPI_exit();
    return rc;

} /* end of wrap_MPI_Type_free() */


int 
wrap_MPI_Type_struct(int count, int *array_of_blocklengths, MPI_Aint *array_of_displacements, MPI_Datatype *array_of_types, MPI_Datatype *newtype)
{

_scs_vtime_t t1;
int rc;


    t1= MPI_entry(__func__);
    Err("Not tested yet!\n", ""); _scs_abort();
    rc= PMPI_Type_struct(count, array_of_blocklengths, array_of_displacements, array_of_types, newtype);
    MPI_exit();
    return rc;

} /* end of wrap_MPI_Type_struct() */


int 
wrap_MPI_Type_vector(int count, int blocklength, int stride, MPI_Datatype oldtype, MPI_Datatype *newtype)
{

_scs_vtime_t t1;
int rc;


    t1= MPI_entry(__func__);
    Err("Not tested yet!\n", ""); _scs_abort();
    rc= PMPI_Type_vector(count, blocklength, stride, oldtype, newtype);
    MPI_exit();
    return rc;

} /* end of wrap_MPI_Type_vector() */


int
wrap_MPI_Comm_create(MPI_Comm comm, MPI_Group group, MPI_Comm *comm_out)
{

_scs_vtime_t t1;
int rc;


    t1= MPI_entry(__func__);
    Err("Not tested yet!\n", ""); _scs_abort();
    SET_COMM();
    rc= PMPI_Comm_create(comm, group, comm_out);
    MPI_exit();
    return rc;

}  /* end of wrap_MPI_Comm_create() */


int
wrap_MPI_Comm_free(MPI_Comm *commp)
{

MPI_Comm comm;
int rc;
_scs_vtime_t t1;


    t1= MPI_entry(__func__);
    if (MPI_COMM_WORLD == *commp)    {
        Err("Can't free MPI_COMM_WORLD! Seshat needs it!\n", ""); _scs_abort();
	comm= _scs_app_comm;
    } else   {
	comm= *commp;
    }

    rc= PMPI_Comm_free(&comm);

    *commp= comm;
    MPI_exit();
    return rc;

}  /* end of wrap_MPI_Comm_free() */


int
wrap_MPI_Comm_group(MPI_Comm comm, MPI_Group *group)
{

_scs_vtime_t t1;
int rc;


    t1= MPI_entry(__func__);
    Err("Not tested yet!\n", ""); _scs_abort();
    SET_COMM();
    rc= PMPI_Comm_group(comm, group);
    MPI_exit();
    return rc;

}  /* end of wrap_MPI_Comm_group() */


int
wrap_MPI_Pack_size(int incount, MPI_Datatype datatype, MPI_Comm comm, int *size)
{

_scs_vtime_t t1;
int rc;


    t1= MPI_entry(__func__);
    SET_COMM();
    Debug(DBG_MPI2, "\n", "");
    rc= PMPI_Pack_size(incount, datatype, comm, size);
    MPI_exit();
    return rc;


}  /* end of wrap_MPI_Pack_size() */


int
wrap_MPI_Pack(const void *inbuf, int incount, MPI_Datatype datatype, void *outbuf, int outsize, int *position, MPI_Comm comm)
{

_scs_vtime_t t1;
int rc;


    t1= MPI_entry(__func__);
    SET_COMM();
    Debug(DBG_MPI2, "\n", "");
    rc= PMPI_Pack(inbuf, incount, datatype, outbuf, outsize, position, comm);
    MPI_exit();
    return rc;

}  /* end of wrap_MPI_Pack() */



int
wrap_MPI_Unpack(const void *inbuf, int insize, int *position, void *outbuf, int outcount, MPI_Datatype datatype, MPI_Comm comm)
{

_scs_vtime_t t1;
int rc;


    t1= MPI_entry(__func__);
    SET_COMM();
    Debug(DBG_MPI2, "\n", "");
    rc= PMPI_Unpack(inbuf, insize, position, outbuf, outcount, datatype, comm);
    MPI_exit();
    return rc;

}  /* end of wrap_MPI_Unpack() */



int
wrap_MPI_Cart_create(MPI_Comm comm, int ndims, const int *dims, const int *periods, int reorder, MPI_Comm *comm_cart)
{

_scs_vtime_t t1;
int rc;


    t1= MPI_entry(__func__);
    SET_COMM();
    Debug(DBG_MPI2, "\n", "");
    rc= PMPI_Cart_create(comm, ndims, dims, periods, reorder, comm_cart);
    MPI_exit();
    return rc;

}  /* end of wrap_MPI_Cart_create() */



int
wrap_MPI_Cart_get(MPI_Comm comm, int maxdims, int *dims, int *periods, int *coords)
{

_scs_vtime_t t1;
int rc;


    t1= MPI_entry(__func__);
    SET_COMM();
    Debug(DBG_MPI2, "\n", "");
    rc= PMPI_Cart_get(comm, maxdims, dims, periods, coords);
    MPI_exit();
    return rc;

}  /* end of wrap_MPI_Cart_get() */



int
wrap_MPI_Cart_rank(MPI_Comm comm, const int *coords, int *rank)
{

_scs_vtime_t t1;
int rc;


    t1= MPI_entry(__func__);
    SET_COMM();
    Debug(DBG_MPI2, "\n", "");
    rc= PMPI_Cart_rank(comm, coords, rank);
    MPI_exit();
    return rc;

}  /* end of wrap_MPI_Cart_rank() */



int
wrap_MPI_Cart_shift(MPI_Comm comm, int direction, int disp, int *rank_source, int *rank_dest)
{

_scs_vtime_t t1;
int rc;


    t1= MPI_entry(__func__);
    SET_COMM();
    Debug(DBG_MPI2, "\n", "");
    rc= PMPI_Cart_shift(comm, direction, disp, rank_source, rank_dest);
    MPI_exit();
    return rc;

}  /* end of wrap_MPI_Cart_shift() */


int
wrap_MPI_Sendrecv(const void *sendbuf, int sendcount, MPI_Datatype sendtype, int dest, int sendtag,
        void *recvbuf, int recvcount, MPI_Datatype recvtype, int source, int recvtag, MPI_Comm comm,
        MPI_Status *status)
{

_scs_vtime_t t1;
int rc;


    t1= MPI_entry(__func__);
    SET_COMM();
    Debug(DBG_MPI2, "\n", "");
    rc= PMPI_Sendrecv(sendbuf, sendcount, sendtype, dest, sendtag, recvbuf, recvcount, recvtype,
            source, recvtag, comm, status);
    MPI_exit();
    return rc;

}  /* end of wrap_MPI_Sendrecv() */


/*
** -----------------------------------------------------------------------------------------------------------
** FIXME:
** Below here are functions that work for now, but probably need more work
** in the future.
*/
int
wrap_MPI_Group_free(MPI_Group *group)
{

_scs_vtime_t t1;
int rc;


    t1= MPI_entry(__func__);
    Err("Not tested yet!\n", ""); _scs_abort();
    rc= PMPI_Group_free(group);
    MPI_exit();
    return rc;

}  /* end of wrap_MPI_Group_free() */


int
wrap_MPI_Group_incl(MPI_Group group, int n, const int *ranks, MPI_Group *group_out)
{

_scs_vtime_t t1;
int rc;


    t1= MPI_entry(__func__);
    Debug(DBG_MPI2, "group %ld, ...\n", (long)group);
    rc= PMPI_Group_incl(group, n, ranks, group_out);
    MPI_exit();
    return rc;

}  /* end of wrap_MPI_Group_incl() */


int
wrap_MPI_Group_range_incl(MPI_Group group, int n, int ranges[][3], MPI_Group *newgroup)
{

_scs_vtime_t t1;
int rc;


    t1= MPI_entry(__func__);
    Err("Not tested yet!\n", ""); _scs_abort();
    rc= PMPI_Group_range_incl(group, n, ranges, newgroup);
    MPI_exit();
    return rc;

}  /* end of wrap_MPI_Group_range_incl() */


int
wrap_MPI_Cancel(MPI_Request *request)
{

_scs_vtime_t t1;
int rc;


    t1= MPI_entry(__func__);
    /* We need to remove any knowledge we have about this request */
    Warn("Warning!!! MPI_Cancel() Need to delete request\n", "");
    rc= PMPI_Cancel(request);
    MPI_exit();
    return rc;

}  /* end of wrap_MPI_Cancel() */



/*
** -----------------------------------------------------------------------------------------------------------
** Below are the MPI functions we have not written wrappers for. Some of them will not
** need a wrapper. We only need wrappers for those that take a communicator or
** a group. (Plus a few special MPI functions like MPI_Init() which we already have.)
*/
#define NOOP		{fprintf(stderr, "No wrapper exists yet for function %s! Aborting...\n", __func__); \
			_scs_abort(); return MPI_ERR_UNSUPPORTED_OPERATION;}

#define NOOP_COMM	{fprintf(stderr, "No wrapper exists yet for function %s! Aborting...\n", __func__); \
			_scs_abort(); return MPI_COMM_NULL;}


#define NOOP_FILE	{fprintf(stderr, "No wrapper exists yet for function %s! Aborting...\n", __func__); \
			_scs_abort(); return MPI_FILE_NULL;}

#define NOOP_GROUP	{fprintf(stderr, "No wrapper exists yet for function %s! Aborting...\n", __func__); \
			_scs_abort(); return MPI_GROUP_NULL;}

#define NOOP_INFO	{fprintf(stderr, "No wrapper exists yet for function %s! Aborting...\n", __func__); \
			_scs_abort(); return MPI_INFO_NULL;}

#define NOOP_OP		{fprintf(stderr, "No wrapper exists yet for function %s! Aborting...\n", __func__); \
			_scs_abort(); return MPI_OP_NULL;}

#define NOOP_REQUEST	{fprintf(stderr, "No wrapper exists yet for function %s! Aborting...\n", __func__); \
			_scs_abort(); return MPI_REQUEST_NULL;}

#define NOOP_DT		{fprintf(stderr, "No wrapper exists yet for function %s! Aborting...\n", __func__); \
			_scs_abort(); return (MPI_Datatype)NULL;}

#define NOOP_WIN	{fprintf(stderr, "No wrapper exists yet for function %s! Aborting...\n", __func__); \
			_scs_abort(); return MPI_WIN_NULL;}

#define NOOP_ERR	{fprintf(stderr, "No wrapper exists yet for function %s! Aborting...\n", __func__); \
			_scs_abort(); return MPI_ERRHANDLER_NULL;}


/* I need to define this, but I don't know why!!! */
int wrap_MPI_Error_string(int errorcode, char *string, int *resultlen)NOOP

/*
** MPI-1.1 from http://www.mpi-forum.org/docs/mpi-11-html/mpi-report.html
*/

/* Point-to-Point */
/* no wrapper needed: int MPI_Get_count(MPI_Status *status, MPI_Datatype datatype, int *count) */
int wrap_MPI_Bsend(void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm)NOOP
int wrap_MPI_Ssend(void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm)NOOP
/* no wrapper needed: int MPI_Buffer_attach(void *buffer, int size) */
/* no wrapper needed: int MPI_Buffer_detach(void *buffer, int *size) */
int wrap_MPI_Ibsend(void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request)NOOP
int wrap_MPI_Issend(void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request)NOOP
int wrap_MPI_Irsend(void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request)NOOP
/* no wrapper needed: int MPI_Test(MPI_Request *request, int *flag, MPI_Status *status) */
int wrap_MPI_Request_free(MPI_Request *request)NOOP
int wrap_MPI_Waitany(int count, MPI_Request array_of_requests[], int *index, MPI_Status *status)NOOP
/* no wrapper needed: int MPI_Testany(int count, MPI_Request array_of_requests[], int *index, int *flag, MPI_Status *status) */
int wrap_MPI_Testall(int count, MPI_Request array_of_requests[], int *flag, MPI_Status array_of_statuses[])NOOP
int wrap_MPI_Waitsome(int incount, MPI_Request array_of_requests[], int *outcount, int array_of_indices[], MPI_Status array_of_statuses[])NOOP
int wrap_MPI_Testsome(int incount, MPI_Request array_of_requests[], int *outcount, int array_of_indices[], MPI_Status array_of_statuses[])NOOP
int wrap_MPI_Probe(int source, int tag, MPI_Comm comm, MPI_Status *status)NOOP
int wrap_MPI_Test_cancelled(MPI_Status *status, int *flag)NOOP
int wrap_MPI_Send_init(void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request)NOOP
int wrap_MPI_Bsend_init(void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request)NOOP
int wrap_MPI_Ssend_init(void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request)NOOP
int wrap_MPI_Rsend_init(void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request)NOOP
int wrap_MPI_Recv_init(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Request *request)NOOP
int wrap_MPI_Start(MPI_Request *request)NOOP
int wrap_MPI_Startall(int count, MPI_Request *array_of_requests)NOOP
int wrap_MPI_Sendrecv_replace(void *buf, int count, MPI_Datatype datatype, int dest, int sendtag, int source, int recvtag, MPI_Comm comm, MPI_Status *status)NOOP
int wrap_MPI_Type_hvector(int count, int blocklength, MPI_Aint stride, MPI_Datatype oldtype, MPI_Datatype *newtype)NOOP
int wrap_MPI_Type_hindexed(int count, int *array_of_blocklengths, MPI_Aint *array_of_displacements, MPI_Datatype oldtype, MPI_Datatype *newtype)NOOP
/* no wrapper needed: int MPI_Address(void *location, MPI_Aint *address) */
int wrap_MPI_Type_extent(MPI_Datatype datatype, MPI_Aint *extent)NOOP
/* no wrapper needed: int MPI_Type_size(MPI_Datatype datatype, int *size) */
int wrap_MPI_Type_lb(MPI_Datatype datatype, MPI_Aint *displacement)NOOP
int wrap_MPI_Type_ub(MPI_Datatype datatype, MPI_Aint *displacement)NOOP
int wrap_MPI_Get_elements(MPI_Status *status, MPI_Datatype datatype, int *count)NOOP

/* Collective Communications */
int wrap_MPI_Scatter(const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm)NOOP
/* no wrapper needed: int wrap_MPI_Op_create(MPI_User_function *function, int commute, MPI_Op *op)NOOP */
int wrap_MPI_Op_free(MPI_Op *op)NOOP

/* Groups, Contexts, and Communicators */
int wrap_MPI_Group_size(MPI_Group group, int *size)NOOP
int wrap_MPI_Group_rank(MPI_Group group, int *rank)NOOP
int wrap_MPI_Group_translate_ranks(MPI_Group group1, int n, int *ranks1, MPI_Group group2, int *ranks2)NOOP
int wrap_MPI_Group_compare(MPI_Group group1, MPI_Group group2, int *result)NOOP
int wrap_MPI_Group_union(MPI_Group group1, MPI_Group group2, MPI_Group *newgroup)NOOP
int wrap_MPI_Group_intersection(MPI_Group group1, MPI_Group group2, MPI_Group *newgroup)NOOP
int wrap_MPI_Group_difference(MPI_Group group1, MPI_Group group2, MPI_Group *newgroup)NOOP
int wrap_MPI_Group_excl(MPI_Group group, int n, int *ranks, MPI_Group *newgroup)NOOP
int wrap_MPI_Group_range_excl(MPI_Group group, int n, int ranges[][3], MPI_Group *newgroup)NOOP
int wrap_MPI_Comm_compare(MPI_Comm comm1, MPI_Comm comm2, int *result)NOOP
int wrap_MPI_Comm_test_inter(MPI_Comm comm, int *flag)NOOP
int wrap_MPI_Comm_remote_size(MPI_Comm comm, int *size)NOOP
int wrap_MPI_Comm_remote_group(MPI_Comm comm, MPI_Group *group)NOOP
int wrap_MPI_Intercomm_create(MPI_Comm local_comm, int local_leader, MPI_Comm peer_comm, int remote_leader, int tag, MPI_Comm *newintercomm)NOOP
int wrap_MPI_Intercomm_merge(MPI_Comm intercomm, int high, MPI_Comm *newintracomm)NOOP
int wrap_MPI_Keyval_create(MPI_Copy_function *copy_fn, MPI_Delete_function *delete_fn, int *keyval, void *extra_state)NOOP
int wrap_MPI_Keyval_free(int *keyval)NOOP
int wrap_MPI_Attr_put(MPI_Comm comm, int keyval, void *attribute_val)NOOP
int wrap_MPI_Attr_get(MPI_Comm comm, int keyval, void *attribute_val, int *flag)NOOP
int wrap_MPI_Attr_delete(MPI_Comm comm, int keyval)NOOP

/* Process Tolpologies */
int wrap_MPI_Dims_create(int nnodes, int ndims, int *dims)NOOP
int wrap_MPI_Graph_create(MPI_Comm comm_old, int nnodes, int *index, int *edges, int reorder, MPI_Comm *comm_graph)NOOP
int wrap_MPI_Topo_test(MPI_Comm comm, int *status)NOOP
int wrap_MPI_Graphdims_get(MPI_Comm comm, int *nnodes, int *nedges)NOOP
int wrap_MPI_Graph_get(MPI_Comm comm, int maxindex, int maxedges, int *index, int *edges)NOOP
int wrap_MPI_Cartdim_get(MPI_Comm comm, int *ndims)NOOP
int wrap_MPI_Cart_coords(MPI_Comm comm, int rank, int maxdims, int *coords)NOOP
int wrap_MPI_Graph_neighbors_count(MPI_Comm comm, int rank, int *nneighbors)NOOP
int wrap_MPI_Graph_neighbors(MPI_Comm comm, int rank, int maxneighbors, int *neighbors)NOOP
int wrap_MPI_Cart_sub(MPI_Comm comm, int *remain_dims, MPI_Comm *newcomm)NOOP
int wrap_MPI_Cart_map(MPI_Comm comm, int ndims, int *dims, int *periods, int *newrank)NOOP
int wrap_MPI_Graph_map(MPI_Comm comm, int nnodes, int *index, int *edges, int *newrank)NOOP

/* Environmental Enquiry */
int wrap_MPI_Get_processor_name(char *name, int *resultlen)NOOP
int wrap_MPI_Errhandler_create(MPI_Handler_function *function, MPI_Errhandler *errhandler)NOOP
int wrap_MPI_Errhandler_set(MPI_Comm comm, MPI_Errhandler errhandler)NOOP
int wrap_MPI_Errhandler_get(MPI_Comm comm, MPI_Errhandler *errhandler)NOOP
int wrap_MPI_Errhandler_free(MPI_Errhandler *errhandler)NOOP
int wrap_MPI_Error_class(int errorcode, int *errorclass)NOOP
double wrap_MPI_Wtick(void)NOOP
int wrap_MPI_Initialized(int *flag)NOOP

/* Profiling */
int wrap_MPI_Pcontrol(const int level, ...)NOOP


/*
** MPI-1.2 from http://www-unix.mcs.anl.gov/mpi/mpi-standard/mpi-report-2.0/mpi2-report.htm#Node0
*/
int wrap_MPI_Get_version(int *version, int *subversion)NOOP



/*
** MPI-2 from http://www-unix.mcs.anl.gov/mpi/mpi-standard/mpi-report-2.0/mpi2-report.htm#Node0
*/

/* Miscellany */
int wrap_MPI_Alloc_mem(MPI_Aint size, MPI_Info info, void *baseptr)NOOP
int wrap_MPI_Comm_create_errhandler(MPI_Comm_errhandler_fn *function, MPI_Errhandler *errhandler)NOOP
#if !defined (MPI_Comm_f2c)
    /* Some implementations define this one as a macro. Luckily, we don't need to do anything with it. */
/* no wrapper needed:     MPI_Comm MPI_Comm_f2c(MPI_Fint comm) */
#endif
int wrap_MPI_Comm_get_errhandler(MPI_Comm comm, MPI_Errhandler *errhandler)NOOP
int wrap_MPI_Comm_set_errhandler(MPI_Comm comm, MPI_Errhandler errhandler)NOOP
MPI_Fint wrap_MPI_File_c2f(MPI_File file)NOOP
int wrap_MPI_File_create_errhandler(MPI_File_errhandler_fn *function, MPI_Errhandler *errhandler)NOOP
MPI_File wrap_MPI_File_f2c(MPI_Fint file)NOOP_FILE
int wrap_MPI_File_get_errhandler(MPI_File file, MPI_Errhandler *errhandler)NOOP
int wrap_MPI_File_set_errhandler(MPI_File file, MPI_Errhandler errhandler)NOOP
int wrap_MPI_Finalized(int *flag)NOOP
int wrap_MPI_Free_mem(void *base)NOOP
int wrap_MPI_Get_address(void *location, MPI_Aint *address)NOOP
#if !defined (MPI_Group_c2f)
    /* Some implementations define this one as a macro. Luckily, we don't need to do anything with it. */
    MPI_Fint wrap_MPI_Group_c2f(MPI_Group group)NOOP
#endif
#if !defined (MPI_Group_f2c)
    /* Some implementations define this one as a macro. Luckily, we don't need to do anything with it. */
    MPI_Group wrap_MPI_Group_f2c(MPI_Fint group)NOOP_GROUP
#endif
#if !defined (MPI_Info_c2f)
    /* Some implementations define this one as a macro. Luckily, we don't need to do anything with it. */
    MPI_Fint wrap_MPI_Info_c2f(MPI_Info info)NOOP
#endif
int wrap_MPI_Info_create(MPI_Info *info)NOOP
int wrap_MPI_Info_delete(MPI_Info info, char *key)NOOP
int wrap_MPI_Info_dup(MPI_Info info, MPI_Info *newinfo)NOOP
#if !defined (MPI_Info_f2c)
    /* Some implementations define this one as a macro. Luckily, we don't need to do anything with it. */
    MPI_Info wrap_MPI_Info_f2c(MPI_Fint info)NOOP_INFO
#endif
int wrap_MPI_Info_free(MPI_Info *info)NOOP
int wrap_MPI_Info_get(MPI_Info info, char *key, int valuelen, char *value, int *flag)NOOP
int wrap_MPI_Info_get_nkeys(MPI_Info info, int *nkeys)NOOP
int wrap_MPI_Info_get_nthkey(MPI_Info info, int n, char *key)NOOP
int wrap_MPI_Info_get_valuelen(MPI_Info info, char *key, int *valuelen, int *flag)NOOP
int wrap_MPI_Info_set(MPI_Info info, char *key, char *value)NOOP
#if !defined (MPI_Op_c2f)
    /* Some implementations define this one as a macro. Luckily, we don't need to do anything with it. */
    MPI_Fint wrap_MPI_Op_c2f(MPI_Op op)NOOP
#endif
int wrap_MPI_Pack_external(char *datarep, void *inbuf, int incount, MPI_Datatype datatype, void *outbuf, MPI_Aint outsize, MPI_Aint *position)NOOP
int wrap_MPI_Pack_external_size(char *datarep, int incount, MPI_Datatype datatype, MPI_Aint *size)NOOP
int wrap_MPI_Request_get_status(MPI_Request request, int *flag, MPI_Status *status)NOOP
#if !defined (MPI_Type_c2f)
    /* Some implementations define this one as a macro. Luckily, we don't need to do anything with it. */
    MPI_Fint wrap_MPI_Type_c2f(MPI_Datatype datatype)NOOP
#endif
int wrap_MPI_Type_create_darray(int size, int rank, int ndims, int array_of_gsizes[], int array_of_distribs[], int array_of_dargs[], int array_of_psizes[], int order, MPI_Datatype oldtype, MPI_Datatype *newtype)NOOP
int wrap_MPI_Type_create_hindexed(int count, int array_of_blocklengths[], MPI_Aint array_of_displacements[], MPI_Datatype oldtype, MPI_Datatype *newtype)NOOP
int wrap_MPI_Type_create_hvector(int count, int blocklength, MPI_Aint stride, MPI_Datatype oldtype, MPI_Datatype *newtype)NOOP
int wrap_MPI_Type_create_indexed_block(int count, int blocklength, int array_of_displacements[], MPI_Datatype oldtype, MPI_Datatype *newtype)NOOP
int wrap_MPI_Type_create_resized(MPI_Datatype oldtype, MPI_Aint lb, MPI_Aint extent, MPI_Datatype *newtype)NOOP
int wrap_MPI_Type_create_struct(int count, int array_of_blocklengths[], MPI_Aint array_of_displacements[], MPI_Datatype array_of_types[], MPI_Datatype *newtype)NOOP
int wrap_MPI_Type_create_subarray(int ndims, int array_of_sizes[], int array_of_subsizes[], int array_of_starts[], int order, MPI_Datatype oldtype, MPI_Datatype *newtype)NOOP
#if !defined (MPI_Type_f2c)
    /* Some implementations define this one as a macro. Luckily, we don't need to do anything with it. */
/* no wrapper needed:     MPI_Datatype MPI_Type_f2c(MPI_Fint datatype) */
#endif
int wrap_MPI_Type_get_extent(MPI_Datatype datatype, MPI_Aint *lb, MPI_Aint *extent)NOOP
int wrap_MPI_Type_get_true_extent(MPI_Datatype datatype, MPI_Aint *true_lb, MPI_Aint *true_extent)NOOP
int wrap_MPI_Unpack_external(char *datarep, void *inbuf, MPI_Aint insize, MPI_Aint *position, void *outbuf, int outcount, MPI_Datatype datatype)NOOP


/* Process Creation and Management */
int wrap_MPI_Close_port(char *port_name)NOOP
int wrap_MPI_Comm_accept(char *port_name, MPI_Info info, int root, MPI_Comm comm, MPI_Comm *newcomm)NOOP
int wrap_MPI_Comm_connect(char *port_name, MPI_Info info, int root, MPI_Comm comm, MPI_Comm *newcomm)NOOP
int wrap_MPI_Comm_disconnect(MPI_Comm *comm)NOOP
int wrap_MPI_Comm_get_parent(MPI_Comm *parent)NOOP
int wrap_MPI_Comm_join(int fd, MPI_Comm *intercomm)NOOP
int wrap_MPI_Comm_spawn(char *command, char *argv[], int maxprocs, MPI_Info info, int root, MPI_Comm comm, MPI_Comm *intercomm, int array_of_errcodes[])NOOP
int wrap_MPI_Comm_spawn_multiple(int count, char *array_of_commands[], char **array_of_argv[], int array_of_maxprocs[], MPI_Info array_of_info[], int root, MPI_Comm comm, MPI_Comm *intercomm, int array_of_errcodes[])NOOP
int wrap_MPI_Lookup_name(char *service_name, MPI_Info info, char *port_name)NOOP
int wrap_MPI_Open_port(MPI_Info info, char *port_name)NOOP
int wrap_MPI_Publish_name(char *service_name, MPI_Info info, char *port_name)NOOP
int wrap_MPI_Unpublish_name(char *service_name, MPI_Info info, char *port_name)NOOP

/* One-Sided Communications */
int wrap_MPI_Accumulate(void *origin_addr, int origin_count, MPI_Datatype origin_datatype, int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Op op, MPI_Win win)NOOP
int wrap_MPI_Get(void *origin_addr, int origin_count, MPI_Datatype origin_datatype, int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Win win)NOOP
int wrap_MPI_Put(void *origin_addr, int origin_count, MPI_Datatype origin_datatype, int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Win win)NOOP
int wrap_MPI_Win_complete(MPI_Win win)NOOP
int wrap_MPI_Win_create(void *base, MPI_Aint size, int disp_unit, MPI_Info info, MPI_Comm comm, MPI_Win *win)NOOP
int wrap_MPI_Win_fence(int assert, MPI_Win win)NOOP
int wrap_MPI_Win_free(MPI_Win *win)NOOP
int wrap_MPI_Win_get_group(MPI_Win win, MPI_Group *group)NOOP
int wrap_MPI_Win_lock(int lock_type, int rank, int assert, MPI_Win win)NOOP
int wrap_MPI_Win_post(MPI_Group group, int assert, MPI_Win win)NOOP
int wrap_MPI_Win_start(MPI_Group group, int assert, MPI_Win win)NOOP
int wrap_MPI_Win_test(MPI_Win win, int *flag)NOOP
int wrap_MPI_Win_unlock(int rank, MPI_Win win)NOOP
int wrap_MPI_Win_wait(MPI_Win win)NOOP

/* Extended Collective Operations */
int wrap_MPI_Alltoallw(const void *sendbuf, int sendcounts[], int sdispls[], MPI_Datatype sendtypes[], void *recvbuf, int recvcounts[], int rdispls[], MPI_Datatype recvtypes[], MPI_Comm comm)NOOP
int wrap_MPI_Exscan(const void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm)NOOP

/* External Interfaces */
int wrap_MPI_Add_error_class(int *errorclass)NOOP
int wrap_MPI_Add_error_code(int errorclass, int *errorcode)NOOP
int wrap_MPI_Add_error_string(int errorcode, char *string)NOOP
int wrap_MPI_Comm_call_errhandler(MPI_Comm comm, int errorcode)NOOP
int wrap_MPI_Comm_create_keyval(MPI_Comm_copy_attr_function *comm_copy_attr_fn, MPI_Comm_delete_attr_function *comm_delete_attr_fn, int *comm_keyval, void *extra_state)NOOP
int wrap_MPI_Comm_delete_attr(MPI_Comm comm, int comm_keyval)NOOP
int wrap_MPI_Comm_free_keyval(int *comm_keyval)NOOP
int wrap_MPI_Comm_get_attr(MPI_Comm comm, int comm_keyval, void *attribute_val, int *flag)NOOP
int wrap_MPI_Comm_get_name(MPI_Comm comm, char *comm_name, int *resultlen)NOOP
int wrap_MPI_Comm_set_attr(MPI_Comm comm, int comm_keyval, void *attribute_val)NOOP
int wrap_MPI_Comm_set_name(MPI_Comm comm, char *comm_name)NOOP
int wrap_MPI_File_call_errhandler(MPI_File fh, int errorcode)NOOP
int wrap_MPI_Grequest_complete(MPI_Request request)NOOP
int wrap_MPI_Grequest_start(MPI_Grequest_query_function *query_fn, MPI_Grequest_free_function *free_fn, MPI_Grequest_cancel_function *cancel_fn, void *extra_state, MPI_Request *request)NOOP
int wrap_MPI_Init_thread(int *argc, char **argv[], int required, int *provided)NOOP
int wrap_MPI_Is_thread_main(int *flag)NOOP
int wrap_MPI_Query_thread(int *provided)NOOP
int wrap_MPI_Status_set_cancelled(MPI_Status *status, int flag)NOOP
int wrap_MPI_Status_set_elements(MPI_Status *status, MPI_Datatype datatype, int count)NOOP
int wrap_MPI_Type_create_keyval(MPI_Type_copy_attr_function *type_copy_attr_fn, MPI_Type_delete_attr_function *type_delete_attr_fn, int *type_keyval, void *extra_state)NOOP
int wrap_MPI_Type_delete_attr(MPI_Datatype type, int type_keyval)NOOP
int wrap_MPI_Type_dup(MPI_Datatype type, MPI_Datatype *newtype)NOOP
int wrap_MPI_Type_free_keyval(int *type_keyval)NOOP
int wrap_MPI_Type_get_attr(MPI_Datatype type, int type_keyval, void *attribute_val, int *flag)NOOP
int wrap_MPI_Type_get_contents(MPI_Datatype datatype, int max_integers, int max_addresses, int max_datatypes, int array_of_integers[], MPI_Aint array_of_addresses[], MPI_Datatype array_of_datatypes[])NOOP
int wrap_MPI_Type_get_envelope(MPI_Datatype datatype, int *num_integers, int *num_addresses, int *num_datatypes, int *combiner)NOOP
int wrap_MPI_Type_get_name(MPI_Datatype type, char *type_name, int *resultlen)NOOP
int wrap_MPI_Type_set_attr(MPI_Datatype type, int type_keyval, void *attribute_val)NOOP
int wrap_MPI_Type_set_name(MPI_Datatype type, char *type_name)NOOP
int wrap_MPI_Win_call_errhandler(MPI_Win win, int errorcode)NOOP
int wrap_MPI_Win_create_keyval(MPI_Win_copy_attr_function *win_copy_attr_fn, MPI_Win_delete_attr_function *win_delete_attr_fn, int *win_keyval, void *extra_state)NOOP
int wrap_MPI_Win_delete_attr(MPI_Win win, int win_keyval)NOOP
int wrap_MPI_Win_free_keyval(int *win_keyval)NOOP
int wrap_MPI_Win_get_attr(MPI_Win win, int win_keyval, void *attribute_val, int *flag)NOOP
int wrap_MPI_Win_get_name(MPI_Win win, char *win_name, int *resultlen)NOOP
int wrap_MPI_Win_set_attr(MPI_Win win, int win_keyval, void *attribute_val)NOOP
int wrap_MPI_Win_set_name(MPI_Win win, char *win_name)NOOP

/* I/O */
int wrap_MPI_File_close(MPI_File *fh)NOOP
int wrap_MPI_File_delete(char *filename, MPI_Info info)NOOP
int wrap_MPI_File_get_amode(MPI_File fh, int *amode)NOOP
int wrap_MPI_File_get_atomicity(MPI_File fh, int *flag)NOOP
int wrap_MPI_File_get_byte_offset(MPI_File fh, MPI_Offset offset, MPI_Offset *disp)NOOP
int wrap_MPI_File_get_group(MPI_File fh, MPI_Group *group)NOOP
int wrap_MPI_File_get_info(MPI_File fh, MPI_Info *info_used)NOOP
int wrap_MPI_File_get_position(MPI_File fh, MPI_Offset *offset)NOOP
int wrap_MPI_File_get_position_shared(MPI_File fh, MPI_Offset *offset)NOOP
int wrap_MPI_File_get_size(MPI_File fh, MPI_Offset *size)NOOP
int wrap_MPI_File_get_type_extent(MPI_File fh, MPI_Datatype datatype, MPI_Aint *extent)NOOP
int wrap_MPI_File_get_view(MPI_File fh, MPI_Offset *disp, MPI_Datatype *etype, MPI_Datatype *filetype, char *datarep)NOOP
int wrap_MPI_File_iread(MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Request *request)NOOP
int wrap_MPI_File_iread_at(MPI_File fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPI_Request *request)NOOP
int wrap_MPI_File_iread_shared(MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Request *request)NOOP
int wrap_MPI_File_iwrite(MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Request *request)NOOP
int wrap_MPI_File_iwrite_at(MPI_File fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPI_Request *request)NOOP
int wrap_MPI_File_iwrite_shared(MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Request *request)NOOP
int wrap_MPI_File_preallocate(MPI_File fh, MPI_Offset size)NOOP
int wrap_MPI_File_read(MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status)NOOP
int wrap_MPI_File_read_all(MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status)NOOP
int wrap_MPI_File_read_all_begin(MPI_File fh, void *buf, int count, MPI_Datatype datatype)NOOP
int wrap_MPI_File_read_all_end(MPI_File fh, void *buf, MPI_Status *status)NOOP
int wrap_MPI_File_read_at(MPI_File fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPI_Status *status)NOOP
int wrap_MPI_File_read_at_all(MPI_File fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPI_Status *status)NOOP
int wrap_MPI_File_read_at_all_begin(MPI_File fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype)NOOP
int wrap_MPI_File_read_at_all_end(MPI_File fh, void *buf, MPI_Status *status)NOOP
int wrap_MPI_File_read_ordered(MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status)NOOP
int wrap_MPI_File_read_ordered_begin(MPI_File fh, void *buf, int count, MPI_Datatype datatype)NOOP
int wrap_MPI_File_read_ordered_end(MPI_File fh, void *buf, MPI_Status *status)NOOP
int wrap_MPI_File_read_shared(MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status)NOOP
int wrap_MPI_File_seek(MPI_File fh, MPI_Offset offset, int whence)NOOP
int wrap_MPI_File_seek_shared(MPI_File fh, MPI_Offset offset, int whence)NOOP
int wrap_MPI_File_set_atomicity(MPI_File fh, int flag)NOOP
int wrap_MPI_File_set_info(MPI_File fh, MPI_Info info)NOOP
int wrap_MPI_File_set_size(MPI_File fh, MPI_Offset size)NOOP
int wrap_MPI_File_set_view(MPI_File fh, MPI_Offset disp, MPI_Datatype etype, MPI_Datatype filetype, char *datarep, MPI_Info info)NOOP
int wrap_MPI_File_sync(MPI_File fh)NOOP
int wrap_MPI_File_write(MPI_File fh, const void *buf, int count, MPI_Datatype datatype, MPI_Status *status)NOOP
int wrap_MPI_File_write_all(MPI_File fh, const void *buf, int count, MPI_Datatype datatype, MPI_Status *status)NOOP
int wrap_MPI_File_write_all_begin(MPI_File fh, const void *buf, int count, MPI_Datatype datatype)NOOP
int wrap_MPI_File_write_all_end(MPI_File fh, const void *buf, MPI_Status *status)NOOP
int wrap_MPI_File_write_at(MPI_File fh, MPI_Offset offset, const void *buf, int count, MPI_Datatype datatype, MPI_Status *status)NOOP
int wrap_MPI_File_write_at_all(MPI_File fh, MPI_Offset offset, const void *buf, int count, MPI_Datatype datatype, MPI_Status *status)NOOP
int wrap_MPI_File_write_at_all_begin(MPI_File fh, MPI_Offset offset, const void *buf, int count, MPI_Datatype datatype)NOOP
int wrap_MPI_File_write_at_all_end(MPI_File fh, const void *buf, MPI_Status *status)NOOP
int wrap_MPI_File_write_ordered(MPI_File fh, const void *buf, int count, MPI_Datatype datatype, MPI_Status *status)NOOP
int wrap_MPI_File_write_ordered_begin(MPI_File fh, const void *buf, int count, MPI_Datatype datatype)NOOP
int wrap_MPI_File_write_ordered_end(MPI_File fh, void *buf, MPI_Status *status)NOOP
int wrap_MPI_File_write_shared(MPI_File fh, const void *buf, int count, MPI_Datatype datatype, MPI_Status *status)NOOP
int wrap_MPI_Register_datarep(const char *datarep, MPI_Datarep_conversion_function *read_conversion_fn, MPI_Datarep_conversion_function *write_conversion_fn, MPI_Datarep_extent_function *dtype_file_extent_fn, void *extra_state)NOOP

/* Language Bindings */
int wrap_MPI_Type_create_f90_complex(int p, int r, MPI_Datatype *newtype)NOOP
int wrap_MPI_Type_create_f90_integer(int r, MPI_Datatype *newtype)NOOP
int wrap_MPI_Type_create_f90_real(int p, int r, MPI_Datatype *newtype)NOOP
int wrap_MPI_Type_match_size(int typeclass, int size, MPI_Datatype *type)NOOP

/* ??? */
#if !defined (MPI_Errhandler_f2c)
    /* Some implementations define this one as a macro. Luckily, we don't need to do anything with it. */
    MPI_Errhandler wrap_MPI_Errhandler_f2c(MPI_Fint errhandler)NOOP_ERR
#endif
#if !defined (MPI_Errhandler_c2f)
    /* Some implementations define this one as a macro. Luckily, we don't need to do anything with it. */
    MPI_Fint wrap_MPI_Errhandler_c2f(MPI_Errhandler errhandler)NOOP
#endif
