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

#ifndef _BLOCKCONTROLLERTARGET_H
#define _BLOCKCONTROLLERTARGET_H

#include "MMCSCommandReply.h"
#include <vector>
#include "BlockHelper.h"

class BCTargetInfo;
class BCNodeInfo;
class BCIconInfo;
class BCNodecardInfo;
class BCLinkchipInfo;

// A list of pointers to nodes comprising the target of a BlockController operation.
//
class BlockControllerTarget
{
private:
    BlockPtr             	 _blockController; 	// block from which targets are to be selected
    const std::string          	 _spec;     		// specification string
    const char*              	 _cur;      		// current parsing position in specification string
    std::vector<BCTargetInfo*>   _targets;              // all targets that were selected by specification string
    std::vector<BCIconInfo*>  	 _icons;        	// icon chips that were selected by specification string
    std::vector<BCNodeInfo*>   	 _nodes;    		// nodes that were selected by specification string
    std::vector<BCLinkchipInfo*> _linkchips;     	// link chips that were selected by specification string
private:
    bool parseSpec(MMCSCommandReply& reply);
    bool parseRegexp(MMCSCommandReply& reply);
    bool parseGroup(MMCSCommandReply& reply);
    bool parseIndex(unsigned *index, MMCSCommandReply& reply);
    bool error(const std::string& message, MMCSCommandReply& reply);

public:
    BlockControllerTarget(BlockPtr blockController, const std::string& selectionSpecification, MMCSCommandReply& reply);
    BlockPtr getBlockController() 			        { return _blockController; }
    const std::vector<BCTargetInfo *>&   getTargets() const	{ return _targets; }
    const std::vector<BCNodeInfo *>&     getNodes() const 	{ return _nodes; }
    const std::vector<BCLinkchipInfo *>& getLinkchips() const 	{ return _linkchips; }
    const std::vector<BCIconInfo *>&     getIcons() const       { return _icons; }
    const std::string& getSpec() const 				{ return _spec; }
    unsigned numTargets() 					{ return _targets.size(); }
    unsigned numIcons() 					{ return _icons.size(); }
    unsigned numNodes() 					{ return _nodes.size(); }
    unsigned numLinkchips() 					{ return _linkchips.size(); }
};

#endif
