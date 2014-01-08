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
#include "common/Corner.h"

#include "common/logging.h"

#include <bgq_util/include/Location.h>

#include <boost/assert.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/throw_exception.hpp>

#include <iostream>
#include <stdexcept>

LOG_DECLARE_FILE( runjob::log );

namespace runjob {

Corner::Corner(
        const std::string& value
        ) :
    _location(),
    _block_coordinates(),
    _midplane_coordinates()
{
    if ( value.empty() ) return;

    this->setLocation( value );
}

void
Corner::setLocation(
        const std::string& location
        )
{
    Uci uci( location );
    std::swap( _location, uci );

    try {
        this->validate();
    } catch ( const std::exception& e ) {
        std::swap( uci, _location );

        // rethrow with more information
        BOOST_THROW_EXCEPTION(
                std::invalid_argument( location + " is not a valid corner location" )
                );
    }
}

void
Corner::setLocation(
        const Uci& location
        )
{
    Uci uci( location );
    std::swap( _location, uci );
    try {
        this->validate();
    } catch ( const std::exception& e ) {
        LOG_WARN_MSG( "invalid UCI: " << e.what() );
        std::swap( uci, _location );
    }
}

void
Corner::setCore(
        uint8_t core
        )
{
    if ( core >= bgq::util::Location::ComputeCardCoresOnBoard - 1 ) {
        BOOST_THROW_EXCEPTION(
                std::invalid_argument(
                    "invalid core location (" +
                    boost::lexical_cast<std::string>(static_cast<unsigned>(core)) +
                    " only cores 0 through 15 are valid"
                    )
                );
    }

    BG_UniversalComponentIdentifier uci( _location.get() );
    uci |= BG_UCI_SET_COMPONENT( BG_UCI_Component_ComputeCardCoreOnNodeBoard );
    uci |= BG_UCI_SET_CORE( core );
    _location = Uci( uci );
}

void
Corner::validate()
{
    switch ( BG_UCI_GET_COMPONENT(_location.get()) ) {
        case BG_UCI_Component_ComputeCardCoreOnNodeBoard:
            // validate core location since core 16 is not valid for running jobs
            if ( this->getCore() >= bgq::util::Location::ComputeCardCoresOnBoard - 1 ) {
                BOOST_THROW_EXCEPTION(
                        std::invalid_argument(
                            "core is not valid, only cores 0 through 15 can be used"
                            )
                        );
            }
            break;
        case BG_UCI_Component_ComputeCardOnNodeBoard:
            break;
        default:
            BOOST_THROW_EXCEPTION(
                    std::invalid_argument(
                        boost::lexical_cast<std::string>( _location )
                        )
                    );
    }
}

void
Corner::setBlockCoordinates(
        const Coordinates& coordinates
        )
{
    _block_coordinates = coordinates;
    LOG_DEBUG_MSG( *this << " block coordinates " << _block_coordinates );
}

void
Corner::setMidplaneCoordinates(
        const Coordinates& coordinates
        )
{
    _midplane_coordinates = coordinates;
    LOG_DEBUG_MSG( *this << " midplane coordinates " << _midplane_coordinates );
}

bool
Corner::isValid() const
{
    // UCI set or block coordinates set means this corner is valid
    return _location.get() || _block_coordinates.valid();
}

bool
Corner::isCoreSet() const
{
    return BG_UCI_GET_COMPONENT(_location.get()) == BG_UCI_Component_ComputeCardCoreOnNodeBoard;
}

std::string
Corner::getMidplane() const
{
    BG_UniversalComponentIdentifier midplane(0);
    midplane |= BG_UCI_SET_ROW( BG_UCI_GET_ROW(_location.get()) );
    midplane |= BG_UCI_SET_COLUMN( BG_UCI_GET_COLUMN(_location.get()) );
    midplane |= BG_UCI_SET_MIDPLANE( BG_UCI_GET_MIDPLANE(_location.get()) );
    midplane |= BG_UCI_SET_COMPONENT( BG_UCI_Component_Midplane );

    return boost::lexical_cast<std::string>( Uci(midplane) );
}

uint8_t
Corner::getCore() const
{
    return BG_UCI_GET_CORE( _location.get() );
}

uint8_t
Corner::getCard() const
{
    return BG_UCI_GET_COMPUTE_CARD( _location.get() );
}

uint8_t
Corner::getBoard() const
{
    return BG_UCI_GET_NODE_BOARD( _location.get() );
}

const Uci&
Corner::getUci() const
{
    return _location;
}

std::ostream&
operator<<(
        std::ostream& stream,
        const Corner& corner
        )
{
    if ( corner.getUci().get() == 0 ) {
        // empty corner is valid
        return stream;
    }

    stream << corner.getUci();

    return stream;
}

std::istream&
operator>>(
        std::istream& stream,
        Corner& corner
        )
{
    std::string value;
    stream >> value;
    try {
        corner.setLocation( value );
    } catch ( const std::invalid_argument& e ) {
        std::cerr << e.what() << std::endl;
        stream.setstate( std::ios::failbit );
    }

    return stream;
}

} // runjob
