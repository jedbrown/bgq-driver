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

#ifndef BGSCHED_REALTIME_ABSTRACT_DATABASE_CHANGE_H_
#define BGSCHED_REALTIME_ABSTRACT_DATABASE_CHANGE_H_


#include "AbstractMessage.h"

#include <boost/shared_ptr.hpp>

#include <boost/serialization/access.hpp>
#include <boost/serialization/base_object.hpp>


namespace bgsched {
namespace realtime {


class AbstractDatabaseChangeVisitor;


class AbstractDatabaseChange : public AbstractMessage
{
public:
    typedef boost::shared_ptr<AbstractDatabaseChange> Ptr;

    virtual bool merge( const AbstractDatabaseChange& /*other*/ )  { return false; }
        // returns true if was able to merge.

    virtual void accept( AbstractDatabaseChangeVisitor& v ) =0;


protected:
    virtual ~AbstractDatabaseChange() { /* Nothing to do */ }

private:

    friend class boost::serialization::access;

    template<class Archive>
    void serialize( Archive & ar, const unsigned int /*version*/ )
    {
        ar & boost::serialization::base_object<AbstractMessage>(*this);
    }
};


} } // namespace bgsched::realtime


#endif
