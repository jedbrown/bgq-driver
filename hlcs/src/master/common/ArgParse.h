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

#ifndef MASTER_ARG_PARSE_H_
#define MASTER_ARG_PARSE_H_


#include <utility/include/Properties.h>

#include <utility/include/portConfiguration/PortConfiguration.h>

#include <map>
#include <string>
#include <vector>


//! \brief Small class for grabbing properties and parsing arguments
//! for BGmaster commands
class Args
{
    //! \brief host/port pairs to connect
    bgq::utility::PortConfiguration::Pairs _portpairs;

    //! \brief map of other args
    std::map<std::string, std::string> _argpairs;
    std::vector<std::string> _otherargs;
    bool setupLogger(const std::string& verbarg) const;
    void setupLoggerDefaults() const;
    bgq::utility::Properties::ConstPtr _props;

public:
    /*!
     * \brief ctor
     *
     * - A single "*" entry in valargs indicates the option is positional.
     * - A single "**" entry in valargs indicates unlimited positional options.
     */
    Args(
            const int argc,                         //!< [in] standard c-style argument count
            const char** argv,                      //!< [in] standard c-style argument vector
            void (*usage)(),                        //!< [in] function pointer to usage function
            void (*help)(),                         //!< [in] function pointer to help function
            std::vector<std::string>& valargs,      //!< [in] options to be followed with parameters
            const std::vector<std::string>& singles //!< [in] arguments that don't take a parameter
        );

    const bgq::utility::Properties::ConstPtr& get_props() const { return _props; }
    const bgq::utility::PortConfiguration::Pairs& get_portpairs() const { return _portpairs; }

    bool find_arg(const std::string& arg) const {
        return std::find(_otherargs.begin(), _otherargs.end(), arg) != _otherargs.end();
    }

    // Vector ops and iterators to get the "free form"
    // arguments out.
    typedef std::vector<std::string>::iterator iterator;
    typedef std::vector<std::string>::const_iterator const_iterator;
    size_t size() const {
        return _otherargs.size(); }
    iterator begin() {
        return _otherargs.begin(); }
    iterator end() {
        return _otherargs.end(); }
    const_iterator begin() const {
        return _otherargs.begin(); }
    const_iterator end() const {
        return _otherargs.end(); }
    std::string operator[](const std::string& value) const;
};

#endif
