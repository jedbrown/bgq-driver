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

#ifndef _ARG_PARSE_H
#define _ARG_PARSE_H

#include <vector>
#include <map>
#include <utility/include/portConfiguration/ServerPortConfiguration.h>

//! \brief Small class for grabbing properties and parsing arguments
//! for BGmaster commands
class Args {

    //! \brief host/port pairs to connect
    bgq::utility::PortConfiguration::Pairs _portpairs;

    //! \brief map of other args
    std::map<std::string, std::string> _argpairs;
    std::vector<std::string> _otherargs;
    bool setupLogger(std::string& verbarg);
    void setupLoggerDefaults();
public:
    //! \brief BG properties file
    static bgq::utility::Properties::Ptr _props;

    //! \brief ctor
    //! \param argc standard c-style argument count
    //! \param argv standard c-style argument vector
    //! \param usage function pointer to usage function
    //! \param help function pointer to help function
    //! \param valargs vector of options to be followed with
    //  parameters.  Ex: "--foo bar".  "--foo" is in the valargs vector.
    //! \param singles Vector of arguments that don't take a parameter
    Args(unsigned int argc, const char** argv, void (*usage)(),
         void (*help)(), 
         std::vector<std::string>& valargs,
         std::vector<std::string>& singles,
         int default_port = 32042,  // Default server port for bgmaster_server.
         bool ignore_defaults = false);

    bgq::utility::Properties::Ptr get_props() { return _props; }
    bgq::utility::PortConfiguration::Pairs get_portpairs() { return _portpairs; }

    bool find_arg(std::string& arg) {
        if(std::find(_otherargs.begin(), _otherargs.end(), arg) != _otherargs.end())
            return true; else return false;
    }

    // Vector ops and iterators to get the "free form"
    // arguments out.
    typedef std::vector<std::string>::iterator iterator;
    typedef std::vector<std::string>::const_iterator const_iterator;
    size_t size() {
        return _otherargs.size(); }
    iterator begin() {
        return _otherargs.begin(); }
    iterator end() {
        return _otherargs.end(); }
    const_iterator begin() const {
        return _otherargs.begin(); }
    const_iterator end() const {
        return _otherargs.end(); }
    void clear() {
        _otherargs.clear(); }
    std::string operator[] (std::string& value) { return _argpairs[value]; }
};

#endif
