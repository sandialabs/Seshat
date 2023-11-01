/*
** Copyright (2007) Sandia Corporation. Under the terms of Contract DE-AC04-94AL85000
** with Sandia Corporation, the U.S. Government retains certain rights in this software.
**
** Seshat
** $Id: scs.h,v 1.67 2007/11/21 22:15:27 rolf Exp $
** Rolf Riesen (rolf@sandia.gov), June 2006, Sandia National Laboratories
*/

#ifndef _SCS_H_
#define _SCS_H_

#include <stdio.h>
#include <mpi.h>



/* Globals */
extern int _scs_verbose;
extern int _scs_rank;		/* Within all simulators (the real MPI_COMM_WORLD) */
extern int _scs_nnodes;		/* Within all simulators (the real MPI_COMM_WORLD) */
extern MPI_Comm _scs_app_comm;	/* The communicator used by the application */
extern MPI_Comm _scs_comm_world;/* The real MPI_COMM_WORLD */
extern int _scs_net_sim_node;	/* Which node the net sim occupies (currently n - 1) */
extern double _scs_wtime_offset;



/* Some handy defines */
#ifndef TRUE
#define 	TRUE	(1)
#endif  /* TRUE */
#ifndef FALSE
#define 	FALSE	(0)
#endif  /* FALSE */
#ifndef MAX
#define 	MAX(a,b)	 ((a) > (b) ? (a):(b))
#endif  /* MAX */

#define MAX_LINE	(1024)



/* Virtual time */
typedef double _scs_vtime_t;
_scs_vtime_t _scs_get_lvtime(void);
void _scs_set_lvtime(_scs_vtime_t t);
void _scs_reset_lvtime(void);
double _scs_systime(void);
void _scs_vtime_stats(void);



/*
** Data file management
*/
void _scs_open_out(char *fname, char *type, FILE **file);
int _scs_create_out(char *fname, char *type);
void _scs_file_print(FILE *file, char *leader, char *fmt, ...);
extern FILE *_scs_dbg_out;	/* File pointer for debug messages */
extern FILE *_scs_vtime_t_out;	/* File pointer for virtual time info */
extern FILE *_scs_mdd_out;	/* File pointer for message density info */
extern FILE *_scs_ddd_out;	/* File pointer for data density info */
extern FILE *_scs_col_out;	/* File pointer for collectives info */
extern FILE *_scs_p2p_out;	/* File pointer for point-to-point versus collectives info */
extern FILE *_scs_msize_out;	/* File pointer for message size info */
extern FILE *_scs_trace_out;	/* File pointer for trace file */
extern FILE *_scs_msg_data_out;	/* File pointer for message data file */
#define OUT_VTIME(leader, fmt, ...) _scs_file_print(_scs_vtime_t_out, leader, fmt, __VA_ARGS__)
#define OUT_MDD(leader, fmt, ...) _scs_file_print(_scs_mdd_out, leader, fmt, __VA_ARGS__)
#define OUT_DDD(leader, fmt, ...) _scs_file_print(_scs_ddd_out, leader, fmt, __VA_ARGS__)
#define OUT_COL(leader, fmt, ...) _scs_file_print(_scs_col_out, leader, fmt, __VA_ARGS__)
#define OUT_P2P(leader, fmt, ...) _scs_file_print(_scs_p2p_out, leader, fmt, __VA_ARGS__)
#define OUT_MSIZE(leader, fmt, ...) _scs_file_print(_scs_msize_out, leader, fmt, __VA_ARGS__)
#define OUT_TRACE(leader, fmt, ...) _scs_file_print(_scs_trace_out, leader, fmt, __VA_ARGS__)



/* Debugging support */
#define Debug(type, fmt, ...) _scs_Debug(__func__, __LINE__, type, fmt, __VA_ARGS__)
#define Warn(fmt, ...) _scs_Warn(__func__, __LINE__, "WARNING", fmt, __VA_ARGS__)
#define Err(fmt, ...) _scs_Warn(__func__, __LINE__,  "ERROR  ", fmt, __VA_ARGS__)
void _scs_Debug(const char *func, int line, int dbg, char *fmt, ...);
void _scs_Warn(const char *func, int line, char *type, char *fmt, ...);

#define DBG_CONFIG		(0x0001)	/* Config file processing */
#define DBG_PROCESSMGMT		(0x0002)	/* Process management */
#define DBG_MPI			(0x0004)	/* MPI related things */
#define DBG_MPI2		(0x0008)	/* MPI verbose */
#define DBG_MPI3		(0x0010)	/* Non-C MPI calls (e.g. Fortran) */
#define DBG_EVENT		(0x0020)	/* Events */
#define DBG_EMU			(0x0040)	/* Emulator */
#define DBG_NET			(0x0080)	/* Network sim */
#define DBG_TIME		(0x0100)	/* (virtual) time */
#define DBG_MPI_INTERNAL	(0x0200)	/* Internal MPI usage */
#define DBG_GENERAL		(0x0400)	/* General (non-verbose) information */
#define DBG_REQ 		(0x0800)	/* Tracking non-blocking calls using the request ptr */

