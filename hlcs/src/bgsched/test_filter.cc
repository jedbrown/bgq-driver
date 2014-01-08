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

#include <bgsched/BlockFilter.h>
#include <bgsched/JobFilter.h>

#include <bgsched/core/core.h>

#include <iostream>

using namespace bgsched;
using namespace std;

void
testJobFilter()
{
    // Create JobFilter
    JobFilter filter;

    // Create Statuses
    JobFilter::Statuses statuses;

    // Set job type filter
    cout << "Calling JobFilter::setJobType() with value 'All'" << endl;
    filter.setJobType(JobFilter::JobType::All);
    JobFilter::JobType::Value jobType = filter.getJobType();
    switch (jobType) {
        case JobFilter::JobType::Active:
            cout << "Job filter has job type of 'Active'" << endl;
            break;
        case JobFilter::JobType::Completed:
            cout << "Job filter has job type of 'Completed'" << endl;
            break;
        case JobFilter::JobType::All:
            cout << "Job filter has job type of 'All'" << endl;
            break;
    }

    // Set jobid filter
    cout << "Calling JobFilter::setJobId() with value 12345" << endl;
    filter.setJobId(12345);
    cout << "Job filter has job id of " << filter.getJobId() << endl;

    // Set scheduler data filter
    cout << "Calling JobFilter::setSchedulerData() with value IBM" << endl;
    filter.setSchedulerData("IBM");
    cout << "Job filter has scheduler data " << filter.getSchedulerData() << endl;

    // Set block name filter
    cout << "Calling JobFilter::setComputeBlockName() with value TESTNAME" << endl;
    filter.setComputeBlockName("TESTNAME");
    cout << "Job filter has block name " << filter.getComputeBlockName() << endl;

    // Set user filter
    cout << "Calling JobFilter::setUser() with value NICK" << endl;
    filter.setUser("NICK");
    cout << "Job filter has user " << filter.getUser() << endl;

    // Set executable filter
    cout << "Calling JobFilter::setExecutable() with value EXEC1" << endl;
    filter.setExecutable("EXEC1");
    cout << "Job filter has executable " << filter.getExecutable() << endl;

    // Exercise isExitStatusSet() method
    if (filter.isExitStatusSet()) {
        cout << "Exit status has been set" << endl;
    } else {
        cout << "Exit status has NOT been set" << endl;
        cout << "Job filter has ignored exit status of " << filter.getExitStatus() << endl;
    }

    // Set exit status filter (only valid for history jobs)
    cout << "Calling JobFilter::setExitStatus() with value 9" << endl;
    filter.setExitStatus(9);

    if (filter.isExitStatusSet()) {
        cout << "Exit status has been set" << endl;
        cout << "Job filter has exit status " << filter.getExitStatus() << endl;
    } else {
        cout << "Exit status has NOT been set" << endl;
        cout << "Job filter has ignored exit status of " << filter.getExitStatus() << endl;
    }

    // Set start time interval filter
    cout << "Calling JobFilter::setStartTimeInterval() with values 20110118T130000 and 20110119T153000" << endl;
    bgsched::TimeInterval timeInterval("20110118T130000", "20110119T153000");
    filter.setStartTimeInterval(timeInterval);
    bgsched::TimeInterval timeValue(filter.getStartTimeInterval());
    cout << "Job filter has start time interval value of start: " << boost::posix_time::to_simple_string(timeValue.getStart()) <<
    " end: " << boost::posix_time::to_simple_string(timeValue.getEnd()) << endl;

    // Set end time interval filter
    cout << "Calling JobFilter::setEndTimeInterval() with values 20110119T130000 and 20110120T153000" << endl;
    bgsched::TimeInterval timeInterval2("20110119T130000", "20110120T153000");
    filter.setEndTimeInterval(timeInterval2);
    bgsched::TimeInterval timeValue2(filter.getEndTimeInterval());
    cout << "Job filter has end time interval value of start: " << boost::posix_time::to_simple_string(timeValue2.getStart()) <<
    " end: " << boost::posix_time::to_simple_string(timeValue2.getEnd()) << endl;

    // Set statuses filter
    cout << "Calling JobFilter::setStatuses() with values Loading, Running, Debug and Terminated" << endl;
    statuses.insert(Job::Loading);
    statuses.insert(Job::Running);
    statuses.insert(Job::Debug);
    statuses.insert(Job::Terminated);
    filter.setStatuses(&statuses);

    // Get statuses filter
    cout << "Calling JobFilter::getStatuses()" << endl;
    JobFilter::Statuses jobStatuses = filter.getStatuses();

    if (jobStatuses.size() > 0) {
        if (jobStatuses.find(Job::Setup) != jobStatuses.end()) {
            cout << "Job filter has Setup status set" << endl;
        }
        if (jobStatuses.find(Job::Loading) != jobStatuses.end()) {
            cout << "Job filter has Loading status set" << endl;
        }
        if (jobStatuses.find(Job::Starting) != jobStatuses.end()) {
            cout << "Job filter has Starting status set" << endl;
        }
        if (jobStatuses.find(Job::Debug) != jobStatuses.end()) {
            cout << "Job filter has Debug status set" << endl;
        }
        if (jobStatuses.find(Job::Running) != jobStatuses.end()) {
            cout << "Job filter has Running status set" << endl;
        }
        if (jobStatuses.find(Job::Cleanup) != jobStatuses.end()) {
            cout << "Job filter has Cleanup status set" << endl;
        }
        if (jobStatuses.find(Job::Terminated) != jobStatuses.end()) {
            cout << "Job filter has Terminated status set" << endl;
        }
        if (jobStatuses.find(Job::Error) != jobStatuses.end()) {
            cout << "Job filter has Error status set" << endl;
        }
    } else {
        cout << "Test failed for JobFilter::getStatuses()" << endl;
    }

    // Clear out previous status values
    statuses.clear();

    // Set statuses filter
    cout << "Calling JobFilter::setStatuses() with values Setup, Starting, Error and Cleanup" << endl;
    statuses.insert(Job::Setup);
    statuses.insert(Job::Starting);
    statuses.insert(Job::Error);
    statuses.insert(Job::Cleanup);
    filter.setStatuses(&statuses);

    // Get statuses filter
    cout << "Calling JobFilter::getStatuses()" << endl;
    jobStatuses = filter.getStatuses();

    if (jobStatuses.size() > 0) {
        if (jobStatuses.find(Job::Setup) != jobStatuses.end()) {
            cout << "Job filter has Setup status set" << endl;
        }
        if (jobStatuses.find(Job::Loading) != jobStatuses.end()) {
            cout << "Job filter has Loading status set" << endl;
        }
        if (jobStatuses.find(Job::Starting) != jobStatuses.end()) {
            cout << "Job filter has Starting status set" << endl;
        }
        if (jobStatuses.find(Job::Debug) != jobStatuses.end()) {
            cout << "Job filter has Debug status set" << endl;
        }
        if (jobStatuses.find(Job::Running) != jobStatuses.end()) {
            cout << "Job filter has Running status set" << endl;
        }
        if (jobStatuses.find(Job::Cleanup) != jobStatuses.end()) {
            cout << "Job filter has Cleanup status set" << endl;
        }
        if (jobStatuses.find(Job::Terminated) != jobStatuses.end()) {
            cout << "Job filter has Terminated status set" << endl;
        }
        if (jobStatuses.find(Job::Error) != jobStatuses.end()) {
            cout << "Job filter has Error status set" << endl;
        }
    } else {
        cout << "Test failed for JobFilter::getStatuses()" << endl;
    }

    cout << "============= Done testing job filters ============" << endl;
}

