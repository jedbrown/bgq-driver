/* begin_generated_IBM_copyright_prolog                             */
/*                                                                  */
/* This is an automatically generated copyright prolog.             */
/* After initializing,  DO NOT MODIFY OR MOVE                       */
/*  --------------------------------------------------------------- */
/* Licensed Materials - Property of IBM                             */
/* Blue Gene/Q 5765-PER 5765-PRP                                    */
/*                                                                  */
/* (C) Copyright IBM Corp. 2011, 2012 All Rights Reserved           */
/* US Government Users Restricted Rights -                          */
/* Use, duplication, or disclosure restricted                       */
/* by GSA ADP Schedule Contract with IBM Corp.                      */
/*                                                                  */
/*  --------------------------------------------------------------- */
/*                                                                  */
/* end_generated_IBM_copyright_prolog                               */
/**
 * \file common/mpi/TypeDefs.h
 * \brief ???
 */

#ifndef __common_mpi_TypeDefs_h__
#define __common_mpi_TypeDefs_h__


#include "components/devices/mpi/mpimessage.h"
#include "components/devices/mpi/mpidevice.h"
#include "common/mpi/NativeInterface.h"
#include "algorithms/geometry/Geometry.h"


typedef PAMI::Device::MPIDevice               MPIDevice;


#define PAMI_NATIVEINTERFACE   MPINativeInterface
#define PAMI_GEOMETRY_CLASS    PAMI::Geometry::Common

#endif