/* All flags on */
#define DBG_ALL			(DBG_CONFIG | DBG_PROCESSMGMT | DBG_MPI | DBG_MPI2 | DBG_MPI3 | \
                                 DBG_EVENT | DBG_EMU | DBG_NET | DBG_TIME | DBG_MPI_INTERNAL | \
                                 DBG_GENERAL | DBG_REQ)



/* The main start function */
int _scs_sim_start(int *argc, char **argv[]);



/*
** Configuration file keywords we understand
*/
#define KEY_DEBUG		"debug"
#define KEY_DEBUG_FILE		"debug_file"
#define KEY_VTIME_FILE		"vtime_file"
#define KEY_MDD_FILE		"mdd_file"
#define KEY_DDD_FILE		"ddd_file"
#define KEY_COL_FILE		"col_file"
#define KEY_P2P_FILE		"p2p_file"
#define KEY_MSIZE_FILE		"msize_file"
#define KEY_NET_BW_FACTOR	"net_bw_factor"
#define KEY_NET_LAT_FACTOR	"net_lat_factor"
#define KEY_NET_COLL_FACTOR	"net_coll_factor"
#define KEY_NET_CPU_FACTOR	"net_cpu_factor"
#define KEY_NET_REPORTING	"net_reporting"
#define KEY_SIM_SLEEP		"sim_sleep"
#define KEY_GENERAL		"general"
#define KEY_TRACE_FILE		"trace_file"
#define KEY_MSG_DATA_FILE	"msg_data_file"
#define KEY_MPI_LONG_PROTO	"mpi_long_proto"
#define KEY_REQ_TRACING 	"debug_req"



/* Data structure to collect global simulator information */
typedef struct   {
    int abort;
    int verbose;
    double net_bw_factor;
    double net_lat_factor;
    double net_coll_factor;
    double net_cpu_factor;
    int net_reporting;
    int mpi_long_proto;
    long int sim_sleep;
    char dbg_out[MAX_LINE];
    char vtime_out[MAX_LINE];
    char mdd_out[MAX_LINE];
    char ddd_out[MAX_LINE];
    char col_out[MAX_LINE];
    char p2p_out[MAX_LINE];
    char msize_out[MAX_LINE];
    char trace_out[MAX_LINE];
    char msg_data_out[MAX_LINE];
} _scs_config_t;

extern _scs_config_t _scs_config;



/* Functions to read and parse the configuration file */
void _scs_config_file(void);
int _scs_parse_config(char *buf, _scs_config_t *cfg, size_t buf_size);



/* Functions to manage the simulators */
void _scs_net_sim_start(void) __attribute__ ((noreturn));
void _scs_build_app_comm(void);



/* Functions and macros that deal with the MPI wrappers */
extern int _scs_MPI_initialized;
void _scs_init_MPI_wrappers(void);
void _scs_req_insert(MPI_Request request, int send, _scs_vtime_t tr, int len, unsigned int token);
int _scs_req_find(MPI_Request request, int *send, _scs_vtime_t *tr, int *len, unsigned int *token);
int _scs_req_dump(void);
void _scs_event_reduce(_scs_vtime_t t1, void *data, int dest, int len, int collective, MPI_Datatype dt);
void _scs_event_bcast(_scs_vtime_t t1, void *data, int root, int len, int collective, MPI_Datatype dt);
void _scs_event_exchange(_scs_vtime_t t1, void *sendbuf, void *recvbuf, int root, int len, int collective, MPI_Datatype dt);



/* Functions to call MPI internally */
int _scs_bytelen(MPI_Datatype dt, int len);
int _scs_bytelenF(MPI_Fint *dt, int len);
int _scs_MPI_Init(int *argc, char **argv[]);
int _scs_MPI_Finalize(void);
int _scs_MPI_Send(void *data, int len, MPI_Datatype dt, int dest, int tag, MPI_Comm comm);
int _scs_MPI_Iprobe(int src, int tag, MPI_Comm comm, int *flag, MPI_Status *status);
int _scs_MPI_Recv(void *data, int len, MPI_Datatype dt, int src, int tag, MPI_Comm comm, MPI_Status *status);
int _scs_MPI_Comm_size(MPI_Comm comm, int *size);
int _scs_MPI_Comm_rank(MPI_Comm comm, int *rank);
int _scs_MPI_Comm_dup(MPI_Comm d1, MPI_Comm *d2);
double _scs_MPI_Wtime(void);
int _scs_MPI_Barrier(MPI_Comm comm);
int _scs_MPI_Bcast(void *buffer, int count, MPI_Datatype dt, int root, MPI_Comm comm);
int _scs_MPI_Comm_group(MPI_Comm d1, MPI_Group *d2);
int _scs_MPI_Group_incl(MPI_Group group, int size, int *nodes, MPI_Group *new_group);
int _scs_MPI_Comm_create(MPI_Comm comm, MPI_Group group, MPI_Comm *new_comm);
int _scs_MPI_Group_size(MPI_Group group, int *size);
int _scs_MPI_Group_rank(MPI_Group group, int *rank);



