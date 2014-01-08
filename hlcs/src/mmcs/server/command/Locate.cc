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
/* (C) Copyright IBM Corp.  2005, 2011                              */
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

/*!
 * \file Locate.cc
 * \brief The locate command shows the addressing information for nodes in a block.
 */

#include "Locate.h"

#include "../BCClockcardInfo.h"
#include "../BCIconInfo.h"
#include "../BCLinkchipInfo.h"
#include "../BCNodecardInfo.h"
#include "../BCNodeInfo.h"
#include "../BCServicecardInfo.h"
#include "../BlockControllerTarget.h"
#include "../BlockHelper.h"
#include "../CNBlockController.h"
#include "../IOBlockController.h"

#include "common/ConsoleController.h"
#include "common/Properties.h"

#include "libmmcs_client/CommandReply.h"

#include <control/include/bgqconfig/BGQPersonality.h>
#include <control/include/bgqconfig/BGQWirePort.h>
#include <control/include/bgqconfig/BGQTopology.h>
#include <utility/include/Log.h>

#define NO_COORD (-1)

using namespace std;

LOG_DECLARE_FILE( "mmcs.server" );

namespace {

// Helper method to dump location info for a single node.
void
dumpLocationInfo(
        mmcs::server::BlockPtr pBlock,
        const char *msg,
        ostream &ostr,
        mmcs::server::BCNodeInfo *nodeInfo,
        const bool bRASFormat,
        const bool ioBlock
)
{
    if (!nodeInfo->_iopos.trainOnly()) {

        ostr << msg <<  "{" << nodeInfo->_locateId << "}\t";
        ostr << "<"
             << setw(2) << (uint16_t)nodeInfo->personality().Network_Config.Acoord << ","
             << setw(2) << (uint16_t)nodeInfo->personality().Network_Config.Bcoord << ","
             << setw(2) << (uint16_t)nodeInfo->personality().Network_Config.Ccoord << ","
             << setw(2) << (uint16_t)nodeInfo->personality().Network_Config.Dcoord << ","
             << setw(2) << (uint16_t)nodeInfo->personality().Network_Config.Ecoord << ">\t";


        if ( bRASFormat || ioBlock ) {
            ostr << "location: " << setw(16) << left << nodeInfo->location();
        } else {
            ostr << "in " << setw(6) << left << nodeInfo->midplanePos()
                 << " board "     << BGQTopology::nodeCardNameFromPos(nodeInfo->_pos.nodeCard())
                 << " card "      << BGQTopology::processorCardNameFromJtagPort(nodeInfo->_pos.jtagPort());
        }

        if (nodeInfo->_open)
            ostr << "\t target: open";
        else
            ostr << "\t target: closed";
    }
}

int
next_coord(
        unsigned start,
        unsigned size,
        const bool torus
)
{
    if (++start == size) {
        if (torus)
            return 0;
        else
            return NO_COORD;
    }
    return start;
}

int
prev_coord(
        unsigned start,
        unsigned size,
        const bool torus
)
{
    if (start == 0) {
        if (torus)
            return size-1;
        else
            return NO_COORD;
    }
    return --start;
}

} // anonymous namespace

