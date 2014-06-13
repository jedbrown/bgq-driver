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

#include <bgsched/Coordinates.h>
#include <bgsched/Dimension.h>
#include <bgsched/SchedUtil.h>

#include <sys/time.h>
#include <sstream>

using namespace bgsched;
using namespace std;

namespace bgsched {

string
SchedUtil::createBlockName(
        const string& prefix
        )
{
    char chartime[32];
    string myPrefix, stime, ftime, day;
    timeval tof;
    int randVal;

    // Check for empty prefix
    if (prefix.empty()) {
        // Use default prefix
        myPrefix = string("BLOCK_");
    } else {
        // Validate the prefix string
        if (prefix.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_") != string::npos) {
            // Use default prefix because invalid character found in prefix
            myPrefix = string("BLOCK_");
        } else {
            // Check if prefix exceeds 16 characters
            if (prefix.size() > 16) {
                myPrefix = prefix.substr(0, 16);
            } else {
                // Good prefix so use it
                myPrefix = prefix;
            }
        }
    }

    gettimeofday(&tof, NULL);

    ctime_r((const time_t*)(&tof.tv_sec),chartime);
    stime = chartime;
    day = stime.substr(8,2);
    if (day[0] == ' ') {
        day[0]='0';
    }
    ftime = day; //day
    ftime += stime.substr(4,3); //month
    ftime += stime.substr(11,2); //hour
    ftime += stime.substr(14,2); //min
    ftime += stime.substr(17,2); //sec
    srand(tof.tv_usec);
    randVal = rand()%10000;
    ostringstream blockName;
    blockName << myPrefix << ftime << randVal;
    return blockName.str();
}

void
SchedUtil::iterate(
    const vector<vector<uint32_t> >& values,
    void(*function)(const vector<uint32_t>&,void*),
    void* otherArgs )
{
    // Build origin
    vector<uint32_t> start;
    for( vector<vector<uint32_t> >::const_iterator originValues = values.begin(); originValues != values.end(); ++originValues )
        start.push_back( (*originValues)[0] );
    recurse( values, function, otherArgs, start, 0 );
}

void
SchedUtil::recurse(
    const vector<vector<uint32_t> >& values,
    void(*function)(const vector<uint32_t>&,void*),
    void* otherArgs,
    vector<uint32_t> coordinates,
    uint32_t dimIndex )
{
    // Iterate over the vector of values for this dimension
    for( vector<uint32_t>::const_iterator value = values[dimIndex].begin(); value != values[dimIndex].end(); ++value )
    {
        coordinates[dimIndex] = *value;
        if( dimIndex == values.size() - 1 ) // last dimension, execute the function
            (*function)( coordinates, otherArgs );

        else // more to go, recurse again
            recurse( values, function, otherArgs, coordinates, dimIndex + 1 );
    }
}

void
SchedUtil::iterateAll(
        const vector<uint32_t>& inDimOrder,
        const vector<bool>& inDimReverse,
        const vector<uint32_t>& mins,
        const vector<uint32_t>& maxes,
        void(*function)(vector<uint32_t>,void*),
        void* otherArgs
        )
{
    vector<uint32_t> dimOrder = inDimOrder;
    if( inDimOrder.size() == 0 )
    {
        for( uint32_t dimension = 0; dimension < mins.size(); ++dimension )
            dimOrder.push_back(dimension); // iterate in "natural" order
    }

    vector<bool> dimReverse = inDimReverse;
    if( inDimReverse.size() == 0 )
    {
        for( uint32_t dimension = 0; dimension < mins.size(); ++dimension )
            dimReverse.push_back(false); // iterate in "natural" order
    }

    if( dimOrder.size() != mins.size() || dimOrder.size() != maxes.size() || dimOrder.size() != dimReverse.size() )
    {
        ostringstream os;
        os << "The three input vectors must be the same size: " << dimOrder.size() << ", " << dimReverse.size() << ", " << mins.size() << ", " << maxes.size();
        throw Exception(Errors::InputVectorSizeMismatch, os.str() );
    }
    vector<uint32_t> coordinates;
    for( uint32_t dim = 0; dim < dimOrder.size(); dim++ )
        coordinates.push_back( mins[dim] );

    recurseAll( dimOrder, dimReverse, mins, maxes, function, otherArgs, coordinates, 0 );

}

void
SchedUtil::recurseAll(
        const vector<uint32_t>& dimOrder,
        const vector<bool>& dimReverse,
        const vector<uint32_t>& mins,
        const vector<uint32_t>& maxes,
        void(*function)(vector<uint32_t>,void*),
        void* otherArgs,
        vector<uint32_t> coordinates,
        uint32_t dimIndex
        )
{
    if( dimOrder[dimIndex] >= dimOrder.size() )
    {
        ostringstream os;
        os << "Dimension order element [" << dimIndex << "] = " << dimOrder[dimIndex] << " is larger than dimension size: " << dimOrder.size();
        throw Exception(Errors::VectorIndexOutOfBounds, os.str() );
    }

    uint32_t startValue = mins[dimOrder[dimIndex]];
    uint32_t endValue = maxes[dimOrder[dimIndex]];
    int increment = 1;

    if( dimReverse[dimOrder[dimIndex]] )
    {
        startValue = maxes[dimOrder[dimIndex]];
        endValue = mins[dimOrder[dimIndex]];
        increment = -1;
    }

    for( uint32_t value = startValue; value != endValue+increment; value+=increment )
    {
        coordinates[dimOrder[dimIndex]] = value;
        if( dimIndex == dimOrder.size() - 1 ) // Last dimension, now execute the function
        {
            (*function)( coordinates, otherArgs );
        }
        else // Not there yet, more dimensions to iterate over
        {
            recurseAll( dimOrder, dimReverse, mins, maxes, function, otherArgs, coordinates, dimIndex+1 );
        }
    }
}

void
SchedUtil::iterateAllMidplanes(
        const vector<uint32_t>& inDimOrder,
        const vector<bool>& inDimReverse,
        const Coordinates& origin,
        const Coordinates& extent,
        void(*function)(const Coordinates&,void*),
        void* otherArgs
        )
{
    vector<uint32_t> dimOrder = inDimOrder;
    bool defaultOrder = false;
    if( inDimOrder.size() == 0 )
        defaultOrder = true;

    vector<bool> dimReverse = inDimReverse;
    bool defaultReverse = false;
    if( inDimReverse.size() == 0 )
        defaultReverse = true;

    vector<uint32_t> mins;
    vector<uint32_t> maxes;

    for( Dimension dimension = Dimension::A; dimension <= Dimension::D; ++dimension )
    {
        if( defaultOrder )
            dimOrder.push_back(dimension); // iterate in "natural" order
        if( defaultReverse )
            dimReverse.push_back(false); // iterate in "natural" order

        mins.push_back(origin[dimension]);
        maxes.push_back(extent[dimension]);
    }

    iterateMidplanes( dimOrder, dimReverse, mins, maxes, function, otherArgs );

}

void
SchedUtil::iterateMidplanes(
        const vector<uint32_t>& inDimOrder,
        const vector<bool>& inDimReverse,
        const vector<uint32_t>& mins,
        const vector<uint32_t>& maxes,
        void(*function)(const Coordinates&,void*),
        void* otherArgs
        )
{
    vector<uint32_t> dimOrder = inDimOrder;
    if( inDimOrder.size() == 0 )
    {
        for( uint32_t dimension = 0; dimension < mins.size(); ++dimension )
            dimOrder.push_back(dimension); // iterate in "natural" order
    }
    vector<bool> dimReverse = inDimReverse;
    if( inDimReverse.size() == 0 )
    {
        for( uint32_t dimension = 0; dimension < mins.size(); ++dimension )
            dimReverse.push_back(false); // iterate in "natural" order
    }

    if( dimOrder.size() != mins.size() || dimOrder.size() != maxes.size() || dimOrder.size() != dimReverse.size() )
    {
        ostringstream os;
        os << "The three input vectors must be the same size: " << dimOrder.size() << ", " << dimReverse.size() << ", " << ", " << mins.size() << ", " << maxes.size();
        throw Exception(Errors::InputVectorSizeMismatch, os.str() );
    }
    Coordinates coordinates(0,0,0,0);
    for( uint32_t dim = 0; dim < dimOrder.size(); dim++ )
        coordinates[dim] = mins[dim];

    recurseMidplanes( dimOrder, dimReverse, mins, maxes, function, otherArgs, coordinates, 0 );
}

void
SchedUtil::recurseMidplanes(
        const vector<uint32_t>& dimOrder,
        const vector<bool>& dimReverse,
        const vector<uint32_t>& mins,
        const vector<uint32_t>& maxes,
        void(*function)(const Coordinates&,void*),
        void* otherArgs,
        Coordinates coordinates,
        uint32_t dimIndex )
{
    if( dimOrder[dimIndex] >= dimOrder.size() )
    {
        ostringstream os;
        os << "Dimension order element [" << dimIndex << "] = " << dimOrder[dimIndex] << " is larger than dimension size: " << dimOrder.size();
        throw Exception(Errors::VectorIndexOutOfBounds, os.str() );
    }

    uint32_t startValue = mins[dimOrder[dimIndex]];
    uint32_t endValue = maxes[dimOrder[dimIndex]];
    int increment = 1;

    if( dimReverse[dimOrder[dimIndex]] )
    {
        startValue = maxes[dimOrder[dimIndex]];
        endValue = mins[dimOrder[dimIndex]];
        increment = -1;
    }

    for( uint32_t value = startValue; value != endValue+increment; value+=increment )
    {
        coordinates[dimOrder[dimIndex]] = value;
        if( dimIndex == dimOrder.size() - 1 ) // Last dimension, now execute the function
        {
            Coordinates coordinatesV(0,0,0,0);
            for( Dimension dimension = Dimension::A; dimension <= Dimension::D; ++dimension )
                coordinatesV[dimension] = coordinates[dimension];

            (*function)( coordinatesV, otherArgs );
        }
        else // Not there yet, more dimensions to iterate over
        {
            recurseMidplanes( dimOrder, dimReverse, mins, maxes, function, otherArgs, coordinates, dimIndex+1 );
        }
    }
}

string
SchedUtil::Errors::toString(
        Value v,
        const string& what
        )
{
    ostringstream os;

    switch(v) {
        case InputVectorSizeMismatch: os << "InputVectorSizeMismatch"; break;
        case VectorIndexOutOfBounds:  os << "VectorIndexOutOfBounds"; break;
        default: BOOST_ASSERT(!"unhandled value");
    }

    // add separator
    if (!what.empty()) {
        os << ": " << what;
    }

    return os.str();
}

} // namespace bgsched

