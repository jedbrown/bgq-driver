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
#include <db/include/api/genblock.h>

#include <utility/include/Properties.h>
#include <utility/include/Log.h>
#include <utility/include/UserId.h>

void
io( 
        const bgq::utility::UserId& uid
  )
{
    {
        const std::string id = "R00-ID";
        const std::string location = "R00-ID";
        const unsigned nodes = 8;
        BGQDB::genIOBlock( id, location, nodes, uid.getUser() );
    }
    {
        const std::string id = "R00-IC";
        const std::string location = "R00-IC";
        const unsigned nodes = 8;
        BGQDB::genIOBlock( id, location, nodes, uid.getUser() );
    }
}

void
midplane(
        const bgq::utility::UserId& uid
        )
{
    const std::string id = "R00-M0";
    const std::string location = "R00-M0";
    BGQDB::genBlock( id, location, uid.getUser() );
}

void
rack(
        const bgq::utility::UserId& uid
    )
{

    BGQDB::genMidplaneBlock( "R00", "R00-M0", 1, 1, 1, 2, uid.getUser() );
}

void
small(
        const bgq::utility::UserId& uid
     )
{
    const std::string midplane = "R00-M0";
    const unsigned nodes = 32;

    for ( unsigned int i = 0; i < 16; ++i ) {
        std::ostringstream name;
        name << "R00-M0-N" << std::setw(2) << std::setfill('0') << i;
        std::ostringstream board;
        board << "N" << std::setw(2) << std::setfill('0') << i;
        BGQDB::genSmallBlock( name.str(), midplane, nodes, board.str(), uid.getUser() );
    }
}

int
main()
{
    const bgq::utility::Properties::ConstPtr properties(
            bgq::utility::Properties::create()
            );

    bgq::utility::initializeLogging( *properties );
    const unsigned size = 1;
    BGQDB::init( properties, size );

    const bgq::utility::UserId uid;

    io( uid );
    midplane( uid );
    rack( uid );
    small( uid );
}


