/*
** Copyright (2007) Sandia Corporation. Under the terms of Contract DE-AC04-94AL85000
** with Sandia Corporation, the U.S. Government retains certain rights in this software.
**
** Seshat
** $Id: scs_vtime.c,v 1.19 2007/06/21 23:48:01 rolf Exp $
** Rolf Riesen (rolf@sandia.gov), September 2005, Sandia National Laboratories
**
** Function to help manage virtual time
*/

#include <stdio.h>
#include <time.h>
#ifdef REDSTORM
    #include <catamount/dclock.h>
#endif
#include "scs.h"

/*
** Globals
*/
double _scs_wtime_offset= 0.0; /* Normalizes clock to 0.0 = start of prog. */
FILE *_scs_vtime_t_out= NULL;


/*
** Local globals
*/
static _scs_vtime_t tadvance= 0.0;
static long int cnt_get= 0;
static long int cnt_set= 0;
static long int cnt_set_back= 0;



#ifdef REDSTORM
    /*
    ** On Red Storm we use dclock
    */
    double
    _scs_systime(void)
    {
	return dclock() - _scs_wtime_offset;
    }  /* end of _scs_systime() */

#else

    /*
    ** MPI_Wtime on my system has about 4 miliseconds of resolution!
    ** clock_gettime() has 1 ns...
    */
    double
    _scs_systime(void)
    {

    double val;
    int rc;
    struct timespec tm;


	rc= clock_gettime(CLOCK_REALTIME, &tm);
	if (rc)   {
	    Err("clock_gettime() returned error!\n", "");
	    _scs_abort();
	}
	val= tm.tv_sec + tm.tv_nsec / 1000000000.0;
	return val - _scs_wtime_offset;

    }  /* end of _scs_systime() */
#endif /* REDSTORM */





/*
** Return the current local virtual time. Depending on the system timer
** in use, this may not advance between calls.
*/
_scs_vtime_t
_scs_get_lvtime(void)
{

    cnt_get++;
    return _scs_systime() + tadvance;

}  /* end of _scs_get_lvtime() */


/*
** Set  current local virtual time
*/
void
_scs_set_lvtime(_scs_vtime_t t)
{

    if ((_scs_systime() + tadvance) > t)   {
	cnt_set_back++;
    }

    tadvance= t - _scs_systime();
    cnt_set++;

}  /* end of _scs_set_lvtime() */


void
_scs_reset_lvtime(void)
{

    tadvance= 0.0;
    _scs_wtime_offset= 0.0;
    _scs_wtime_offset= _scs_systime();
    cnt_get= 0;
    cnt_set= 0;
    cnt_set_back= 0;

}  /* end of _scs_reset_lvtime() */



void
_scs_vtime_stats(void)
{

    OUT_VTIME("# ", "[%3d] Calls to _scs_get_lvtime()    %ld\n", _scs_rank, cnt_get);
    OUT_VTIME("# ", "[%3d] Calls to _scs_set_lvtime()    %ld\n", _scs_rank, cnt_set);
    OUT_VTIME("# ", "[%3d]     Setting vtime back       %ld\n", _scs_rank, cnt_set_back);
    OUT_VTIME("# ", "[%3d] tadvance                     %8.6f\n", _scs_rank, tadvance);
    OUT_VTIME("# ", "[%3d] _scs_get_lvtime()             %8.6f\n", _scs_rank, _scs_get_lvtime());

}  /* end of _scs_vtime_stats() */
