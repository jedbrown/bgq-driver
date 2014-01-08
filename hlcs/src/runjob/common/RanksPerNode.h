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
#ifndef RUNJOB_RANKS_PER_NODE_H
#define RUNJOB_RANKS_PER_NODE_H

#include <boost/serialization/access.hpp>

#include <stdexcept>

namespace runjob {

/*!
 * \brief The number of (MPI) ranks per compute node.
 * \ingroup argument_parsing
 */
class RanksPerNode
{
public:
    /*!
     * \brief ctor.
     *
     * \throws std::logic_error if ranks is not 1, 2, 4, 8, 16, 32, or 64
     */
    RanksPerNode(
            unsigned ranks = 1   //!< [in]
            );

    /*!
     * \brief Get the value.
     */
    unsigned getValue() const { return _value; }

    /*!
     * \brief Conversion to unsigned.
     */
    operator unsigned() const { return _value; }

private:
    friend class boost::serialization::access;
    template<class Archive>
    void __attribute__ ((visibility("hidden"))) serialize(
            Archive &ar,                //!< [in] archive
            const unsigned int          //!< [in] version number
            )
    {
        ar & _value;
    }

private:
    unsigned _value;
};

/*!
 * \brief extraction operator.
 * \ingroup argument_parsing
 */
std::ostream&
operator<<(
        std::ostream& stream,       //!< [in]
        const RanksPerNode& ranks   //!< [in]
        );

/*!
 * \brief insertion operator.
 * \ingroup argument_parsing
 */
std::istream&
operator>>(
        std::istream& stream,       //!< [in]
        RanksPerNode& ranks         //!< [in]
        );

} // runjob

#endif
