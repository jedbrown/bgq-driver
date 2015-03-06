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
 * \file algorithms/protocols/tspcoll/SplitTeam.h
 * \brief ???
 */
#ifndef __algorithms_protocols_tspcoll_SplitTeam_h__
#define __algorithms_protocols_tspcoll_SplitTeam_h__

#include <Team.h>
#include <algorithms/protocols/tspcoll/Collectives.h>

namespace xlpgas{
  /* ******************************************************************* */
  /*                      Enumerated Team                                */
  /* ******************************************************************* */
  template <class Base>
  class EnumTeam: public Base
  {
  public:
    void * operator new (size_t, void * addr) { return addr; }

    EnumTeam (int commID, xlpgas_endpoint_t my_ep, int mynewrank, int nsize, xlpgas_endpoint_t* proclist) {
      //assert (size > 0);
      _ordinal_rank = mynewrank;
      _proclist = proclist; //the endpoints corresponding to this team;

      //what used to be in the base constructor
      this->_id = commID;
      this->_me = my_ep;
      this->_size.node = nsize;
      for (int i=0; i<MAXKIND; i++){
	//printf("L%d[%x] allocate coll %d for TID=%d\n",XLPGAS_MYNODE,this,i,this->_id);
	this->_colls[i] = CollectiveManager::
	  instance(this->_me.ctxt)->allocate (this, (CollectiveKind)i, base_coll_defs());
      }
    }

    int size () const {
      return _size.node;
    }

    int ordinal () const {
      return this->_ordinal_rank;
    }

    xlpgas_endpoint_t endpoint (int ordinal) const {
      return _proclist[ordinal];
    }

    bool is_leader() const {
      return (_ordinal_rank != -1);
    }

    bool contains_endp(xlpgas_endpoint_t& _ep) const {
      //printf("L%d looking for [%d,%d]\n",XLPGAS_MYTHREAD, _ep.node, _ep.ctxt); 
      for (int i=0;i<this->size();++i){
	//printf("L%d contains at %d =[%d,%d]\n", XLPGAS_MYTHREAD, i, _proclist[i].node, _proclist[i].ctxt);
	if(_proclist[i].node == _ep.node &&
	   _proclist[i].ctxt == _ep.ctxt) return true;
      }
      return false;
    }

    int ordinal (xlpgas_endpoint_t& _ep) const
    {
      for (int i=0;i<this->size();++i){
	if(_proclist[i].node == _ep.node &&
	   _proclist[i].ctxt == _ep.ctxt) return i;
      }
      return -1;
    }

  protected:  
    xlpgas_endpoint_t * _proclist; /* list of absolute (node) ranks */
    int _ordinal_rank;//stored; not computed as it may not be a formula anymore
  };


  /* ******************************************************************* */
  /*                      Leaders Team - UPC Shared Memory mode          */
  /* ******************************************************************* */
  template <class Base>
  class SHMLeadersTeam: public Base
  {
  public:
    void * operator new (size_t, void * addr) { return addr; }

    SHMLeadersTeam (int commID,int ctxt,bool leader_or_not) {
      //what used to be in the base constructor
      this->_id = commID;
      xlpgas_endpoint_t ep;
      ep.node = XLPGAS_MYNODE;
      ep.ctxt = ctxt;
      this->_me = ep;
      this->_size.node = XLPGAS_NODES;
      this->_leader = leader_or_not;
      for (int i=0; i<MAXKIND; i++)
	this->_colls[i] = CollectiveManager::
	  instance(this->_me.ctxt)->allocate (this, (CollectiveKind)i, base_coll_defs());
    }

    int size () const {
      return XLPGAS_NODES; 
    }

    int ordinal () const {
      return XLPGAS_MYNODE;
    }

    xlpgas_endpoint_t endpoint (int ordinal) const {
      xlpgas_endpoint_t ep;
      ep.node = ordinal;
      ep.ctxt = 0;
      return ep;
    }

    //GT: to be implemented
    bool is_leader() const {
      return _leader;
    }
  private:
    bool _leader;
  };

  template <class Base>
  class SHMLocalTeam: public Base
  {
  public:
    void * operator new (size_t, void * addr) { return addr; }

    SHMLocalTeam (int commID, int me) {
      //what used to be in the base constructor
      this->_id = commID;
      xlpgas_endpoint_t ep;
      ep.node = XLPGAS_MYNODE;
      ep.ctxt = me;
      this->_me = ep;
      this->_size.node = XLPGAS_SMPTHREADS;
      for (int i=0; i<MAXKIND; i++)
	this->_colls[i] = CollectiveManager::
	  instance(this->_me.ctxt)->allocate (this, (CollectiveKind)i, base_coll_defs());
    }

    int size () const {
      return XLPGAS_SMPTHREADS; 
    }

    int ordinal () const {
      return XLPGAS_MYSMPTHREAD;
    }

    xlpgas_endpoint_t endpoint (int ordinal) const {
      xlpgas_endpoint_t ep;
      ep.node = XLPGAS_MYNODE;
      ep.ctxt = ordinal;
      return ep;
    }
  };



  /* ******************************************************************* */
  /*                     blocked communicator                            */
  /* ******************************************************************* */
  /*
  class BC_Comm: public Team
  {
  public:
    void * operator new (size_t, void * addr) { return addr; }
    BC_Comm (int commID, int BF, int ncomms);

    virtual int  absrankof  (int rank) const;
    virtual int  virtrankof (int rank) const;

  protected:
    int _BF, _ncomms, _mycomm;
  };
  */
}//end namespace

#endif
