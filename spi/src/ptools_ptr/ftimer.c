/* begin_generated_IBM_copyright_prolog                             */
/*                                                                  */
/* This is an automatically generated copyright prolog.             */
/* After initializing,  DO NOT MODIFY OR MOVE                       */
/* ================================================================ */
/*                                                                  */
/* Licensed Materials - Property of IBM                             */
/*                                                                  */
/* Blue Gene/Q                                                      */
/*                                                                  */
/* (C) Copyright IBM Corp.  2011, 2012                              */
/*                                                                  */
/* US Government Users Restricted Rights -                          */
/* Use, duplication or disclosure restricted                        */
/* by GSA ADP Schedule Contract with IBM Corp.                      */
/*                                                                  */
/* This software is available to you under the                      */
/* Eclipse Public License (EPL).                                    */
/*                                                                  */
/* ================================================================ */
/*                                                                  */
/* end_generated_IBM_copyright_prolog                               */

#include "spi/include/ptools_ptr/ctimer.h"

/* Fortran callable version */

int ptr_init_wall_timer(double *PTR_runtime_wall_ticksize,
		     double *init_level,
		     int *PTR_wall_stat)
{
    PTR_Init_wall_timer(PTR_runtime_wall_ticksize, *init_level, PTR_wall_stat);
    return 0;
}

int
ptr_init_usr_timer(double *PTR_runtime_usr_ticksize,
		    double *init_level,
		    int *PTR_usr_stat)
{
    PTR_Init_usr_timer(PTR_runtime_usr_ticksize, *init_level, PTR_usr_stat);
    return 0;
}


int ptr_init_sys_timer(double *PTR_runtime_sys_ticksize,
		    double *init_level,
		    int* PTR_sys_stat)
{
    PTR_Init_sys_timer(PTR_runtime_sys_ticksize, *init_level, PTR_sys_stat);
    return 0;
}


int ptr_get_wall_time(PTR_wall_timer_val *wall_var)
{
    PTR_Get_wall_time(wall_var);
    return 0;
}


int ptr_get_usr_time(PTR_usr_timer_val *usr_var)
{
    PTR_Get_usr_time(usr_var);
    return 0;
}


int ptr_get_sys_time(PTR_sys_timer_val *sys_var)
{
    PTR_Get_sys_time(sys_var);
    return 0;
}


int ptr_get_wall_interval(PTR_wall_timer_val *wall_var1, 
		       PTR_wall_timer_val *wall_var2,
		       double *wall_secs, 
		       int *PTR_wall_stat)
{
    PTR_Get_wall_interval(wall_var1, wall_var2, wall_secs, PTR_wall_stat);
    return 0;
}


int ptr_get_usr_interval(PTR_usr_timer_val *usr_var1, 
		      PTR_usr_timer_val *usr_var2,
		      double *usr_secs,
		      int *PTR_usr_stat)
{
    PTR_Get_usr_interval(usr_var1, usr_var2, usr_secs, PTR_usr_stat);
    return 0;
}


int ptr_get_sys_interval(PTR_sys_timer_val *sys_var1, 
		      PTR_sys_timer_val *sys_var2, 
		      double *sys_secs, 
		      int *PTR_sys_stat)
{
    PTR_Get_sys_interval(sys_var1, sys_var2, sys_secs, PTR_sys_stat);
    return 0;
}


/*						  */
/* Fortran callable version for -qEXTNAME support */
/*						  */

int ptr_init_wall_timer_(double *PTR_runtime_wall_ticksize,
		     double *init_level,
		     int *PTR_wall_stat)
{
    PTR_Init_wall_timer(PTR_runtime_wall_ticksize, *init_level, PTR_wall_stat);
    return 0;
}


int ptr_init_usr_timer_(double *PTR_runtime_usr_ticksize,
		    double *init_level,
		    int *PTR_usr_stat)
{
    PTR_Init_usr_timer(PTR_runtime_usr_ticksize, *init_level, PTR_usr_stat);
    return 0;
}


int ptr_init_sys_timer_(double *PTR_runtime_sys_ticksize,
		    double *init_level,
		    int* PTR_sys_stat)
{
    PTR_Init_sys_timer(PTR_runtime_sys_ticksize, *init_level, PTR_sys_stat);
    return 0;
}


int ptr_get_wall_time_(PTR_wall_timer_val *wall_var)
{
    PTR_Get_wall_time(wall_var);
    return 0;
}


int ptr_get_usr_time_(PTR_usr_timer_val *usr_var)
{
    PTR_Get_usr_time(usr_var);
    return 0;
}


int ptr_get_sys_time_(PTR_sys_timer_val *sys_var)
{
    PTR_Get_sys_time(sys_var);
    return 0;
}


int ptr_get_wall_interval_(PTR_wall_timer_val *wall_var1, 
		       PTR_wall_timer_val *wall_var2,
		       double *wall_secs, 
		       int *PTR_wall_stat)
{
    PTR_Get_wall_interval(wall_var1, wall_var2, wall_secs, PTR_wall_stat);
    return 0;
}


int ptr_get_usr_interval_(PTR_usr_timer_val *usr_var1, 
		      PTR_usr_timer_val *usr_var2,
		      double *usr_secs,
		      int *PTR_usr_stat)
{
    PTR_Get_usr_interval(usr_var1, usr_var2, usr_secs, PTR_usr_stat);
    return 0;
}


int ptr_get_sys_interval_(PTR_sys_timer_val *sys_var1, 
		      PTR_sys_timer_val *sys_var2, 
		      double *sys_secs, 
		      int *PTR_sys_stat)
{
    PTR_Get_sys_interval(sys_var1, sys_var2, sys_secs, PTR_sys_stat);
    return 0;
}
