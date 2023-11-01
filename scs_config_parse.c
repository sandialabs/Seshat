/*
** Copyright (2007) Sandia Corporation. Under the terms of Contract DE-AC04-94AL85000
** with Sandia Corporation, the U.S. Government retains certain rights in this software.
**
** Seshat Configuration File Parser
** $Id: scs_config_parse.c,v 1.28 2007/11/21 22:15:27 rolf Exp $
** Rolf Riesen (rolf@sandia.gov), June 2006, Sandia National Laboratories
**
** Parse the configuration file
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scs.h"


/*
** Some globals
*/
#define COMMENT_CHAR	'#'
#define TOKENSEPARATOR  " \t\n"



/*
** Local functions
*/
static int parse_line(char *line, int line_num, _scs_config_t *cfg);
static int process_keyword(char *keyword, char *arg1, char *arg2, int line_num, _scs_config_t *cfg);
static int set_verbose(char *arg1, char *arg2, int line_num);
static int set_file(char *arg1, char *arg2, int line_num, char *keyword, char *store);



/*
** Read the configuration file and distribute it to everybody.
** buf is a zero terminated string divided into lines by \n.
*/
int
_scs_parse_config(char *buf, _scs_config_t *cfg, size_t buf_size)
{

char line[MAX_LINE];
int pos;
char *current;
int line_num;


    /* Process each line individually */
    line_num= 1;
    pos= 0;
    current= buf;
    while (pos < buf_size)   {
	/* Find the end of the line and terminate the string there */
	while ((buf[pos] != '\n') && (pos < buf_size))   {
	    pos++;
	}
	buf[pos]= 0;
	strncpy(line, current, MAX_LINE);


	/* Parse the current line */
	if (parse_line(line, line_num, cfg) != 0)   {
	    return -1;
	}


	/* Go on to the next line */
	pos++;
	current= &(buf[pos]);
	line_num++;
    }

    return 0;

}  /* end of _scs_parse_config() */



/*
** This function processes a single line of the config file.
** It modifies the line that is passed in!
*/
static int
parse_line(char *line, int line_num, _scs_config_t *cfg)
{

char *comment;
char *next;
char *keyword;
char *arg1, *arg2;


    /* Find any # and replace them with 0; we don't look beyond that */
    comment= strchr(line, COMMENT_CHAR);
    if (comment)   {
	*comment= 0;
    }


    next= line;
    while ((keyword= strsep(&next, TOKENSEPARATOR)) != NULL)   {
	if (keyword[0] != '\0')   {
	    break;
	}
    }

    if ((keyword == NULL) || ((strcmp(keyword, "") == 0)) || (keyword[0] == '\n'))   {
	/* Nothing to do for this line */
	return 0;
    }


    while ((arg1= strsep(&next, TOKENSEPARATOR)) != NULL)   {
	if (arg1[0] != '\0')   {
	    break;
	}
    }


    while ((arg2= strsep(&next, TOKENSEPARATOR)) != NULL)   {
	if (arg2[0] != '\0')   {
	    break;
	}
    }

    return process_keyword(keyword, arg1, arg2, line_num, cfg);

}  /* end of parse_line() */



