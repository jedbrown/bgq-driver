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

/*!
 * \file bgsched/HardwareImpl.h
 * \brief Hardware::Impl class definition.
 */

#ifndef BGSCHED_HARDWARE_IMPL_H_
#define BGSCHED_HARDWARE_IMPL_H_

#include <bgsched/Hardware.h>
#include <bgsched/types.h>

#include <bgq_util/include/Location.h>

#include <boost/shared_ptr.hpp>

#include <iosfwd>
#include <string>

namespace bgsched {

/*!
 * \brief Generic hardware class.
 */
class Hardware::Impl
{
public:
    /*!
     * \brief
     */
    Impl(
            const std::string& location     //!< [in] Hardware location
    );

    /*!
     * \brief
     */
    virtual ~Impl() = 0;

    /*!
     * \brief Get hardware state.
     *
     * \return Hardware state.
     */
    State getState( ) const;

    /*!
     * \brief Get hardware sequence id.
     *
     * \return Hardware sequence id.
     */
    SequenceId getSequenceId() const;

    /*!
     * \brief Set hardware state.
     */
    void setState(
            Hardware::State state,  //!< [in] New State
            bool fromDatabase       //!< [in] Indicator if state update originated from database change
            );
    /*!
     * \brief Returns true if hardware state represents state in database or false if hardware
     * state is virtual (overridden by setState() method).
     *
     * \return True if hardware state originated from database and false if state was overridden.
     */
    bool isLiveState() const;

    /*!
     * \brief Set sequence ID.
     */
    void setSequenceId(
            SequenceId sequenceId   //!< [in] New sequence ID
            );

    /*!
     * \brief Get hardware location string.
     *
     * \return Hardware location string.
     */
    const std::string& getLocationString () const;

    /*!
     * \brief Get hardware location in Location format.
     *
     * \return Hardware location.
     */
    const bgq::util::Location getLocation () const;

    /*!
     * \brief Convert a database state to a State value.
     *
     * \throws bgsched::InternalException with value:
     * - bgsched::InternalErrors::InconsistentDataError - if unexpected hardware state
     */
    static Hardware::State convertDatabaseState(
            const char *state   //!< [in] Database state
            );

protected:

    Hardware::State     _state;       //!< Hardware state
    bool                _isLiveState; //!< Live or virtual state indicator
    bgq::util::Location _location;    //!< Hardware location
    SequenceId          _sequenceId;  //!< Hardware sequence id

};

/*!
 * \brief ostream operator
 */
std::ostream&
operator<<(
        std::ostream& os,               //!< [in/out] Stream to write to
        const Hardware::Impl& hardware  //!< [in] Hardware
        );

/*!
 * \brief ostream operator
 */
std::ostream&
operator<<(
        std::ostream& os,      //!< [in/out] Stream to write to
        Hardware::State state  //!< [in] State
        );

} // namespace bgsched

#endif
