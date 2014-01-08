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

#include "job/types.h"

namespace BGQDB {
namespace job {

namespace status_code {

const std::string Cleanup( "N" );
const std::string Debug( "D" );
const std::string Error( "E" );
const std::string Loading( "L" );
const std::string Running( "R" );
const std::string Setup( "P" );
const std::string Starting( "S" );
const std::string Terminated( "T" );

} // namespace BGQDB::job::status_code

const std::string& valueToCode( status::Value value )
{
    return (value == status::Cleanup ? status_code::Cleanup :
            value == status::Debug ? status_code::Debug :
            value == status::Loading ? status_code::Loading :
            value == status::Running ? status_code::Running :
            value == status::Setup ? status_code::Setup :
            value == status::Starting ? status_code::Starting :
            value == status::Error ? status_code::Error :
            status_code::Terminated);
}

bool isCode( const std::string& code )
{
    return (code == status_code::Cleanup ||
            code == status_code::Debug ||
            code == status_code::Loading ||
            code == status_code::Running ||
            code == status_code::Setup ||
            code == status_code::Starting ||
            code == status_code::Error ||
            code == status_code::Terminated);
}

status::Value codeToValue( const std::string& code )
{
    return (code == status_code::Cleanup ? status::Cleanup :
            code == status_code::Debug ? status::Debug :
            code == status_code::Loading ? status::Loading :
            code == status_code::Running ? status::Running :
            code == status_code::Setup ? status::Setup :
            code == status_code::Starting ? status::Starting :
            code == status_code::Error ? status::Error :
            status::Terminated);
}

} } // namespace BGQDB::job
