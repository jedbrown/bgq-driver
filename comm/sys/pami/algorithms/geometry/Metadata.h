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
 * \file algorithms/geometry/Metadata.h
 * \brief ???
 */

#ifndef __algorithms_geometry_Metadata_h__
#define __algorithms_geometry_Metadata_h__


namespace PAMI
{
  namespace Geometry
  {
    
    // This is a simple wrapper class around the C style pami metadata struct
    // This is to guarantee that some value has been set for the metadata
    // fields for every collective
    // This sets the fields to "always works" and "not hw accelerated"
    // Any code that uses this must set the appropriate fields
    // The input name must have storage for the lifetime of the metadata object
    class Metadata: public pami_metadata_t
    {
    public:
      inline Metadata(const char *in_name)
        {
          // Misc info
          this->name                              = (char*)in_name;
          this->version                           = 1;
          this->check_fn                          = NULL;
          this->range_lo                          = 0;
          this->range_hi                          = MD_SIZE_MAX;
          this->send_min_align                    = 1;
          this->recv_min_align                    = 1;

          // Correctness Check
          this->check_correct.bitmask_correct     = 0;
          this->check_correct.values.checkrequired = 0;
          this->check_correct.values.nonlocal     = 0;
          this->check_correct.values.sendminalign = 1;
          this->check_correct.values.recvminalign = 1;
          this->check_correct.values.alldtop      = 1;
          this->check_correct.values.contigsflags = 0;
          this->check_correct.values.contigrflags = 0;
          this->check_correct.values.continsflags = 0;
          this->check_correct.values.continrflags = 0;
          this->check_correct.values.blocking     = 0;
          this->check_correct.values.inplace      = 1;
          this->check_correct.values.asyncflowctl = 0;
          this->check_correct.values.oneatatime   = 0;
          this->check_correct.values.global_order = 0;

          // Performance Checks
          this->check_perf.bitmask_perf           = 0;
          this->check_perf.values.hw_accel        = 0;
          this->range_lo_perf                     = 0;
          this->range_hi_perf                     = MD_SIZE_MAX;
        }
    private:
      static const size_t MD_SIZE_MAX = -1;

    };
  };
};

#endif
//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
