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
#ifndef RUNJOB_SAMPLE_PLUGIN_PROCESS_TREE_H_
#define RUNJOB_SAMPLE_PLUGIN_PROCESS_TREE_H_

#include <ostream>

#include <sys/types.h>

/*!
 * \brief
 */
class ProcessTree
{
public:
    /*!
     * \brief ctor.
     */
    explicit ProcessTree(
            pid_t pid
            );

    /*!
     * \brief Get pid.
     */
    pid_t getPid() const { return _pid; }

private:
    pid_t _pid;
};

/*!
 * \brief insertion operator.
 */
std::ostream&
operator<<(
        std::ostream& stream,       //!< [in]
        const ProcessTree& tree     //!< [in]
        );

#endif
