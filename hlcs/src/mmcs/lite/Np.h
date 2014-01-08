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

#ifndef MMCS_LITE_NP_H
#define MMCS_LITE_NP_H

#include "server/BlockHelper.h"

#include <spi/include/mu/Classroute_inlines.h>

#include <boost/shared_array.hpp>


namespace mmcs {
namespace lite {


class JobInfo;

/*!
 * \brief
 */
class Np
{
public:
    /*!
     * \brief Container type.
     */
    typedef boost::shared_array<CR_COORD_T> Container;

public:
    Np(
            const JobInfo& info,    //!< [in]
            const server::BlockPtr& block   //!< [in]
      );

    /*!
     * \brief Get the container.
     */
    Container& container() { return _container; }

    /*!
     * \brief
     */
    int size() const { return _size; };

    /*!
     * \brief
     */
    CR_RECT_T& world() { return _world; }

    /*!
     * \brief
     */
    CR_RECT_T& rectangle() { return _rectangle; }

    /*!
     * \brief
     */
    int* mapping() { return _mapping; }

    /*!
     * \brief
     */
    int primaryDimension() const { return _primaryDimension; }

private:
    void defaultMapping();
    void createWorldRectangle();
    void createRectangle();

private:
    const JobInfo& _info;
    const server::BlockPtr& _block;
    Container _container;
    CR_RECT_T _world;
    CR_RECT_T _rectangle;
    int _primaryDimension;
    unsigned _includeCount;
    int _mapping[5];
    int _size;
};

} } // namespace mmcs::lite

#endif

