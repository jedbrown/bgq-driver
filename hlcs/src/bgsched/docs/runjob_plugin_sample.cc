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
#include <bgsched/runjob/Corner.h>
#include <bgsched/runjob/Shape.h>

int
main()
{
    using namespace bgsched::runjob;

    // one way to set coordinates and shape
    {
        Corner corner;
        corner.coordinates(
                Coordinates().a(0).b(0).c(0).d(0).e(0) 
            );
        Shape shape;
        shape.a(1).b(1).c(2).d(1).e(1);
    }

    // or another way
    {
        const unsigned coordinates[5] = {0,0,0,0,0};
        const unsigned shape[5] = {1,1,2,1,1};
        Corner my_corner( coordinates );
        Shape my_shape( shape );
    }
}
