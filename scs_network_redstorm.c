/*
** Copyright (2007) Sandia Corporation. Under the terms of Contract DE-AC04-94AL85000
** with Sandia Corporation, the U.S. Government retains certain rights in this software.
**
** Seshat
** $Id: scs_network_redstorm.c,v 1.15 2007/06/21 23:48:01 rolf Exp $
** Rolf Riesen (rolf@sandia.gov), April 2006, Sandia National Laboratories
**
** This is a very simple network that mimics the one I measured on Red Storm April 14.
** It accepts messages, adds a delay, and generates the corresponding event.
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
    double log2(double x);
#include "scs.h"


/*
** Define what system we are simulating
*/
char simsystem[]= "Cray Unicos/lc 1.5.39";


/*
** Local functions
*/
static double node_model(int len, double bw_factor, double lat_factor);
static double board_model(int len, double bw_factor, double lat_factor);



/*
** *************************************************************************************************
** This is a model of the Red Storm network
*/
double
_scs_model_col(int len, int nnodes, double coll_factor)
{

double res;
double a, b;
double x1, x2, y1, y2;


    /*
    ** The divide by 2.0 at the end is because the model was for a alltoall with a
    ** fan-out and fan-in. The simulator returns a seperate delay for each direction.
    */



    /*
    ** Starting with release 1.4.45 the model is not that simple anymore...
    */
    x1= 32768.0;
    x2= 196608.0;
    if (nnodes <= 4)   {
	y2= 0.003124;
	y1= 0.000435;
    } else if (nnodes <= 16)   {
	if (len < (32 * 1024 * sizeof(int)))   {
	    x2= 31744.0;
	    x1= 16.0;
	    y2= 0.00551444;
	    y1= 0.0000725682;
	} else   {
	    y2= 0.01278218275;
	    y1= 0.0023147818;
	}
    } else if (nnodes <= 64)   {
	if (len < (32 * 1024 * sizeof(int)))   {
	    x2= 31744.0;
	    x1= 16.0;
	    y2= 0.03783153;
	    y1= 0.000149668;
	} else   {
	    y2= 0.100295;
	    y1= 0.017838;
	}
    } else if (nnodes <= 128)   {
        if (len < (32 * 1024 * sizeof(int)))   {
            x2= 31744.0;
            x1= 16.0;
            y2= 0.0788110682;
            y1= 0.0002342304;
        } else   {
            y2= 0.278757;
            y1= 0.047909;
        }
    } else   {
	y2= 0.791887;
	y1= 0.151284;
    }

    b= (y2 - y1) / (x2 - x1);			/* Calculate slope */
    a= y1 - (((y2 - y1) * x1) / (x2 - x1));	/* Calculate y intercept */
    res= a + b * len / sizeof(int);		/* Calculate y at len */
    return res / 2.0;				/* One direction only */

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


    if (len < 16)   {
        delay= 0.00000574 * lat_factor;
    }

    if (len >= 16)   {
        bw_in= 2900000000 * bw_factor;
        lat_in= 0.00000605 * lat_factor;
        delay= lat_in + (len / bw_in);
    }

    if (len >= 32)   {
        bw_in= 2900000000 * bw_factor;
        lat_in= 0.00000910 * lat_factor;
        delay= lat_in + (len / bw_in);
    }

    if (len >= 300)   {
        bw_in= 2900000000 * bw_factor;
        lat_in= 0.00000930 * lat_factor;
        delay= lat_in + (len / bw_in);
    }

    if (len >= 2 * 1024)   {
        bw_in= 2500000000 * bw_factor;
        lat_in= 0.00000930 * lat_factor;
        delay= lat_in + (len / bw_in);
    }

    if (len >= 16 * 1024)   {
        bw_in= 2400000000 * bw_factor;
        lat_in= 0.00000900 * lat_factor;
        delay= lat_in + (len / bw_in);
    }

    if (len >= 32 * 1024)   {
        bw_in= 2120000000 * bw_factor;
        lat_in= 0.00000680 * lat_factor;
        delay= lat_in + (len / bw_in);
    }

    if (len >= 126 * 1024)   {
        bw_in= 2020000000 * bw_factor;
        lat_in= 0.00000480 * lat_factor;
        delay= lat_in + (len / bw_in);
    }

    if (len > 128 * 1024)   {
        bw_in= 2075000000 * bw_factor;
        lat_in= 0.00000620 * lat_factor;
        delay= lat_in + (len / bw_in);
    }

    return delay;

}  /* end of node_model() */



static double
board_model(int len, double bw_factor, double lat_factor)
{

double bw_in, lat_in;
double delay;


    if (len < 16)   {
        delay= 0.00000340 * lat_factor;
    }

    if (len >= 16)   {
        bw_in= 2000000000.0 * bw_factor;
        lat_in= 0.00000480 * lat_factor;
        delay= lat_in + (len / bw_in);
    }

    if (len >= 2 * 2024)   {
        bw_in= 2090000000.0 * bw_factor;
        lat_in= 0.00000430 * lat_factor;
        delay= lat_in + (len / bw_in);
    }

    if (len >= 32 * 1024)   {
        bw_in= 1700000000.0 * bw_factor;
        lat_in= 0.00000090 * lat_factor;
        delay= lat_in + (len / bw_in);
    }

    if (len >= 64 * 1024)   {
        bw_in= 1880000000.0 * bw_factor;
        lat_in= 0.00000500 * lat_factor;
        delay= lat_in + (len / bw_in);
    }

    if (len >= 126 * 1024)   {
        bw_in= 1850000000.0 * bw_factor;
        lat_in= 0.00000550 * lat_factor;
        delay= lat_in + (len / bw_in);
    }

    if (len >= 255 * 1024)   {
        bw_in= 1850000000.0 * bw_factor;
        lat_in= 0.00000530 * lat_factor;
        delay= lat_in + (len / bw_in);
    }

    if (len >= 384 * 1024)   {
        bw_in= 1455000000.0 * bw_factor;
        lat_in= -0.00005200 * lat_factor;
        delay= lat_in + (len / bw_in);
    }

    if (len > 512 * 1024)   {
        bw_in= 2010000000.0 * bw_factor;
        lat_in= 0.00001200 * lat_factor;
        delay= lat_in + (len / bw_in);
    }

    return delay;

}  /* end of board_model() */
