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


#ifndef BGQ_UTILITY_PORT_CONFIGURATION_H_
#define BGQ_UTILITY_PORT_CONFIGURATION_H_

/*! \file
 *
 *  \brief Host and port configuration for clients and servers, base class.
 *
 */


#include <utility/include/Properties.h>

#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include <stdint.h>


namespace bgq {
namespace utility {


/*! \brief Abstract base class for host and port configuration for clients and servers.
 *
 *  See the concrete subclasses for more info.
 *
 */
class PortConfiguration
{
public:

    /*! \brief Exception thrown if a port string is not valid.
     *
     */
    class InvalidPortStr : public std::invalid_argument {
    public:
        InvalidPortStr( const std::string& what_str ) : std::invalid_argument( what_str )  {}
    };


    typedef std::pair<std::string,std::string> Pair; //!< A host-port pair, first is host name second is service name.

    typedef std::vector<Pair> Pairs; //!< Collection of Pair.

    typedef std::vector<std::string> Strings; //!< Collection of string.

    /*! \brief Parse a single host-port string.
     *
     *  A host-port string is formatted like <i>host</i>:<i>port</i>.
     *  Where:
     *  - <i>host</i> is a hostname or IP address.
     *  - <i>port</i> is a service name or port number.
     *  - host can be left off, where the default will be used. The subclass defines the default.
     *  - :port can be left off, indicating to use default_service_name.
     *
     *  \return The Pair representing the host:port.
     *
     *  \throws InvalidPortStr the port string is not valid.
     */
    static Pair parsePortStr(
            const std::string& port_str, //!< host:port string.
            const std::string& default_service_name //!< Service name to use if the port is left off.
        );

    /*! \brief Parse a comma-separated list of host-ports.
     *
     *  This function parses a string like <i>host-port</i>,<i>host-port</i>,...
     *
     *  See parsePortStr( const std::string&, const std::string& ) for
     *  the format of the <host-port> part.
     *
     *  \throws InvalidPortStr the port string is not valid.
     */
    static void parsePortsStr(
            const std::string& ports_str, //!< comma-separated list of host:port.
            const std::string& default_service_name, //!< Service name to use if the port is left off.
            Pairs& pairs_out //!< [out] pairs.
        );

    /*! \brief Parse a collection of comma-separated list of host-ports.
     *
     *  This is primary used to parse the command-line options.
     *
     *  See parsePortsStr(const std::string&,const std::string&,Pairs&) for the format of each element of ports_strs.
     *
     *  \throws InvalidPortStr the port string is not valid.
     */
    static void parsePortsStrs(
            const Strings& ports_strs, //!< Collection of comma-separated lists of host:port.
            const std::string& default_service_name, //!< Service name to use if the port is left off.
            Pairs& pairs_out //!< [out] pairs.
        );


    /*! \brief Create a port configuration.
     */
    explicit PortConfiguration(
            uint32_t default_tcp_port //!< The port that will be used if none is provided.
        );

    /*! \brief Create a port configuration.
     */
    explicit PortConfiguration(
            const std::string& default_service_name //!< The service name that will be used if none is provided.
        );


    /*! \brief Get the default service name */
    const std::string& getDefaultServiceName() const  { return _default_service_name; }

    /*! \brief Explicitly set the pairs.
     *
     * If pairs is empty, then the defaults will be used.
     */
    void setPairs( const Pairs& pairs )  { _pairs = pairs; }

    /*! \brief Set the ports using a ports string.
     *
     * The format of ports_str is described in parsePortsStr(const std::string&,const std::string&,Pairs& pairs_out).
     *
     * \throws InvalidPortStr the port string is not valid.
     */
    void setPorts( const std::string& ports_str );

    /*! \brief Set the ports using a collection of ports strings.
     *
     * The format of ports_str is described in parsePortsStr(const std::string&,const std::string&,Pairs& pairs_out).
     *
     * \throws InvalidPortStr the port string is not valid.
     */
    void setPorts( const Strings& ports_strs );

    /*! \brief Set the properties file.
     *
     * When notifyComplete() is called,
     * if the pairs have not been set
     * and section_name is not empty,
     * then the port configuration will be retrieved
     * from the given section of
     * this configuration file.
     *
     * The name of the property in the section is retrieved using
     * _getPropertyName().
     *
     * The SSL configuration will be read
     * from the given properties file.
     */
    void setProperties(
            Properties::ConstPtr properties_ptr,
            const std::string& section_name
        );

    /*! \brief Notify this class that configuration is complete.
     *
     * Call this after this class is configured (any setters have
     * been called) and before calling getPairs().
     *
     * \throws InvalidPortStr the port string is not valid.
     */
    void notifyComplete();

    /*! \brief Get the configured host-port pairs.
     *
     *  Call this only after notifyComplete() has been called.
     *
     */
    const Pairs& getPairs() const;

    const std::string& getAdministrativeCn() const  { return _administrative_cn; }
    const std::string& getCommandCn() const  { return _command_cn; }

    /*! \brief dtor.  */
    virtual ~PortConfiguration() { /* Nothing to do */ }


protected:

    /*! \brief Get the name of the property in the configuration file.
     *
     *  If this class needs to get the value from the property file
     *  it will call this to get the property name to use.
     *
     */
    virtual std::string _getPropertyName() const =0;

    /*! \brief Get the default value.
     *
     *  If this class needs to get the default value, it will
     *  call this.
     *
     */
    virtual Pairs _getDefault() const =0;


protected:

    std::string _default_service_name;

    Properties::ConstPtr _properties_ptr;
    std::string _section_name;

    Pairs _pairs;

    std::string _administrative_cn, _command_cn;


    void _handlePairsComplete();

};


} // namespace bgq::utility
} // namespace bgq


#endif
