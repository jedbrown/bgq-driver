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
 * \file MMCSCommand_block.h
 */

#ifndef _MMCSCONSOLECOMMAND_BLOCK_H
#define _MMCSCONSOLECOMMAND_BLOCK_H

#include "MMCSCommand.h"

/*!
** gen_midplane_block <blockid> <corner> <asize> <bsize> <csize> <dsize> [aPT bPT cPT dPT]
** Generate a block for a set of midplanes.

** For <corner> specify midplane by location, i.e. Rxx-Mx.
** The size is provided in terms of number of midplanes in A, B, C and D dimensions.
** The <corner> will be in the 0,0,0,0 position of the midplanes that make up the block.
** Except for the <corner>, the midplanes included in the generated block
** depend on A,B,C,D cabling of your machine.
*/
class MMCSServerCommand_gen_midplane_block : public MMCSCommand
{
public:
    MMCSServerCommand_gen_midplane_block (const char* name, const char* description, const MMCSCommandAttributes& attributes)
    : MMCSCommand(name,description,attributes) { usage = "gen_midplane_block <blockid> <corner> <asize> <bsize> <csize> <dsize> [aPT bPT cPT dPT]";}
    static  MMCSServerCommand_gen_midplane_block* build();    // factory method
    static  std::string cmdname() { return "gen_midplane_block"; }
    void execute(std::deque<std::string> args,
             MMCSCommandReply& reply,
             ConsoleController* pController,
             BlockControllerTarget* pTarget,
             std::vector<std::string>* validnames);
    void execute(std::deque<std::string> args,
             MMCSCommandReply& reply,
             ConsoleController* pController,
             BlockControllerTarget* pTarget);
    bool checkArgs(std::deque<std::string>& args) { if((args.size() != 6) && (args.size() != 10)) return false; else return true;}
    void help(std::deque<std::string> args,
              MMCSCommandReply& reply);
};

/*!
** gen_block <blockid> <midplane>
** Generate block for a midplane.  Specify <midplane> by location, i.e. Rxx-Mx.
*/
class MMCSServerCommand_gen_block : public MMCSCommand
{
public:
    MMCSServerCommand_gen_block (const char* name, const char* description, const MMCSCommandAttributes& attributes)
    : MMCSCommand(name,description,attributes) { usage = "gen_block <blockid> <midplane>";}
    static  MMCSServerCommand_gen_block* build();    // factory method
    static  std::string cmdname() { return "gen_block"; }
    void execute(std::deque<std::string> args,
             MMCSCommandReply& reply,
             ConsoleController* pController,
             BlockControllerTarget* pTarget,
             std::vector<std::string>* validnames);
    void execute(std::deque<std::string> args,
             MMCSCommandReply& reply,
             ConsoleController* pController,
             BlockControllerTarget* pTarget);
    bool checkArgs(std::deque<std::string>& args) { if(args.size() < 2) return false; else return true;}
    void help(std::deque<std::string> args,
              MMCSCommandReply& reply);
};

/*!
** gen_blocks [<blockidprefix>]
** Generate a block for each midplane on a machine.
** The <blockidprefix> is optional.
** If omitted, each block name will be the position of the midplane, i.e. Rxx-Mx.
*/
class MMCSServerCommand_gen_blocks : public MMCSCommand
{
public:
    MMCSServerCommand_gen_blocks (const char* name, const char* description, const MMCSCommandAttributes& attributes)
    : MMCSCommand(name,description,attributes) { usage = "gen_blocks [<blockidprefix>]";}
    static  MMCSServerCommand_gen_blocks* build();    // factory method
    static  std::string cmdname() { return "gen_blocks"; }
    void execute(std::deque<std::string> args,
             MMCSCommandReply& reply,
             ConsoleController* pController,
             BlockControllerTarget* pTarget,
             std::vector<std::string>* validnames);
    void execute(std::deque<std::string> args,
             MMCSCommandReply& reply,
             ConsoleController* pController,
             BlockControllerTarget* pTarget);
    void help(std::deque<std::string> args,
              MMCSCommandReply& reply);
    bool checkArgs(std::deque<std::string>& args) { if((args.size() != 0) && (args.size() != 1)) return false; else return true;  }
};

