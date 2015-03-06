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

/*!
 * \file bg_consolepy.cc
 * \brief python enable bg_console
 */

#include "CommandProcessor.h"
#include "ConnectionMonitor.h"
#include "Options.h"

#include "command/MmcsServerCmd.h"

#include "common/ConsoleController.h"
#include "common/Properties.h"

#include "libmmcs_client/CommandReply.h"

#include <utility/include/Log.h>
#include <utility/include/Properties.h>
#include <utility/include/UserId.h>
#include <utility/include/version.h>

#include <boost/regex.hpp>
#include <boost/python.hpp>
#include <Python.h>

#include <boost/scoped_ptr.hpp>

#include <cstring>

#include <signal.h>
#include <stdlib.h>
#include <dlfcn.h>

LOG_DECLARE_FILE( "mmcs.console" );

using namespace std;

using mmcs::MMCSCommandMap;
using mmcs::MMCSCommandProcessor;

using mmcs::common::AbstractCommand;
using mmcs::common::ConsoleController;
using mmcs::common::Properties;

using mmcs::console::ConnectionMonitor;
using mmcs::console::Options;

using mmcs::console::command::MmcsServerCmd;

void reg_pyconsole ();
int     seteuid(uid_t __uid ) throw();
int     (*os_seteuid)(uid_t __uid );
typedef int     (*os_seteuid_fn)(uid_t __uid );

int     __setreuid(uid_t ruid, uid_t euid );
int     (*os_setreuid)(uid_t ruid, uid_t euid );
typedef int     (*os_setreuid_fn)(uid_t ruid, uid_t euid  );

// need to snarf setresuid also

//
// local statics to control access to the seteuid function
//
static bool ena_seteuid = false;

//std::string propfile = "";

void
signal_handler(int signum)
{
    if (signum == SIGUSR1) {
        return;
    }
}

/*!
// Class to override the MMCS Command reply to process the reply the way we want to here..
*/
class PyConsoleCommandReply : public mmcs_client::CommandReply
{
public:
    PyConsoleCommandReply() :
        mmcs_client::CommandReply(1, 0, false) {};

    std::string str(bool with_status=false);

#if 0
    virtual std::string str(bool with_status=false) { return str(with_status, _replyFormat); }
    virtual std::string str(bool with_status, unsigned replyFormat);
#endif
    /*
    ** sync() writes the buffer
    */
    virtual int sync();

protected:
    // overflow is called when the buffer needs to be written out
   virtual int overflow (int c = EOF);
   int push_buffered_data();
   string _overflowdata;
};

class BgConsole
{
public:
    BgConsole(const string& args = "");
    ~BgConsole() {};

    string cmd(const string& args);

protected:
    void connect();
    boost::scoped_ptr<mmcs::console::CommandProcessor> _commandProcessor;
    boost::scoped_ptr<ConsoleController> _pController;
    boost::scoped_ptr<Options> _options;

private:

};



int
PyConsoleCommandReply::push_buffered_data()
{
    int  len = pptr() - pbase(); // length of data in _outbuf
    if (len == 0)
        return(0);     // nothing to do here.

    _overflowdata.append(pbase(), len);        // append the overflow data...
    setp(pbase(), epptr());      // reset the pcurr() ptr...
    return(0);      // return success
}

/*!
//override so we can get the buffered string out of the reply
//
*/
std::string
PyConsoleCommandReply::str(bool with_status)
{

    int  len = pptr() - pbase(); // length of data in _outbuf
    string s(_overflowdata);
    if (len)
        s.append(pbase(), len);
    return (s);
}

/*!
// overflow is called when the buffer needs to be written out
*/
int
PyConsoleCommandReply::overflow (int c)
{
    push_buffered_data();
    if (c != EOF)
        return sputc(c);
    else
        return std::streambuf::traits_type::not_eof(c); // don't return eof, it indicates an error

}

