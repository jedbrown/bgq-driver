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


#include <assert.h>
#include "spi/include/ptools_ptr/ctimer.h"
#include "hwi/include/bqc/A2_inlines.h"

static double 	PTR_wall_run_val;
static double 	PTR_usr_run_val;
static double 	PTR_sys_run_val;

/******************************************************************************
 *  PTR_Init_wall_timer()
 *
 *****************************************************************************/
void
PTR_Init_wall_timer(double *PTR_runtime_wall_ticksize,
		    double init_level,
		    int *PTR_wall_stat)
{
    if (init_level == 0) 
    {
        /* no runtime calibration needed */
	
        /* if a value for the tick period is supplied,
           use it, otherwise use PTR_WALL_NOM_TICK as
           the default value. */
	PTR_wall_run_val = (*PTR_runtime_wall_ticksize != 0) ?
	    *PTR_runtime_wall_ticksize : PTR_WALL_NOM_TICK;
    }
    else 
    {
        /* may need to do runtime calibration. */
	
        /* use default value for the time being.*/
	PTR_wall_run_val = PTR_WALL_NOM_TICK;
    }
}


/******************************************************************************
 *  PTR_Init_usr_timer()
 *
 *****************************************************************************/
void
PTR_Init_usr_timer(double *PTR_runtime_usr_ticksize,
		   double init_level,
		   int *PTR_usr_stat)
{
    PTR_usr_timer_val	time_init;
    int			status;
    
    if (init_level == 0) {
                                /* no runtime calibration needed. */
	
	                        /* if a value for the tick period is supplied,
				   use it, otherwise use PTR_USR_NOM_TICK as
				   the default value. */
	PTR_usr_run_val = (*PTR_runtime_usr_ticksize != 0) ?
	    *PTR_runtime_usr_ticksize : PTR_USR_NOM_TICK;
    }
    else {
                                /* may need to do runtime calibration. */
	
	                        /* use the value set at system configuration
				   time. */
	PTR_usr_run_val = 1 / (double)sysconf(_SC_CLK_TCK);
    }
    
    /* call the timer once to eliminate potential
       initialization overhead. */
    status = times(&time_init);

}


/******************************************************************************
 *  PTR_Init_sys_timer()
 *
 *****************************************************************************/
void
PTR_Init_sys_timer(double *PTR_runtime_sys_ticksize,
		   double init_level,
		   int *PTR_sys_stat)
{
    PTR_sys_timer_val	time_init;
    int			status;
    
    if (init_level == 0) {
                                /* no runtime calibration needed. */
	
	                        /* if a value for the tick period is supplied,
				   use it, otherwise use PTR_SYS_NOM_TICK as
				   the default value. */
	PTR_sys_run_val = (*PTR_runtime_sys_ticksize != 0) ?
	    *PTR_runtime_sys_ticksize : PTR_SYS_NOM_TICK;
    }
    else {
                                /* may need to do runtime calibration. */
	
	                        /* use the value set at system configuration
				   time. */
	PTR_sys_run_val = 1 / (double)sysconf(_SC_CLK_TCK);
    }

                                /* call the timer once to eliminate potential
				   initialization overhead. */
    status = times(&time_init);
}


/******************************************************************************
 *  PTR_Get_wall_timer()
 *
 *****************************************************************************/
void
PTR_Get_wall_time(PTR_wall_timer_val *wall_var)
{
    *wall_var = GetTimeBase();
}


/******************************************************************************
 *  PTR_Get_usr_timer()
 *
 *****************************************************************************/
void
PTR_Get_usr_time(PTR_usr_timer_val *usr_var)
{
    times(usr_var);
}


/******************************************************************************
 *  PTR_Get_sys_timer()
 *
 *****************************************************************************/
void
PTR_Get_sys_time(PTR_sys_timer_val *sys_var)
{    
    times(sys_var);
}


/******************************************************************************
 *  PTR_Get_wall_interval()
 *
 *****************************************************************************/
void
PTR_Get_wall_interval(PTR_wall_timer_val *wall_start,
		      PTR_wall_timer_val *wall_end,
		      double *wall_secs,
		      int *PTR_wall_stat)
{
    *wall_secs = (*wall_end - *wall_start) / 1600000000.0;
}


/******************************************************************************
 *  PTR_Get_usr_interval()
 *
 *****************************************************************************/
void
PTR_Get_usr_interval(PTR_usr_timer_val *usr_start,
		     PTR_usr_timer_val *usr_end,
		     double *usr_secs,
		     int *PTR_usr_stat)
{
    int	interval_ticks;

                                /* user time + user time (children). */
    interval_ticks = (usr_end->tms_utime + usr_end->tms_cutime) -
	(usr_start->tms_utime + usr_start->tms_cutime);

                                /* if result becomes negative value, there is
				   a rollover problem, sound alarm. */
    if (interval_ticks < 0) {
	*PTR_usr_stat = PTR_ROLLOVER_DETECTED;
    }

                                /* convert time into (double)seconds format. */
    *usr_secs = interval_ticks * PTR_usr_run_val;
}


/******************************************************************************
 *  PTR_Get_sys_interval()
 *
 *****************************************************************************/
void
PTR_Get_sys_interval(PTR_sys_timer_val *sys_start,
		     PTR_sys_timer_val *sys_end,
		     double *sys_secs,
		     int *PTR_sys_stat)
{
    int	interval_ticks;

                                /* system time + system time (children). */
    interval_ticks = (sys_end->tms_stime + sys_end->tms_cstime) -
	(sys_start->tms_stime + sys_start->tms_cstime);

                                /* if result becomes negative value, there is
				   a rollover problem, sound alarm. */
    if (interval_ticks < 0) {
	*PTR_sys_stat = PTR_ROLLOVER_DETECTED;
    }

                                /* convert time into (double)seconds format. */
    *sys_secs = interval_ticks * PTR_sys_run_val;
}
