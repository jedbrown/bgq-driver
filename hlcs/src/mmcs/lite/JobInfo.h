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

#ifndef MMCS_LITE_JOB_INFO_H
#define MMCS_LITE_JOB_INFO_H

#include <string>
#include <vector>

#include <stdint.h>


namespace mmcs {
namespace lite {


/*!
 * \brief describes a job
 */
class JobInfo
{
public:
    /*!
     * \brief ctor.
     */
    JobInfo() :
        _block(),
        _id( 0 ),
        _ip(),
        _np( 0 ),
        _ranks( 1 ),
        _exe(),
        _args(),
        _envs(),
        _cwd(),
        _timeout( 0 ),
        _transitionTimeout( 60 ),
        _label( false ),
        _inline( false )
    {

    }

    // setters
    void setBlock( const std::string& b )           { _block = b; }
    void setId( const uint32_t id )                 { _id = id; }
    void setIp( const std::string& ip )             { _ip = ip; }
    void setExe( const std::string& exe )           { _exe = exe; }
    void addArg( const std::string& arg )           { _args.push_back(arg); }
    void addEnv( const std::string& env )           { _envs.push_back(env); }
    void setCwd( const std::string& cwd )           { _cwd = cwd; }
    void setNp( const uint32_t ranks )              { _np = ranks; }
    void setRanksPerNode( const uint32_t ranks )    { _ranks = ranks; }
    void setTimeout( const unsigned t )             { _timeout = t; }
    void setTransitionTimeout( const unsigned t )   { _transitionTimeout = t; }
    void setLabel( bool l = true )                  { _label = l; }
    void setInline( bool i = true )                 { _inline = i; }

    // getters
    const std::string& getBlock() const { return _block; }
    const uint32_t getId() const { return _id; }
    const std::string& getIp() const { return _ip; }
    const std::string& getExe() const { return _exe; }
    const std::vector<std::string>& getArgs() const { return _args; }
    const std::vector<std::string>& getEnvs() const { return _envs; }
    const std::string& getCwd() const { return _cwd; }
    const uint32_t getNp() const { return _np; }
    const uint32_t getRanksPerNode() const { return _ranks; }
    const unsigned getTimeout() const { return _timeout; }
    const unsigned getTransitionTimeout() const { return _transitionTimeout; }
    const bool getLabel() const { return _label; }
    const bool getInline() const { return _inline; }

private:
    std::string _block;                 //!< block name
    uint32_t _id;                       //!< job ID
    std::string _ip;                    //!< I/O node IP address
    uint32_t _np;                       //!< number of ranks per job
    uint32_t _ranks;                    //!< number of ranks per node
    std::string _exe;                   //!<
    std::vector<std::string> _args;     //!<
    std::vector<std::string> _envs;     //!<
    std::string _cwd;                   //!<
    unsigned _timeout;                  //!<
    unsigned _transitionTimeout;        //!<
    bool _label;                        //!<
    bool _inline;                       //!<
};

} } // namespace mmcs::lite

#endif