static int
process_keyword(char *keyword, char *arg1, char *arg2, int line_num, _scs_config_t *cfg)
{

#ifdef CONF_DEBUG
    Debug(DBG_CONFIG, "process_keyword() %3d: \"%s\" \"%s\" \"%s\"\n", line_num, keyword, arg1, arg2);
#endif

    if (strcmp(keyword, KEY_DEBUG) == 0)   {
	if (set_verbose(arg1, arg2, line_num) != 0)   {
	    return -1;
	}
	cfg->verbose= _scs_verbose;
    } else if (strcmp(keyword, KEY_REQ_TRACING) == 0)   {
	if (set_verbose(arg1, arg2, line_num) != 0)   {
	    return -1;
	}
	cfg->verbose= _scs_verbose;
    } else if (strcmp(keyword, KEY_GENERAL) == 0)   {
	if (set_verbose(arg1, arg2, line_num) != 0)   {
	    return -1;
	}
	cfg->verbose= _scs_verbose;
    } else if (strcmp(keyword, KEY_DEBUG_FILE) == 0)   {
	if (set_file(arg1, arg2, line_num, KEY_DEBUG_FILE, _scs_config.dbg_out) != 0)   {
	    return -1;
	}
    } else if (strcmp(keyword, KEY_VTIME_FILE) == 0)   {
	if (set_file(arg1, arg2, line_num, KEY_VTIME_FILE, _scs_config.vtime_out) != 0)   {
	    return -1;
	}
    } else if (strcmp(keyword, KEY_MDD_FILE) == 0)   {
	if (set_file(arg1, arg2, line_num, KEY_MDD_FILE, _scs_config.mdd_out) != 0)   {
	    return -1;
	}
    } else if (strcmp(keyword, KEY_DDD_FILE) == 0)   {
	if (set_file(arg1, arg2, line_num, KEY_DDD_FILE, _scs_config.ddd_out) != 0)   {
	    return -1;
	}
    } else if (strcmp(keyword, KEY_COL_FILE) == 0)   {
	if (set_file(arg1, arg2, line_num, KEY_COL_FILE, _scs_config.col_out) != 0)   {
	    return -1;
	}
    } else if (strcmp(keyword, KEY_P2P_FILE) == 0)   {
	if (set_file(arg1, arg2, line_num, KEY_P2P_FILE, _scs_config.p2p_out) != 0)   {
	    return -1;
	}
    } else if (strcmp(keyword, KEY_MSIZE_FILE) == 0)   {
	if (set_file(arg1, arg2, line_num, KEY_MSIZE_FILE, _scs_config.msize_out) != 0)   {
	    return -1;
	}

    } else if (strcmp(keyword, KEY_NET_BW_FACTOR) == 0)   {
	if ((arg2 != NULL) && (arg2[0] != '\0'))   {
	    Err("\"%s %s\" on line %d does not take 2nd argument \"%s\"\n", keyword, arg1, line_num, arg2);
	    return -1;
	}
	_scs_config.net_bw_factor= strtod(arg1, NULL);

    } else if (strcmp(keyword, KEY_NET_LAT_FACTOR) == 0)   {
	if ((arg2 != NULL) && (arg2[0] != '\0'))   {
	    Err("\"%s %s\" on line %d does not take 2nd argument \"%s\"\n", keyword, arg1, line_num, arg2);
	    return -1;
	}
	_scs_config.net_lat_factor= strtod(arg1, NULL);

    } else if (strcmp(keyword, KEY_NET_COLL_FACTOR) == 0)   {
	if ((arg2 != NULL) && (arg2[0] != '\0'))   {
	    Err("\"%s %s\" on line %d does not take 2nd argument \"%s\"\n", keyword, arg1, line_num, arg2);
	    return -1;
	}
	_scs_config.net_coll_factor= strtod(arg1, NULL);

    } else if (strcmp(keyword, KEY_NET_CPU_FACTOR) == 0)   {
	if ((arg2 != NULL) && (arg2[0] != '\0'))   {
	    Err("\"%s %s\" on line %d does not take 2nd argument \"%s\"\n", keyword, arg1, line_num, arg2);
	    return -1;
	}
	_scs_config.net_cpu_factor= strtod(arg1, NULL);

    } else if (strcmp(keyword, KEY_MPI_LONG_PROTO) == 0)   {
	if ((arg2 != NULL) && (arg2[0] != '\0'))   {
	    Err("\"%s %s\" on line %d does not take 2nd argument \"%s\"\n", keyword, arg1, line_num, arg2);
	    return -1;
	}
	_scs_config.mpi_long_proto= strtol(arg1, NULL, 10);

    } else if (strcmp(keyword, KEY_NET_REPORTING) == 0)   {
	if ((arg2 != NULL) && (arg2[0] != '\0'))   {
	    Err("\"%s %s\" on line %d does not take 2nd argument \"%s\"\n", keyword, arg1, line_num, arg2);
	    return -1;
	}

	if ((strcmp(arg1, "yes") == 0) || (strcmp(arg1, "on") == 0) || (strcmp(arg1, "1") == 0))   {
	    _scs_config.net_reporting= TRUE;
	} else if ((strcmp(arg1, "no") == 0) || (strcmp(arg1, "off") == 0) || (strcmp(arg1, "0") == 0))   {
	    _scs_config.net_reporting= FALSE;
	} else   {
	    Err("\"%s\" on line %d: invalid argument \"%s\". Must be yes, no, on, off, 1, 0\n",
		keyword, line_num, arg1);
	    return -1;
	}

    } else if (strcmp(keyword, KEY_SIM_SLEEP) == 0)   {
	if ((arg2 != NULL) && (arg2[0] != '\0'))   {
	    Err("\"%s %s\" on line %d does not take 2nd argument \"%s\"\n", keyword, arg1, line_num, arg2);
	    return -1;
	}
	_scs_config.sim_sleep= strtol(arg1, NULL, 10);

    } else if (strcmp(keyword, KEY_TRACE_FILE) == 0)   {
	if (set_file(arg1, arg2, line_num, KEY_TRACE_FILE, _scs_config.trace_out) != 0)   {
	    return -1;
	}

    } else if (strcmp(keyword, KEY_MSG_DATA_FILE) == 0)   {
	/*
	** We don't create the message data file just yet (unlike the trace
	** file for example). Each node creates and writes its own msg data
	** file and creates it a little further down the line.
	*/
	if ((arg2 != NULL) && (arg2[0] != '\0'))   {
	    Err("\"%s %s\" on line %d does not take 2nd argument \"%s\"\n", keyword, arg1, line_num, arg2);
	    return -1;
	}
	strncpy(_scs_config.msg_data_out, arg1, MAX_LINE);

    } else   {
	Err("Unknown keyword \"%s\" on line %d. Args \"%s\", \"%s\"\n", keyword, line_num, arg1, arg2);
	return -1;
    }

    return 0;

}  /* end of process_keyword() */



