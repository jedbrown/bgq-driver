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

// Create a list of pointers to nodes that comprise the target of a BlockController operation.
// Taken: full list of nodes comprising the machine
//        specification of nodes to be selected (see grammar, below)
//

#include "BlockControllerTarget.h"

#include "BCClockcardInfo.h"
#include "BCIconInfo.h"
#include "BCLinkchipInfo.h"
#include "BCNodecardInfo.h"
#include "BCNodeInfo.h"
#include "BCServicecardInfo.h"
#include "BlockControllerBase.h"

using namespace std;

namespace mmcs {
namespace server {

BlockControllerTarget::BlockControllerTarget(BlockPtr blockController, const string& spec, mmcs_client::CommandReply& reply)
    : _blockController(blockController),
      _spec(spec),
      _cur(_spec.c_str())
{
    parseSpec(reply);
}

// <spec> ::=  `{' `*' `}'
//             `{' `i' `}'
//             `{' `c' `}'
//             `{' `l' `}'
//             `{' `nc' `}'
//             `{' `lc' `}'
//             `{' `sc' `}'
//             `{' `cc' `}'
//             '{' <regexp> [ `,' <regexp> ]... '}'
//             `{' <group>  [ `,' <group>  ]... `}'
//
bool
BlockControllerTarget::parseSpec(mmcs_client::CommandReply& reply)
{
    if (_spec == "{*}") {
        for (unsigned i = 0; i < _blockController->getNodes().size(); ++i) {
            _nodes.push_back(_blockController->getNodes()[i]);
        }
        for (unsigned i = 0; i < _blockController->getIcons().size(); ++i) {
            _icons.push_back(_blockController->getIcons()[i]);
        }
        for (unsigned i = 0; i < _blockController->getLinkchips().size(); ++i) {
            _linkchips.push_back(_blockController->getLinkchips()[i]);
        }
    } else {
        if (_spec == "{i}") {
            for (unsigned i = 0; i < _blockController->getNodes().size(); ++i) {
                if (_blockController->getNodes()[i]->isIOnode()) {
                    _nodes.push_back(_blockController->getNodes()[i]);
                }
            }
        }
        else if (_spec == "{c}") {
            for (unsigned i = 0; i < _blockController->getNodes().size(); ++i) {
                if (!_blockController->getNodes()[i]->isIOnode()) {
                    _nodes.push_back(_blockController->getNodes()[i]);
                }
            }
        }
        else if (_spec == "{l}") { // link chips
            for (unsigned i = 0; i < _blockController->getLinkchips().size(); ++i) {
                _linkchips.push_back(_blockController->getLinkchips()[i]);
            }
        }
        else if (_spec == "{nc}") { // node_card icon chips
            for (unsigned i = 0; i < _blockController->getIcons().size(); ++i) {
                if (typeid(*(_blockController->getIcons()[i])) == typeid(BCNodecardInfo)) {
                    _icons.push_back(_blockController->getIcons()[i]);
                }
            }
        }
        else if (_spec == "{sc}") { // service_card icon chips
            for (unsigned i = 0; i < _blockController->getIcons().size(); ++i) {
                if (typeid(*(_blockController->getIcons()[i])) == typeid(BCServicecardInfo)) {
                    _icons.push_back(_blockController->getIcons()[i]);
                }
            }
        }
        else if (_spec == "{cc}") { // clock_card icon chips
            for (unsigned i = 0; i < _blockController->getIcons().size(); ++i) {
                if (typeid(*(_blockController->getIcons()[i])) == typeid(BCClockcardInfo)) {
                    _icons.push_back(_blockController->getIcons()[i]);
                }
            }
        }
        else if (*_cur != '{')
            return error("`{' expected", reply);
        else {
            ++_cur;
            for (;;) {
                if (*_cur == 'R' || *_cur == 'Q') { // regular expression
                    if (!parseRegexp(reply)) {
                        return false;
                    }
                }
                else if (isdigit(*_cur)) { // group expression
                    if (!parseGroup(reply))
                        return false;
                }
                else { // invalid
                    break;
                }

                if (*_cur == ',') { // separator
                    ++_cur;
                    continue;
                }
            }
            if (*_cur != '}') {
                return error("`}' expected", reply);
            }
            ++_cur;
        }
    }

    // add all BCNodeInfo, BCIconInfo, and BCLinkchipInfo objects to BlockControllerTarget::_targets
    for (unsigned i = 0; i < getNodes().size(); ++i) {
        _targets.push_back(getNodes()[i]);
    }

    for (unsigned i = 0; i < getIcons().size(); ++i) {
        _targets.push_back(getIcons()[i]);
    }

    for (unsigned i = 0; i < getLinkchips().size(); ++i) {
        _targets.push_back(getLinkchips()[i]);
    }

    return true;
}

// <regexp> ::= Posix regular expression specifying component location
//
bool
BlockControllerTarget::parseRegexp(mmcs_client::CommandReply& reply)
{
    const char *delim;

    // create a string from the regexp
    for (delim = _cur; *delim != '\0' && *delim != '}' && *delim != ',' && !isspace(*delim); ++delim)
        ;
    string regexp(_cur, delim-_cur);

    Regexp regularExp(regexp);
    for (unsigned i = 0; i < _blockController->getNodes().size(); ++i) {
        if (regularExp.matches(_blockController->getNodes()[i]->location())) {
            _nodes.push_back(_blockController->getNodes()[i]);
        }
    }

    _cur = delim; // skip to the character following the regexp

    return true;
}

// <group> ::= <index>
//             <index> `-' <index>
//
bool
BlockControllerTarget::parseGroup(mmcs_client::CommandReply& reply)
{
    unsigned n, m;
    if (!parseIndex(&n, reply)) {
        return false;
    }

    if (*_cur == '-') {
        ++_cur;
        if (!parseIndex(&m, reply)) {
            return false;
        }
    } else {
        m = n;
    }

    for (unsigned i = n; i <= m; ++i) {
        if (typeid(*(_blockController->getTargets()[i])) == typeid(BCNodeInfo)) {
            _nodes.push_back(dynamic_cast<BCNodeInfo*>(_blockController->getTargets()[i]));
        }
        else if (typeid(*(_blockController->getTargets()[i])) == typeid(BCNodecardInfo))
            _icons.push_back(dynamic_cast<BCNodecardInfo*>(_blockController->getTargets()[i]));
        else if (typeid(*_blockController->getTargets()[i]) == typeid(BCServicecardInfo))
            _icons.push_back(dynamic_cast<BCServicecardInfo*>(_blockController->getTargets()[i]));
        else if (typeid(*_blockController->getTargets()[i]) == typeid(BCClockcardInfo))
            _icons.push_back(dynamic_cast<BCClockcardInfo*>(_blockController->getTargets()[i]));
        else if (typeid(*_blockController->getTargets()[i]) == typeid(BCLinkchipInfo))
            _linkchips.push_back(dynamic_cast<BCLinkchipInfo*>(_blockController->getTargets()[i]));
    }
    return true;
}

// <index> ::= <digit> [ <digit> ]...
//
bool
BlockControllerTarget::parseIndex(unsigned *index, mmcs_client::CommandReply& reply)
{
    if (!isdigit(*_cur)) {
        return error("Index expected", reply);
    }

    *index = atoi(_cur);

    if (*index >= _blockController->getTargets().size()) {
        return error("Index out of range", reply);
    }

    while (isdigit(*_cur)) {
        ++_cur;
    }
    return true;
}

// Print error message and point to the problem.
//
bool
BlockControllerTarget::error(const string& message, mmcs_client::CommandReply& reply)
{
    reply << mmcs_client::FAIL << "Invalid target " << _spec << " : " << message << " near column " << _cur - _spec.c_str() + 1 << mmcs_client::DONE;
    _nodes.clear();
    return false;
}

} } // namespace mmcs::server
