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

#ifndef _BINARYCONTROLLERBASE_H
#define _BINARYCONTROLLERBASE_H

#include <tr1/memory>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread.hpp>
#include <string>
#include "Ids.h"
#include "Policy.h"
#include "Host.h"

class BinaryController;

typedef boost::shared_ptr<BinaryController> BinaryControllerPtr;

//! \brief Controls a managed executable, base class
class BinaryController {
public:

    enum Status { UNINITIALIZED, RUNNING, COMPLETED };

    static std::string status_to_string(Status s) {
        std::string retstr = "";
        switch(s) {
        case UNINITIALIZED:
            retstr = "UNINITIALIZED";
            break;
        case RUNNING:
            retstr = "RUNNING";
            break;
        case COMPLETED:
            retstr = "COMPLETED";
            break;
        }
        return retstr;
    }

    static Status string_to_status(std::string& statstr) {
        Status retstat = UNINITIALIZED;
        if(statstr == "RUNNING") {
            return RUNNING;
        } else if(statstr == "COMPLETED") {
            return COMPLETED;
        }
        return retstat;
    }

    //! \brief Condition variable to notify threads of changes in status.
    boost::condition_variable _status_notifier;
    //! \brief Lock for the status condition variable.
    boost::mutex _status_lock;

    //! \brief Default constructor
    BinaryController() : _status(UNINITIALIZED), _exit_status(0), _binary_bin_path(""),
                             _alias_name(""), _stop_requested(false) {
        _start_time = boost::posix_time::second_clock::local_time();
        _user = "";
    }

    ~BinaryController() {  }

    //! \brief copy constructor
    BinaryController(const BinaryController& base) :
        _status(base._status), _binary_bin_path(base._binary_bin_path), _alias_name(base._alias_name) {
        _binid = base._binid;
        _stop_requested = base._stop_requested;
        _start_time = base._start_time;
        _user = base._user;
        if(_user == "UNINITIALIZED") abort();
    }

    //! \brief construct with a binary id and a status
    BinaryController(BinaryId id, std::string& bin_path, std::string& alias,
                     std::string& user, int exit_status = 0, Status stat = UNINITIALIZED,
                     std::string start_time = "") :
        _status(stat), _exit_status(exit_status), _binary_bin_path(bin_path), _alias_name(alias), _host(), _binid(id), _user(user) {
        _stop_requested = false;
        if(start_time.length() == 0)
            _start_time = boost::posix_time::second_clock::local_time();
        else
            _start_time = boost::posix_time::time_from_string(start_time);
        _stop_requested = false;
        if(_user == "UNINITIALIZED") abort();
    }

    BinaryController(std::string& path, std::string& arguments, std::string& logfile,
                     std::string& alias, Host host, std::string& user) {
        _binary_bin_path = path + " " + arguments;
        _logfile = logfile;
        _alias_name = alias;
        _host = host;
        _start_time = boost::posix_time::second_clock::local_time();
        _stop_requested = false;
        _user = user;
        _exit_status = 0;
        if(_user == "UNINITIALIZED") abort();
    }

    //! \brief construct with an id, full 'bin_path', user, exit status and status
    BinaryController(std::string& id, std::string& bin_path, std::string& alias,
                     std::string& user, int exit_status, int stat, std::string start_time) :
        _binary_bin_path(bin_path), _alias_name(alias), _user(user) {

        _exit_status = exit_status;
        _status = (Status)stat;
        _binid = id;
        _stop_requested = false;
        _start_time = boost::posix_time::time_from_string(start_time);
        if(_user == "UNINITIALIZED") abort();
    }

    //! \brief assignment operator
    BinaryController& operator=(const BinaryController& base) {
        _status = base._status;
        _exit_status = base._exit_status;
        _binary_bin_path = base._binary_bin_path;
        _alias_name = base._alias_name;
        _logfile = base._logfile;
        _binid = base._binid;
        _stop_requested = base._stop_requested;
        _start_time = base._start_time;
        _user = base._user;
        if(_user == "UNINITIALIZED") abort();
        return *this;
    }


    //! \brief start this binary
    //! \param user id of the user who should run this
    //! \returns ID for started binary
    BinaryId startBinary();

    //! \brief stop this binary
    //! \returns true if successful, false if not
    int stopBinary(int signal);

    //! \brief returns binary's running status
    BinaryController::Status binaryStatus();

    //! \brief utility functions
    const std::string& get_binary_bin_path() const { return _binary_bin_path; }
    const std::string& get_alias_name() const { return _alias_name; }
    const std::string& get_user() const { return _user; }
    const std::string& get_error_text() const { return _exec_error; }
    int get_exit_status() { return _exit_status; }
    void set_exit_status(int exit_status) { _exit_status = exit_status; }
    Status get_status() const { return _status; }
    void set_status(Status s) { 
        boost::lock_guard<boost::mutex> lg(_status_lock);
        _status = s; 
        _status_notifier.notify_all();
    }
    BinaryId get_binid() { return _binid; }
    boost::posix_time::ptime get_start_time() { return _start_time; }
    bool valid() {
        bool ret;
        (_status == UNINITIALIZED) ? ret = false : ret = true;
        return ret;
    }

    // \brief setter/getter for stop flag.
    bool stopping(bool stop = false) {
        if(stop == true) _stop_requested = true; return _stop_requested;
    }
protected:

    //! \brief monitor tid
    pthread_t _monitor_tid;

    //! \brief running status
    Status _status;

    //! \brief exit status
    int _exit_status;

    //! \brief The bin_path is the path and arguments for the binary
    std::string _binary_bin_path;

    //! \brief The alias name associated with the binary
    std::string _alias_name;

    //! \brief full path to the logfile
    std::string _logfile;

    //! \brief Host we're on.
    Host _host;

    //! \brief Unique identifier for the binary.  Host/pid.
    BinaryId _binid;

    //! \brief flag to indicate that we have specifically
    //  asked this binary to die.
    bool _stop_requested;

    boost::posix_time::ptime _start_time;

    //! \brief user id under which we started the binary
    std::string _user;

    //! \brief error message returned on exec failures
    std::string _exec_error;
};

inline std::ostream& operator <<(std::ostream& os, const BinaryControllerPtr& b) { 
    BinaryControllerPtr bp = b;
    os << bp->get_binid().str() << "|" << bp->get_alias_name() << "|"
       << bp->BinaryController::status_to_string(bp->get_status()) << "|"
       << bp->get_user();
    return os;
}

#endif
