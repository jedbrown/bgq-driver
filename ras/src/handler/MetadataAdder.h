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
/* (C) Copyright IBM Corp.  2007, 2011                              */
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
#ifndef METADATAADDER_H_
#define METADATAADDER_H_

#include "RasEventHandler.h"

class MetadataAdder : public RasEventHandler
{
public:
    virtual RasEvent& handle(RasEvent& event, const RasEventMetadata& metadata);
    virtual const std::string& name() { return _name; }
    MetadataAdder();
    virtual ~MetadataAdder();
 private: 
    const std::string _name;
};

#endif /*METADATAADDER_H_*/
