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

#ifndef MMCS_LITE_CLASS_ROUTE_H
#define MMCS_LITE_CLASS_ROUTE_H

#include "Np.h"

#include "server/BlockHelper.h"

#include <ramdisk/include/services/JobctlMessages.h>

#include <spi/include/mu/Classroute_inlines.h>

#include <iosfwd>
#include <vector>


namespace mmcs {
namespace lite {


class JobInfo;

/*!
 * \brief
 */
class ClassRoute
{
public:
    /*!
     * \brief Container type.
     */
    typedef std::vector<bgcios::jobctl::NodeClassRoute> Container;

public:
    /*!
     * \brief ctor.
     */
    ClassRoute(
            const JobInfo& info,    //!< [in]
            const server::BlockPtr& block   //!< [in]
            );

    /*!
     * \brief
     */
    const Container& container() const { return _container; }

    /*!
     * \brief
     */
    const CR_COORD_T& leader() const { return _leader; }

private:
    void pickJobLeader();
    void generate( CR_COORD_T& me );
    void printLink( std::ostream& os, uint32_t link );
    void print( std::ostream& os, const bgcios::jobctl::NodeClassRoute& cr );

private:
    Np _np;
    const server::BlockPtr& _block;
    CR_COORD_T _leader;
    Container _container;
};

} } // namespace mmcs::lite

#endif

