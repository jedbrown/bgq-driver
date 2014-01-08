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
#ifndef RUNJOB_SAMPLES_SUB_BLOCK_CORNER_FINDER_OPTIONS_H
#define RUNJOB_SAMPLES_SUB_BLOCK_CORNER_FINDER_OPTIONS_H

#include "common/AbstractOptions.h"

#include <boost/shared_ptr.hpp>

namespace po = boost::program_options;

class BGQMachineXML;

namespace runjob {
namespace samples {

class Options : public AbstractOptions
{
public:
    /*!
     * \brief ctor.
     */
    Options(
            unsigned int argc,      //!< [in]
            char** argv             //!< [in]
           );

    /*!
     * \brief
     */
    void help(
            std::ostream& os    //!< [in]
            ) const;

    const std::string& getBlock() const { return _block; } //!< Get block ID.
    const std::string& getCorner() const { return _corner; } //!< Get corner location.
    const std::string& getShape() const { return _shape; } //!< Get shape.
    boost::shared_ptr<BGQMachineXML> getMachine() const { return _machine; }

private:
    po::options_description _options;
    std::string _block;
    std::string _corner;
    std::string _shape;
    boost::shared_ptr<BGQMachineXML> _machine;      //!< machine XML description
};

} // samples
} // runjob
#endif
