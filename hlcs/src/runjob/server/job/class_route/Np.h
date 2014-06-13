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
#ifndef RUNJOB_SERVER_JOB_CLASS_ROUTE_NP_H
#define RUNJOB_SERVER_JOB_CLASS_ROUTE_NP_H

#include "server/job/class_route/Rectangle.h"
#include "server/job/class_route/fwd.h"

#include "common/fwd.h"

#include <spi/include/mu/Classroute_inlines.h>

#include <utility/include/BoolAlpha.h>
#include <utility/include/Properties.h>

#include <boost/shared_ptr.hpp>
#include <boost/shared_array.hpp>

#include <string>

namespace runjob {
namespace server {
namespace job  {
namespace class_route {

/*!
 * \brief A container of node coordinates to exclude from the class route generation.
 */
class Np
{

public:
    /*!
     * \brief Pointer type.
     */
    typedef boost::shared_ptr<Np> Ptr;

    /*!
     * \brief Container type.
     */
    typedef boost::shared_array<CR_COORD_T> Container;

public:
    /*!
     * \brief ctor.
     */
    Np(
            const JobInfo& job,                 //!< [in]
            Rectangle* world,                   //!< [in]
            Mapping* mapping,                   //!< [in]
            const std::string& mapArchiveFile,  //!< [in]
            bool permutationMappingType,        //!< [in]
            bool retainMappingFiles             //!< [in]
      );

    /*!
     * \brief Get the container.
     */
    Container& container() { return _container; }

    /*!
     * \brief
     */
    unsigned includeCount() const { return _includeCount; }

    /*!
     * \brief
     */
    int size() const { return _size; }

    /*!
     * \brief
     */
    Rectangle& rectangle() { return _rectangle; }

    /*!
     * \brief
     */
    int primaryDimension() const { return _primaryDimension; }

private:
    unsigned countIncludedNodes(
            const JobInfo& info,
            const Rectangle* world
            ) const;

private:
    Container        _container;
    int              _size;
    Rectangle        _rectangle;
    int              _primaryDimension;
    unsigned         _includeCount;
    std::string      _mapArchiveFile;
    bool             _permutationMappingType;
    bool             _retainMappingFiles;

};

} // class_route
} // job
} // server
} // runjob

#endif
