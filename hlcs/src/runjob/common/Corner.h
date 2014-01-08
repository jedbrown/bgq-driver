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
#ifndef RUNJOB_CORNER_H
#define RUNJOB_CORNER_H

#include "common/Coordinates.h"
#include "common/Uci.h"

#include <boost/serialization/access.hpp>

#include <bgq_util/include/Location.h>

#include <iostream>
#include <string>

#include <stdint.h>

namespace runjob {

/*!
 * \brief sub-block corner location.
 * \ingroup argument_parsing
 *
 * \see runjob::SubBlock
 * \see runjob::Shape
 */
class Corner
{
public:
    /*!
     * \brief ctor.
     */
    Corner(
            const std::string& value = std::string()
          );

    /*!
     * \brief Test if Corner is valid.
     */
    bool isValid() const;

    /*!
     * \brief Test if the core value was set.
     */
    bool isCoreSet() const;

    /*!
     * \brief
     */
    void setLocation(
            const std::string& location //!< [in]
            );

    /*!
     * \brief
     */
    void setLocation(
            const Uci& location    //!< [in]
            );

    /*!
     * \brief
     */
    void setCore(
            uint8_t core
            );

    /*!
     * \brief Set Coordinates within the block.
     */
    void setBlockCoordinates(
            const Coordinates& coordinates  //!< [in]
            );

    /*!
     * \brief Set Coordinates within the midplane.
     */
    void setMidplaneCoordinates(
            const Coordinates& coordinates  //!< [in]
            );

    const Coordinates& getBlockCoordinates() const { return _block_coordinates; }  //!< Get Coordinates within the block.
    const Coordinates& getMidplaneCoordinates() const { return _midplane_coordinates; }  //!< Get Coordinates within the midplane.
    std::string getMidplane() const; //!< Get midplane location.
    uint8_t getCore() const;    //!< Get core.
    uint8_t getCard() const;    //!< Get card.
    uint8_t getBoard() const;   //!< Get board.
    const Uci& getUci() const; //!< Get universal component identifier (UCI).

private:
    void __attribute__ ((visibility("hidden"))) validate();

    friend class boost::serialization::access;
    template<class Archive>
    void __attribute__ ((visibility("hidden"))) serialize(
            Archive &ar,                //!< [in] archive
            const unsigned int          //!< [in] version number
            )
    {
        ar & _location;
        ar & _block_coordinates;
        ar & _midplane_coordinates;
    }

private:
    Uci _location;
    Coordinates _block_coordinates;
    Coordinates _midplane_coordinates;
};

/*!
 * \brief insertion operator.
 * \ingroup argument_parsing
 */
std::ostream&
operator<<(
        std::ostream& stream,       //!< [in]
        const Corner& corner        //!< [in]
        );

/*!
 * \brief extraction operator.
 * \ingroup argument_parsing
 */
std::istream&
operator>>(
        std::istream& stream,       //!< [in]
        Corner& corner              //!< [in]
        );

} // runjob

#endif
