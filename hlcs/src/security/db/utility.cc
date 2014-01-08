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
#include "utility.h"

#include <stdexcept>

namespace hlcs {
namespace security {
namespace db {

const char*
toString(
        Action::Type action
        )
{
    switch (action) {
        case Action::Create:    return "C";
        case Action::Read:      return "R";
        case Action::Update:    return "U";
        case Action::Delete:    return "D";
        case Action::Execute:   return "E";
        default:
            throw std::invalid_argument( "invalid action" );
    }
}

const char*
toString(
        Object::Type object
        )
{
    switch (object) {
        case Object::Job:   return "job";
        case Object::Block: return "block";
        default:
            throw std::invalid_argument( "invalid object" );
    }
}

Action::Type
fromChar(
        char action
        )
{
    switch ( action) {
        case 'C':
            return hlcs::security::Action::Create;
        case 'R':
            return hlcs::security::Action::Read;
        case 'U':
            return hlcs::security::Action::Update;
        case 'D':
            return hlcs::security::Action::Delete;
        case 'E':
            return hlcs::security::Action::Execute;
        default:
            return Action::Invalid;
    }
}

} // db
} // security
} // hlcs