/* Events */
typedef enum {E_NONE, E_DONE, E_SEND, E_RECV, E_POST, E_MATCHED, E_COLLECTIVE} _scs_event_type_t;

typedef struct _scs_event_t    {
    _scs_event_type_t type;
    _scs_vtime_t time;
    _scs_vtime_t delay;
    int dest;                   /* within MPI_COMM_WORLD */
    int src;                    /* within MPI_COMM_WORLD */
    int tag;
    int len;                    /* in bytes */
    int collective;             /* 0 for p2p, type of collective otherwise */
    unsigned int token;         /* We use this to match only once in the simulator */
    MPI_Datatype dt;            /* Edgar needs this in the trace file */
    int mpi_count;              /* Number of data type units: mpi_len * sizoef(dt) = len */
    long int msg_data_pos;	/* Msg Data is at offset msg_data_pos in the file */
    struct _scs_event_t *next;
} _scs_event_t;

typedef struct _scs_eventQ_t    {
    _scs_event_t *head;
    _scs_event_t *tail;
} _scs_eventQ_t;

#define NO_TOKEN        (0)
#define COLL_TOKEN        (1)
void _scs_node_event_init(void);
unsigned int _scs_gen_token(void);
void _scs_event_send(_scs_vtime_t t, int len, MPI_Datatype dt, int dest, int tag, unsigned int token, int collective, long int pos);
void _scs_event_post(_scs_vtime_t t, int src, int tag, unsigned int token);
void _scs_event_wait(_scs_vtime_t *t, _scs_vtime_t *delay, unsigned int token);
void _scs_event_done(void);
void _scs_event_to_node(_scs_event_t *e, int where);
void check_for_remote_events(_scs_eventQ_t *queue);



/* Event Queue Management */
_scs_event_t * _scs_create_event(void);
void _scs_delete_event(_scs_event_t *e);

void _scs_create_eventQ(_scs_eventQ_t **queue);
void _scs_delete_eventQ(_scs_eventQ_t *queue);
void _scs_push_eventQ(_scs_eventQ_t *queue, _scs_event_t *event);
_scs_event_type_t _scs_match_eventQ(_scs_eventQ_t *queue, _scs_event_t **event, int src, int dest, int tag, unsigned int token);
_scs_event_type_t _scs_pop_eventQ(_scs_eventQ_t *queue, _scs_event_t **event);



/* The network simulator */
void _scs_main_net(void);

/* Name the system we are simulating, and ensure only one sim is linked in. */
extern char simsystem[];



/* General functions */
void _scs_abort(void) __attribute__ ((noreturn));
void _scs_normal_exit(void);



/* "Tags" to identify which collectives have been used. */
#define MPI_Reduce_TAG		(0x7420)
#define MPI_Allreduce_TAG	(0x7421)
#define MPI_Alltoall_TAG	(0x7422)
#define MPI_Alltoallv_TAG	(0x7423)
#define MPI_Barrier_TAG		(0x7424)
#define MPI_Bcast_TAG		(0x7425)
#define MPI_Scatterv_TAG	(0x7426)
#define MPI_Allgather_TAG	(0x7427)
#define MPI_Allgatherv_TAG	(0x7428)
#define MPI_Gatherv_TAG		(0x7429)
#define MPI_Reduce_scatter_TAG	(0x742a)
#define MPI_Gather_TAG		(0x742b)
#define MPI_Scan_TAG		(0x742c)

/* Tags used by event handler communications */
#define NEW_EVENT               (0x7510)




/* Functions to keep message passing statistics */
void _scs_init_records(int nnodes);
void _scs_record_event(_scs_event_t *e);
void _scs_print_report(int nnodes);



/* Functions a network simulator must provide */
double _scs_model_ptop(int len, double bw_factor, double lat_factor, int same_board);
double _scs_model_col(int len, int nodes, double coll_factor);


/* Functions and definitions for tracing */
int _scs_init_trace(int nnodes);
void _scs_trace_event(_scs_event_t *e);

/* Change this if code changes in Seshat will break external parsers of trace files */
#define TRACE_V_MAJOR	(1)

/* Change this if code changes in Seshat will NOT break external parsers of trace files */
#define TRACE_V_MINOR	(1)

#define TRACE_S		"Seshat Trace File Version:"




/* Functions for writing message data to a file */
int _scs_init_msg_data(void);
long int _scs_msg_data_write(const void *buf, int len);


#endif /* _SCS_H_ */
