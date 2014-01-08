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
#ifndef _EMON_EMON_H_ // Prevent multiple inclusion.
#define _EMON_EMON_H_

#include <spi/include/kernel/envmon.h>

// Setup for power measurement
// RETURN value :
//   SUCCESS-> 0
//   FAIL   -> -1

extern int EMON_SetupPowerMeasurement2(int);

#define EMON_SetupPowerMeasurement() EMON_SetupPowerMeasurement2(0)


// Gets power measurement result
// RETURN value : 
//   SUCCESS-> power in watt (>= 0.0)
//   FAIL   -> -1

extern double EMON_GetPower(void);
extern double EMON_ReportPower(void);

//********************************************************************************
// For old version of FPGA
//********************************************************************************

// information of each domain
extern struct domain_info {
  double imon_unit;
  double vo_intbus_unit;
  double vbuf_unit;
  unsigned vbuf_shift;
  unsigned domain_id;
  unsigned k_const;
} domain_info[EMON_DCA_N_DOMAINS];

#ifdef EMON_DEFINE_GLOBALS
struct domain_info domain_info[EMON_DCA_N_DOMAINS] =  {
  { // domain 1 = 0.8V
    0.128, 
    0.004 * (52/2.1),
    0.002, 
    1,
    1,
    40
  }, { // domain 2 = 1.4V
    0.032,
    0.004 * (52/2.1),
    0.004,  
    0,
    2,
    32
  }, { // domain 3 = 2.5V
    0.016,
    0.004 * (52/2.1),
    0.004,
    1,
    3,
    16
  }, { // domain 4 = 3.3V
    0.016,
    0.004 * (52/2.1),
    0.008,
    0,
    4,
    12
  }, { // domain 6 = 1.5V
    0.016,
    0.004 * (52/2.1),
    0.004,
    0,
    6,
    32
  }, { // domain 8 = 1.0V
    0.016,
    0.004 * (52/2.1),
    0.002,
    1,
    8,
    40
  }, { // domain 7 = 0.9V
      0, // N/A
      0, // N/A
      0, // N/A
      0, // N/A
      7, // N/A
      32
  }
};
#endif

//********************************************************************************
// Gets power measurement result
// RETURN value : 
//   SUCCESS-> power in watt (>= 0.0)
//   FAIL   -> -1

__INLINE__
double EMON_GetPower_impl(double volt[], double amps[])
{
  double power = 0; // in watt
  int i, j;
  int rc;

  uint64_t raw_v_data[14]; 
  uint64_t raw_a_data[14];
  
  int domain_id;

  rc = Kernel_GetRawEnvmonPowerData(emon_mode, NULL, raw_v_data, raw_a_data);
  if (rc != 0) {
    return -1;
  }

  if (fpga_version == EMON_DCA_FV_LONG) {
      for(i = 0; i < 14; i++) { // 7 domains * 2 DCAs
	  domain_id = i >> 1;
	  int use_vointbus =  USE_VOINTBUS && (domain_id != 7);
	  double v_unit = use_vointbus ? 52.0/2.1 : 1;
	  double v = raw_v_data[i] * v_unit * 0.001;
	  double a = raw_a_data[i] * 0.001;
	  unsigned k_const = domain_info[domain_id].k_const;
	  power += use_vointbus ? v * a : v * a * k_const;
	  volt[i] = v;
	  amps[i] = a;
      }
  } else {
      for(i = 0; i < 12; i+=3) { // 6 domains * 2 DCAs,  get 3 per each envmon access
	  
	  for(j = 0; j < 3; j++) {
	      domain_id = (i+j) >> 1;
	      double v_unit = USE_VOINTBUS ? domain_info[domain_id].vo_intbus_unit : domain_info[domain_id].vbuf_unit;
	      double v = raw_v_data[i+j] * v_unit;
	      double a = raw_a_data[i+j] * domain_info[domain_id].imon_unit;
	      unsigned k_const = domain_info[domain_id].k_const;
	      power += USE_VOINTBUS ? v * a : v * a * k_const;
	      volt[i+j] = v;
	      amps[i+j] = a;
	  }
      }
      for(i = 12; i < 14; i++) {
	  volt[i] = amps[i] = 0;
      }
  }

  return power;
}

