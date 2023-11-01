/*
** Copyright (2007) Sandia Corporation. Under the terms of Contract DE-AC04-94AL85000
** with Sandia Corporation, the U.S. Government retains certain rights in this software.
**
** Seshat
** $Id: scs_config_read.c,v 1.32 2008/04/12 16:04:01 rolf Exp $
** Rolf Riesen (rolf@sandia.gov), June 2006, Sandia National Laboratories
**
** Read and distribute the configuration file
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include "scs.h"


/*
** Some constants for the config file
*/
#define CONFIG_DEFAULT_NAME	"scs_conf"
#define CONFIG_ENV_NAME		"SCS_CONF_FILE_NAME"
static char config_file[MAX_LINE];


/*
** The data structure that holds all global info about the simulator
*/
_scs_config_t _scs_config;



/*
** Local functions
*/
static void read_config(char *fname, _scs_config_t *cfg);
static void _scs_print_verbose(void);
static void _scs_print_config(void);



/*
** Figure out the name of the configuration file (dafault or from env)
** and let node 0 read and distribute it.
*/
void
_scs_config_file(void)
{

char *fname;
int rc;


    _scs_verbose= DBG_CONFIG;
    if (_scs_rank == 0)   {
	fname= getenv(CONFIG_ENV_NAME);
	if (fname == NULL)   {
	    /* Use default */
	    strncpy(config_file, CONFIG_DEFAULT_NAME, MAX_LINE);
	    Debug(DBG_CONFIG, "Default config file name: \"%s\"\n", config_file);
	} else   {
	    /* Use name from env */
	    strncpy(config_file, fname, MAX_LINE);
	    Debug(DBG_CONFIG, "Config file name from env: \"%s\"\n", config_file);
	}


	/* Set some defaults */
	memset(&_scs_config, 0, sizeof(_scs_config));
	_scs_config.abort= FALSE;
	_scs_config.net_bw_factor= 1.0;
	_scs_config.net_lat_factor= 1.0;
	_scs_config.net_coll_factor= 1.0;
	_scs_config.net_cpu_factor= 1.0;
	_scs_config.net_reporting= 0;
	_scs_config.mpi_long_proto= 128 * 1024;
	_scs_config.sim_sleep= 0;
	read_config(config_file, &_scs_config);
    }


    /* Distribute the data */
    _scs_verbose= 0;
    rc= _scs_MPI_Bcast(&_scs_config, sizeof(_scs_config), MPI_BYTE, 0, _scs_comm_world);
    if (rc != MPI_SUCCESS)   {
	Err("Broadcast of config file data failed!\n", "");
	_scs_abort();
    }
    _scs_verbose= _scs_config.verbose;

    if (_scs_config.abort == TRUE)   {
	_scs_abort();
    }


    /* Open the various output files */
    _scs_open_out(_scs_config.dbg_out, KEY_DEBUG_FILE, &_scs_dbg_out);
    _scs_open_out(_scs_config.vtime_out, KEY_VTIME_FILE, &_scs_vtime_t_out);


    /* 
    ** Only the net sim writes to these.
    ** _scs_net_sim_node is not set yet. Therefore the (_scs_nnodes - 1)
    */
    if (_scs_rank == (_scs_nnodes - 1))   {
	_scs_open_out(_scs_config.mdd_out, KEY_MDD_FILE, &_scs_mdd_out);
	_scs_open_out(_scs_config.ddd_out, KEY_DDD_FILE, &_scs_ddd_out);
	_scs_open_out(_scs_config.col_out, KEY_COL_FILE, &_scs_col_out);
	_scs_open_out(_scs_config.p2p_out, KEY_P2P_FILE, &_scs_p2p_out);
	_scs_open_out(_scs_config.msize_out, KEY_MSIZE_FILE, &_scs_msize_out);
	_scs_open_out(_scs_config.trace_out, KEY_TRACE_FILE, &_scs_trace_out);
    }

    _scs_print_config();

}  /* end of _scs_config_file() */



/*
** Read the configuration file.
*/
static void
read_config(char *fname, _scs_config_t *cfg)
{

char *buf;
int fd, rc;
struct stat stat_buf;


    /*
    ** For now we let each node just read the file themselves.
    ** Later we can use a broadcast
    */
    fd= open(fname, O_RDONLY);
    if (fd == -1)   {
	Err("Cannot open configuration file %s: %s\n", fname, strerror(errno));
	/* Just use all the defaults */
	return;
    }

    rc= fstat(fd, &stat_buf);
    if (rc != 0)   {
	Err("Cannot stat configuration file %s: %s\n", fname, strerror(errno));
	cfg->abort= TRUE;
	return;
    }

    buf= (char *)malloc(stat_buf.st_size);
    if (buf == NULL)   {
	Err("Out of memory\n", "");
	cfg->abort= TRUE;
	return;
    }

    rc= read(fd, buf, stat_buf.st_size);
    if (rc != stat_buf.st_size)   {
	Err("Only read %d bytes out of %d from %s: %s\n", rc, (int)stat_buf.st_size, fname, strerror(errno));
	close(fd);
	free(buf);
	cfg->abort= TRUE;
	return;
    }

    Debug(DBG_CONFIG, "Read configuration file \"%s\" (%d bytes)\n", fname, rc);



    /* Get it parsed */
    rc= _scs_parse_config(buf, cfg, stat_buf.st_size);
    free(buf);
    close(fd);
    
    if (rc != 0)   {
	Err("Error parsing configuration file \"%s\"\n", fname);
	cfg->abort= TRUE;
    }

    return;

}  /* end of scs_read_config() */



