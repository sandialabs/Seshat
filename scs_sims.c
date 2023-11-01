/*
** Copyright (2007) Sandia Corporation. Under the terms of Contract DE-AC04-94AL85000
** with Sandia Corporation, the U.S. Government retains certain rights in this software.
**
** Seshat
** $Id: scs_sims.c,v 1.13 2007/06/21 23:48:01 rolf Exp $
** Rolf Riesen (rolf@sandia.gov), June 2006, Sandia National Laboratories
**
** Functions and structures to manage the simulators
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "scs.h"



/*
** Globals
*/
int _scs_net_sim_node= -1;
MPI_Comm _scs_app_comm;



/*
** Create a group communicator that includes only the application nodes.
*/
void
_scs_build_app_comm(void)
{

int i, rc;
MPI_Group group_world;
MPI_Group group;
int *nodes;


    nodes= (int *)malloc(sizeof(int) * _scs_nnodes - 1);
    if (nodes == NULL)   {
	Err("Out of memory!\n", "");
	_scs_abort();
    }
    for (i= 0; i < _scs_nnodes; i++)   {
	nodes[i]= i;
    }

    rc= _scs_MPI_Comm_group(_scs_comm_world, &group_world);
    if (rc != MPI_SUCCESS)   {
	Err("MPI_Comm_group() failed\n", "");
	_scs_abort();
    }

    rc= _scs_MPI_Group_incl(group_world, _scs_nnodes - 1, nodes, &group);
    if (rc != MPI_SUCCESS)   {
	Err("MPI_Group_incl() failed\n", "");
	_scs_abort();
    }

    if (_scs_verbose & DBG_MPI2)   {
	char str[10 * 1024];
	str[0]= '\0';
	for (i= 0; i < _scs_nnodes - 1; i++)   {
	    sprintf(str, "%s %d", str, nodes[i]);
	}
	Debug(DBG_MPI2, "Building a group out of %s\n", str);
    }

    rc= _scs_MPI_Comm_create(_scs_comm_world, group, &_scs_app_comm);
    if (rc != MPI_SUCCESS)   {
	Err("MPI_Comm_create() failed\n", "");
	_scs_abort();
    }


    if (_scs_verbose & DBG_MPI2)   {
	int gsize;
	int wrank, grank;

	Debug(DBG_MPI2, "MPI_COMM_WORLD %d, _scs_comm_world %d, _scs_app_comm %d\n",
	    MPI_COMM_WORLD, _scs_comm_world, _scs_app_comm);

	_scs_MPI_Comm_rank(_scs_comm_world, &wrank);
	_scs_MPI_Group_size(group, &gsize);
	_scs_MPI_Group_rank(group, &grank);

	if (grank == MPI_UNDEFINED)   {
	    Debug(DBG_MPI2, "World rank %2d: Not in this group!\n", wrank);
	} else   {
	    Debug(DBG_MPI2, "World rank %2d, group rank %2d, group size %2d\n", wrank, grank, gsize);
	}
    }

    /* The net sim is fixed (for now) at n - 1 */
    _scs_net_sim_node= _scs_nnodes - 1;

    /* Only the app nodes need to (can) call this */
    if (_scs_rank != _scs_net_sim_node)   {
	_scs_init_MPI_wrappers();
    }

}  /* end of scs_node_sim_init_mpi() */



void
_scs_net_sim_start(void)
{

    _scs_main_net();
    _scs_normal_exit();

    /* We never return */
    _exit(EXIT_SUCCESS);

}  /* end of scs_net_sim_star() */
