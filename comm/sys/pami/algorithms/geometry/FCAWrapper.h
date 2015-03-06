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
 * \file algorithms/geometry/FCAWrapper.h
 * \brief ???
 */

#ifndef __algorithms_geometry_FCAWrapper_h__
#define __algorithms_geometry_FCAWrapper_h__

#include "algorithms/protocols/CollectiveProtocolFactory.h"
#include "algorithms/composite/Composite.h"
#include "algorithms/geometry/Metadata.h"
#include "components/devices/fca/fcafunc.h"
#include "lapi_itrace.h"

namespace PAMI{namespace CollRegistration{
// The following classes are used to implement a CCMI wrapper
// around the FCA RT classes
// --------------  FCA Wrapper base class -------------
template <class T_Composite>
class FCAFactory;
class GeometryInfo
{
public:
  GeometryInfo(void *reg):
    _registration(reg),
    _fca_comm(NULL),
    _amRoot(false),
    _ep(-1)
    {
    }
  void            *_registration;
  fca_comm_t      *_fca_comm;
  fca_comm_desc_t  _fca_comm_desc;
  bool             _amRoot;
  pami_endpoint_t  _ep;
};

template <class T_Geometry>
class FCAComposite:public CCMI::Executor::Composite
{
  typedef FCAFactory<FCAComposite> Factory;
public:
  typedef T_Geometry GeometryType;
  FCAComposite(T_Geometry *g, Factory *f):
    _g(g),
    _f(f)
    {
    }
  virtual void start()                  = 0;
  inline  void setComm(fca_comm_t *c)
    {
      _c = c;
    }
  inline  void setEP(pami_endpoint_t ep)
    {
      _my_endpoint = ep;
    }
  inline void executeDoneCallback()
    {
      if(this->_cb_done)
        this->_cb_done(this->_context, this->_clientdata, PAMI_SUCCESS);
    }
protected:
  T_Geometry        *_g;
  Factory           *_f;
  fca_comm_t        *_c;
  pami_endpoint_t    _my_endpoint;
};


// --------------  FCA Factory base class -------------
template <class T_Composite>
class FCAFactory : public CCMI::Adaptor::CollectiveProtocolFactory
{
typedef typename T_Composite::GeometryType T_Geometry;
public:
  FCAFactory(pami_context_t           ctxt,
             size_t                   ctxt_id,
             pami_mapidtogeometry_fn  cb_geometry):
    CollectiveProtocolFactory(ctxt,ctxt_id,cb_geometry)
  {
  }
  virtual CCMI::Executor::Composite * generate(pami_geometry_t  geometry,
                                               void            *cmd)
  {
    T_Composite  *c    = (T_Composite*)_alloc.allocateObject();
    pami_xfer_t  *xfer = (pami_xfer_t *)cmd;
    T_Geometry   *g    = (T_Geometry*)geometry;
    GeometryInfo *gi   = (GeometryInfo*)(g->getKey(this->_context_id,
                                                   Geometry::CKEY_FCAGEOMETRYINFO));
    new(c)T_Composite((T_Geometry*)g,this);
    c->setDoneCallback(xfer->cb_done, xfer->cookie);
    c->setContext(_context);
    c->setComm(gi->_fca_comm);
    c->setEP(gi->_ep);
    c->setxfer(xfer);
    return c;
  }
  virtual void metadata(pami_metadata_t *mdata, pami_geometry_t geometry)
  {
    T_Geometry   *g  = (T_Geometry*)geometry;
    GeometryInfo *gi = (GeometryInfo*)(g->getKey(this->_context_id,
                                                 Geometry::CKEY_FCAGEOMETRYINFO));
    fca_comm_caps_t comm_caps;
    int rc = FCA_Comm_get_caps(gi->_fca_comm, &comm_caps); (void) rc;
    PAMI_assertf(rc == 0, "FCA_Comm_get_caps failed with rc=%d [%s]",
            rc,
            FCA_Strerror(rc));

    T_Composite::metadata(mdata);
    mdata->check_perf.values.hw_accel     = 1;
    mdata->range_lo                       = 0;
    mdata->range_hi                       = comm_caps.max_payload;
    mdata->range_lo_perf                  = 0;
    mdata->range_hi_perf                  = comm_caps.max_payload;

  }
  virtual void returnComposite(T_Composite *composite)
    {
      _alloc.returnObject(composite);
    }
  PAMI::MemoryAllocator<sizeof(T_Composite),16>  _alloc;
  const char                                    *_string;
};

const fca_reduce_op_t    _fca_reduce_op_tbl[PAMI_OP_COUNT] =
  {
    FCA_OP_LAST                /*PAMI_COPY (invalid FCA operation) */,
    FCA_OP_LAST                /*PAMI_NOOP (invalid FCA operation) */,
    FCA_OP_MAX                 /*PAMI_MAX*/,
    FCA_OP_MIN                 /*PAMI_MIN*/,
    FCA_OP_SUM                 /*PAMI_SUM*/,
    FCA_OP_PROD                /*PAMI_PROD*/,
    FCA_OP_LAND                /*PAMI_LAND*/,
    FCA_OP_LOR                 /*PAMI_LOR*/,
    FCA_OP_LXOR                /*PAMI_LXOR*/,
    FCA_OP_BAND                /*PAMI_BAND*/,
    FCA_OP_BOR                 /*PAMI_BOR*/,
    FCA_OP_BXOR                /*PAMI_BXOR*/,
    FCA_OP_MAXLOC              /*PAMI_MAXLOC*/,
    FCA_OP_MINLOC              /*PAMI_MINLOC*/
  };

const fca_reduce_dtype_t _fca_reduce_dtype_tbl[PAMI_DT_COUNT] = 
  {
    FCA_DTYPE_UNSIGNED_CHAR    /*PAMI_BYTE  According to standard, MPI_BYTE is similar to MPI_UNSIGNED_CHAR*/,
    FCA_DTYPE_CHAR             /*PAMI_SIGNED_CHAR*/,
    FCA_DTYPE_SHORT            /*PAMI_SIGNED_SHORT*/,
    FCA_DTYPE_INT              /*PAMI_SIGNED_INT*/,
    FCA_DTYPE_LONG             /*PAMI_SIGNED_LONG*/,
    FCA_DTYPE_LAST             /*PAMI_SIGNED_LONG_LONG (invalid FCA type)*/,

    FCA_DTYPE_UNSIGNED_CHAR    /*PAMI_UNSIGNED_CHAR*/,
    FCA_DTYPE_UNSIGNED_SHORT   /*PAMI_UNSIGNED_SHORT*/,
    FCA_DTYPE_UNSIGNED         /*PAMI_UNSIGNED_INT*/,
    FCA_DTYPE_UNSIGNED_LONG    /*PAMI_UNSIGNED_LONG*/,
    FCA_DTYPE_LAST             /*PAMI_UNSIGNED_LONG_LONG (invalid FCA type)*/,

    FCA_DTYPE_FLOAT            /*PAMI_FLOAT*/,
    FCA_DTYPE_DOUBLE           /*PAMI_DOUBLE*/,
    FCA_DTYPE_LAST             /*PAMI_LONG_DOUBLE (invalid FCA type)*/,

    FCA_DTYPE_LAST             /*PAMI_LOGICAL1 (invalid FCA type)*/,
    FCA_DTYPE_LAST             /*PAMI_LOGICAL2 (invalid FCA type)*/,
    FCA_DTYPE_LAST             /*PAMI_LOGICAL4 (invalid FCA type)*/,
    FCA_DTYPE_LAST             /*PAMI_LOGICAL8 (invalid FCA type)*/,

    FCA_DTYPE_LAST             /*PAMI_SINGLE_COMPLEX (invalid FCA type)*/,
    FCA_DTYPE_LAST             /*PAMI_DOUBLE_COMPLEX (invalid FCA type)*/,

    FCA_DTYPE_2INT             /*PAMI_LOC_2INT*/,
    FCA_DTYPE_LAST             /*PAMI_LOC_2FLOAT (invalid FCA type)*/,
    FCA_DTYPE_LAST             /*PAMI_LOC_2DOUBLE (invalid FCA type)*/,
    FCA_DTYPE_SHORT_INT        /*PAMI_LOC_SHORT_INT*/,
    FCA_DTYPE_FLOAT_INT        /*PAMI_LOC_FLOAT_INT*/,
    FCA_DTYPE_DOUBLE_INT       /*PAMI_LOC_DOUBLE_INT*/,
    FCA_DTYPE_LONG_INT         /*PAMI_LOC_LONG_INT (invalid FCA type ??)*/,
    FCA_DTYPE_LAST             /*PAMI_LOC_LONGDOUBLE_INT (invalid FCA type ??)*/
  };


static inline fca_reduce_dtype_t p_dtype_to_fca_dtype(pami_dt dt)
{
  PAMI_assert(dt < PAMI_DT_COUNT);
  fca_reduce_dtype_t fca_dt = _fca_reduce_dtype_tbl[dt];
  return fca_dt;
}
static inline fca_reduce_op_t p_func_to_fca_op(pami_op op)
{
  PAMI_assert(op < PAMI_OP_COUNT);
  fca_reduce_op_t fca_op = _fca_reduce_op_tbl[op];
  return fca_op;
}


template <class T_reduce_type>
inline metadata_result_t op_dt_metadata_function(struct pami_xfer_t *in)
{
  T_reduce_type     *reduction = (T_reduce_type*)&in->cmd;
  metadata_result_t  result    = {0};
  uintptr_t          op;
  uintptr_t          dt;
  PAMI::Type::TypeFunc::GetEnums(reduction->stype,
                                 reduction->op,
                                 dt,
                                 op);
  fca_reduce_dtype_t fca_dt = p_dtype_to_fca_dtype((pami_dt)dt);
  fca_reduce_op_t    fca_op = p_func_to_fca_op((pami_op)op);
  bool is_invalid = true;
  if(fca_op != FCA_OP_LAST && fca_dt != FCA_DTYPE_LAST)   is_invalid=false;

  // Replace this with a table for faster lookup
  if(fca_op == FCA_OP_LAND && fca_dt == FCA_DTYPE_FLOAT)  is_invalid=true;
  if(fca_op == FCA_OP_LOR && fca_dt == FCA_DTYPE_FLOAT)   is_invalid=true;
  if(fca_op == FCA_OP_LXOR && fca_dt == FCA_DTYPE_FLOAT)  is_invalid=true;
  if(fca_op == FCA_OP_LAND && fca_dt == FCA_DTYPE_DOUBLE) is_invalid=true;
  if(fca_op == FCA_OP_LOR && fca_dt == FCA_DTYPE_DOUBLE)  is_invalid=true;
  if(fca_op == FCA_OP_LXOR && fca_dt == FCA_DTYPE_DOUBLE) is_invalid=true;

  if(fca_op == FCA_OP_BAND && fca_dt == FCA_DTYPE_FLOAT)  is_invalid=true;
  if(fca_op == FCA_OP_BOR && fca_dt == FCA_DTYPE_FLOAT)   is_invalid=true;
  if(fca_op == FCA_OP_BXOR && fca_dt == FCA_DTYPE_FLOAT)  is_invalid=true;
  if(fca_op == FCA_OP_BAND && fca_dt == FCA_DTYPE_DOUBLE) is_invalid=true;
  if(fca_op == FCA_OP_BOR && fca_dt == FCA_DTYPE_DOUBLE)  is_invalid=true;
  if(fca_op == FCA_OP_BXOR && fca_dt == FCA_DTYPE_DOUBLE) is_invalid=true;

  result.check.datatype_op = is_invalid;
  return(result);
}
// TODO:  convert endpoint based roots to TASKS
// TODO:  figure out progress function details
// --------------  FCA Reduce wrapper classes -------------
template <class T_Geometry>
class FCAReduceExec:public FCAComposite<T_Geometry>
{
  typedef PAMI::Type::TypeCode      Type;
  typedef FCAComposite<T_Geometry>  FCAComp;
  typedef FCAFactory<FCAReduceExec> Factory;
  typedef FCAFactory<FCAComp>       FactoryP;
public:
  FCAReduceExec(T_Geometry *g, Factory *f):
    FCAComp(g,(FactoryP*)f)
  {
  }
  virtual void start()
  {
    ITRC(IT_FCA, 
         "FCA_reduce: root %d sbuf 0x%p rbuf 0x%p dtype %d len %d op %d\n",
         _spec.root,
         _spec.sbuf,
         _spec.rbuf,
         _spec.dtype,
         _spec.length,
         _spec.op);
    int rc = FCA_Do_reduce(this->_c, &_spec);(void)rc;
    PAMI_assertf(rc == 0, "FCA_Do_reduce failed with rc=%d [%s]",
            rc,
            FCA_Strerror(rc));

    this->executeDoneCallback();
    this->_f->returnComposite(this);
  }
  static inline void metadata(pami_metadata_t *m)
    {
      new(m) PAMI::Geometry::Metadata(myName());
      m->check_correct.values.alldtop   = 0;
      m->check_fn                       = op_dt_metadata_function<pami_reduce_t>;
    }
  static inline const char *myName()
    {
      return "I1:Reduce:FCA:FCA";
    }
  inline void setxfer(pami_xfer_t *xfer)
    {
      pami_reduce_t *cmd = &(xfer->cmd.xfer_reduce);
      uintptr_t      dt,op;
      if(cmd->sndbuf == PAMI_IN_PLACE)
        PAMI::Type::TypeFunc::GetEnums(cmd->rtype,cmd->op,
                                     dt,op);
      else
        PAMI::Type::TypeFunc::GetEnums(cmd->stype,cmd->op,
                                     dt,op);
      //SSS: FCA expects the root to be the index of the task (endpoint) in the topology.
      //     W/o this conversion subgeometries can crash or produce erroneous results.
      Topology *topo = (Topology *) (this->_g->getTopology(PAMI::Geometry::DEFAULT_TOPOLOGY_INDEX));
      _spec.root     = topo->endpoint2Index(cmd->root);
      _spec.sbuf     = (cmd->sndbuf == PAMI_IN_PLACE)?cmd->rcvbuf:cmd->sndbuf;
      _spec.rbuf     = cmd->rcvbuf;
      _spec.dtype    = p_dtype_to_fca_dtype((pami_dt)dt);
      _spec.length   = (cmd->sndbuf == PAMI_IN_PLACE)?cmd->rtypecount:cmd->stypecount;
      _spec.op       = p_func_to_fca_op((pami_op)op);
    }
private:
  fca_reduce_spec_t  _spec;
};

// --------------  FCA Allreduce wrapper classes -------------
template <class T_Geometry>
class FCAAllreduceExec:public FCAComposite<T_Geometry>
{
public:
  typedef PAMI::Type::TypeCode         Type;
  typedef FCAComposite<T_Geometry>     FCAComp;
  typedef FCAFactory<FCAAllreduceExec> Factory;
  typedef FCAFactory<FCAComp>          FactoryP;
  FCAAllreduceExec(T_Geometry *g, Factory *f):
    FCAComp(g,(FactoryP*)f)
  {
  }
  virtual void start()
  {
    ITRC(IT_FCA, 
         "FCA_allreduce: root %d sbuf 0x%p rbuf 0x%p dtype %d len %d op %d\n",
         _spec.root,
         _spec.sbuf,
         _spec.rbuf,
         _spec.dtype,
         _spec.length,
         _spec.op);
    int rc = FCA_Do_all_reduce(this->_c, &_spec);(void)rc;
    PAMI_assertf(rc == 0, "FCA_Do_all_reduce failed with rc=%d [%s]",
            rc,
            FCA_Strerror(rc));
    this->executeDoneCallback();
    this->_f->returnComposite(this);
  }
  static inline void metadata(pami_metadata_t *m)
    {
      new(m) PAMI::Geometry::Metadata(myName());
      m->check_correct.values.alldtop   = 0;
      m->check_fn                       = op_dt_metadata_function<pami_allreduce_t>;
    }
  static inline const char *myName()
    {
      return "I1:Allreduce:FCA:FCA";
    }
  inline void setxfer(pami_xfer_t *xfer)
    {
      pami_allreduce_t *cmd = &(xfer->cmd.xfer_allreduce);
      uintptr_t         dt,op;
      if(cmd->sndbuf == PAMI_IN_PLACE)
        PAMI::Type::TypeFunc::GetEnums(cmd->rtype,cmd->op,
                                     dt,op);
      else
        PAMI::Type::TypeFunc::GetEnums(cmd->stype,cmd->op,
                                     dt,op);
      _spec.root     = -1; // will be ignored in FCA
      _spec.sbuf     = (cmd->sndbuf == PAMI_IN_PLACE)?cmd->rcvbuf:cmd->sndbuf;
      _spec.rbuf     = cmd->rcvbuf;
      _spec.dtype    = p_dtype_to_fca_dtype((pami_dt)dt);
      _spec.length   = (cmd->sndbuf == PAMI_IN_PLACE)?cmd->rtypecount:cmd->stypecount;
      _spec.op       = p_func_to_fca_op((pami_op)op);
    }
private:
  fca_reduce_spec_t  _spec;
};

// --------------  FCA Broadcast wrapper classes -------------
template <class T_Geometry>
class FCABroadcastExec:public FCAComposite<T_Geometry>
{
public:
  typedef PAMI::Type::TypeCode         Type;
  typedef FCAComposite<T_Geometry>     FCAComp;
  typedef FCAFactory<FCABroadcastExec> Factory;
  typedef FCAFactory<FCAComp>          FactoryP;
  FCABroadcastExec(T_Geometry *g, Factory *f):
    FCAComp(g,(FactoryP*)f)
    {
    }
  virtual void start()
  {
    ITRC(IT_FCA, 
         "FCA_bcast: root %d buf 0x%p size %d\n",
         _spec.root,
         _spec.buf,
         _spec.size);
    int rc = FCA_Do_bcast(this->_c, &_spec);(void)rc;
    PAMI_assertf(rc == 0, "FCA_Do_bcast failed with rc=%d [%s]",
            rc,
            FCA_Strerror(rc));

    this->executeDoneCallback();
    this->_f->returnComposite(this);
  }
  static inline void metadata(pami_metadata_t *m)
    {
      new(m) PAMI::Geometry::Metadata(myName());
    }
  static inline const char *myName()
    {
      return "I1:Broadcast:FCA:FCA";
    }
  inline void setxfer(pami_xfer_t *xfer)
  {
    pami_broadcast_t *cmd = &(xfer->cmd.xfer_broadcast);
    //SSS: FCA expects the root to be the index of the task (endpoint) in the topology.
    //     W/o this conversion subgeometries can crash or produce erroneous results.
    Topology        *topo = (Topology *) (this->_g->getTopology(PAMI::Geometry::DEFAULT_TOPOLOGY_INDEX));
    _spec.root            = topo->endpoint2Index(cmd->root);
    _spec.buf             = cmd->buf;
    _spec.size            = cmd->typecount * ((Type*)cmd->type)->GetExtent();
  }
private:
  fca_bcast_spec_t _spec;
};

// --------------  FCA Allgather wrapper classes -------------
template <class T_Geometry>
class FCAAllgatherExec:public FCAComposite<T_Geometry>
{
public:
  typedef PAMI::Type::TypeCode         Type;
  typedef FCAComposite<T_Geometry>     FCAComp;
  typedef FCAFactory<FCAAllgatherExec> Factory;
  typedef FCAFactory<FCAComp>           FactoryP;
  FCAAllgatherExec(T_Geometry *g, Factory *f):
    FCAComp(g,(FactoryP*)f)
    {
    }
  virtual void start()
  {
    ITRC(IT_FCA, 
         "FCA_allgather: root %d sbuf 0x%p rbuf 0x%p size %d\n",
         _spec.root,
         _spec.sbuf,
         _spec.rbuf,
         _spec.size);
    int rc = FCA_Do_allgather(this->_c, &_spec);(void)rc;
    PAMI_assertf(rc == 0, "FCA_Do_allgather failed with rc=%d [%s]",
            rc,
            FCA_Strerror(rc));

    this->executeDoneCallback();
    this->_f->returnComposite(this);
  }
  static inline void metadata(pami_metadata_t *m)
    {
      new(m) PAMI::Geometry::Metadata(myName());
    }
  static inline const char *myName()
    {
      return "I1:Allgather:FCA:FCA";
    }
  inline void setxfer(pami_xfer_t *xfer)
    {
      PAMI_assert_alwaysf(this->_my_endpoint != (unsigned)-1 ,"Invalid endpoint I1:Allgather:FCA:FCA");
      pami_allgather_t *cmd = &(xfer->cmd.xfer_allgather);
      size_t        myindex = this->_g->ordinal(this->_my_endpoint);
      _spec.root            = -1;  // will be ignored in FCA
      _spec.sbuf            = (cmd->sndbuf == PAMI_IN_PLACE)?
                              (char*)cmd->rcvbuf + myindex * cmd->rtypecount * ((Type*)cmd->rtype)->GetExtent() : 
                              cmd->sndbuf;
      _spec.rbuf            = cmd->rcvbuf;
      _spec.size            = (cmd->sndbuf == PAMI_IN_PLACE)?
                              cmd->rtypecount * ((Type*)cmd->rtype)->GetExtent():
                              cmd->stypecount * ((Type*)cmd->stype)->GetExtent();
    }
private:
  fca_gather_spec_t _spec;
};

// --------------  FCA Allgatherv_int wrapper classes -------------
template <class T_Geometry>
class FCAAllgathervIntExec:public FCAComposite<T_Geometry>
{
public:
  typedef PAMI::Type::TypeCode             Type;
  typedef FCAComposite<T_Geometry>         FCAComp;
  typedef FCAFactory<FCAAllgathervIntExec> Factory;
  typedef FCAFactory<FCAComp>              FactoryP;
  FCAAllgathervIntExec(T_Geometry *g, Factory *f):
    FCAComp(g,(FactoryP*)f)
    {
    }
  virtual void start()
  {
    ITRC(IT_FCA, 
         "FCA_allgatherv: root %d sbuf 0x%p rbuf 0x%p sendsize %d\n",
         _spec.root,
         _spec.sbuf,
         _spec.rbuf,
         _spec.sendsize);
    int rc = FCA_Do_allgatherv(this->_c, &_spec); (void)rc;
    PAMI_assertf(rc == 0, "FCA_Do_allgatherv failed with rc=%d [%s]",
            rc, 
            FCA_Strerror(rc));
    __global.heap_mm->free(_recvsizes);
    this->executeDoneCallback();
    this->_f->returnComposite(this);
  }
  static inline void metadata(pami_metadata_t *m)
    {
      new(m) PAMI::Geometry::Metadata(myName());
    }
  static inline const char *myName()
    {
      return "I1:AllgathervInt:FCA:FCA";
    }
  inline void setxfer(pami_xfer_t *xfer)
    {
      PAMI_assert_alwaysf(this->_my_endpoint != (unsigned)-1,"Invalid endpoint I1:AllgathervInt:FCA:FCA");
      pami_allgatherv_int_t *cmd = &(xfer->cmd.xfer_allgatherv_int);
      size_t             myindex = this->_g->ordinal(this->_my_endpoint);;
      int                   *ptr = NULL;
      pami_result_t           rc = PAMI_SUCCESS; (void)rc;
      size_t                  sz = this->_g->size();
      _spec.root                 = -1;  // will be ignored in FCA
      _spec.sbuf                 = (cmd->sndbuf == PAMI_IN_PLACE) ?
                                    cmd->rcvbuf + cmd->rdispls[myindex] *
                                    ((TypeCode *)cmd->rtype)->GetExtent() :
                                    cmd->sndbuf;
      _spec.rbuf                 = cmd->rcvbuf;
      _spec.sendsize             = cmd->stypecount  *((Type*)cmd->stype)->GetExtent();
      rc                         = __global.heap_mm->memalign((void**)&ptr,
                                                              0,
                                                              2*sz*sizeof(int));
      PAMI_assertf(ptr && (rc == PAMI_SUCCESS),
                   "Unable to allocate FCA size/displacement buffers: size=%lu\n", sz);
      _recvsizes             = &ptr[0];
      _displs                = &ptr[sz];
      size_t extent          = ((Type*)cmd->rtype)->GetExtent();
      for(size_t i=0;i<sz;i++)
      {
        _recvsizes[i] = cmd->rtypecounts[i] * extent;
        _displs[i]    = cmd->rdispls[i]     * extent;
      }
      _spec.recvsizes        = _recvsizes;
      _spec.displs           = _displs;
    }
private:
  fca_gatherv_spec_t  _spec;
  int                *_recvsizes;
  int                *_displs;
};

// --------------  FCA Barrier wrapper classes -------------
template <class T_Geometry>
class FCABarrierExec:public FCAComposite<T_Geometry>
{
public:
  typedef FCAComposite<T_Geometry>   FCAComp;
  typedef FCAFactory<FCABarrierExec> Factory;
  typedef FCAFactory<FCAComp>        FactoryP;
  FCABarrierExec(T_Geometry *g, Factory *f):
    FCAComp(g,(FactoryP*)f)
    {
    }
  virtual void start()
  {
    int rc = FCA_Do_barrier(this->_c); (void)rc;
    PAMI_assertf(rc == 0, "FCA_Do_barrier failed with rc=%d [%s]",
            rc,
            FCA_Strerror(rc));

    this->executeDoneCallback();
    this->_f->returnComposite(this);
  }
  static inline void metadata(pami_metadata_t *m)
    {
      new(m) PAMI::Geometry::Metadata(myName());
    }
  static inline const char *myName()
    {
      return "I1:Barrier:FCA:FCA";
    }
  inline void setxfer(pami_xfer_t *xfer)
    {
    }
private:

};

}//namespace CollRegistration
}//namespace PAMI


#endif
