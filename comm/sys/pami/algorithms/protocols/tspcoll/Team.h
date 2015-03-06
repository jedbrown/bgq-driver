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
 * \file algorithms/protocols/tspcoll/Team.h
 * \brief ???
 */
#ifndef __algorithms_protocols_tspcoll_Team_h__
#define __algorithms_protocols_tspcoll_Team_h__

#include "stdio.h"
#include "algorithms/protocols/tspcoll/Collective.h"


/* ************************************************************************* */
/* ************************************************************************* */

namespace xlpgas
{
#if 0
  /*
   * Used to store the global picture of local/global teams and their
   * associated leaders; stored in the team and filled in during sub
   * topology build
   */
  struct xlpgas_local_info {
    int node;
    int ctxt;
    int leader;
  };

  template <class T_NI>
  class Team
  {
  public:
    static const int MAX_COMMS = 1024;

    static void Initialize (int ncontexts);
    static Team * get (int ctxt, int teamID);
    static void set (int ctxt, int teamID, Team *);
    int allocate_team_id () { return ++_max_team_id;}

  public:
    int                      commID     (void)        const { return _id; }
    virtual int              ordinal    (void)        const;
    virtual int              size       (void)        const;
    virtual xlpgas_endpoint_t endpoint   (void)       const;//current endpoint
    virtual xlpgas_endpoint_t endpoint   (int ordinal) const;
    Collective<T_NI>             * coll       (CollectiveKind) const;
    Team(){}
    Team (int commID, xlpgas_endpoint_t me, xlpgas_endpoint_t size);
    void * operator new (size_t, void * addr) { return addr; }
    void set_leader_team_id(int _l){_leaders_team_id=_l;}
    void set_local_team_id(int _l) {_local_team_id=_l;}
    int leader_team_id(void) const { return _leaders_team_id;}
    int local_team_id(void) const  { return _local_team_id;}
    virtual bool is_leader(void) const { return true;}//to be overloaded in the derived class
    virtual bool contains_endp(xlpgas_endpoint_t&) const { return true;};//to be overloaded in non global teams 
    virtual int ordinal(xlpgas_endpoint_t&) const;
    int leader(int root) const;//find the leader of the input arg 
                               //(ordinal of an endpoint of the team)
    /* ---------------------- */
    /* communicator utilities */
    /* ---------------------- */

    virtual void split(int ctxt, int newID, int mynewrank, int nsize, xlpgas_endpoint_t *plist);
    virtual void set_sub_topologies(int){}

  protected:  
    int                _id; //team id
    xlpgas_endpoint_t  _me;
    xlpgas_endpoint_t  _size;
    Collective<T_NI>      * _colls[MAXKIND];

    static Team *     _instances[MAX_COMMS];
    static int        _ncontexts;
    int               _max_team_id;
    int               _leaders_team_id;
    int               _local_team_id;
    xlpgas_local_info* _all_info;
  };
#endif



  /*
  template <class Tm>
  void create_teams(Tm*& inst, int& _nctxt, int ncontexts){
    assert (ncontexts > 0);
    _nctxt = ncontexts;

    //inst = (Team *) __global.heap_mm->malloc (sizeof(Team) *
    //		    ncontexts * Tm::MAX_COMMS);
    //assert (inst != NULL);

    for (int i=0; i<ncontexts; i++)
      {
	xlpgas_endpoint_t me    = { XLPGAS_MYNODE, i };
	xlpgas_endpoint_t size  = { XLPGAS_NODES, ncontexts };
	Tm* nt = (Tm*) __global.heap_mm->malloc (sizeof(Tm));
	new (nt) Tm (0, me, size);
	inst
      }
  }//end create teams
  */

}//end namespace

#endif
