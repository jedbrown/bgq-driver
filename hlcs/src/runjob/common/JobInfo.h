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
#ifndef RUNJOB_COMMON_JOB_INFO_H_
#define RUNJOB_COMMON_JOB_INFO_H_

#include "common/defaults.h"
#include "common/Environment.h"
#include "common/ExportedEnvironment.h"
#include "common/Mapping.h"
#include "common/PositiveInteger.h"
#include "common/RanksPerNode.h"
#include "common/Strace.h"
#include "common/SubBlock.h"

#include <utility/include/UserId.h>

#include <boost/serialization/access.hpp>
#include <boost/serialization/list.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/vector.hpp>

#include <string>
#include <vector>

#include <sys/types.h>

namespace runjob {

/*!
 * \brief describes a job
 */
class JobInfo
{
public:
    /*!
     * \brief Container of key value pairs
     */
    typedef std::vector<Environment> EnvironmentVector;

    /*!
     * \brief Np type.
     */
    typedef PositiveInteger<int32_t> Np;
   
    /*!
     * \brief Stdin rank type.
     */
    typedef PositiveIntegerWithZero<int32_t> StdinRank;

public:
    /*!
     * \brief ctor.
     */
    JobInfo();

    void setBlock(const std::string& b) { _block = b; } //!< Set the block name.
    void setCorner(const Corner& corner);   //!< Set the sub-block corner location.
    void setExe(const std::string& exe);    //!< Set the executable name.
    void setArgs(const std::vector<std::string>& args); //!< Set the executable arguments.
    void addAllEnvironment();   //!< Add all environment variables.
    void addExportedEnvironmentVector(const std::vector<ExportedEnvironment>& envs);    //!< Add exported environment variables.
    void addEnvironmentVector(const EnvironmentVector& envs);    //!< Add multiple environment variables.
    void setCwd(const std::string& cwd); //!< Set the current working directory.
    void setHostname(const std::string& hostname) { _hostname = hostname; } //!< Set the hostname.
    void setPid(pid_t pid) { _pid = pid; } //!< Set the pid.
    void setUserId(bgq::utility::UserId::Ptr uid) { _uid = uid; } //!< Set the UserId.
    void setRanksPerNode(const RanksPerNode& ranks); //!< Set the ranks per node.
    void setNp(const Np& np) { _np = np; } //!< Set the number of ranks in the job..
    void setMapping(const Mapping& mapping); //!< Set the mapping.
    void setSchedulerData(const std::string& data) { _schedulerData = data; } //!< Set the scheduler data.
    void setShape(const Shape& shape) { _subBlock.setShape(shape); }   //!< Set the sub-block shape.
    void setStrace(const Strace& strace) { _strace = strace; }  //!< Set strace.
    void setSubBlock(const SubBlock& sb) { _subBlock = sb; }  //!< Set sub block.
    void setStdinRank(const StdinRank& rank) { _stdinRank = rank; }   //!< Set stdin rank.

    const std::string& getBlock() const { return _block; } //!< Get the block name.
    const SubBlock& getSubBlock() const { return _subBlock; } //!< Get the sub-block location information.
    SubBlock& getSubBlock() { return _subBlock; } //!< Get the sub-block location information.
    const std::string& getExe() const { return _exe; }  //!< Get the executable.
    const std::vector<std::string>& getArgs() const { return _args; }   //!< Get the executable arguments.
    const EnvironmentVector& getEnvs() const { return _envs; }   //!< Get the environment variables.
    EnvironmentVector& getEnvs() { return _envs; }   //!< Get the environment variables.
    const std::string& getCwd() const { return _cwd; }  //!< Get the current working directory.
    const std::string& getHostname() const { return _hostname; }  //!< Get the hostname.
    pid_t getPid() const { return _pid; } //!< Get the pid.
    bgq::utility::UserId::Ptr getUserId() const { return _uid; } //!< Get the UserId.
    const RanksPerNode& getRanksPerNode() const { return _ranks; } //!< Get the ranks per node.
    const Np& getNp() const { return _np; }    //!< Get the number of ranks in the job.
    const Mapping& getMapping() const { return _mapping; }    //!< Get the mapping.
    const std::string& getSchedulerData() const { return _schedulerData; }  //!< Get the scheduler data.
    const Strace& getStrace() const { return _strace; } //!< Get strace.
    size_t getStdinRank() const { return _stdinRank; }  //!< Get stdin rank.
    mode_t getUmask() const { return _umask; } //!< Get umask.

private:
    void __attribute__ ((visibility("hidden"))) addEnvironment(
            const Environment& env
            );
    void __attribute__ ((visibility("hidden"))) validateArgumentSize() const;
    void __attribute__ ((visibility("hidden"))) validateMappingPath();

    friend class boost::serialization::access;
    template<class Archive>
    void __attribute__ ((visibility("hidden"))) serialize(
            Archive &ar,                //!< [in] archive
            const unsigned int          //!< [in] version number
            )
    {
        ar & _block;
        ar & _subBlock;
        ar & _exe;
        ar & _args;
        ar & _envs;
        ar & _cwd;
        ar & _hostname;
        ar & _pid;
        ar & _uid;
        ar & _ranks;
        ar & _np;
        ar & _mapping;
        ar & _schedulerData;
        ar & _strace;
        ar & _stdinRank;
        ar & _umask;
    }

private:
    std::string _block;                 //!<
    SubBlock _subBlock;                 //!<
    std::string _exe;                   //!<
    std::vector<std::string> _args;     //!<
    EnvironmentVector _envs;            //!<
    std::string _cwd;                   //!<
    std::string _hostname;              //!< host where job was launched
    pid_t _pid;                         //!< pid of runjob process
    bgq::utility::UserId::Ptr _uid;     //!< credentials of user
    RanksPerNode _ranks;                //!< ranks per node
    Np _np;                             //!< number of ranks in the job
    Mapping _mapping;                   //!< ABCDET permutation mapping or path to mapping file
    std::string _schedulerData;         //!< arbitrary data from a job scheduler.
    Strace _strace;                     //!< system call tracing
    uint32_t _stdinRank;                //!<
    mode_t _umask;                      //!<
};

} // runjob

namespace boost {
namespace serialization {

// need to split bgq::utility::UserId into an explicit load and store so we can invoke placement new
// using a customer constructor. The default ctor can throw if the getuid() result is not found, whic
// isn't something that can be handled (or should be) since the uid, name, and groups will be replaced
// with whatever is deserialized anyhow

template <class Archive>
void serialize(
        Archive& /* ar */,
        bgq::utility::UserId& /* uid */,
        const unsigned int /* version */
        )
{

}

template <class Archive>
void 
save_construct_data(
        Archive & ar, 
        const bgq::utility::UserId* userid,
        const unsigned int /* version */
        )
{
    // save data required to construct instance
    ar & userid->getUser();
    const uid_t uid = userid->getUid();
    ar & uid;
    ar & userid->getGroups();
}

template <class Archive>
void
load_construct_data(
        Archive & ar, 
        bgq::utility::UserId* userid,
        const unsigned int /* version */
        )
{
    // retrieve data from archive required to construct new instance
    std::string user;
    ar & user;
    uid_t uid;
    ar & uid;
    bgq::utility::UserId::GroupList groups; 
    ar & groups;

    // invoke placement new for UserId object
    // see http://www.parashift.com/c++-faq-lite/dtors.html#faq-11.10
    (void)::new(userid) bgq::utility::UserId( "nobody", true );
    userid->setUser( user );
    userid->setUid( uid );
    userid->setGroups( groups );
}

} // serialization
} // boost


#endif