static void
_scs_print_verbose(void)
{

int comma= 0;
char str[MAX_LINE];


    memset(str, 0, MAX_LINE);
    strcat(str, "    Debug options:                               ");
    if (_scs_verbose & DBG_CONFIG)   {
	strcat(str, "config");
	comma= 1;
    }

    if (_scs_verbose & DBG_REQ)   {
	if (comma)   strcat(str, ", ");
	strcat(str, "requests");
	comma= 1;
    }

    if (_scs_verbose & DBG_PROCESSMGMT)   {
	if (comma)   strcat(str, ", ");
	strcat(str, "process");
	comma= 1;
    }

    if (_scs_verbose & DBG_MPI)   {
	if (comma)   strcat(str, ", ");
	strcat(str, "MPI");
	comma= 1;
    }

    if (_scs_verbose & DBG_MPI2)   {
	if (comma)   strcat(str, ", ");
	strcat(str, "MPI 2");
	comma= 1;
    }

    if (_scs_verbose & DBG_MPI3)   {
	if (comma)   strcat(str, ", ");
	strcat(str, "MPI 3");
	comma= 1;
    }

    if (_scs_verbose & DBG_MPI_INTERNAL)   {
	if (comma)   strcat(str, ", ");
	strcat(str, "MPI internal");
	comma= 1;
    }

    if (_scs_verbose & DBG_GENERAL)   {
	if (comma)   strcat(str, ", ");
	strcat(str, "general");
	comma= 1;
    }

    if (_scs_verbose & DBG_EVENT)   {
	if (comma)   strcat(str, ", ");
	strcat(str, "event");
	comma= 1;
    }

    if (_scs_verbose & DBG_EMU)   {
	if (comma)   strcat(str, ", ");
	strcat(str, "emulation");
	comma= 1;
    }

    if (_scs_verbose & DBG_NET)   {
	if (comma)   strcat(str, ", ");
	strcat(str, "network");
	comma= 1;
    }

    if (_scs_verbose & DBG_TIME)   {
	if (comma)   strcat(str, ", ");
	strcat(str, "virtual time");
	comma= 1;
    }
    Debug(DBG_CONFIG, "%s\n", str);

}  /* end of _scs_print_verbose() */



static void
_scs_print_config(void)
{
    /* Let the net sim node do it */
    if (_scs_rank == 0)   {
	Debug(DBG_CONFIG, "Default values and input from config file:\n", "");
	_scs_print_verbose();
	Debug(DBG_CONFIG, "    Network bandwidth factor                     %9.6g\n",
	    _scs_config.net_bw_factor);
	Debug(DBG_CONFIG, "    Network latency factor                       %9.6g\n",
	    _scs_config.net_lat_factor);
	Debug(DBG_CONFIG, "    Network collective factor                    %9.6g\n",
	    _scs_config.net_coll_factor);
	Debug(DBG_CONFIG, "    Network cpu factor                           %9.6g\n",
	    _scs_config.net_cpu_factor);
	Debug(DBG_CONFIG, "    MPI cross-over point to long protocol       %d\n",
	    _scs_config.mpi_long_proto);
	if (_scs_config.net_reporting)   {
	    Debug(DBG_CONFIG, "    Network reporting is:                        on\n", "");
	} else   {
	    Debug(DBG_CONFIG, "    Network reporting is:                        off\n", "");
	}
	Debug(DBG_CONFIG, "    Simulator delay time:                       %12.9f s\n",
	    (float)_scs_config.sim_sleep / 1000000000.0);
	Debug(DBG_CONFIG, "    Debug output file:                           \"%s\"\n", _scs_config.dbg_out);
	Debug(DBG_CONFIG, "    Virtual time output file:                    \"%s\"\n", _scs_config.vtime_out);
	Debug(DBG_CONFIG, "    Message density distribution output file:    \"%s\"\n", _scs_config.mdd_out);
	Debug(DBG_CONFIG, "    Data density distribution output file:       \"%s\"\n", _scs_config.ddd_out);
	Debug(DBG_CONFIG, "    Collective usage output file:                \"%s\"\n", _scs_config.col_out);
	Debug(DBG_CONFIG, "    Point-to-point output file:                  \"%s\"\n", _scs_config.p2p_out);
	Debug(DBG_CONFIG, "    Message size output file:                    \"%s\"\n", _scs_config.msize_out);
	Debug(DBG_CONFIG, "    Trace file:                                  \"%s\"\n", _scs_config.trace_out);
    }

}  /* end of _scs_print_config() */
