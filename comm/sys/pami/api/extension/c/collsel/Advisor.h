/* begin_generated_IBM_copyright_prolog                             */
/*                                                                  */
/* This is an automatically generated copyright prolog.             */
/* After initializing,  DO NOT MODIFY OR MOVE                       */
/*  --------------------------------------------------------------- */
/* Licensed Materials - Property of IBM                             */
/* Blue Gene/Q 5765-PER 5765-PRP                                    */
/*                                                                  */
/* (C) Copyright IBM Corp. 2011, 2012 All Rights Reserved           */
/* US Government Users Restricted Rights -                          */
/* Use, duplication, or disclosure restricted                       */
/* by GSA ADP Schedule Contract with IBM Corp.                      */
/*                                                                  */
/*  --------------------------------------------------------------- */
/*                                                                  */
/* end_generated_IBM_copyright_prolog                               */
/**
 * \file api/extension/c/collsel/Advisor.h
 */
#ifndef __api_extension_c_collsel_Advisor_h__
#define __api_extension_c_collsel_Advisor_h__

#include "api/extension/c/collsel/CollselExtension.h"

namespace PAMI{
class Advisor
{
public:
  Advisor(pami_client_t            client,
          advisor_configuration_t  configuration[],
          size_t                   num_configs,
          pami_context_t           contexts[],
          size_t                   num_contexts);
  ~Advisor();
  size_t getNumContext();
  friend class AdvisorTable;
private:
  pami_client_t            _client;
  advisor_configuration_t *_configuration;
  size_t                   _num_configs;
  pami_context_t          *_contexts;
  size_t                   _num_contexts;

};


inline Advisor::Advisor(pami_client_t            client,
                        advisor_configuration_t  configuration[],
                        size_t                   num_configs,
                        pami_context_t           contexts[],
                        size_t                   num_contexts):
  _client(client),
  _configuration(configuration),
  _num_configs(num_configs),
  _contexts(contexts),
  _num_contexts(num_contexts)
{
  
  
}

inline Advisor::~Advisor()
{
  
  
}

inline size_t Advisor::getNumContext(){
  return _num_contexts;
}

} // Nnamespace pami



#endif // __api_extension_c_collsel_Extension_h__
