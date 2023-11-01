/*
** Copyright (2007) Sandia Corporation. Under the terms of Contract DE-AC04-94AL85000
** with Sandia Corporation, the U.S. Government retains certain rights in this software.
**
** Seshat
** $Id: scs_main.c,v 1.21 2007/09/28 21:36:26 rolf Exp $
** Rolf Riesen (rolf@sandia.gov), June 2006, Sandia National Laboratories
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "scs.h"

/*
** Globals
*/
int _scs_rank, _scs_nnodes;
int _scs_verbose;
MPI_Comm _scs_comm_world;



/*
** Local functions
*/
static void MPI_timer_resolution(void);



/*
** This is where the SuperComputer Simulator starts.
** Currently we asume the network simulator runs on the last node of the group (n-1).
** All other nodes run the appplication.
*/
int
_scs_sim_start(int *argc, char **argv[])
{

int rc;


    /* Set some defaults */
    _scs_verbose= 0;


    /* Initialize MPI */
    rc= _scs_MPI_Init(argc, argv);
    _scs_comm_world= MPI_COMM_WORLD;
    _scs_MPI_Comm_size(_scs_comm_world, &_scs_nnodes);
    _scs_MPI_Comm_rank(_scs_comm_world, &_scs_rank);


    /* Read and distribute the configuration file */
    _scs_config_file();


    /* Make sure we have a working timer. */
    MPI_timer_resolution();


    /* Setup the groups and communicators we need */
    _scs_build_app_comm();


    /* Make sure everybody is ready and has printed everything that needs to go out! */
    fflush(stderr);
    fflush(stdout); 
    _scs_MPI_Barrier(_scs_comm_world);


    /* Start the net sim and let the other nodes return to the application */
    if (_scs_rank == _scs_net_sim_node)   {
	/* This will not return */
	_scs_net_sim_start();
    }

    /* Initialize the node side event management system */
    _scs_node_event_init();

    /* For Edgars traces with data, we need to open the file here and get a file pointer */
    _scs_init_msg_data();

    Debug(DBG_PROCESSMGMT, "Starting application\n", "");
    return rc;

}  /* end of _scs_sim_start() */



void
_scs_abort(void)
{

    /* Kill everybody and exit */
    Warn("!!! _scs_abort() got called on node %d\n", _scs_rank);
    _exit(EXIT_FAILURE);

}  /* end of _scs_abort() */



/*
** Check the timer resolution...
*/
static void
MPI_timer_resolution(void)
{

double tickval;
int icnt, cnt;
double t1, t2;
int max= 0;


    t2= _scs_get_lvtime();
    cnt= 0;
    for (icnt= 0; icnt < 100000; icnt++)   {
	t1= _scs_get_lvtime();
	if (t1 < t2)   {
	    Warn("lvtime went backwards!\n", "");
	    _scs_abort();
	}
	if (t1 == t2) cnt++;
	if (t1 > t2) cnt= 0;
	if (cnt > max) max= cnt;
	t2= t1;
    }

    Debug(DBG_TIME, "_scs_get_lvtime() returned the same number at most %d times\n", max);

    t1= _scs_get_lvtime();
    t1= _scs_get_lvtime();
    _scs_set_lvtime(t1 + 2000);
    t1= _scs_get_lvtime();
    t2= _scs_get_lvtime(); /* Warm up */
    t2= _scs_get_lvtime();
    if (t1 == t2)   {
	Warn("No advance after scs_set_time()!\n", "");
	/* _scs_abort(); */
    }



    if (_scs_verbose & DBG_TIME)   {
	tickval= 1.0e6;
	for (icnt= 0; icnt < 10; icnt++)   {
	    cnt= 100000;
	    t1= _scs_MPI_Wtime();
	    while (cnt--)   {
		t2= _scs_MPI_Wtime();
		if (t2 > t1)
		    break;
	    }
	    if (cnt && t2 > t1 && t2 - t1 < tickval)
		tickval= t2 - t1;
	}

	Debug(DBG_TIME, "MPI resolution is %8.6f\n", tickval);
    }


    _scs_reset_lvtime();

}  /* end of MPI_timer_resolution() */