namespace mmcs {
namespace server {
namespace command {

Locate*
Locate::build()
{
    Attributes commandAttributes;
    commandAttributes.requiresBlock(true);             // does require a BlockController object
    commandAttributes.requiresConnection(false);       // does not require  mc_server connections
    commandAttributes.requiresTarget(true);            // does require a BlockControllerTarget object
    commandAttributes.mmcsServerCommand(true);
    commandAttributes.mmcsLiteCommand(true);
    Attributes::AuthPair blockread(hlcs::security::Object::Block, hlcs::security::Action::Read);
    commandAttributes.addAuthPair(blockread);
    return new Locate("locate", "[<target>] locate [neighbors [verbose]] [ras_format] [summary]", commandAttributes);
}

void
Locate::execute(
        deque<string> args,
        mmcs_client::CommandReply& reply,
        common::ConsoleController* pController,
        BlockControllerTarget* pTarget
)
{
    bool bNeighbors = false;
    bool bRASFormat = false;
    bool bVerbose = false;
    bool bSummary = false;

    for (unsigned i = 0; i < args.size(); ++i) {
        if (args[i] == "neighbors")  { bNeighbors   = true;    continue; }
        if (args[i] == "ras_format") { bRASFormat   = true;    continue; }
        if (args[i] == "verbose")    { bVerbose     = true;    continue; }
        if (args[i] == "summary")    { bSummary     = true;    continue; }
        reply << mmcs_client::FAIL << "Unrecognized argument: " << args[i] << mmcs_client::DONE;
        return;
    }

    if ( bNeighbors && bSummary ) {
        reply << mmcs_client::FAIL << "Specify one of neighbors or summary." << mmcs_client::DONE;
        return;
    } else if ( bVerbose && !bNeighbors ) {
        reply << mmcs_client::FAIL << "Specify verbose with neighbors." << mmcs_client::DONE;
        return;
    }

    const BlockPtr pBlock = pController->getBlockHelper()->getBase();
    const CNBlockPtr compute_block = boost::dynamic_pointer_cast<CNBlockController>( pBlock );
    const IOBlockPtr io_block = boost::dynamic_pointer_cast<IOBlockController>( pBlock );
    BOOST_ASSERT( compute_block || io_block );

    const BGQBlockNodeConfig* bnc = pBlock->getBlockNodeConfig();
    BOOST_ASSERT( bnc );

    reply << mmcs_client::OK;

    if ( bSummary ) {
        // summarize the block output
        if ( compute_block ) {
            if ( pTarget->getNodes().size() >= 512 ) {
                string mp = "none";
                for (unsigned i = 0; i < pTarget->getNodes().size(); ++i) {
                    BCNodeInfo *nodeInfo = pTarget->getNodes()[i];
                    if (!nodeInfo->_iopos.trainOnly() && mp != string(nodeInfo->midplanePos())) {
                        if ( mp != "none" ) reply << endl;
                        reply << nodeInfo->midplanePos();
                        mp = string(nodeInfo->midplanePos());
                    }
                }
            } else {
                string nb = "none";
                for (unsigned i = 0; i < pTarget->getNodes().size(); ++i) {
                    BCNodeInfo *nodeInfo = pTarget->getNodes()[i];
                    if (!nodeInfo->_iopos.trainOnly() && nb != (string(nodeInfo->midplanePos()) + string(BGQTopology::nodeCardNameFromPos(nodeInfo->_pos.nodeCard())))) {
                        if ( nb != "none" ) reply << endl;
                        reply << nodeInfo->midplanePos() << "-" << BGQTopology::nodeCardNameFromPos(nodeInfo->_pos.nodeCard());
                        nb = string(nodeInfo->midplanePos()) + string(BGQTopology::nodeCardNameFromPos(nodeInfo->_pos.nodeCard()));
                    }
                }
            }
        } else {
            string iob = "none";
            for (unsigned i = 0; i < pTarget->getNodes().size(); ++i) {
                BCNodeInfo *nodeInfo = pTarget->getNodes()[i];
                if ( iob != nodeInfo->ioboardNodeConfig()->posInMachine() ) {
                    if ( iob != "none" ) reply << endl;
                    iob = nodeInfo->ioboardNodeConfig()->posInMachine();
                    reply << iob;
                }
            }
        }

        reply << mmcs_client::DONE;
        return;
    }

    for (unsigned i = 0; i < pTarget->getNodes().size(); ++i) {
        BCNodeInfo *nodeInfo = pTarget->getNodes()[i];
        ostringstream ostr;

        if ( !nodeInfo->_iopos.trainOnly() ) {
            dumpLocationInfo(pBlock,"", ostr, nodeInfo, bRASFormat, io_block);
            string fromPos(nodeInfo->midplanePos());
            if (bNeighbors) {
                int coord;
                std::string linkInfo;
                ostr << '\n';
                BCNodeInfo *nb;
                // For torus just search for nodes in the +/- directions

                // A+
                coord = next_coord(nodeInfo->personality().Network_Config.Acoord,
                                   nodeInfo->personality().Network_Config.Anodes,
                                   nodeInfo->personality().Network_Config.NetFlags & ND_ENABLE_TORUS_DIM_A);

                if (coord != NO_COORD) {
                    nb = pBlock->findNodeInfo(coord,
                                              nodeInfo->personality().Network_Config.Bcoord,
                                              nodeInfo->personality().Network_Config.Ccoord,
                                              nodeInfo->personality().Network_Config.Dcoord,
                                              nodeInfo->personality().Network_Config.Ecoord);

                    if (nb) {
                        dumpLocationInfo(pBlock,"    A+ ", ostr, nb, bRASFormat, io_block);
                        if (bVerbose) {
                            linkInfo = BGQTopology::neighborInfo(nodeInfo->_pos.jtagPort(), nodeInfo->_pos.nodeCard(), nb->_pos.nodeCard(),
                                                      fromPos,nb->midplanePos(),
                                                      (coord == 0) , "A+");
                            ostr << "\t" << linkInfo;
                        }
                        ostr << '\n';
                    }
                }

                // A-
                coord = prev_coord(nodeInfo->personality().Network_Config.Acoord,
                                   nodeInfo->personality().Network_Config.Anodes,
                                   nodeInfo->personality().Network_Config.NetFlags & ND_ENABLE_TORUS_DIM_A);

                if (coord != NO_COORD) {
                    nb = pBlock->findNodeInfo(coord,
                                              nodeInfo->personality().Network_Config.Bcoord,
                                              nodeInfo->personality().Network_Config.Ccoord,
                                              nodeInfo->personality().Network_Config.Dcoord,
                                              nodeInfo->personality().Network_Config.Ecoord);

                    if (nb) {
                        dumpLocationInfo(pBlock,"    A- ", ostr, nb, bRASFormat, io_block);
                        if (bVerbose) {
                            linkInfo = BGQTopology::neighborInfo(nb->_pos.jtagPort(), nodeInfo->_pos.nodeCard(), nb->_pos.nodeCard(),
                                                      fromPos,nb->midplanePos(),
                                                      (nodeInfo->personality().Network_Config.Acoord == 0) , "A-");
                            ostr << "\t" << linkInfo;
                        }
                        ostr << '\n';
                    }
                }

                // B+
                coord = next_coord(nodeInfo->personality().Network_Config.Bcoord,
                                   nodeInfo->personality().Network_Config.Bnodes,
                                   nodeInfo->personality().Network_Config.NetFlags & ND_ENABLE_TORUS_DIM_B);

                if (coord != NO_COORD) {
                    nb = pBlock->findNodeInfo(nodeInfo->personality().Network_Config.Acoord,
                                              coord,
                                              nodeInfo->personality().Network_Config.Ccoord,
                                              nodeInfo->personality().Network_Config.Dcoord,
                                              nodeInfo->personality().Network_Config.Ecoord);
                    if (nb) {
                        dumpLocationInfo(pBlock,"    B+ ", ostr, nb, bRASFormat, io_block);
                        if (bVerbose) {
                            linkInfo = BGQTopology::neighborInfo(nodeInfo->_pos.jtagPort(), nodeInfo->_pos.nodeCard(), nb->_pos.nodeCard(),
                                                      fromPos,nb->midplanePos(),
                                                      (coord == 0) , "B+");
                            ostr << "\t" << linkInfo;
                        }
                        ostr << '\n';
                    }
                }

                // B-
                coord = prev_coord(nodeInfo->personality().Network_Config.Bcoord,
                                   nodeInfo->personality().Network_Config.Bnodes,
                                   nodeInfo->personality().Network_Config.NetFlags & ND_ENABLE_TORUS_DIM_B);

                if (coord != NO_COORD) {
                    nb = pBlock->findNodeInfo(nodeInfo->personality().Network_Config.Acoord,
                                              coord,
                                              nodeInfo->personality().Network_Config.Ccoord,
                                              nodeInfo->personality().Network_Config.Dcoord,
                                              nodeInfo->personality().Network_Config.Ecoord);
                    if (nb) {
                        dumpLocationInfo(pBlock,"    B- ", ostr, nb, bRASFormat, io_block);
                        if (bVerbose) {
                            linkInfo = BGQTopology::neighborInfo(nb->_pos.jtagPort(), nodeInfo->_pos.nodeCard(), nb->_pos.nodeCard(),
                                                      fromPos,nb->midplanePos(),
                                                      (nodeInfo->personality().Network_Config.Bcoord == 0) , "B-");
                            ostr << "\t" << linkInfo;
                        }
                        ostr << '\n';
                    }
                }

                // C+
                coord = next_coord(nodeInfo->personality().Network_Config.Ccoord,
                                   nodeInfo->personality().Network_Config.Cnodes,
                                   nodeInfo->personality().Network_Config.NetFlags & ND_ENABLE_TORUS_DIM_C);

                if (coord != NO_COORD) {
                    nb = pBlock->findNodeInfo(nodeInfo->personality().Network_Config.Acoord,
                                              nodeInfo->personality().Network_Config.Bcoord,
                                              coord,
                                              nodeInfo->personality().Network_Config.Dcoord,
                                              nodeInfo->personality().Network_Config.Ecoord);
                    if (nb) {
                        dumpLocationInfo(pBlock,"    C+ ", ostr, nb, bRASFormat, io_block);
                        if (bVerbose) {
                            linkInfo = BGQTopology::neighborInfo(nodeInfo->_pos.jtagPort(), nodeInfo->_pos.nodeCard(), nb->_pos.nodeCard(),
                                                      fromPos,nb->midplanePos(),
                                                      (coord == 0) , "C+");
                            ostr << "\t" << linkInfo;
                        }
                        ostr << '\n';
                    }
                }

                // C-
                coord = prev_coord(nodeInfo->personality().Network_Config.Ccoord,
                                   nodeInfo->personality().Network_Config.Cnodes,
                                   nodeInfo->personality().Network_Config.NetFlags & ND_ENABLE_TORUS_DIM_C);

                if (coord != NO_COORD) {
                    nb = pBlock->findNodeInfo(nodeInfo->personality().Network_Config.Acoord,
                                              nodeInfo->personality().Network_Config.Bcoord,
                                              coord,
                                              nodeInfo->personality().Network_Config.Dcoord,
                                              nodeInfo->personality().Network_Config.Ecoord);
                    if (nb) {
                        dumpLocationInfo(pBlock,"    C- ", ostr, nb, bRASFormat, io_block);
                        if (bVerbose) {
                            linkInfo = BGQTopology::neighborInfo(nb->_pos.jtagPort(), nodeInfo->_pos.nodeCard(), nb->_pos.nodeCard(),
                                                      fromPos,nb->midplanePos(),
                                                      (nodeInfo->personality().Network_Config.Ccoord == 0) , "C-");
                            ostr << "\t" << linkInfo;
                        }
                        ostr << '\n';
                    }
                }

                if (compute_block) {
                    // D+
                    coord = next_coord(nodeInfo->personality().Network_Config.Dcoord,
                                       nodeInfo->personality().Network_Config.Dnodes,
                                       nodeInfo->personality().Network_Config.NetFlags & ND_ENABLE_TORUS_DIM_D);

                    if (coord != NO_COORD) {
                        nb = pBlock->findNodeInfo(nodeInfo->personality().Network_Config.Acoord,
                                                  nodeInfo->personality().Network_Config.Bcoord,
                                                  nodeInfo->personality().Network_Config.Ccoord,
                                                  coord,
                                                  nodeInfo->personality().Network_Config.Ecoord);
                        if (nb) {
                            dumpLocationInfo(pBlock,"    D+ ", ostr, nb, bRASFormat, io_block);
                            if (bVerbose) {
                                linkInfo = BGQTopology::neighborInfo(nodeInfo->_pos.jtagPort(), nodeInfo->_pos.nodeCard(), nb->_pos.nodeCard(),
                                                          fromPos,nb->midplanePos(),
                                                          (coord == 0) , "D+");
                                ostr << "\t" << linkInfo;
                            }
                            ostr << '\n';
                        }
                    }

                    // D-
                    coord = prev_coord(nodeInfo->personality().Network_Config.Dcoord,
                                       nodeInfo->personality().Network_Config.Dnodes,
                                       nodeInfo->personality().Network_Config.NetFlags & ND_ENABLE_TORUS_DIM_D);

                    if (coord != NO_COORD) {
                        nb = pBlock->findNodeInfo(nodeInfo->personality().Network_Config.Acoord,
                                                  nodeInfo->personality().Network_Config.Bcoord,
                                                  nodeInfo->personality().Network_Config.Ccoord,
                                                  coord,
                                                  nodeInfo->personality().Network_Config.Ecoord);
                        if (nb) {
                            dumpLocationInfo(pBlock,"    D- ", ostr, nb, bRASFormat, io_block);
                            if (bVerbose) {
                                linkInfo = BGQTopology::neighborInfo(nb->_pos.jtagPort(), nodeInfo->_pos.nodeCard(), nb->_pos.nodeCard(),
                                                          fromPos,nb->midplanePos(),
                                                          (nodeInfo->personality().Network_Config.Dcoord == 0) , "D-");
                                ostr << "\t" << linkInfo;
                            }
                            ostr << '\n';
                        }
                    }

                    // E+
                    coord = next_coord(nodeInfo->personality().Network_Config.Ecoord,
                                       nodeInfo->personality().Network_Config.Enodes,
                                       nodeInfo->personality().Network_Config.NetFlags & ND_ENABLE_TORUS_DIM_E);

                    if (coord != NO_COORD) {
                        nb = pBlock->findNodeInfo(nodeInfo->personality().Network_Config.Acoord,
                                                  nodeInfo->personality().Network_Config.Bcoord,
                                                  nodeInfo->personality().Network_Config.Ccoord,
                                                  nodeInfo->personality().Network_Config.Dcoord,
                                                  coord);
                        if (nb) {
                            dumpLocationInfo(pBlock,"    E+ ", ostr, nb, bRASFormat, io_block);
                            if (bVerbose) {
                                linkInfo = BGQTopology::neighborInfo(nodeInfo->_pos.jtagPort(), nodeInfo->_pos.nodeCard(), nb->_pos.nodeCard(),
                                                          fromPos,nb->midplanePos(),
                                                          false , "E+");
                                ostr << "\t" << linkInfo;
                            }
                            ostr << '\n';
                        }
                    }

                    // E-
                    coord = prev_coord(nodeInfo->personality().Network_Config.Ecoord,
                                       nodeInfo->personality().Network_Config.Enodes,
                                       nodeInfo->personality().Network_Config.NetFlags & ND_ENABLE_TORUS_DIM_E);

                    if (coord != NO_COORD) {
                        nb = pBlock->findNodeInfo(nodeInfo->personality().Network_Config.Acoord,
                                                  nodeInfo->personality().Network_Config.Bcoord,
                                                  nodeInfo->personality().Network_Config.Ccoord,
                                                  nodeInfo->personality().Network_Config.Dcoord,
                                                  coord);
                        if (nb) {
                            dumpLocationInfo(pBlock,"    E- ", ostr, nb, bRASFormat, io_block);
                            if (bVerbose) {
                                linkInfo = BGQTopology::neighborInfo(nb->_pos.jtagPort(), nodeInfo->_pos.nodeCard(), nb->_pos.nodeCard(),
                                                          fromPos,nb->midplanePos(),
                                                          false , "E-");
                                ostr << "\t" << linkInfo;
                            }
                            ostr << '\n';
                        }
                    }

                    // include the connected IO node
                    ostr << "    Connected IO: " << bnc->connectedIONode(nodeInfo->location()) << "\n";
                }
            }
            reply << ostr.str() << '\n';
        }
    }

    // location information on icon cards
    for (unsigned i = 0; i < pTarget->getIcons().size(); ++i) {
        BCIconInfo *iconInfo = pTarget->getIcons()[i];
        ostringstream ostr;
        // if this is an io location in a compute block, skip it, since its included only for training
        if (iconInfo->_ioboard == false  || io_block) {
            ostr << "{" << iconInfo->_locateId << "}\t";
            if (iconInfo->_ioboard)
                ostr << setw(10) << left << "<io>" << "\t";
            else if (typeid(*iconInfo) == typeid(BCNodecardInfo))
                ostr << setw(10) << left << "<nc>" << "\t";
            else if (typeid(*iconInfo) == typeid(BCServicecardInfo))
                ostr << setw(10) << left << "<sc>" << "\t";
            else if (typeid(*iconInfo) == typeid(BCClockcardInfo))
                ostr << setw(10) << left << "<cc>" << "\t";
            if (bRASFormat || io_block) {
                ostr << "location: " << setw(16) << left << iconInfo->location();
            } else {
                ostr << "in " << setw(6) << left << iconInfo->midplanePos();
                ostr << " board "     << iconInfo->cardName();
            }
            if (iconInfo->_open)
                ostr << "\t target: open";
            else
                ostr << "\t target: closed";
            reply << ostr.str() << '\n';
        }
    }

    // location information on link chips
    for (unsigned i = 0; i < pTarget->getLinkchips().size(); ++i) {
        ostringstream ostr;
        BCLinkchipInfo *linkchipInfo = pTarget->getLinkchips()[i];

        ostr << "{" << linkchipInfo->_locateId << "}\t<l>\t";
        if (bRASFormat) {
            ostr << "location: " << setw(16) << left << linkchipInfo->location();
        } else {
            ostr << "in " << setw(6) << left << linkchipInfo->midplanePos()
                << " chip "  << linkchipInfo->linkchipPos();
        }
        ostr << "\troutes:";
        if (linkchipInfo->_open)
            ostr << "\t target: open";
        else
            ostr << "\t target: closed";
        reply << ostr.str() << '\n';
    }
    reply << mmcs_client::DONE;
}

void
Locate::help(
        deque<string> args,
        mmcs_client::CommandReply& reply
)
{
    reply << mmcs_client::OK << description()
          << ";Lists physical location (midplane, board, card, slot) of all nodes in the selected block. "
          << ";If target node is specified, lists the location of the specified node. "
          << ";Add 'neighbors' to get the + and - neighbors in a,b,c,d,e dimensions, and the connected"
          << ";  I/O node, for a compute block. Add verbose to see how the connection is made."
          << ";Add 'ras_format' to get location information in the format found in RAS events."
          << ";Add 'summary' to get summary board or midplane information instead of node locations."
          << mmcs_client::DONE;
}

} } } // namespace mmcs::server::command