void
testBlockFilter()
{
    // Create BlockFilter
    BlockFilter filter;

    // Create Statuses
    BlockFilter::Statuses statuses;

    // Set user filter
    cout << "Calling BlockFilter::setUser() with value NICK" << endl;
    filter.setUser("NICK");
    cout << "Block filter has user " << filter.getUser() << endl;

    // Set name filter
    cout << "Calling BlockFilter::setName() with value TESTNAME" << endl;
    filter.setName("TESTNAME");
    cout << "Block filter has name " << filter.getName() << endl;

    // Set owner filter
    cout << "Calling BlockFilter::setOwner() with value LAMONT" << endl;
    filter.setOwner("LAMONT");
    cout << "Block filter has owner " << filter.getOwner() << endl;

    // Set extended info filter
    cout << "Calling BlockFilter::setExtendedInfo() with value true" << endl;
    filter.setExtendedInfo(true);
    if (filter.getExtendedInfo()) {
        cout << "Block filter has extended info value of true" << endl;
    } else {
        cout << "Block filter has extended info value of false" << endl;
    }
    cout << "Calling BlockFilter::setExtendedInfo() with value false" << endl;
    filter.setExtendedInfo(false);
    if (filter.getExtendedInfo()) {
        cout << "Block filter has extended info value of true" << endl;
    } else {
        cout << "Block filter has extended info value of false" << endl;
    }

    // Set include jobs filter
    cout << "Calling BlockFilter::setIncludeJobs() with value true" << endl;
    filter.setIncludeJobs(true);
    if (filter.getIncludeJobs()) {
        cout << "Block filter has include jobs value of true" << endl;
    } else {
        cout << "Block filter has include jobs value of false" << endl;
    }
    cout << "Calling BlockFilter::setIncludeJobs() with value false" << endl;
    filter.setIncludeJobs(false);
    if (filter.getIncludeJobs()) {
        cout << "Block filter has include jobs value of true" << endl;
    } else {
        cout << "Block filter has include jobs value of false" << endl;
    }

    // Set size filter
    cout << "Calling BlockFilter::setSize() with value Large" << endl;
    filter.setSize(BlockFilter::BlockSize::Large);
    if (filter.getSize() == BlockFilter::BlockSize::Large) {
        cout << "Block filter has size Large" << endl;
    } else {
        if (filter.getSize() == BlockFilter::BlockSize::Small) {
            cout << "Block filter has size Small" << endl;
        } else {
            if (filter.getSize() == BlockFilter::BlockSize::All) {
                cout << "Block filter has size All" << endl;
            }
        }
    }
    cout << "Calling BlockFilter::setSize() with value Small" << endl;
    filter.setSize(BlockFilter::BlockSize::Small);
    if (filter.getSize() == BlockFilter::BlockSize::Large) {
        cout << "Block filter has size Large" << endl;
    } else {
        if (filter.getSize() == BlockFilter::BlockSize::Small) {
            cout << "Block filter has size Small" << endl;
        } else {
            if (filter.getSize() == BlockFilter::BlockSize::All) {
                cout << "Block filter has size All" << endl;
            }
        }
    }
    cout << "Calling BlockFilter::setSize() with value All" << endl;
    filter.setSize(BlockFilter::BlockSize::All);
    if (filter.getSize() == BlockFilter::BlockSize::Large) {
        cout << "Block filter has size Large" << endl;
    } else {
        if (filter.getSize() == BlockFilter::BlockSize::Small) {
            cout << "Block filter has size Small" << endl;
        } else {
            if (filter.getSize() == BlockFilter::BlockSize::All) {
                cout << "Block filter has size All" << endl;
            }
        }
    }

    // Set bloc statuses filter
    cout << "Calling BlockFilter::setStatuses() with values Free and Initialized" << endl;
    statuses.insert(Block::Free);
    statuses.insert(Block::Initialized);
    filter.setStatuses(&statuses);

    // Set statuses filter
    cout << "Calling BlockFilter::getStatuses()" << endl;
    BlockFilter::Statuses blockStatuses = filter.getStatuses();

    if (blockStatuses.size() > 0) {
        if (blockStatuses.find(Block::Free) != blockStatuses.end()) {
            cout << "Block filter has Free status set" << endl;
        }
        if (blockStatuses.find(Block::Allocated) != blockStatuses.end()) {
            cout << "Block filter has Allocated status set" << endl;
        }
        if (blockStatuses.find(Block::Booting) != blockStatuses.end()) {
            cout << "Block filter has Booting status set" << endl;
        }
        if (blockStatuses.find(Block::Initialized) != blockStatuses.end()) {
            cout << "Block filter has Initialized status set" << endl;
        }
        if (blockStatuses.find(Block::Terminating) != blockStatuses.end()) {
            cout << "Block filter has Terminating status set" << endl;
        }
    } else {
        cout << "Test failed for BlockFilter::getStatuses()" << endl;
    }

    // Clear out previous status values
    statuses.clear();
    // Set statuses filter
    cout << "Calling BlockFilter::setStatuses() with values Terminating, Booting and Allocated" << endl;
    statuses.insert(Block::Terminating);
    statuses.insert(Block::Allocated);
    statuses.insert(Block::Booting);
    filter.setStatuses(&statuses);

    // Get block statuses filter
    cout << "Calling BlockFilter::getStatuses()" << endl;
    blockStatuses = filter.getStatuses();

    if (blockStatuses.size() > 0) {
        if (blockStatuses.find(Block::Free) != blockStatuses.end()) {
            cout << "Block filter has Free status set" << endl;
        }
        if (blockStatuses.find(Block::Allocated) != blockStatuses.end()) {
            cout << "Block filter has Allocated status set" << endl;
        }
        if (blockStatuses.find(Block::Booting) != blockStatuses.end()) {
            cout << "Block filter has Booting status set" << endl;
        }
        if (blockStatuses.find(Block::Initialized) != blockStatuses.end()) {
            cout << "Block filter has Initialized status set" << endl;
        }
        if (blockStatuses.find(Block::Terminating) != blockStatuses.end()) {
            cout << "Block filter has Terminating status set" << endl;
        }
    } else {
        cout << "Test failed for BlockFilter::getStatuses()" << endl;
    }

    cout << "========= Done testing block filters ==========" << endl;
}

int
main()
{
    testJobFilter();
    cout << endl;
    cout << "===================================================================" << endl;
    cout << endl;
    testBlockFilter();
}

