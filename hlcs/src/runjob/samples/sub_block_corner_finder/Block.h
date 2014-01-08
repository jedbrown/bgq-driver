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
/* (C) Copyright IBM Corp.  2010, 2011                              */
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
#ifndef RUNJOB_SAMPLES_SUB_BLOCK_CORNER_FINDER_BLOCK_H
#define RUNJOB_SAMPLES_SUB_BLOCK_CORNER_FINDER_BLOCK_H

#include <string>

namespace runjob {
namespace samples {

class Options;

class Block
{
public:
    /*!
     * \brief ctor.
     *
     * \throws std::invalid_argument if block is empty
     */
    explicit Block(
            const Options& options     //!< [in] program options
         );

private:
    const Options& _options;
    const std::string& _id;
};

} // samples
} // runjob
#endif
