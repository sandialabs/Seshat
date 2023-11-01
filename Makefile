#
# Copyright (2007) Sandia Corporation. Under the terms of Contract DE-AC04-94AL85000
# with Sandia Corporation, the U.S. Government retains certain rights in this software.
#
# A Makefile to build Seshat
# $Id: Makefile,v 1.26 2007/11/09 23:48:16 rolf Exp $
# Rolf Riesen (rolf@sandia.gov), June 2006, Sandia National Laboratories
#

.PHONY.:        all clean realclean sim

FILES = scs_config_parse \
	scs_config_read \
	scs_debug \
	scs_events \
	scs_eventQ \
	scs_main \
	scs_mpi_internal \
	scs_mpi_wrappers \
	scs_mpi_wrappers_support \
	scs_mpi_wrappersC \
	scs_mpi_wrappersF \
	scs_msg_data_file \
	scs_net_main \
	scs_net_stats \
	scs_sims \
	scs_trace_file \
	scs_vtime

ALLFILES = $(FILES)


#
# Choose the system we are simulating
#
#SIMSYSTEM = Liberty
SIMSYSTEM = RedStorm

ifeq ($(SIMSYSTEM), Liberty)
	FILES += scs_network_liberty
endif
ALLFILES += scs_network_liberty

ifeq ($(SIMSYSTEM), RedStorm)
	FILES += scs_network_redstorm
endif
ALLFILES += scs_network_redstorm



#
# Build Everything
#
all:	libscs.a

libscs.a:	$(addsuffix .o, $(FILES))
	ar rs $@ $^


#
# The simulator files itself must be compiled with gcc (or a compliant C99
# compiler) because the code uses variable number argument macros, which most
# other compilers don't understand.
MPIHOME = /usr/include/x86_64-linux-gnu/mpich
%.o:	%.c
	gcc -O3 -g -c -Wall $(DEFS) -I$(MPIHOME) -I. $(INCS) $< -o $@


#
## Dependencies
#
$(addsuffix .o, $(FILES)):				scs.h
scs_mpi_wrappersF.o:					scs_mpi_wrappers.h
scs_mpi_wrappersC.o:					scs_mpi_wrappers.h
scs_mpi_wrappers.o:					scs_mpi_wrappers.h


#
## Build tags
#
tags:	$(addsuffix .c, $(FILES))
	find . -type d -exec dirtags {} \;
	# ctags --file-scope=no -R
	ctags -R


#
## Clean up
#
clean:
	@rm -f $(addsuffix .o, $(ALLFILES))

realclean:	clean
	@find . -name tags -exec rm -f {} \;
	@rm -f libscs.a