//********************************************************************************
// Gets power measurement result
// RETURN value : 
//   SUCCESS-> power in watt (>= 0.0)
//   FAIL   -> -1


__INLINE__
double EMON_GetPower(void)
{
  double v[14], a[14];
  double power = EMON_GetPower_impl(v, a);
  return power;
}


#define INT_PART(X) (((uint64_t)((X)*1000))/1000UL)
#define FRC_PART(X) (((uint64_t)((X)*1000))%1000UL)
#define BOTH_PART(X) INT_PART(X), FRC_PART(X)

// OLD version, for compatibility
__INLINE__
double EMON_ReportPower_vointbus(void)
{
  double v[14], a[14];
  double power = EMON_GetPower_impl(v, a);
  int dom, dca_id;

  if (power < 0) { return power; }

  printf("************* EMON_ReportPower (FPGA V%d) : total power = %lu.%03lu watt\n", fpga_version, INT_PART(power), FRC_PART(power));
  printf("DCA\tDomain\t(Vout)\t\tVolt\tAmps\tWatt\n");

  for(dca_id = 0; dca_id < 2; dca_id ++) {
    for(dom = 0; dom < 6; dom++) {

      double volt = v[dom*2+dca_id];
      double amps = a[dom*2+dca_id];
      double watt = volt*amps;
      double vout = volt / domain_info[dom].k_const;
      printf("%d\t%d\t%lu.%03lu\t\t%lu.%03lu\t%lu.%03lu\t%lu.%03lu\t\n", 
	     dca_id, domain_info[dom].domain_id,
	     BOTH_PART(vout), BOTH_PART(volt), BOTH_PART(amps), BOTH_PART(watt));
    }
  }
  return power;
}

__INLINE__
double EMON_ReportPower(void)
{

  double v[14], a[14];
  double power;
  int dom, dca_id;

  // for backward compatibility
  if (USE_VOINTBUS) return EMON_ReportPower_vointbus();

  power = EMON_GetPower_impl(v, a);

  if (power < 0) { return power; }

  printf("************* EMON_ReportPower (FPGA V%d) : total power = %lu.%03lu watt\n", fpga_version, INT_PART(power), FRC_PART(power));
  printf("DCA   Domain   VTM Vout(V)   VTM Iin(A)   VTM ratio   Power(W) (=Vout*Iin*ratio)\n");

  for(dca_id = 0; dca_id < 2; dca_id ++) {
    for(dom = 0; dom < 7; dom++) {

      double volt = v[dom*2+dca_id];
      double amps = a[dom*2+dca_id];
      unsigned k_const = domain_info[dom].k_const;
      double watt = volt*amps* k_const;
      printf("%d       %d       %1lu.%03lu       %4lu.%03lu         %u     %5lu.%03lu\n", 
	     dca_id, domain_info[dom].domain_id,
	     BOTH_PART(volt), BOTH_PART(amps), k_const, BOTH_PART(watt));
    }
  }
  return power;

}


//********************************************************************************
// initialization
//********************************************************************************

// initialization : check FPGA version and so on
__INLINE__
int EMON_SetupPowerMeasurement2(int mode)
{ 
  int rc;
  emon_mode = mode;

  if (fpga_version == EMON_DCA_FV_INVALID) {
      rc = Kernel_GetRawEnvmonPowerData(emon_mode, &fpga_version, NULL, NULL);
      if (rc != 0) {
	  printf("EMON_SetupPowerMeasurement2: ERROR : synchronization with FPGA failed\n");
	  return -1;
      }
  }

  // printf("EMON_SetupPowerMeasurement2: INFO: FPGA version is %d\n", fpga_version); 
  return 0;
}



#endif // _EMON_EMON_H_
