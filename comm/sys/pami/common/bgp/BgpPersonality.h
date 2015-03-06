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
#ifndef __common_bgp_BgpPersonality_h__
#define __common_bgp_BgpPersonality_h__
///
///  \file common/bgp/BgpPersonality.h
///  \brief Blue Gene/P Personality Object
///
///  This object currently provides the following features:
///  - Access to coordinate information
///  - Access to partition size information
///  - Access to operating modes (vnm, etc)
///  - Access to pset information
///  - Ability to dump the entire personality
///
///  Definitions:
///  - Personality is an object that returns machine specific hardware info
///

#define PERS_SIZE 1024

#include <sys/types.h>
#include <util/common.h>

#include <unistd.h>
#include <stdlib.h>

#include <spi/kernel_interface.h>
#include <common/bgp_personality.h>
#include <common/bgp_personality_inlines.h>

namespace PAMI
{
    class BgpPersonality : public _BGP_Personality_t
    {
      public:

        BgpPersonality ();

        void location (char location[], size_t size);
        void dumpPersonality ();

        ///
        /// \brief Retreives the x coordinate of the node
        /// \return X coordinate
        ///
        size_t xCoord() const { return _x; }

        ///
        /// \brief Retreives the y coordinate of the node
        /// \return Y coordinate
        ///
        size_t yCoord() const { return _y; }

        ///
        /// \brief Retreives the z coordinate of the node
        /// \return Z coordinate
        ///
        size_t zCoord() const { return _z; }

        ///
        /// \brief Retreives the t coordinate of the node
        /// \return T coordinate
        ///
        size_t tCoord() const { return _t; }

        ///
        /// \brief Retreives the x size of the partition.  Does not
        ///        consider the mapping.
        /// \return x size
        ///
        size_t xSize()  const { return _Xnodes; }

        ///
        /// \brief Retreives the y size of the partition.  Does not
        ///        consider the mapping.
        /// \return y size
        ///
        size_t ySize()  const { return _Ynodes; }

        ///
        /// \brief Retreives the z size of the partition.  Does not
        ///        consider the mapping.
        /// \return z size
        ///
        size_t zSize()  const { return _Znodes; }

        ///
        /// \brief Retreives the t size of the partition.  Does not
        ///        consider the mapping.
        /// \return t size
        ///
        size_t tSize()  const { return _Tnodes; }

        ///
        /// \brief Returns if the partition is a torus
        /// \return boolean true if the partition is a torus in the X dimension
        ///         false if the partition is a mesh in the X dimension
        ///
        bool isTorusX ()    const { return _isTorusX; }

        ///
        /// \brief Returns if the partition is a torus
        /// \return boolean true if the partition is a torus in the Y dimension
        ///         false if the partition is a mesh in the Y dimension
        ///
        bool isTorusY ()    const { return _isTorusY; }

        ///
        /// \brief Returns if the partition is a torus
        /// \return boolean true if the partition is a torus in the Z dimension
        ///         false if the partition is a mesh in the Y dimension
        ///
        bool isTorusZ ()    const { return _isTorusZ; }

        ///
        /// \brief Returns if the partition is a torus
        /// \return boolean true if the partition is a torus
        ///         false if the partition is a mesh
        ///
        bool isTorus ()    const { return _isTorus; }






        ///
        /// \brief Returns maximum number of threads possible in this
        ///        virtual node, including the main thread.
        /// \return Maximum number of threads
        ///
        size_t getMaxThreads() const { return _maxThreads; }

        ///
        /// \brief Get the size of NODE memory
        /// \return _node_ memory size in MiB
        ///
        size_t  memSize()  const { return _memSize;}

        ///
        /// \brief Gets the clock speed in MEGA-Hz
        /// \return MHz
        ///
        size_t  clockMHz()  const { return _clockMHz; }

        ///
        /// \brief Returns if the partition is in high throughput
        ///        computing mode
        /// \return boolean true if the partition in in HTC mode
        ///
        bool isHTCmode ()    const { return _isHTCmode; }

#if 0
        ///
        /// \brief Returns if the partition has GI enabled
        /// \return boolean true if the partition has GI enabled
        ///
        bool hasGI ()    const { return _hasGI; }
#endif

        ///
        /// \brief Retrives the rank in the pset
        /// \return rank
        ///
        unsigned  rankPset()  const { return _rankpset; }

        ///
        /// \brief Retrives rank of this pset (not node rank in pset)
        /// \return pset rank
        ///
        unsigned  numPset()   const { return _numpset;  }

        ///
        /// \brief Retrives the number of nodes in the pset
        /// \return size of pset
        ///
        unsigned  sizePset()  const { return _sizepset; }
#if 0
        ///
        /// \brief Retrives p2p tree adress of the IO node
        /// \return address
        ///
        unsigned  ioNodeAddr()const { return _ionodeaddr; }

        ///
        /// \brief Retrives p2p tree address of the node
        /// \return address
        ///
        unsigned  treeAddr()  const { return _treeaddr; }
#endif


      protected:

        size_t   _x;
        size_t   _y;
        size_t   _z;
        size_t   _t;
        size_t   _Xnodes;
        size_t   _Ynodes;
        size_t   _Znodes;
        size_t   _Tnodes;
        bool     _isTorusX;
        bool     _isTorusY;
        bool     _isTorusZ;
        bool     _isTorus;

        int      _maxThreads;
        int      _memSize;
        int      _clockMHz;
        int      _rankpset;
        int      _numpset;
        int      _sizepset;
        //int      _ionodeaddr;
        //int      _treeaddr;
        bool     _isHTCmode;
        //bool     _hasGI;
    };	// class BgpPersonality
};	// namespace PAMI

#endif // __pami_components_sysdep_bgp_bgppersonnality_h__
