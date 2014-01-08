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

#ifndef BGWS_BLUEGENE_HPP_
#define BGWS_BLUEGENE_HPP_


#include <db/include/api/BGQDBlib.h>

#include <boost/multi_array.hpp>
#include <boost/utility.hpp>

#include <stdint.h>
#include <string>
#include <vector>


namespace bgws {


class BlueGene : boost::noncopyable
{
public:

    struct MachineInfo {
        typedef boost::multi_array<std::string,BGQDB::Dimension::Count> LocationsMultiArray;

        uint32_t rows, columns;
        unsigned midplane_count;
        LocationsMultiArray mp_locations;
        std::vector<std::string> io_drawer_locations;
    };


    BlueGene();

    const MachineInfo& getMachineInfo() const  { return _machine_info; }


private:

    MachineInfo _machine_info;
};


} // namespace bgws

#endif
