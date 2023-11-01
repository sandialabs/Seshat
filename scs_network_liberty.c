/*
** Copyright (2007) Sandia Corporation. Under the terms of Contract DE-AC04-94AL85000
** with Sandia Corporation, the U.S. Government retains certain rights in this software.
**
** Seshat
** $Id: scs_network_liberty.c,v 1.6 2007/06/21 23:48:01 rolf Exp $
** Rolf Riesen (rolf@sandia.gov), June 2006, Sandia National Laboratories
*/

#include <stdio.h>
#include <stdlib.h>
#include "scs.h"



/*
** Define what system we are simulating
*/
char simsystem[]= "Liberty Myrinet Cluster";


/*
** Local functions
*/
static double node_model(int len, double bw_factor, double lat_factor);
static double board_model(int len, double bw_factor, double lat_factor);



/*
** *************************************************************************************************
** This is a model of the liberty network
*/
double
_scs_model_col(int len, int nodes, double coll_factor)
{

int dummy;


    /* FIXME: Not implemented yet! */
    dummy= nodes;
    return _scs_model_ptop(len, 1.0, 1.0, 0) * coll_factor;

} /* end of _scs_model_col() */



double
_scs_model_ptop(int len, double bw_factor, double lat_factor, int same_board)
{

    if (same_board)   {
	return board_model(len, bw_factor, lat_factor);
    } else   {
	return node_model(len, bw_factor, lat_factor);
    }

}  /* end of _scs_model_ptop() */



static double
node_model(int len, double bw_factor, double lat_factor)
{

double bw_in, lat_in;
double delay;


    bw_in= 180000000.0 * bw_factor;
    lat_in= 0.00000800 * lat_factor;

    if (len >= 256)   {
	lat_in= 0.0000105 * lat_factor;
    }

    if (len >= 2 * 1024)   {
	bw_in= 208000000.0 * bw_factor;
	lat_in= 0.00001600 * lat_factor;
    }

    if (len > 5 * 1024)   {
	bw_in= 200000000.0 * bw_factor;
    }

    if (len > 16 * 1024)   {
	lat_in= 0.00001500 * lat_factor;
    }

    if (len >= 32 * 1024)   {
	lat_in= 0.000040 * lat_factor;
	bw_in= 245000000.0 * bw_factor;
    }

    delay= lat_in + (len / bw_in);

    return delay;

}  /* end of node_model() */



static double
board_model(int len, double bw_factor, double lat_factor)
{

double delay;
double lat_in, bw_in;


    bw_in= 1230000000.0 * bw_factor;
    lat_in= 0.00000100 * lat_factor;

    if (len < 2 * 1024)   {
	lat_in= 0.00000002 * lat_factor;
	bw_in= 4800000000.0 * bw_factor;
    } 

    if (len >= 2 * 1024)   {
	lat_in= 0.00000050 * lat_factor;
    }

    if (len >= 3 * 1024)   {
	lat_in= 0.00000300 * lat_factor;
    }

    if (len >= 4 * 1024)   {
	lat_in= 0.00000380 * lat_factor;
    }

    if (len >= 5 * 1024)   {
	lat_in= 0.00000160 * lat_factor;
    }

    if (len >= 6 * 1024)   {
	lat_in= 0.00000090 * lat_factor;
    }

    if (len >= 32 * 1024)   {
	bw_in= 1180000000.0 * bw_factor;
	lat_in= 0.00000178 * lat_factor;
    }

    if (len < 32 * 1024)   {
	delay= lat_in + (len / bw_in);

    } else if (len < 70 * 1024)   {
	delay= (lat_in + 32 * 1024 / bw_in);
	delay+= (len - 32 * 1024) * (lat_in / 1800);

    } else if (len < 88 * 1024)   {
	delay= (lat_in + 32 * 1024 / bw_in);
	delay+= (len - 32 * 1024) * (lat_in / 1800);
	delay+= (len - 70 * 1024) * (lat_in / 12000);

    } else if (len < 96 * 1024)   {
	lat_in= len * 0.00000000070 * lat_factor;
	bw_in= 3400000000.0 * bw_factor;
	delay= lat_in + (len / bw_in);

    } else if (len < 104 * 1024)   {
	lat_in= 0.0000220 * lat_factor;
	bw_in= 1200000000.0 * bw_factor;
	delay= lat_in + (len / bw_in);

    } else if (len < 112 * 1024)   {
	lat_in= 0.0000130 * lat_factor;
	bw_in= 1000000000.0 * bw_factor;
	delay= lat_in + (len / bw_in);

    } else   {
	lat_in= 0.0000130 * lat_factor;
	bw_in=  970000000.0 * bw_factor;
	delay= lat_in + (len / bw_in);

    }

    return delay;

}  /* end of board_model() */