/*
** gen_full_block <blockid>
** Generate a block for the entire machine.
*/
class MMCSServerCommand_gen_full_block : public MMCSCommand
{
public:
    MMCSServerCommand_gen_full_block (const char* name, const char* description, const MMCSCommandAttributes& attributes)
    : MMCSCommand(name,description,attributes) { usage = "gen_full_block <blockid>";}
    static  MMCSServerCommand_gen_full_block* build();    // factory method
    static  std::string cmdname() { return "gen_full_block"; }
    void execute(std::deque<std::string> args,
             MMCSCommandReply& reply,
             ConsoleController* pController,
             BlockControllerTarget* pTarget,
             std::vector<std::string>* validnames);
    void execute(std::deque<std::string> args,
             MMCSCommandReply& reply,
             ConsoleController* pController,
             BlockControllerTarget* pTarget);
    bool checkArgs(std::deque<std::string>& args) { if(args.size() != 1) return false; else return true;  }
    void help(std::deque<std::string> args,
              MMCSCommandReply& reply);
};

/*
** gen_small_block <blockid> <midplane> <cnodes> <nodecard>
** Generate a sub-midplane block.
** Specify <midplane> by location, i.e. Rxx-Mx.
** The <cnodes> is the total number of compute nodes and must be 32, 64, 128 or 256.
** The <nodecard> is the starting location of the compute nodes for the block, i.e. N00.
*/
class MMCSServerCommand_gen_small_block : public MMCSCommand
{
public:
    MMCSServerCommand_gen_small_block (const char* name, const char* description, const MMCSCommandAttributes& attributes)
    : MMCSCommand(name,description,attributes) { usage = "gen_small_block <blockid> <midplane> <cnodes> <nodecard>";}
    static  MMCSServerCommand_gen_small_block* build();    // factory method
    static  std::string cmdname() { return "gen_small_block"; }
    void execute(std::deque<std::string> args,
             MMCSCommandReply& reply,
             ConsoleController* pController,
             BlockControllerTarget* pTarget,
             std::vector<std::string>* validnames);
    void execute(std::deque<std::string> args,
             MMCSCommandReply& reply,
             ConsoleController* pController,
             BlockControllerTarget* pTarget);
    bool checkArgs(std::deque<std::string>& args) { if(args.size() != 4) return false; else return true;  }
    void help(std::deque<std::string> args,
              MMCSCommandReply& reply);
};

/*!
** gen_io_block <blockid> <location> <ionodes>
*/
class MMCSServerCommand_gen_io_block : public MMCSCommand
{
public:
    MMCSServerCommand_gen_io_block (const char* name, const char* description, const MMCSCommandAttributes& attributes)
    : MMCSCommand(name,description,attributes) { usage = "gen_io_block <blockid> <location> <ionodes>";}
    static  MMCSServerCommand_gen_io_block* build();    // factory method
    static  std::string cmdname() { return "gen_io_block"; }
    void execute(std::deque<std::string> args,
             MMCSCommandReply& reply,
             ConsoleController* pController,
             BlockControllerTarget* pTarget,
             std::vector<std::string>* validnames);
    void execute(std::deque<std::string> args,
             MMCSCommandReply& reply,
             ConsoleController* pController,
             BlockControllerTarget* pTarget);
    bool checkArgs(std::deque<std::string>& args) { if(args.size() != 3) return false; else return true;}
    void help(std::deque<std::string> args,
              MMCSCommandReply& reply);
};

/*
** set_block_info <blockid> <uloader> <cnload> <ioload> [<bootoptions>]
** Set images for a block.
*/
class MMCSServerCommand_set_block_info : public MMCSCommand
{
public:
    MMCSServerCommand_set_block_info (const char* name, const char* description, const MMCSCommandAttributes& attributes)
    : MMCSCommand(name,description,attributes) { usage = "set_block_info <blockid> <uloader> <nodecfg> [<bootoptions>]";}
    static  MMCSServerCommand_set_block_info* build();    // factory method
    static  std::string cmdname() { return "set_block_info"; }
    void execute(std::deque<std::string> args,
             MMCSCommandReply& reply,
             ConsoleController* pController,
             BlockControllerTarget* pTarget,
             std::vector<std::string>* validnames);
    void execute(std::deque<std::string> args,
             MMCSCommandReply& reply,
             ConsoleController* pController,
             BlockControllerTarget* pTarget);
    bool checkArgs(std::deque<std::string>& args) { if(args.size() < 3) return false; else return true;}
    void help(std::deque<std::string> args,
              MMCSCommandReply& reply);
};


