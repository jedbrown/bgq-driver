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

#include "timed_events.h"

#include <time.h>
#include <unistd.h>

#include <iostream>
#include <sstream>

using namespace std;

//-------------------------------------------------------------------------

class sec_timed_event_t : public timed_event_t
{
    public:

        sec_timed_event_t( int seconds );

        const string& getName() const  { return _name; }

        void fire();


    private:

        static struct timeval _createTimeval( int seconds );
        static string _createName( int seconds );

        const string _name;
};


struct timeval sec_timed_event_t::_createTimeval( int seconds )
{
    struct timeval ret;
    ret.tv_sec = seconds;
    ret.tv_usec = 0;
    return ret;
} // sec5_timed_event_t::_createTimeval()


string sec_timed_event_t::_createName( int seconds )
{
    ostringstream oss;
    oss << "sec" << seconds;
    return oss.str();
} // sec_timed_event_t::_createName()


sec_timed_event_t::sec_timed_event_t( int seconds )
    :  timed_event_t( _createTimeval( seconds ) ),
       _name(_createName( seconds ))
{
    // Nothing to do.
} // sec_timed_event_t::sec_timed_event_t()


void sec_timed_event_t::fire()
{
    cout << getName() << " fired!" << endl;
} // sec_timed_event_t::fire()


//-------------------------------------------------------------------------

int main( int argc, char* argv[] )
{
    cout << "Testing timed events..." << endl;

    timed_events_t timed_events1;

    cout << "timed_events1 millis to next event (expect -1): " << timed_events1.getMillisToNextEvent() << endl;

    timed_event_t::ptr_t sec5_timed_event_ptr(new sec_timed_event_t(5));
    timed_event_t::ptr_t sec10_timed_event_ptr(new sec_timed_event_t(10));

    cout << "sec5 should NOT be < sec10! (expect false): " << (*sec5_timed_event_ptr < *sec10_timed_event_ptr) << endl;
    cout << "sec10 should be < sec5! (expect true): " << (*sec10_timed_event_ptr < *sec5_timed_event_ptr) << endl;

    cout << "time to sec5 (should be ~5000): " << sec5_timed_event_ptr->calcTimeMillis() << endl;
    cout << "time to sec10 (should be ~10000): " << sec10_timed_event_ptr->calcTimeMillis() << endl;

    timed_events1.add( sec10_timed_event_ptr );
    cout << "timed_events1 millis to next event (expect ~10000): " << timed_events1.getMillisToNextEvent() << endl;

    timed_events1.add( sec5_timed_event_ptr );
    cout << "timed_events1 millis to next event (expect ~5000): " << timed_events1.getMillisToNextEvent() << endl;

    cout << "Firing current events (expect no firings)" << endl;
    timed_events1.fireCurrentEvents();
    cout << endl;

    timed_events_t timed_events2;

    timed_events2.add( sec5_timed_event_ptr );
    cout << "timed_events2 millis to next event (expect ~5000): " << timed_events2.getMillisToNextEvent() << endl;

    timed_events2.add( sec10_timed_event_ptr );
    cout << "timed_events1 millis to next event (expect ~5000): " << timed_events2.getMillisToNextEvent() << endl;

    struct timeval cur_time;
    gettimeofday( &cur_time, NULL );
    cout << "Is time 5 now? (expect false) " << sec5_timed_event_ptr->isTime( cur_time ) << endl;
    cout << "Is time 10 now? (expect false) " << sec10_timed_event_ptr->isTime( cur_time ) << endl;

    sleep( 6 );

    cout << "time to sec5 (should be 0): " << sec5_timed_event_ptr->calcTimeMillis() << endl;
    cout << "time to sec10 (should be ~4000): " << sec10_timed_event_ptr->calcTimeMillis() << endl;

    gettimeofday( &cur_time, NULL );
    cout << "Is time 5 now? (expect true) " << sec5_timed_event_ptr->isTime( cur_time ) << endl;
    cout << "Is time 10 now? (expect false) " << sec10_timed_event_ptr->isTime( cur_time ) << endl;


    cout << "timed_events1 millis to next event (expect 0): " << timed_events1.getMillisToNextEvent() << endl;

    cout << "Firing current events (expect sec10 fired)" << endl;
    timed_events1.fireCurrentEvents();
    cout << endl;

    cout << "timed_events1 millis to next event (expect ~4000): " << timed_events1.getMillisToNextEvent() << endl;

    cout << "Firing current events (expect no fires)" << endl;
    timed_events1.fireCurrentEvents();
    cout << endl;

    cout << "timed_events2 millis to next event (expect 0): " << timed_events2.getMillisToNextEvent() << endl;


    sleep( 6 );

    cout << "time to sec5 (should be 0): " << sec5_timed_event_ptr->calcTimeMillis() << endl;
    cout << "time to sec10 (should be 0): " << sec10_timed_event_ptr->calcTimeMillis() << endl;

    gettimeofday( &cur_time, NULL );
    cout << "Is time 5 now? (expect true) " << sec5_timed_event_ptr->isTime( cur_time ) << endl;
    cout << "Is time 10 now? (expect true) " << sec10_timed_event_ptr->isTime( cur_time ) << endl;


    cout << "timed_events1 millis to next event (expect 0): " << timed_events1.getMillisToNextEvent() << endl;

    cout << "Firing current events (expect sec5 fired)" << endl;
    timed_events1.fireCurrentEvents();
    cout << endl;

    cout << "timed_events1 millis to next event (expect -1): " << timed_events1.getMillisToNextEvent() << endl;

    cout << "Firing current events (expect no fires)" << endl;
    timed_events1.fireCurrentEvents();
    cout << endl;

    cout << "Firing current events for 2 (expect sec5 and sec10 fired)" << endl;
    timed_events2.fireCurrentEvents();
    cout << endl;

    cout << "Firing current events (expect no fires)" << endl;
    timed_events2.fireCurrentEvents();
    cout << endl;

    cout << "Done!" << endl;
} // main()