/* *****************************************************************************
 * Helper functions to process arguments for some keywords
**/
static int
set_verbose(char *arg1, char *arg2, int line_num)
{
    /* None of the options below take a second argument */
    if ((arg2 != NULL) && (arg2[0] != '\0'))   {
	Err("\"debug %s\" on line %d does not take 2nd argument \"%s\"\n", arg1, line_num, arg2);
	return -1;
    }

    if (strcmp(arg1, "config") == 0)   {
	_scs_verbose |= DBG_CONFIG;
    } else if (strcmp(arg1, "process") == 0)   {
	_scs_verbose |= DBG_PROCESSMGMT;
    } else if (strcmp(arg1, "mpi") == 0)   {
	_scs_verbose |= DBG_MPI;
    } else if (strcmp(arg1, "mpi2") == 0)   {
	_scs_verbose |= DBG_MPI2;
    } else if (strcmp(arg1, "mpi3") == 0)   {
	_scs_verbose |= DBG_MPI3;
    } else if (strcmp(arg1, "mpiint") == 0)   {
	_scs_verbose |= DBG_MPI_INTERNAL;
    } else if (strcmp(arg1, "general") == 0)   {
	_scs_verbose |= DBG_GENERAL;
    } else if (strcmp(arg1, "event") == 0)   {
	_scs_verbose |= DBG_EVENT;
    } else if (strcmp(arg1, "emu") == 0)   {
	_scs_verbose |= DBG_EMU;
    } else if (strcmp(arg1, "net") == 0)   {
	_scs_verbose |= DBG_NET;
    } else if (strcmp(arg1, "req") == 0)   {
	_scs_verbose |= DBG_REQ;
    } else if (strcmp(arg1, "time") == 0)   {
	_scs_verbose |= DBG_TIME;
    } else if (strcmp(arg1, "all") == 0)   {
	_scs_verbose |= DBG_ALL;
    } else   {
	Err("Unknown debug argument \"%s\" on line %d\n", arg1, line_num);
	return -1;
    }

    return 0;

}  /* end of set_verbose() */



static int
set_file(char *arg1, char *arg2, int line_num, char *keyword, char *store)
{


    if ((arg2 != NULL) && (arg2[0] != '\0'))   {
	Err("\"%s %s\" on line %d does not take 2nd argument \"%s\"\n", keyword, arg1, line_num, arg2);
	return -1;
    }

    if (_scs_create_out(arg1, keyword) != 0)   {
	return -1;
    }

    strncpy(store, arg1, MAX_LINE);

    return 0;

}  /* end of set_file() */