int
PyConsoleCommandReply::sync()
{
    return(push_buffered_data());

}
#if 0
std::string str(bool with_status=false) { return str(with_status, _replyFormat); }
std::string str(bool with_status, unsigned replyFormat);
#endif

void dropeuid()
{
     ena_seteuid = true;
    // drop privileges after SSL private key and certificate have been read
    // so the editline history file can be read and written
    if ( getuid() != geteuid() ) {
        LOG_TRACE_MSG( "euid : " << geteuid() );
        LOG_TRACE_MSG( "uid  : " << getuid() );
        if ( seteuid(getuid()) ) {
            LOG_WARN_MSG( "Could not seteuid(" << getuid() << ") " << strerror(errno) );
        } else {
            LOG_TRACE_MSG( "Dropped euid to " << geteuid() );
        }
    } else {
        LOG_TRACE_MSG( "euid and uid match" );
    }
    ena_seteuid = false;

}
void raiseeuid()
{
    ena_seteuid = true;
    // get real, effective, and saved uid to determine if we need to raise effective
    // uid to read private key for SSL handshaking
    uid_t real( 0 );
    uid_t effective( 0 );
    uid_t saved( 0 );
    (void)getresuid( &real, &effective, &saved );
    LOG_TRACE_MSG( "real uid:       " << real );
    LOG_TRACE_MSG( "effective uid:  " << effective );
    LOG_TRACE_MSG( "saved uid:      " << saved );

    // raise priviliges to read private key
    if ( effective != saved ) {
        if ( seteuid(saved) ) {
            LOG_WARN_MSG( "Could not seteuid(" << saved << ") " << strerror(errno) );
        } else {
            LOG_TRACE_MSG( "Raised euid to " << geteuid() );
        }
    }
}

BgConsole::BgConsole(const string& args)
{
    raiseeuid();

    // tokenize the arguments, allowing for quoted strings.
    vector<string> arg_v;
    arg_v.push_back("pybg_console");       // app name is always the first parameter.
    if (args.size()) {
        boost::regex re("(\".*\")|(\'.*\')|([^\\s]+)");
        boost::sregex_iterator i(args.begin(), args.end(), re, boost::match_default);
        boost::sregex_iterator j;

        while (i != j) {
            arg_v.push_back(i->str());
            i++;
        }
    }

    // fill in a standard argv, argc list, so we can use the console parameter class
    unsigned argc = 0;
    char argv_buffer[args.size()+1+arg_v[0].size()];
    vector<char *>argv;

    argv_buffer[0] = 0;
    char *p = argv_buffer;

    for (unsigned n = 0; n < arg_v.size(); n++) {
        strcpy(p, arg_v[n].c_str());
        argv.push_back(p);
        p += strlen(p)+1;
        argc++;
    }

    //for (unsigned n = 0; n < argc; n++)
    //    cout << "argv[" << n << "] = " << argv[n] << endl;

    _options.reset(new Options( argc, &argv[0] ));

    //_pConsolePort.reset(new
    // give it the name of the app
    std::string basename  = "pybg_console";
    ostringstream version;
    version << bgq::utility::DriverName << " (revision " << bgq::utility::Revision << ")";
    version << " " << __DATE__ << " " << __TIME__;
    Properties::setProperty(MMCS_VERSION, version.str());
    Properties::setProperty(MMCS_PROCESS, basename);

    // create the list of bg_console commands
    AbstractCommand::Attributes attr;  attr.bgConsoleCommand(true); attr.externalCommand(true);
    AbstractCommand::Attributes mask;  mask.bgConsoleCommand(true); mask.externalCommand(true);
    MMCSCommandMap* mmcsCommands(
            MMCSCommandProcessor::createCommandMap(attr, mask, Properties::getExternalCmds())
            );

    // Create the bg_console command processor
    _commandProcessor.reset( new mmcs::console::CommandProcessor(mmcsCommands) );


    // Create the ConsoleController
    bgq::utility::UserId uid;
    _pController.reset(new ConsoleController( _commandProcessor.get(), uid ));
    _pController->setPortConfiguration( _options->getPortConfigurationPtr() );

    connect();
}


