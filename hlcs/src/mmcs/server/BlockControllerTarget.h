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
/* (C) Copyright IBM Corp.  2004, 2011                              */
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

#ifndef MMCS_SERVER_BLOCK_CONTROLLER_TARGET_H_
#define MMCS_SERVER_BLOCK_CONTROLLER_TARGET_H_

#include "types.h"

#include "libmmcs_client/CommandReply.h"

#include <string>
#include <vector>

namespace mmcs {
namespace server {

/*!
 * \brief A list of pointers to nodes comprising the target of a BlockController operation.
 */
class BlockControllerTarget
{
private:
    BlockPtr                        _blockController;  // block from which targets are to be selected
    const std::string               _spec;             // specification string
    const char*                     _cur;              // current parsing position in specification string
    std::vector<BCTargetInfo*>      _targets;          // all targets that were selected by specification string
    std::vector<BCIconInfo*>        _icons;            // icon chips that were selected by specification string
    std::vector<BCNodeInfo*>        _nodes;            // nodes that were selected by specification string
    std::vector<BCLinkchipInfo*>    _linkchips;        // link chips that were selected by specification string
private:
    bool parseSpec(mmcs_client::CommandReply& reply);
    bool parseRegexp(mmcs_client::CommandReply& reply);
    bool parseGroup(mmcs_client::CommandReply& reply);
    bool parseIndex(unsigned *index, mmcs_client::CommandReply& reply);
    bool error(const std::string& message, mmcs_client::CommandReply& reply);

public:
    BlockControllerTarget(BlockPtr blockController, const std::string& selectionSpecification, mmcs_client::CommandReply& reply);
    BlockPtr getBlockController() { return _blockController; }
    const std::vector<BCTargetInfo *>& getTargets() const { return _targets; }
    const std::vector<BCNodeInfo *>& getNodes() const { return _nodes; }
    const std::vector<BCLinkchipInfo *>& getLinkchips() const { return _linkchips; }
    const std::vector<BCIconInfo *>& getIcons() const { return _icons; }
    const std::string& getSpec() const { return _spec; }
    unsigned numTargets() { return _targets.size(); }
    unsigned numIcons() { return _icons.size(); }
    unsigned numNodes() { return _nodes.size(); }
    unsigned numLinkchips() { return _linkchips.size(); }
};

} } // namespace mmcs::server

#endif
