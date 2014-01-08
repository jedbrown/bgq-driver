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

#ifndef MMCS_COMMON_ABSTRACT_COMMAND_H_
#define MMCS_COMMON_ABSTRACT_COMMAND_H_

#include "fwd.h"

#include "libmmcs_client/CommandReply.h"

#include "security/Enforcer.h"
#include "security/Types.h"

#include <boost/utility.hpp>

#include <deque>
#include <string>
#include <vector>

namespace mmcs {
namespace common {

enum HELP_CAT {
    DEFAULT = 0,
    USER,
    ADMIN,
    SPECIAL
};

/*!
 * \brief Abstract base class for MMCS commands.
 *
 * To implement a specific command
 * -- Define a new class derived from AbstractCommand
 * -- Implement the build(),  execute(), and help() methods
 * -- Invoke the build() method from mmcs
 */
class AbstractCommand : private boost::noncopyable
{
public:
    /*!
     * \class Attributes
     * \brief Attributes of an MMCS command:
     * requiresBlock -- does the command require a selected BlockController object?
     * requiresTarget -- does the command take a target prefix? (e.g., {*})
     * requiresConnection -- does the command require an active ido connection to the node cards?
     * internalCommand -- is the command for internal use only?
     * bgConsoleCommand -- can the command be executed by bg_console?
     * mmcsServerCommand -- can the command be executed by mmcs_server?
     * mmcsLiteCommand --  can the command be executed by mmcs_lite?
     * requiresObjNames -- special attribute to ensure that a block argument is sent to the command when a block is selected
     */
    class Attributes
    {
    public:
        typedef std::pair<hlcs::security::Object::Type, hlcs::security::Action::Type> AuthPair;

        Attributes()
            : _requiresBlock(false),
              _requiresConnection(false),
              _requiresTarget(false),
              _internalCommand(false),
              _bgConsoleCommand(false),
              _mmcsServerCommand(false),
              _mmcsLiteCommand(false),
              _external(false),
              _helpCategory(DEFAULT),
              _bgadminAuth(false),
              _requiresObjNames(false),
              _internalAuth(false),
              _specialAuths("")
        {}
        bool  requiresBlock() { return _requiresBlock; }
        void  requiresBlock(bool tf) { _requiresBlock = tf; }
        bool  requiresTarget() { return _requiresTarget; }
        void  requiresTarget(bool tf) { _requiresTarget = tf; }
        bool  requiresConnection() { return _requiresConnection; }
        void  requiresConnection(bool tf) { _requiresConnection = tf; }
        bool  internalCommand() { return _internalCommand; }
        void  internalCommand(bool tf) { _internalCommand = tf; }
        bool  bgConsoleCommand() { return _bgConsoleCommand; }
        void  bgConsoleCommand(bool tf) { _bgConsoleCommand = tf; }
        bool  mmcsServerCommand() { return _mmcsServerCommand; }
        void  mmcsServerCommand(bool tf) { _mmcsServerCommand = tf; }
        bool  mmcsLiteCommand() { return _mmcsLiteCommand; }
        void  mmcsLiteCommand(bool tf) { _mmcsLiteCommand = tf; }
        bool  externalCommand() { return _external; }
        void  externalCommand(bool tf) { _external = tf; }
        void  addAuthPair(AuthPair& ap) { _authmap.push_back(ap); }
        bool  bgadminAuth(bool auth) { auth?_bgadminAuth = true:_bgadminAuth = false;return _bgadminAuth; }
        bool  getBgAdminAuth() { return _bgadminAuth; }
        void  requiresObjNames(bool tf) { _requiresObjNames = tf; }
        bool  requiresObjNames() { return _requiresObjNames; }
        bool  internalAuth(bool auth) { auth?_internalAuth = true:_internalAuth = false;return _internalAuth; }
        bool  getInternalAuth() { return _internalAuth; }
        std::string specialAuthString(std::string auths = "") { if(auths.length() != 0) _specialAuths = auths; return _specialAuths; }
        std::vector<AuthPair>* getAuthPairs() { return &_authmap; }
        HELP_CAT helpCategory() { return _helpCategory; }
        void helpCategory(HELP_CAT hc) { _helpCategory = hc; }
    private:
        bool   _requiresBlock;                  //!< is the command valid before a block is allocated?
        bool   _requiresConnection;             //!< is the command valid before a block is connected?
        bool   _requiresTarget;                 //!< does the command use a BlockControllerTarget?
        bool   _internalCommand;                //!< Is this command for internal use only?
        bool   _bgConsoleCommand;               //!< can the command be executed by bg_console?
        bool   _mmcsServerCommand;              //!< can the command be executed by mmcs_server?
        bool   _mmcsLiteCommand;                //!< can the command be executed by mmcs_lite?
        bool   _external;                       //!< Is this the special "external" command
        HELP_CAT _helpCategory;                 //!< The command's help category
        bool _bgadminAuth;                      //!< Requires bgadmin authority
        bool _requiresObjNames;                 //!< All object names must be passed to this command.
        bool _internalAuth;                     //!< Command that acts on no security objects
        std::string _specialAuths;              //!< Description of special authorizations performed by command
        //! \brief vector of types to associated authorities
        std::vector<AuthPair> _authmap;
    };