/*
** set_boot_options <blockid|ALL> <bootoptions|NULL>
** Set boot options for a block, or for all blocks.
*/
class MMCSServerCommand_set_boot_options : public MMCSCommand
{
public:
    MMCSServerCommand_set_boot_options (const char* name, const char* description, const MMCSCommandAttributes& attributes)
    : MMCSCommand(name,description,attributes) { usage = "set_boot_options <blockid|*ALL|*ALLIO|*ALLCOMPUTE> <bootoptions|NULL>";}
    static  MMCSServerCommand_set_boot_options* build();    // factory method
    static  std::string cmdname() { return "set_boot_options"; }
    void execute(std::deque<std::string> args,
             MMCSCommandReply& reply,
             ConsoleController* pController,
             BlockControllerTarget* pTarget,
             std::vector<std::string>* validnames);
    void execute(std::deque<std::string> args,
             MMCSCommandReply& reply,
             ConsoleController* pController,
             BlockControllerTarget* pTarget);
    bool checkArgs(std::deque<std::string>& args) { if(args.size() != 2) return false; else return true;}
    void help(std::deque<std::string> args,
              MMCSCommandReply& reply);
};

/*
** add_boot_option <blockid|ALL> <bootoptions>
** Add a boot option for a block, or for all blocks.
*/
class MMCSServerCommand_add_boot_option : public MMCSCommand
{
public:
    MMCSServerCommand_add_boot_option (const char* name, const char* description, const MMCSCommandAttributes& attributes)
    : MMCSCommand(name,description,attributes) { usage = "add_boot_option <blockid|*ALL|*ALLIO|*ALLCOMPUTE> <bootoption>";}
    static  MMCSServerCommand_add_boot_option* build();    // factory method
    static  std::string cmdname() { return "add_boot_option"; }
    void execute(std::deque<std::string> args,
             MMCSCommandReply& reply,
             ConsoleController* pController,
             BlockControllerTarget* pTarget,
             std::vector<std::string>* validnames);
    void execute(std::deque<std::string> args,
             MMCSCommandReply& reply,
             ConsoleController* pController,
             BlockControllerTarget* pTarget);
    bool checkArgs(std::deque<std::string>& args) { if(args.size() != 2) return false; else return true;}
    void help(std::deque<std::string> args,
              MMCSCommandReply& reply);
};

/*
** copy_block <existingblockid> <newblockid>
** Copy a block
*/
class MMCSServerCommand_copy_block : public MMCSCommand
{
public:
    MMCSServerCommand_copy_block (const char* name, const char* description, const MMCSCommandAttributes& attributes)
    : MMCSCommand(name,description,attributes) { usage = "copy_block <existingblockid> <newblockid>";}
    static  MMCSServerCommand_copy_block* build();    // factory method
    static  std::string cmdname() { return "copy_block"; }
    void execute(std::deque<std::string> args,
             MMCSCommandReply& reply,
             ConsoleController* pController,
             BlockControllerTarget* pTarget,
             std::vector<std::string>* validnames);
    void execute(std::deque<std::string> args,
             MMCSCommandReply& reply,
             ConsoleController* pController,
             BlockControllerTarget* pTarget);
    std::vector<std::string> getBlockObjects(std::deque<std::string>& cmdString, DBConsoleController* pController);
    bool doSpecialAuths(std::vector<std::string>& blocks, boost::shared_ptr<hlcs::security::Enforcer>& enforcer, MMCSCommandProcessorStatus::procstat& stat, bgq::utility::UserId& user);
    bool checkArgs(std::deque<std::string>& args) { if(args.size() != 2) return false; else return true;}
    void help(std::deque<std::string> args,
              MMCSCommandReply& reply);
};


/*
** create_node_config
** Create a node configuration for use in booting a block
*/
class MMCSServerCommand_create_node_config : public MMCSCommand
{
public:
    MMCSServerCommand_create_node_config (const char* name, const char* description, const MMCSCommandAttributes& attributes)
    : MMCSCommand(name,description,attributes) { usage = "create_node_config <configname> [<raspolicy>] [<nodeoptions>] <domaininfo>";}
    static  MMCSServerCommand_create_node_config* build();    // factory method
    static  std::string cmdname() { return "create_node_config"; }
    void execute(std::deque<std::string> args,
             MMCSCommandReply& reply,
             ConsoleController* pController,
             BlockControllerTarget* pTarget,
             std::vector<std::string>* validnames);
    void execute(std::deque<std::string> args,
             MMCSCommandReply& reply,
             ConsoleController* pController,
             BlockControllerTarget* pTarget);
    void help(std::deque<std::string> args,
              MMCSCommandReply& reply);
    bool checkArgs(std::deque<std::string>& args) { return true; }
    std::vector<std::string> getBlockObjects(std::deque<std::string>& cmdString, DBConsoleController* pController) {
        std::vector<std::string> empty_vec;
        return empty_vec;
    }
};


#endif
