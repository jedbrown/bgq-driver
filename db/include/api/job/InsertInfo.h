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

#ifndef BGQDB_JOB_INSERT_INFO
#define BGQDB_JOB_INSERT_INFO

#include "Shape.h"

#include <string>
#include <vector>

#include <stdint.h>

#include <sys/types.h>

namespace BGQDB {
namespace job {

/*! \brief Arguments to BGQDB::job::insert(). */
class InsertInfo {
public:

    typedef std::vector<std::string> Args;
    typedef std::vector<std::string> Envs;
    typedef std::vector<std::string> NodeBoardPositions;


    /*! \brief Generates a string that can be inserted to the DB from the arguments. */
    static std::string format( const std::vector<std::string>& args_or_envs );


    InsertInfo();

    /*! \brief Block must match an existing block ID otherwise insert will fail. */
    void setBlock( const std::string& block )  { _block = block; }

    /*! \brief Defaults to empty string. */
    void setExe( const std::string& exe )  { _exe = exe; }

    /*! \brief Defaults to empty args. */
    void setArgs( const Args& args )  { _args = args; }

    /*! \brief Defaults to empty envs. */
    void setEnvs( const Envs& envs )  { _envs = envs; }

    /*! \brief Defaults to empty string. */
    void setCwd( const std::string& cwd )  { _cwd = cwd; }

    /*! \brief Defaults to empty string. */
    void setMapping( const std::string& mapping ) { _mapping = mapping; }

    /*! \brief Defaults to empty user name. */
    void setUserName( const std::string& user_name )  { _user_name = user_name; }

    /*! \brief Defaults to 0. */
    void setProcessesPerNode( uint16_t processes_per_node )  { _processes_per_node = processes_per_node; }

    /*! \brief Defaults to Shape::Block (use the shape of the block). */
    void setShape( const Shape& shape )  { _shape = shape; }

    /*! \brief Defaults to empty string. */
    void setSchedulerData( const std::string& scheduler_data )  { _scheduler_data = scheduler_data; }

    /*! \brief Set only if sub-block job, to node or core if single-core job.
     *
     *  Defaults to empty string (not a sub-block job).
     *  If no corner (corner is empty string) then CORNER will be NULL.
     */
    void setCorner( const std::string& corner )  { _corner = corner; }

    /*! \brief Defaults to no node board positions, set if sub-block job and not an entire midplane. */
    void setNodeBoardPositions( const NodeBoardPositions& node_board_positions )  { _node_board_positions = node_board_positions; }

    /*! \brief Defaults to empty string. */
    void setHostname( const std::string& hostname )  { _hostname = hostname; }

    /*! \brief Defaults to -1. */
    void setPid( pid_t pid )  { _pid = pid; }

    /*! \brief Defaults to 0. */
    void setNp( unsigned np )  { _np = np; }

    /*! \brief Defaults to 0. */ 
    void setClient( unsigned client )  { _client = client; }

    const std::string& getBlock() const  { return _block; }
    const std::string& getExe() const  { return _exe; }
    const Args& getArgs() const  { return _args; }
    const Envs& getEnvs() const  { return _envs; }
    const std::string& getCwd() const  { return _cwd; }
    const std::string& getUserName() const  { return _user_name; }
    const std::string& getMapping() const  { return _mapping; }
    uint16_t getProcessesPerNode() const  { return _processes_per_node; }
    const Shape& getShape() const  { return _shape; }
    const std::string& getSchedulerData() const  { return _scheduler_data; }

    const std::string& getCorner() const  { return _corner; }
    const NodeBoardPositions& getNodeBoardPositions() const  { return _node_board_positions; }

    const std::string& getHostname() const  { return _hostname; }
    pid_t getPid() const  { return _pid; }

    unsigned getNp() const  { return _np; }
    unsigned getClient() const  { return _client; }


private:

    std::string _block;
    std::string _exe;
    Args _args;
    Envs _envs;
    std::string _cwd;
    std::string _user_name;
    std::string _mapping;
    uint16_t _processes_per_node;
    Shape _shape;
    std::string _scheduler_data;

    std::string _corner;
    NodeBoardPositions _node_board_positions;

    std::string _hostname;
    pid_t _pid;
    unsigned _np;
    unsigned _client;
};


} } // namespace BGQDB::job

#endif