    /*!
     ** AbstractCommand constructor
     ** The constructor is normally invoked only by the build() method
     ** @param name the command name
     ** @param desc a brief, one line description
     ** @param attr describes the attributes and requirements of the command
     */
    AbstractCommand(const char* name, const char* description, const Attributes& attributes)
            : _name(name),
            _description(description),
            _attributes(attributes)
    {}

    /*!
     ** build() - AbstractCommand factory
     ** This is invoked at MMCS startup when MMCS builds its list of commands
     ** @return an AbstractCommand object for this specific command
     */
    static AbstractCommand* build() { return NULL; }    // factory method

    /*!
     ** AbstractCommand destructor
     */
    virtual ~AbstractCommand() {}

    /*!
     ** execute() - Perform specific MMCS command
     ** This variety of execute() is intended to be executed overridden by BlockController functions
     ** @param args the command arguments
     ** @param reply       the command output stream. Refer to class mmcs_client::CommandReply
     ** @param pController the ConsoleController object that the command is to work on
     ** @param pTarget     the BlockControllerTarget list that the command is to work on (optional)
     */
    virtual void execute(
            std::deque<std::string> ,//args,
            mmcs_client::CommandReply& reply,
            ConsoleController* ,//pController,
            server::BlockControllerTarget* ,//pTarget,
            std::vector<std::string>* //validnames
            )
    {
        reply << mmcs_client::FAIL << "Internal error occurred: return code=1." << mmcs_client::DONE;
        return;
    }

    /*!
     ** execute() - Perform specific MMCS command
     ** This variety of execute() is intended to be executed overridden by DBBlockController functions
     ** @param args the command arguments
     ** @param reply       the command output stream. Refer to class mmcs_client::CommandReply
     ** @param pController the server::DBConsoleController object that the command is to work on
     ** @param pTarget     the server::BlockControllerTarget list that the command is to work on (optional)
     */
    virtual void execute(
            std::deque<std::string> args,
            mmcs_client::CommandReply& reply,
            server::DBConsoleController* pController,
            server::BlockControllerTarget* pTarget,
            std::vector<std::string>* validnames
            )
    {
        return execute(
                args,
                reply,
                (ConsoleController*)pController,
                pTarget,
                validnames
                );
    }


    // These two versions are the same as the first two, except
    // that they do not take a list of object names that have
    // been authorized (validnames).
    virtual void execute(
            std::deque<std::string> ,//args,
            mmcs_client::CommandReply& reply,
            ConsoleController* ,//pController,
            server::BlockControllerTarget* //pTarget
            )
    {
        reply << mmcs_client::FAIL << "Internal error occurred: return code=2." << mmcs_client::DONE;
        return;
    }

    virtual void execute(
            std::deque<std::string> args,
            mmcs_client::CommandReply& reply,
            server::DBConsoleController* pController,
            server::BlockControllerTarget* pTarget=NULL
            )
    {
        return execute(
                args,
                reply,
                (ConsoleController*)pController,
                pTarget
                );
    }

    /*!
     ** help() - Print extended command help to the reply stream
     ** @param args  the help command arguments
     ** @param reply the command output stream. Refer to class mmcs_client::CommandReply
     */
    virtual void help(std::deque<std::string> args, mmcs_client::CommandReply& reply) = 0;

    /*!
     ** name() - Returns the command name
     ** @return command name
     */
    const std::string& name() const { return _name; }

    /*!
     ** description() - Returns the brief description
     ** @return brief command description
     */
    const std::string& description() const { return _description; }

    /*!
     ** attributes() - Return the command attributes
     ** @return Attributes for this object
     */
    Attributes& attributes() { return _attributes; };

    const std::string& usage() const { return _usage; }

    // Return whether the number of args is within useful bounds for the command.
    // By default, all commands expect zero arguments.
    // Your command should override this if it expects anything other than exactly zero arguments.
    // Even if it doesn't care how many arguments it gets, it should return 'true'.
    virtual bool checkArgs(std::deque<std::string>& args) { if(0 != args.size()) return false; else return true; }

    virtual bool doSpecialAuths(
            std::vector<std::string>& ,//blocks,
            const boost::shared_ptr<hlcs::security::Enforcer>& ,//enforcer,
            procstat& ,//stat,
            const bgq::utility::UserId& //user
        )
    {
        return false;
    }

    // Takes the command arguments and returns the block objects the command would like to use.
    // Security validation in the command processor will determine whether the command can
    // actually use the objects.  By default, the first argument is returned.
    virtual std::vector<std::string> getBlockObjects(
            std::deque<std::string>& cmdString,
            server::DBConsoleController* //pController
        )
    {
        std::vector<std::string> arg0_vec;
        if(cmdString.size() != 0) {
            arg0_vec.push_back(cmdString[0]);
        }
        return arg0_vec;
    }

protected:
    const std::string _name;     // name of the command
    const std::string _description;    // brief (one line) description -- use help() for extended help
    Attributes  _attributes;     // command attributes
    std::string _usage;
};

} } // namespace mmcs::common

#endif
