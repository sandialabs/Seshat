#ifndef _scs_mpi_wrappersF_h_
#define _scs_mpi_wrappersF_h_

/* All MPI objects are integers in Fortran */
#define MPI_StatusF MPI_Fint
#define MPI_RequestF MPI_Fint
#define MPI_DatatypeF MPI_Fint
#define MPI_CommF MPI_Fint
#define MPI_OpF MPI_Fint
#define MPI_InfoF MPI_Fint
#define MPI_FileF MPI_Fint
#define MPI_GroupF MPI_Fint

#endif /* _scs_mpi_wrappersF_h_ */
