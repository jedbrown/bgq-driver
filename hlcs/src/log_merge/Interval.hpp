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


#ifndef LOG_MERGE_INTERVAL_HPP_
#define LOG_MERGE_INTERVAL_HPP_


#include <boost/shared_ptr.hpp>

#include <boost/date_time/posix_time/posix_time.hpp>

#include <iosfwd>


namespace log_merge {


class Interval
{
public:

    typedef boost::shared_ptr<const Interval> ConstPtr;
    typedef boost::shared_ptr<Interval> Ptr;


    static const ConstPtr DefaultPtr;


    static Ptr create(
            const boost::posix_time::ptime& start,
            const boost::posix_time::ptime& end
        )
    { return Ptr( new Interval( start, end ) ); }


    Interval(
            const boost::posix_time::ptime& start,
            const boost::posix_time::ptime& end
        ) :
            _start(start), _end(end)
    { /* nothing to do */ }


    Interval()
        :
            _start(boost::posix_time::min_date_time),
            _end(boost::posix_time::max_date_time)
    { /* nothing to do */ }


    bool isSet() const  { return (_start != boost::posix_time::min_date_time) || (_end != boost::posix_time::max_date_time);  }

    void setStart( const boost::posix_time::ptime& start )  { _start = start; }
    void setEnd( const boost::posix_time::ptime& end )  { _end = end; }

    void validate() const;

    bool includes( const boost::posix_time::ptime& t ) const  { return ((! before( t )) && (! after( t ))); }
    bool before( const boost::posix_time::ptime& t ) const  { return (t < _start); }
    bool after( const boost::posix_time::ptime& t ) const  { return (t > _end); }


    bool operator==( const Interval& other ) const  { return ((_start == other._start) && (_end == other._end)); }
    bool operator!=( const Interval& other ) const  { return (! (*this == other)); }


private:

    friend std::ostream& operator<<( std::ostream& os, const Interval& i );


    boost::posix_time::ptime _start, _end;
};


std::ostream& operator<<( std::ostream& os, const Interval& i );


} // namespace log_merge

#endif
