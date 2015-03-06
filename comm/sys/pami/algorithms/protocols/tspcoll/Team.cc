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
 * \file algorithms/protocols/tspcoll/Team.cc
 * \brief ???
 */
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "algorithms/protocols/tspcoll/Collective.h"
#include "algorithms/protocols/tspcoll/Team.h"
#include "algorithms/protocols/tspcoll/Collectives.h"
#include "algorithms/protocols/tspcoll/SplitTeam.h"

#undef TRACE
//#define TRACE(x) fprintf x
#define TRACE(x)

/* ************************************************************************ */
/* ************************************************************************ */

xlpgas::Team * xlpgas::Team::_instances[MAX_COMMS];
int            xlpgas::Team::_ncontexts;
/* ************************************************************************ */
/* ************************************************************************ */

void xlpgas::Team::Initialize(int ncontexts)
{
  //create_teams<Team>(_instances, _ncontexts, ncontexts);
  assert (ncontexts > 0);
  _ncontexts = ncontexts;
  for (int i=0; i<ncontexts; i++)
    {
      xlpgas_endpoint_t me    = { XLPGAS_MYNODE, i };
      xlpgas_endpoint_t size  = { XLPGAS_NODES, ncontexts };
      Team* nt = (Team*) __global.heap_mm->malloc (sizeof(Team));
      new (nt) Team (0, me, size);
      _instances[i]=nt;
    }
}

/* ************************************************************************ */
/* ************************************************************************ */

xlpgas::Team * xlpgas::Team::get (int ctxt, int teamID)
{
  assert(_ncontexts * teamID + ctxt < MAX_COMMS);
  return _instances[_ncontexts * teamID + ctxt];
}

void xlpgas::Team::set (int ctxt, int teamID, xlpgas::Team * t)
{
  assert(_ncontexts * teamID + ctxt < MAX_COMMS);
  _instances[_ncontexts * teamID + ctxt] = t;
}
/* ************************************************************************ */
/*                  communicator constructor                                */
/* ************************************************************************ */
xlpgas::Team::Team (int commID, xlpgas_endpoint_t me, xlpgas_endpoint_t size)
{
  _max_team_id=0;
  _id = commID;
  _me = me;
  _size  = size;
  for (int i=0; i<MAXKIND; i++)
    _colls[i] = CollectiveManager::
      instance(me.ctxt)->allocate (this, (CollectiveKind)i, base_coll_defs());
}

int xlpgas::Team::size () const
{
  return _size.node * _size.ctxt;
}

int xlpgas::Team::ordinal () const
{
  return _me.node * _size.ctxt + _me.ctxt;
}

int xlpgas::Team::ordinal (xlpgas_endpoint_t& _ep) const
{
  return _ep.node * _size.ctxt + _ep.ctxt;
}


xlpgas_endpoint_t xlpgas::Team::endpoint (void) const
{
  return _me;
}

xlpgas_endpoint_t xlpgas::Team::endpoint (int ordinal) const
{
  xlpgas_endpoint_t ep = { ordinal / _size.ctxt, ordinal % _size.ctxt };
  return ep;
}

xlpgas::Collective * xlpgas::Team::coll (CollectiveKind kind) const
{
  assert (kind < MAXKIND);
  return _colls[(int)kind];
}

void xlpgas::Team::split(int ctxt, int newTeamID, int mynewrank, int nsize, xlpgas_endpoint_t * plist)
{
  xlpgas::Team * c = NULL;
  c = (xlpgas::Team *) __global.heap_mm->malloc (sizeof(xlpgas::EnumTeam<Team>));
  if (!c) xlpgas_fatalerror (-1, "%s: __global.heap_mm->malloc error", __FUNCTION__);
  new (c) xlpgas::EnumTeam<Team> (newTeamID, this->_me, mynewrank, nsize, plist);
  xlpgas::Team::set(ctxt, newTeamID, c);
}

int xlpgas::Team::leader(int root) const {
  assert(_all_info != NULL);
  xlpgas_endpoint_t root_ep = endpoint(root);
  for(int i=0;i<size();++i){
    if(_all_info[i].node == root_ep.node &&
       _all_info[i].ctxt == root_ep.ctxt) 
      return _all_info[i].leader;
  }
  assert(false);//fail if not in the table
  return -1;
}

/* ************************************************************************ */
/* ************************************************************************ */
#define COURSEOF(place) ((place)/(_BF*_ncomms))
#define COMMOF(place) (((place)/_BF)%_ncomms)
#define VIRTOF(place) (((place)%_BF)+(_BF*COURSEOF(place)))

/*
xlpgas::BC_Comm::BC_Comm (int commID, int BF, int ncomms):
  Team(commID), _BF(BF), _ncomms(ncomms)
{
  _rank         = VIRTOF(XLPGAS_MYPLACE);
  _mycomm       = COMMOF(XLPGAS_MYPLACE);
  int c = COMMOF(XLPGAS_PLACES);
  if (c < _mycomm)
    {
      _size = _BF * (COURSEOF(XLPGAS_PLACES));
    }
  else if (c == _mycomm)
    {
      _size = _BF * COURSEOF(XLPGAS_PLACES) + (XLPGAS_PLACES%_BF);
    }
  else
    {
      _size = _BF * (COURSEOF(XLPGAS_PLACES)+1);
    }
}
*/

/* ************************************************************************ */
/*    absolute rank corresponding to virtual rank in *my* communicator      */
/* ************************************************************************ */
/*  rank/BF == block corresponding to rank                                  */
/*  (rank/BF) * BF * ncomms = course                                        */
/* ************************************************************************ */

// int xlpgas::BC_Comm::absrankof (int rank) const
// {

//   return
//     (rank/_BF)*_BF*_ncomms +    /* current course of rank               */
//     _mycomm * _BF +             /* block in course of *my* communicator */
//     (rank%_BF);                 /* rank's phase in block                */
// }

/* ************************************************************************ */
/*    virtual rank of a particular absolute rank                            */
/* ************************************************************************ */
  /*
int xlpgas::BC_Comm::virtrankof (int rank) const
{
  if (COMMOF(rank)==_mycomm) return VIRTOF(rank);
  return -1;
}
  */
