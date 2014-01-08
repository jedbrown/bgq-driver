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
#include "Options.h"

#include "common/logging.h"

#include <control/include/bgqconfig/xml/BGQMachineXML.h>

#include <db/include/api/BGQDBlib.h>

using namespace runjob::samples;

LOG_DECLARE_FILE( runjob::log );

Options::Options(
        unsigned int argc,
        char ** argv
        ) :
    runjob::AbstractOptions( argc, argv, runjob::log ),
    _options(),
    _block(),
    _corner(),
    _shape(),
    _machine()
{
    // create options
    _options.add_options()
        ("block", po::value(&_block), "block ID")
        ("corner", po::value(&_corner), "corner location (e.g. R32-M0-N04-J01)")
        ("shape", po::value(&_shape), "5 dimensional shape (e.g. 1x1x1x1x1)")
        ;

    // add generic args
    this->add( _options );

    // parse and notify
    this->parse( _options );
    this->notify();

    // initialize database
    BGQDB::init( this->getProperties(), 10 );

    // get machine XML
    LOG_DEBUG_MSG( "extracting machine XML description" );
    std::stringstream xml;
    BGQDB::STATUS result = BGQDB::getMachineXML(xml);
    if (result != BGQDB::OK) {
        throw std::runtime_error( "could not get machine XML" );
    }

    // create bgqconfig object
    LOG_DEBUG_MSG( "creating machine XML object" );
    _machine.reset( BGQMachineXML::create(xml) );
}

void
Options::help(
        std::ostream& os
        ) const
{
    os << _options;
}