void BgConsole::connect()
{
    // connect to the mmcs server
    raiseeuid();
    LOG_INFO_MSG("Connecting to mmcs_server");
    deque<string> mmcs_connect = MMCSCommandProcessor::parseCommand("mmcs_server_connect");
    PyConsoleCommandReply reply;
    //mmcs_client::CommandReply reply(1, 0 /*_pController->getReplyFormat()*/, false);
    _commandProcessor->execute(mmcs_connect, reply, _pController.get());
    dropeuid();
    if (reply.getStatus() == 0) {
        LOG_INFO_MSG("Connected to mmcs_server");
    } else {
        // we are running disconnected from the server
        LOG_ERROR_MSG(reply.str());
        LOG_ERROR_MSG("The mmcs_server is down or still initializing, try again in a few minutes. If the problem persists, contact the system administrator.");
        cerr << "The mmcs_server is down or still initializing, try again in a few minutes. If the problem persists, contact the system administrator." << endl;
        throw runtime_error(reply.str());
    }
}

string BgConsole::cmd(const string& args)
{
    deque<string> mmcs_connect = MMCSCommandProcessor::parseCommand(args);
    PyConsoleCommandReply reply;
    ena_seteuid = true;
    _commandProcessor->execute(mmcs_connect, reply, _pController.get());
    ena_seteuid = false;
    if (reply.getStatus() != 0)
        throw runtime_error(reply.str());
    return(reply.str());
}

extern "C" void initpybg_console();

int
main(int argc, char *argv[])
{
    try {
        // install an interposer in the seteuid funciton.
        os_seteuid = (os_seteuid_fn)dlsym(RTLD_NEXT, "seteuid");
        os_setreuid = (os_setreuid_fn)dlsym(RTLD_NEXT, "__setreuid");

        // ignore SIGPIPE
        struct sigaction sa;
        memset( &sa, 0, sizeof(sa) );
        sa.sa_handler = SIG_IGN;
        if ( sigaction( SIGPIPE, &sa, NULL ) != 0 ) {
            LOG_WARN_MSG( "Could not ignore SIGPIPE" );
        }

        // install handler for SIGUSR1
        sa.sa_handler = &signal_handler;
        if ( sigaction( SIGUSR1, &sa, NULL ) != 0 ) {
            LOG_WARN_MSG( "Could not install SIGUSR1 handler" );
        }

        // any call after here to seteuid came from the python application
        // turn it off.
        dropeuid();

        Py_Initialize(); // python stuff

        //boost::python::detail::init_module((char*)"pybg_console", &init_module_pybg_console);
	    char module_name[] = "pybg_console";
        if (PyImport_AppendInittab(module_name, initpybg_console) == -1) {
            cerr << "PyImport_AppendInittab failed" << endl;
        }

        Py_Main(argc, argv);

        // loop reading and processing commands
        //pController->run();

        // cleanup and exit
        cerr  << "terminating, please wait..." << endl;

        exit( EXIT_SUCCESS );
    } catch ( const exception& e ) {
        cerr << e.what() << endl;
        exit( EXIT_FAILURE );
    }
}

int
seteuid(uid_t __uid ) throw()
{
    if (ena_seteuid)
        return((*os_seteuid)(__uid));
    else
        return(EPERM);

}

int
__setreuid(uid_t ruid, uid_t euid )
{
    if (ena_seteuid)
        return((*os_setreuid)(ruid, euid));
    else
        return(EPERM);

}

using namespace boost::python;
BOOST_PYTHON_MODULE(pybg_console)
{
    // non copyable, so we can have auto_ptr,s and such in the object...
    class_<BgConsole, boost::noncopyable>("BgConsole", init< optional<std::string> >())
        .def("cmd", &BgConsole::cmd)

        ;
}

