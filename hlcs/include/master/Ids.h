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

#include "Host.h"

#ifndef _IDS_H
#define _IDS_H

//! \brief base class for ID representations
class Id {
protected:
    //! \brief The ONLY data member
    //! Everything else is sugar
    std::string _id;

    //! \brief Construction without a string not allowed
    Id();
public:
    //! \brief Copy constructor
    Id(Id& id);
    Id(const std::string& id_string);

    //! \brief conversion operator
    operator std::string() { return _id; }
    Id& operator=(const Id& id);
    Id& operator=(const std::string& id) { _id = id; return *this; }
    bool operator==(const Id& id) const;
    bool operator<(const Id& id) const;

    //! Return a Host object from the ID
    Host get_host();

    //! \brief Return the raw string representation
    std::string str() const { return _id; }

    //! \brief comparison operator for Id map
    struct Comp {
        bool operator()(const Id& lhs, const Id& rhs);
    };
};

//! \brief Representation of the universal identifier for a bgagent
//! A BGAgentId is a unique identifier for a bgagentd consisting
//! of its hostname and listening port in the format
//! host:port
class BGAgentId : public Id {
public:
    BGAgentId() : Id("") {}
    BGAgentId(const std::string& id_string);
    BGAgentId(const BGAgentId& id);
    BGAgentId(const int port, const std::string& host);
    BGAgentId(const std::string& port, const std::string& host);

    //! \brief Return the port part of the BGAgentId
    int get_port();
};

//! \brief Representation of the universal identifier for a binary
//! A BinaryId is a unique identifier for a managed binary
//! consisting of its hostname and process id in the format
//! host:pid
class BinaryId : public Id {
public:
    BinaryId() : Id("") {}
    BinaryId(const std::string& id_string);
    BinaryId(const BinaryId& id);
    BinaryId(const int pid, const std::string& host);
    BinaryId(const std::string& pid, const std::string& host);

    //! \brief Return the process ID of the binary
    int get_pid();
};

//! \brief Representation of the universal identifier for a client
//! A ClientId is a unique identifier for a client consisting
//! of its hostname and local port.  (There is no listener on a client connection.)
//! host:port
class ClientId : public Id {
public:
    ClientId() {}
    ClientId(const std::string& id_string);
    ClientId(const ClientId& id);
    ClientId(const int port, const std::string& host);
    ClientId(const std::string& port, const std::string& host);

    //! \brief Return the port part of the ClientId
    int get_port();
};

#endif
