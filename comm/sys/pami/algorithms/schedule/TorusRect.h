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
 * \file algorithms/schedule/TorusRect.h
 * \brief ???
 */

#ifndef __algorithms_schedule_TorusRect_h__
#define __algorithms_schedule_TorusRect_h__
#ifndef __pami_target_socklinux__

/*-------------------------------------------------*/
/*     Basic utility classes collectives           */
/*-------------------------------------------------*/

  #include "util/ccmi_debug.h"
  #include "util/ccmi_util.h"

  #include "algorithms/interfaces/Schedule.h"
  #include "common/TorusMappingInterface.h"
  #include "Global.h"

  #include "util/ccmi_debug.h"
/// \todo put this trace facility somewhere common
  #include "util/trace.h"

  #ifdef CCMI_TRACE_ALL
    #define DO_TRACE_ENTEREXIT 1
    #define DO_TRACE_DEBUG     1
  #else
    #define DO_TRACE_ENTEREXIT 0
    #define DO_TRACE_DEBUG     0
  #endif


///
/// \brief This schedule implements the following broadcast algorithm
/// on 1-3 dimensional meshes and tori. The following is the schematic
/// of an X color broadcast on a 2D mesh in SMP mode.
///
///    1Y  2G  1Y  1Y
///    1Y  2G  1Y  1Y
///    0X   R  0X  0X
///    1Y  2G  1Y  1Y
///
///  R : Root
///  0X : Processor receives data in phase 0 from dimension X
///  1Y : Processor receives data in phase 1 from dimension Y
///  2G : As the root needs to use the Y links for another color, we have
///       ghost nodes that dont get data in the first two phases of a 2D mesh
///       broadcast. Their neigbors have to send them data in last phase (2).
///
///     In modes where there are more than one core per node the peer of the
///     root locally broadcasts data to all the cores.
///

namespace CCMI
{
  namespace Schedule
  {

#define MY_TASK         __global.mapping.task()
#define COORD(a,b)      a.net_coord(b)
#define TORUS_DIMS      _glb_ndims
#define REDUCE_GHOST_PHASE 16 
#define TASK2NET(a,b)   __global.mapping.task2network(a,b,PAMI_N_TORUS_NETWORK)
#define NET2TASK(a,b,c) __global.mapping.network2task(a,b,c)
#define RECV            0
#define SEND            1
#define NUM_OPS         (CCMI::REDUCE_OP+1)
#define MESH     PAMI::Interface::Mapping::Mesh
#define POSITIVE PAMI::Interface::Mapping::TorusPositive
#define NEGATIVE PAMI::Interface::Mapping::TorusNegative

    static const uint16_t torus_rect_mod5_table [16] = { 0, 1, 2, 3, 4, 
							 0, 1, 2, 3, 4, 
							 0, 1, 2, 3, 4, 
							 0 };

    enum TRLocalAlgorithm {
      TR_BinomialTree = 0,
      TR_LineReduce,
      TR_DirectReduce,
    };
    
    template <TRLocalAlgorithm TL>
    class TorusRectT: public CCMI::Interfaces::Schedule
    {
    public:
      static const unsigned NO_COLOR = 0;
      
      TorusRectT()
      {
      }

      TorusRectT(unsigned myrank, 
                PAMI::Topology *rect,
                unsigned color):
      _color(color)
	//      _start_phase((unsigned)-1)
      {
        TRACE_FN_ENTER();	
	unsigned i;	
        _my_local_phase = -1;

	for (i = 0; i < NUM_OPS; i++) _iam_ghost[i] = _phases[i] = 0;
	
        _glb_ndims = __global.mapping.torusDims();
	TASK2NET(MY_TASK, &_self_coord);
	
	PAMI_assertf(rect->type() == PAMI_COORD_TOPOLOGY, 
		     "Type %u",rect->type());
        rect->rectSeg(&_ll, &_ur, &_torus_link);	

	_num_active_dims = 0;
	_dim_sizes[TORUS_DIMS] = _peers = 
	  _ur->net_coord(TORUS_DIMS) - _ll->net_coord(TORUS_DIMS) + 1;
        //printf("size is %d and %zu %zu, peers %d\n",
	//     _dim_sizes[TORUS_DIMS],_ur->net_coord(TORUS_DIMS),
	//     _ll->net_coord(TORUS_DIMS), _peers);
	for (i = 0; i < TORUS_DIMS; i++) 
	{
	   size_t size = _ur->net_coord(i) - _ll->net_coord(i) + 1;
	   _dim_sizes[i] = size;

           if (size > 1)
             _num_active_dims++;
	}

        TRACE_FORMAT("<%u:%p>_ndims %d, _color  %u",_color,this,TORUS_DIMS, _color);
        TRACE_FN_EXIT();
      }

      virtual void init(int root, int op, int &start, int &nphases);
      virtual pami_result_t getSrcUnionTopology(PAMI::Topology *topo, 
						pami_endpoint_t *src_eps=NULL);
      virtual pami_result_t getDstUnionTopology(PAMI::Topology *topology, 
						pami_endpoint_t *dst_eps=NULL,
						unsigned num_eps=0);
      virtual void getSrcTopology(unsigned phase, PAMI::Topology *topology, 
				  pami_endpoint_t *src_eps=NULL);
      virtual void getDstTopology(unsigned phase, PAMI::Topology *topology, 
				  pami_endpoint_t *dst_eps=NULL);

      virtual unsigned getLastReducePhase();

      unsigned color()
      {
        TRACE_FN_ENTER();
        TRACE_FORMAT("<%u:%p>_color=%u ",_color,this,_color);
        TRACE_FN_EXIT();
        return _color;
      }

      void setColor(unsigned c)
      {
        TRACE_FN_ENTER();
        _color  = c;
        TRACE_FORMAT("<%u:%p>_color=%u ",_color,this,_color);
        TRACE_FN_EXIT();
      }

      static void getColors(PAMI::Topology *rect, unsigned &ideal,
                            unsigned &max, unsigned *colors = NULL)
      {
        TRACE_FN_ENTER();
        uint i = 0;
        ideal = 0;
        pami_coord_t *ll=NULL, *ur=NULL;
        unsigned char *torus_link=NULL;
        size_t torus_dims, sizes[PAMI_MAX_DIMS];

        torus_dims = __global.mapping.torusDims();
        PAMI_assertf(rect->type() == PAMI_COORD_TOPOLOGY, "Type %u",
                     rect->type());
        rect->rectSeg(&ll, &ur, &torus_link);

        for (i = 0; i < torus_dims; i++)
        {
          sizes[i] = ur->u.n_torus.coords[i] - ll->u.n_torus.coords[i] + 1;
          if (sizes[i] > 1)
          {
            TRACE_FORMAT("color[%u]=%u", ideal, i);
            colors[ideal++] = i;
          }
        }
        max = ideal;

#if 1 //enable -ve colors
        for (i = 0; i < torus_dims; i++) {
          if (sizes[i] > 1 && torus_link[i])
          {
            TRACE_FORMAT("color[%u]=%zu",  max, i + torus_dims);
            colors[max++] = i+torus_dims;
          }
        }

        if (max == 2 * ideal) {
          ideal = max;
	}
#endif
        if (ideal == 0) //single node
        {
          ideal = max = 10;
          colors[0] = 0;
          colors[1] = 1;
          colors[2] = 2;
          colors[3] = 3;        
          colors[4] = 4;
          colors[5] = 5;
          colors[6] = 6;
          colors[7] = 7;        
          colors[8] = 8;        
          colors[9] = 9;        
	}
        TRACE_FN_EXIT();
      }
    protected:
      unsigned             _glb_ndims;
      unsigned             _color;
      int                  _my_local_phase;
      unsigned             _phases[NUM_OPS];
      unsigned             _start_phase[NUM_OPS];
      unsigned             _iam_ghost[NUM_OPS];
      unsigned             _peers;
      pami_coord_t       * _ll;
      pami_coord_t       * _ur;
      unsigned char      * _torus_link;
      unsigned             _num_active_dims;
      unsigned             _local_reduce_phases;
      unsigned             _dim_sizes[PAMI_MAX_DIMS];
      pami_task_t          _root;
      pami_coord_t         _root_coord;
      pami_coord_t         _self_coord;
      int                  _op;

      //Reduce methods
      unsigned getReduceAxis(unsigned phase);
      pami_result_t getReduceSrcTopology(unsigned phase, PAMI::Topology *topo);
      pami_result_t getReduceDstTopology(unsigned phase, PAMI::Topology *topo);
      pami_result_t setupReduceGhostTopology(unsigned phase, int mode, 
                                             PAMI::Topology * topo);
      pami_result_t setupReduceLocalTopology(unsigned phase,
					     int data_mode,
					     PAMI::Topology *topo);
      pami_result_t setupReduceLocalBinoTopology(unsigned phase,
						 int data_mode,
						 PAMI::Topology *topo);
      pami_result_t setupReduceLocalLineTopology(unsigned phase,
						 int data_mode,
						 PAMI::Topology *topo);
      pami_result_t setupReduceLocalDirectTopology(unsigned phase,
						   int data_mode,
						   PAMI::Topology *topo);

      // Bcast methods
      void getBcastAxialAttributes(unsigned phase,  int &axis, size_t &ll,
                                   size_t &ur, uint8_t &tlink);
      void setupBroadcast(unsigned phase,  int &axis, size_t &ll, size_t &ur,
                          uint8_t &tlink);
      void setupBcastGhost(int &axis, size_t &ll, size_t &ur, uint8_t &tlink);
      void setupBcastLocal(int &axis, size_t &ll, size_t &ur, uint8_t &tlink);
    };  //-- TorusRectT

    //typedef TorusRectT<TR_BinomialTree> TorusRect;
    //typedef TorusRectT<TR_LineReduce> TorusRect;
    typedef TorusRectT<TR_DirectReduce> TorusRect;

  };  //-- Schedule
}; //-- CCMI


//-------------------------------------------------------------------
//------  TorusRect Schedule Functions ----------------------
//-------------------------------------------------------------------

/**
 * \brief Initialize the schedule for collective operation
 * \param root : the root of the collective
 * \param op : the collective operation
 * \param startphase : the phase where I become active
 * \param nphases : number of phases
 * \param maxranks : total number of processors to communicate
 *  with. Mainly needed in the executor to allocate queues
 *  and other resources
 */

template <CCMI::Schedule::TRLocalAlgorithm TL>
inline void CCMI::Schedule::TorusRectT<TL>::init(int root,
						 int op,
						 int &start,
						 int &nphases)
{
  TRACE_FN_ENTER();

  CCMI_assert (op == CCMI::BROADCAST_OP ||
               op == CCMI::REDUCE_OP ||
               op == CCMI::ALLREDUCE_OP||
               op == CCMI::BARRIER_OP);
  
  _op = op;

  //if allreduce, then set _root_id coordinates to that of lower left corner
  if (root == -1)
  {
    pami_network type;
    if (TL == TR_LineReduce || TL == TR_DirectReduce) {
      _root_coord = *_ll;
      //Switch local dim of root based on color
      _root_coord.net_coord(TORUS_DIMS) += _color % _peers;
      NET2TASK(&_root_coord, &_root, &type);
    }
    else {
      _root_coord = *_ll;
      NET2TASK(_ll, &_root, &type);
    }
  }
  else
  {
    _root = root;
    TASK2NET(_root, &_root_coord);
  }

  size_t color_axis[PAMI_MAX_DIMS] = {0};
  unsigned int i, axis, color = _color;  
  for (axis = 0; axis < TORUS_DIMS; axis++) 
    color_axis[axis] = color++ % TORUS_DIMS;
  
  if (op == CCMI::BROADCAST_OP || op == CCMI::ALLREDUCE_OP)
  {
    bool myphase = false;

    if (MY_TASK == (size_t) _root)
      _start_phase[CCMI::BROADCAST_OP] = 0;    
    // if I share the same first coordinate with the root, then am a ghost node
    else if (COORD(_self_coord, color_axis[0]) 
	     == COORD(_root_coord, color_axis[0]))
    {
      _start_phase[CCMI::BROADCAST_OP] = TORUS_DIMS;
      _iam_ghost[CCMI::BROADCAST_OP] = 1;
    }
    
    else
    {
      for (axis = 0; axis < TORUS_DIMS; axis++)
      {
        for (myphase = true, i = axis + 1; i < TORUS_DIMS && myphase; i++)
        {
          if (COORD(_self_coord, color_axis[i]) != 
	      COORD(_root_coord, color_axis[i]))
            myphase = false;
        }
        if (myphase)
          break;
      }
      _start_phase[CCMI::BROADCAST_OP] = axis;
    }
    
    start = _start_phase[CCMI::BROADCAST_OP];

    // 2: 1 for local comm if any, 1 for ghost
    nphases = _phases[CCMI::BROADCAST_OP] = TORUS_DIMS + 2 - start;

    // this checks for local peers
    if (_peers == 1)
    {
      nphases = --(_phases[CCMI::BROADCAST_OP]);
    }
  }

  if (op == CCMI::REDUCE_OP || op == CCMI::ALLREDUCE_OP)
  {
    // phases 0-5 are reserved for locals, even if peers = 0
    // phase 6 is reserved for ghosts (even if there is no ghosts)
    // phasea 7+ are for torus

    // non ghost nodes
    start = _start_phase[CCMI::REDUCE_OP] = 0;

    // we need max of 6 locals and 1 for ghost
    _phases[CCMI::REDUCE_OP] = REDUCE_GHOST_PHASE + 1;

    // compute torus phases
    for (i = 0; i < TORUS_DIMS; i++)
      _phases[CCMI::REDUCE_OP] += _dim_sizes[i] - 1;
    

    // compute local phases
    _local_reduce_phases = 0;
    
    if (_peers > 1) {
      if (TL == TR_BinomialTree) {
	if (_peers == 2)
	  _local_reduce_phases = 1;
	else if (_peers == 4)
	  _local_reduce_phases = 2;
	else if (_peers == 8)
	  _local_reduce_phases = 3;
	else if (_peers == 16)
	  _local_reduce_phases = 4;
	else if (_peers == 32)
	  _local_reduce_phases = 5;
	else if (_peers == 64)
	  _local_reduce_phases = 6;
      }
      else if (TL == TR_LineReduce || TL == TR_DirectReduce)
	_local_reduce_phases = REDUCE_GHOST_PHASE;
    }
     
    nphases = _phases[CCMI::REDUCE_OP];
  } 
 
  if (op == CCMI::ALLREDUCE_OP)
  {
    //we just set nphases in reduce
    nphases += _phases[CCMI::BROADCAST_OP] + _start_phase[CCMI::BROADCAST_OP];
  }
  //printf("bcast start %d bcast phases %d  reduce start %d reduce phases %d and nphases %u\n",
  //     _start_phase[CCMI::BROADCAST_OP], _phases[CCMI::BROADCAST_OP], _start_phase[CCMI::REDUCE_OP],
  //         _phases[CCMI::REDUCE_OP], nphases);
  TRACE_FN_EXIT();
}


template <CCMI::Schedule::TRLocalAlgorithm TL>
inline pami_result_t
CCMI::Schedule::TorusRectT<TL>::getSrcUnionTopology(PAMI::Topology *topo,
                                               pami_endpoint_t *src_eps)
{
  (void)src_eps;
  //printf("no need to implement\n");
  new (topo) PAMI::Topology(_root);
  return PAMI_SUCCESS;
}

template <CCMI::Schedule::TRLocalAlgorithm TL>
inline void
CCMI::Schedule::TorusRectT<TL>::getSrcTopology(unsigned phase,
					      PAMI::Topology *topo,
					      pami_endpoint_t *src_eps)
{
  (void)phase;
  (void)topo;
  (void)src_eps;
  
  // we should always start with an empty topology
  new (topo) PAMI::Topology();
  
  unsigned endr = 0;
  switch (_op)
  {
    case CCMI::REDUCE_OP:
    case CCMI::ALLREDUCE_OP:
      endr = _start_phase[CCMI::REDUCE_OP] + _phases[CCMI::REDUCE_OP];
      
      if (phase < endr)
	getReduceSrcTopology(phase, topo);
      //Return self topology after bcast phases to notify bcast
      else if (phase - endr == _start_phase[CCMI::BROADCAST_OP] &&
	       MY_TASK != _root)
	new (topo) PAMI::Topology(_root);
      break;
    
      //Following ops not implemented
    case CCMI::BROADCAST_OP:
    case CCMI::BARRIER_OP:
    default:
      CCMI_abort();
  }
}

/**
   * \brief Get Destination node phase list
   *
   * \param[in] phase	Phase for which to extract information
   * \param[out] dstranks	Array to hold destination node(s)
   * \param[out] ndst	Number of destination nodes (and subtasks)
   * \param[out] subtask	Array to hold subtasks (operation, e.g. LINE_BCAST_XM)
   * \return	nothing (else).
   */
template <CCMI::Schedule::TRLocalAlgorithm TL>
inline void
CCMI::Schedule::TorusRectT<TL>::getBcastAxialAttributes(unsigned  phase,  
						       int     & raxis, 
						       size_t  & ll, 
						       size_t  & ur, 
						       uint8_t & tlink) 
{
  size_t core_dim, torus_dims;
  torus_dims = TORUS_DIMS;
  core_dim = torus_dims;

  if (_self_coord.net_coord(core_dim) == _root_coord.net_coord(core_dim))
  {
    //call setup broadcast with phase 0 which implies root
    if (MY_TASK == _root && phase == 0) {
      setupBroadcast(phase, raxis, ll, ur, tlink);
      CCMI_assert (raxis < PAMI_MAX_DIMS);
    }
    else if (MY_TASK != _root && phase > _start_phase[BROADCAST_OP])
    {
      // setup the destination processors to foreward the data along
      // the next dimension in the torus
      if (phase < TORUS_DIMS) {
        setupBroadcast(phase, raxis, ll, ur, tlink);
	CCMI_assert (raxis < PAMI_MAX_DIMS);
      }
      ///Process ghost nodes
      else if (phase == TORUS_DIMS) {
        setupBcastGhost(raxis, ll, ur, tlink);
	CCMI_assert (raxis < PAMI_MAX_DIMS);
      }
    }

    ///Process local broadcasts
    if ((phase == ( TORUS_DIMS + 1)) && (_peers > 1))
      setupBcastLocal(raxis, ll, ur, tlink);
  }
}


/**
   * \brief Get Destination node phase list
   *
   * \param[in] phase	Phase for which to extract information
   * \param[out] dstranks	Array to hold destination node(s)
   * \param[out] ndst	Number of destination nodes (and subtasks)
   * \param[out] subtask	Array to hold subtasks (operation, e.g. LINE_BCAST_XM)
   * \return	nothing (else).
   */
template <CCMI::Schedule::TRLocalAlgorithm TL>
inline void
CCMI::Schedule::TorusRectT<TL>::getDstTopology(unsigned phase,
					      PAMI::Topology *topo,
					      pami_endpoint_t *dst_eps)
{
  (void)dst_eps;
  TRACE_FN_ENTER();
  
  new (topo) PAMI::Topology();
  unsigned endr = 0;
  switch (_op)
  {
    case CCMI::REDUCE_OP:
    case CCMI::ALLREDUCE_OP:
      endr = _start_phase[CCMI::REDUCE_OP] + _phases[CCMI::REDUCE_OP];

      if (phase < endr)
        getReduceDstTopology(phase, topo);
      else
      {
        if (phase - endr == _start_phase[CCMI::BROADCAST_OP])
	  getDstUnionTopology(topo, dst_eps); // return union of axials        
      }
      break;      
      
    case CCMI::BROADCAST_OP:
      printf("no need to implement\n");
      break;
      
    case CCMI::BARRIER_OP:
    default:
      CCMI_abort();
  }

  TRACE_FN_EXIT();
}

/**
 * \brief Get Destinations for the non-ghost non-local phases
 *
 * \param[in] phase	Phase for which to extract information
 * \param[out] dstranks	Array to hold destination node(s)
 * \param[out] subtask	Array to hold subtasks (operation, e.g. LINE_BCAST_XM)
 * \param[out] ndst	Number of destination nodes (and subtasks)
 * \return	nothing (else).
 */
template <CCMI::Schedule::TRLocalAlgorithm TL>
inline void
CCMI::Schedule::TorusRectT<TL>::setupBroadcast(unsigned  phase,  
					      int     & raxis, 
					      size_t  & ll, 
					      size_t  & ur, 
					      uint8_t & tlink)
{
  TRACE_FN_ENTER();
  //Find the axis to do the line broadcast on
  //int axis = (phase + _color) % TORUS_DIMS;
  int axis = torus_rect_mod5_table[(phase + _color)];  
  CCMI_assert (axis < PAMI_MAX_DIMS);

  if (_torus_link[axis])
  {
    tlink = POSITIVE;
    if (_color >= TORUS_DIMS)
      tlink = NEGATIVE;
  }

  raxis = (int) axis;
  ll = MIN(_ll->net_coord(axis),
	   _self_coord.net_coord(axis));
  ur = MAX(_ur->net_coord(axis),
	   _self_coord.net_coord(axis));

  TRACE_FORMAT("<%u:%p>phase %u, axis %d, _self %zu, ll %zu, ur %zu \n",_color,this,phase, axis,_self_coord.net_coord(axis),_ll->net_coord(axis),_ur->net_coord(axis));
  
  TRACE_FN_EXIT();
}

/**
 * \brief Get Destinations for the phases to process ghost nodes
 *
 * \param[out] dstranks	Array to hold destination node(s)
 * \param[out] subtask	Array to hold subtasks (operation, e.g. LINE_BCAST_XM)
 * \param[out] ndst	Number of destination nodes (and subtasks)
 * \return	nothing (else).
 */
template <CCMI::Schedule::TRLocalAlgorithm TL>
inline void
CCMI::Schedule::TorusRectT<TL>::setupBcastGhost(int     & raxis, 
					       size_t  & ll, 
					       size_t  & ur, 
					       uint8_t & tlink)
{
  TRACE_FN_ENTER();
  //pami_coord_t dst;
  size_t dst_coord;
  size_t ref;

  //size_t axis = _color % TORUS_DIMS;
  size_t axis = torus_rect_mod5_table[_color];
  size_t dim_size = _ur->net_coord(axis) - _ll->net_coord(axis) + 1;

  CCMI_assert(dim_size > 1);

  ref = COORD(_self_coord, axis);

  if (_torus_link[axis]) // if this dim or axis is a torus
  {
    if (_color < TORUS_DIMS) { //+ve colors
      ref = (COORD(_root_coord, axis) + 1) % dim_size;
      tlink = NEGATIVE;
    }
    else {
      ref = (COORD(_root_coord, axis) + dim_size - 1) % dim_size;
      tlink = POSITIVE;
    }
  }
  else
  {
    ref = COORD(_root_coord, axis) + 1;
    if (ref >= dim_size + _ll->net_coord(axis))
    {
      ref =  COORD(_root_coord, axis) - 1;
    }
  }
  TRACE_FORMAT("<%u:%p>axis %zu, _self %zu, ref %zu\n",_color,this, axis,_self_coord.net_coord(axis),ref);
  //The nodes that are different from the root in one dimension (not
  //the leading dimension of the color) are the ghosts. The nodes
  //just before them have to send data to them
  if (COORD(_self_coord, axis) == ref /*&& _start_phase > 0*/)
  {
    pami_coord_t dst = _self_coord;
    COORD(dst, axis) = COORD(_root_coord, axis); 
    if (COORD(dst, 0) == COORD(_root_coord, 0) &&
	COORD(dst, 1) == COORD(_root_coord, 1) &&
	COORD(dst, 2) == COORD(_root_coord, 2) &&
	COORD(dst, 3) == COORD(_root_coord, 3) &&
	COORD(dst, 4) == COORD(_root_coord, 4))
      return;
    
    dst_coord = COORD(_root_coord, axis);

    raxis = (int) axis;
    ll = COORD(_self_coord, axis);
    ur = COORD(_self_coord, axis);
    
    if (tlink == MESH) {
      ll  = MIN(dst_coord,
		COORD(_self_coord, axis));
      ur = MAX(dst_coord,
	       COORD(_self_coord, axis));
    }
    //Data must go from self to dst
    else if (tlink == POSITIVE) {
      //In the case of torus we may have a wrapped  (low is self)
      ur = dst_coord;
    }
    else if (tlink == NEGATIVE) {
      //In the case of torus we may have a wrapped  (high is self)
      ll = dst_coord;
    }
    
    TRACE_FORMAT("<%u:%p>axis %zu, _self %zu, dst %zu tlink %d\n",_color,this,axis,_self_coord.net_coord(axis),dst_coord, 
		 tlink);      
  }
  TRACE_FN_EXIT();
}


/**
 * \brief Get Destinations for the local peers
 *
 * \param[out] dstranks	Array to hold destination node(s)
 * \param[out] subtask	Array to hold subtasks (operation, e.g. LINE_BCAST_XM)
 * \param[out] ndst	Number of destination nodes (and subtasks)
 * \return	nothing (else).
 */
template <CCMI::Schedule::TRLocalAlgorithm TL>
inline void
CCMI::Schedule::TorusRectT<TL>::setupBcastLocal(int     & raxis, 
					       size_t  & ll, 
					       size_t  & ur, 
					       uint8_t & tlink)
{
  TRACE_FN_ENTER();
  tlink = 0;

  // the cores dim is the first one after the physical torus dims
  size_t core_dim = TORUS_DIMS;
  bool match = true; // matches the root local dims?

  for (size_t i = core_dim; i < __global.mapping.globalDims() && match; i++)
    if (_self_coord.net_coord(i) != _root_coord.net_coord(i))
      match = false;

  if (match)
  {
    raxis = (int) core_dim;
    ll = MIN(_self_coord.net_coord(core_dim),
	     _ll->net_coord(core_dim));
    ur = MAX(_self_coord.net_coord(core_dim),
	     _ur->net_coord(core_dim));
  }

  TRACE_FORMAT("<%u:%p>match %u, core_dim %zu, global dims %zu",_color,this,match, core_dim, __global.mapping.globalDims());
  TRACE_FN_EXIT();
}


template <CCMI::Schedule::TRLocalAlgorithm TL>
inline pami_result_t
CCMI::Schedule::TorusRectT<TL>::getDstUnionTopology(PAMI::Topology *topology,pami_endpoint_t *dst_eps,unsigned num_eps)
{
  (void)dst_eps;
  (void)num_eps;
  TRACE_FN_ENTER();

  unsigned char torus_link[PAMI_MAX_DIMS] = {0};

  pami_coord_t low, high;
  low  = _self_coord;
  high = _self_coord;
  pami_result_t result = PAMI_SUCCESS;

  int start_phase = (int)_start_phase[BROADCAST_OP];
  int nphases     = (int)_phases[BROADCAST_OP];
  for (int i = start_phase; i < start_phase + nphases; i++)
  {
    int axis = -1;
    size_t ll = 0, ur = 0;
    uint8_t tlink = 0;

    getBcastAxialAttributes(i, axis, ll, ur, tlink);

    if (axis >= 0)
    {
      CCMI_assert (axis < PAMI_MAX_DIMS);
      torus_link[axis] |= tlink;
      
      ///On a torus network when the wrap links are used lo and hi are relative (SK)       
      if (ll  != _self_coord.net_coord(axis))
	low.net_coord(axis)  =  ll; 
      
      if (ur != _self_coord.net_coord(axis))
	high.net_coord(axis) = ur; 
    }      
  }

  // make an axial topology
  new (topology) PAMI::Topology(&low, &high, &_self_coord, torus_link);  
  if((topology->size() == 1) /*&& (topology->index2Endpoint(0) == __global.mapping.task())*/)
    new (topology) PAMI::Topology();

  TRACE_FN_EXIT();
  return result;
}


template <CCMI::Schedule::TRLocalAlgorithm TL>
inline unsigned
CCMI::Schedule::TorusRectT<TL>::getReduceAxis(unsigned phase)
{
  int relative_phase, bounds[PAMI_MAX_DIMS];      
  
  unsigned i;
  int sum = 0;
  
  // we subtract 7 because that is the first torus phase
  relative_phase = phase - REDUCE_GHOST_PHASE - 1;
  
  //color 0 (A+ color) pick 4 (E Axis) where the reduce happens first
  unsigned axis = (_color + TORUS_DIMS - 1) % TORUS_DIMS;
  bounds[0] = _dim_sizes[axis] - 1;   
  for (i = 1; i < TORUS_DIMS; i++)
  {
    axis = (_color + TORUS_DIMS - 1 - i) % TORUS_DIMS;
    bounds[i] = bounds[i - 1] + _dim_sizes[axis] - 1;
  }
  
  for (i = 0; i < TORUS_DIMS; i++)
  {
    if (relative_phase >= bounds[i])
      sum++;
    //sum += (((int)phase / bounds[i]) -
    //    ((int)phase - bounds[i]) / bounds[i]);    
    //fprintf(stderr,"phase %u _bounds[%u] %u sum %d\n", phase, i, bounds[i], sum);
  }
  
  return sum;
}

template <CCMI::Schedule::TRLocalAlgorithm TL>
inline unsigned
CCMI::Schedule::TorusRectT<TL>::getLastReducePhase()
{
  CCMI_assert (_op == CCMI::REDUCE_OP ||
               _op == CCMI::ALLREDUCE_OP);

  return _start_phase[CCMI::REDUCE_OP] + _phases[CCMI::REDUCE_OP] - 1;

}

template <CCMI::Schedule::TRLocalAlgorithm TL>
inline pami_result_t
CCMI::Schedule::TorusRectT<TL>::setupReduceLocalTopology(unsigned phase,
							int data_movement,
							PAMI::Topology * topo)
{
  if (TL == TR_BinomialTree)
    return setupReduceLocalBinoTopology(phase, data_movement, topo);
  else if (TL == TR_LineReduce)
    return setupReduceLocalLineTopology(phase, data_movement, topo);    
  else if (TL == TR_DirectReduce)
    return setupReduceLocalDirectTopology(phase, data_movement, topo);
}

template <CCMI::Schedule::TRLocalAlgorithm TL>
inline pami_result_t
CCMI::Schedule::TorusRectT<TL>::setupReduceLocalBinoTopology
(unsigned phase,
 int data_movement,
 PAMI::Topology * topo)
{
  pami_network type;
  pami_task_t partner_id = 0;
  pami_coord_t partner = _self_coord;
  int mask = 1;

  int core_id = COORD(_self_coord, TORUS_DIMS);

  //while (mask <= (int) phase)
  //mask <<= 1;
  mask = 1 << (int)phase;

  //printf("in local, I am %zu mask %d core_id %d and & is %d localphase %d\n",
  //     MY_TASK, mask, core_id, mask&core_id, _my_local_phase);

  if ((mask & core_id) == 0)
  {
    if ( data_movement == RECV && 
	 (_my_local_phase == -1 || (int)phase < _my_local_phase) )
    {
      //printf(" I am %zu op %d  phase %u core id %d mask %d and recv from %d", MY_TASK, data_movement, phase, core_id, mask, core_id|mask);
      COORD(partner, TORUS_DIMS) = core_id | mask;
    }
  }
  else
  {
    if ((_my_local_phase == -1 || _my_local_phase == (int) phase) &&
        data_movement == SEND)
    {
      _my_local_phase = (int) phase;
      COORD(partner, TORUS_DIMS) = core_id & (~mask);
    }
  }
  
  NET2TASK(&partner, &partner_id, &type);
  if (partner_id != MY_TASK)
  {
    new (topo) PAMI::Topology(partner_id);
    //printf(" I am %zu op %d  phase %u core id %d ", MY_TASK, data_movement,
    //phase, core_id);
    //printf(" partner is %u\n", partner_id);
  }
  
  return PAMI_SUCCESS;
}

template <CCMI::Schedule::TRLocalAlgorithm TL>
inline pami_result_t
CCMI::Schedule::TorusRectT<TL>::setupReduceLocalLineTopology
(unsigned phase,
 int data_movement,
 PAMI::Topology * topo)
{
  pami_network type;
  int my_idx = COORD(_self_coord, TORUS_DIMS);
  int tsize  = _dim_sizes[TORUS_DIMS];
  int head = COORD(_root_coord, TORUS_DIMS);
  int tail = (head + tsize - 1) % tsize;
  pami_task_t partner_id = 0;
  pami_coord_t partner = _self_coord;

  //in phase 0, tail-1 receives from tail
  int recv_phase = (tail - my_idx - 1 + tsize) % tsize; 
  //in phase 0 tail sends to tail-1
  int send_phase = (tail - my_idx + tsize) % tsize;    

  if (data_movement == RECV && my_idx != tail && (int)phase == recv_phase)
    COORD(partner, TORUS_DIMS) = (my_idx + 1) % tsize;
  
  if (data_movement == SEND && my_idx != head && (int)phase == send_phase)
    COORD(partner, TORUS_DIMS) = (my_idx - 1 + tsize) % tsize;

  NET2TASK(&partner, &partner_id, &type);
  if (partner_id != MY_TASK) {
    new (topo) PAMI::Topology(partner_id);  
    //printf ("line reduce color %d phase %d send/recv %d rank %d\n", 
    //    _color, phase, data_movement, partner_id);
  }

  return PAMI_SUCCESS; 
}

template <CCMI::Schedule::TRLocalAlgorithm TL>
inline pami_result_t
CCMI::Schedule::TorusRectT<TL>::setupReduceLocalDirectTopology
(unsigned phase,
 int data_movement,
 PAMI::Topology * topo)
{  
  pami_network type;
  int my_idx = COORD(_self_coord, TORUS_DIMS) - COORD((*_ll), TORUS_DIMS);
  unsigned tsize  = _dim_sizes[TORUS_DIMS];
  int root   = COORD(_root_coord, TORUS_DIMS) - COORD((*_ll), TORUS_DIMS);
  pami_task_t partner_id = 0;
  pami_coord_t partner = _self_coord;
  
  if (phase >= tsize)
    return PAMI_SUCCESS;

  if (data_movement==RECV && my_idx==root && (int)phase!=root)
    COORD(partner, TORUS_DIMS) = phase + COORD((*_ll), TORUS_DIMS);
  
  if (data_movement==SEND && my_idx!=root && (int)phase==my_idx)
    COORD(partner, TORUS_DIMS) = root + COORD((*_ll), TORUS_DIMS);
  
  NET2TASK(&partner, &partner_id, &type);
  if (partner_id != MY_TASK) {
    new (topo) PAMI::Topology(partner_id);  
    //printf ("direct reduce color %d phase %d send/recv %d rank %d\n", 
    //    _color, phase, data_movement, partner_id);
  }
  
  return PAMI_SUCCESS; 
}


template <CCMI::Schedule::TRLocalAlgorithm TL>
inline pami_result_t
CCMI::Schedule::TorusRectT<TL>::getReduceSrcTopology(unsigned phase,
						    PAMI::Topology * topo)
{
  pami_result_t result = PAMI_SUCCESS;
  unsigned int index, curr_axis, curr_phase, color_axis;
  unsigned int total = 0;
  unsigned i;
  pami_network type;
  pami_task_t src_id = 0;
  pami_coord_t src = _self_coord;
  
  curr_phase = (int) phase;  

  //fprintf(stderr, "In getReduceSrcTopology color %d phase %d\n", _color,phase);

  // do local reduce inside the node
  if (curr_phase < REDUCE_GHOST_PHASE)
  {
    if (_peers > 1 && curr_phase < _local_reduce_phases)
      result = setupReduceLocalTopology(curr_phase, RECV, topo);
    return result;
  }
  
  if (COORD(_self_coord, TORUS_DIMS) != COORD((_root_coord), TORUS_DIMS)) 
    return result;

  if (curr_phase == REDUCE_GHOST_PHASE)
  {
    if (_num_active_dims > 1)
      result = setupReduceGhostTopology(curr_phase, RECV, topo);
    return result;
  }

  // this is a subsequent phase past ghost phase, if I am a ghost, do nothing
  if (_iam_ghost[CCMI::REDUCE_OP])
    return result;

  // this can be confusing. Reduce data flows opposite of bcast.
  // so for an A+ color, the data actually is flowing negatively
  // so the direction is basically opposite of the color
  uint8_t dir = NEGATIVE;
  if (_color >= TORUS_DIMS)
    dir = POSITIVE;

  // we pass -2 here to account for local and ghost reductions
  index = getReduceAxis(curr_phase);
  color_axis = (_color + TORUS_DIMS - 1 - index) % TORUS_DIMS;
  curr_axis = color_axis;

  if ( (MY_TASK == _root) && (color_axis != (_color % TORUS_DIMS)) )
  {
    return PAMI_SUCCESS;
  }

  //fprintf(stderr, "After root check color %d\n", _color);

  //    checks if I am part of this phase
  //    it starts with everyone active and then those who dont share the root's
  //    coordinate of the previos dim. For example, assume 3D rect XYZ of 2x2x2.
  //    The XY plane on Z=1 send their data to XY plane at Z=0. In the next
  //    round, only those on XY plane at Z=0 are active, and those at Z=1, are
  //    dropped out.
  total = REDUCE_GHOST_PHASE+1;
  for (i = 0; i < (unsigned) index; i++)
  {
    color_axis = (_color + TORUS_DIMS - 1 - i) % TORUS_DIMS;
    // this means this isnt my phase, I have nothing to do in this phase
    if (COORD(_self_coord, color_axis) != COORD(_root_coord, color_axis))
    {
      //fprintf(stderr, "coords dont match color_axis %d color %d\n",
      //      color_axis, _color);
      return PAMI_SUCCESS;
    }
    total += (_dim_sizes[color_axis] - 1);
  }
  
  // we subtract total visited phases from current phase to land on a phase
  // number that is relative to the axis we are working with currently
  curr_phase -= total;

  size_t head = 0; 
  size_t tail = _dim_sizes[curr_axis] - 1;
  size_t my_coord = COORD(_self_coord, curr_axis) - COORD((*_ll), curr_axis);
  size_t root_coord = COORD(_root_coord, curr_axis) - COORD((*_ll), curr_axis);

  //fprintf(stderr, "root_coord %ld dir %d my_coord %ld tail %ld curr_phase %d color %d\n",
  // root_coord, dir, my_coord, tail, curr_phase, _color);
  
  // if root is at head of this dim
  if (root_coord == head)
  {
    // if this line is a torus, and data is moving in a positive direction,
    if (_torus_link[curr_axis] && dir == POSITIVE)
    {
      if (my_coord == (curr_phase + 2) % _dim_sizes[curr_axis])
      {
        COORD(src, curr_axis) = (my_coord - 1 + _dim_sizes[curr_axis]) %
          _dim_sizes[curr_axis];
	COORD(src, curr_axis) += COORD((*_ll), curr_axis);  
	//fprintf(stderr, "torus: I am here %zu and src is %zu color %d\n", MY_TASK, COORD(src, curr_axis), _color);
      }
    } 
    // data moves in a negative direction, or this dim isn't a torus line.
    else if (my_coord == tail - 1 - curr_phase)
    {
      COORD(src, curr_axis) = my_coord + 1;
      COORD(src, curr_axis) += COORD((*_ll), curr_axis);  
      //fprintf(stderr, "I am here %zu and src is %zu color %d\n", MY_TASK, my_coord+1, _color);
    }
  }
  
  // if root is at tail of this dim
  else if (root_coord == tail)
  {
    // if this line is a torus and data is moving in a negative direction,
    // and I am not the node before the root
    if (_torus_link[curr_axis] && dir == NEGATIVE)
    {
      if (my_coord == (tail - 2 - curr_phase + _dim_sizes[curr_axis]) %
          _dim_sizes[curr_axis])
      {
        COORD(src, curr_axis) = (my_coord + 1) % _dim_sizes[curr_axis];
	COORD(src, curr_axis) += COORD((*_ll), curr_axis);  
      }
    }
    // Data either moves in positive direction, or this line isn't a torus line.
    else if (my_coord == curr_phase + 1)
    {
      COORD(src, curr_axis) = (my_coord - 1 + _dim_sizes[curr_axis]) %
	                      _dim_sizes[curr_axis];
      COORD(src, curr_axis) += COORD((*_ll), curr_axis);  
    }
  }
  
  else // root is in the middle of this dim
  {
    // if torus dim, then data moves in one direction
    if (_torus_link[curr_axis])
    {
      // positive direction, the node after the root recvs nothing
      if (dir == POSITIVE &&
          my_coord == (curr_phase + root_coord + 2) % _dim_sizes[curr_axis])
      {
        COORD(src, curr_axis) = (my_coord - 1 + _dim_sizes[curr_axis]) %
                                _dim_sizes[curr_axis];
	COORD(src, curr_axis) += COORD((*_ll), curr_axis);  
      }
      
      // negative direction, the node before the root recv nothing
      else if (my_coord == (root_coord - curr_phase - 2 +
                           _dim_sizes[curr_axis]) %
                           _dim_sizes[curr_axis])
      {
        COORD(src, curr_axis) = (my_coord + 1) % _dim_sizes[curr_axis];
	COORD(src, curr_axis) += COORD((*_ll), curr_axis);  
      }      
    }
  
    else // data gets reduced to root of this dim from - and + directions
    {
      if (my_coord != head && my_coord != tail)
      {
        // if I am before the root
        if (my_coord <= root_coord &&
            my_coord == curr_phase + 1)
        {
          COORD(src, curr_axis) = my_coord - 1;
	  COORD(src, curr_axis) += COORD((*_ll), curr_axis);  
        }
        
        //if I am after the root
        if (my_coord >= root_coord &&        
            my_coord == (tail + root_coord - (phase + 1)))
        {
          COORD(src, curr_axis) = my_coord + 1;
	  COORD(src, curr_axis) += COORD((*_ll), curr_axis);  
        }
      }
    }
  }
  
  NET2TASK(&src, &src_id, &type);
  if (src_id != MY_TASK)
  {
    new (topo) PAMI::Topology(src_id);
    //printf("done with srctopo and src is %u at phase %u color %d\n", src_id, curr_phase+total, _color);
  }
  return PAMI_SUCCESS;
}


template <CCMI::Schedule::TRLocalAlgorithm TL>
inline pami_result_t
CCMI::Schedule::TorusRectT<TL>::getReduceDstTopology(unsigned phase,
						    PAMI::Topology * topo)
{
  unsigned i, total, index, curr_axis, curr_phase, color_axis;
  pami_network type;
  pami_task_t dst_id = -1;
  pami_coord_t dst = _self_coord;
  pami_result_t result = PAMI_SUCCESS;
  //printf("%p GETRED_DST phase %u color %d\n", this, phase, _color);

  uint8_t dir = NEGATIVE;
  if (_color >= TORUS_DIMS) dir = POSITIVE; // -ve bcast color

  curr_phase = (int) phase;

  // do local reduce inside the node
  if (curr_phase < REDUCE_GHOST_PHASE)
  {
    if (_peers > 1 && curr_phase < _local_reduce_phases)
      result = setupReduceLocalTopology(curr_phase, SEND, topo);
    return result;
  }
  
  if (COORD(_self_coord, TORUS_DIMS) != COORD((_root_coord), TORUS_DIMS)) 
    return result;
  
  if (curr_phase == REDUCE_GHOST_PHASE)
  {
    if (_num_active_dims > 1)
      result = setupReduceGhostTopology(curr_phase, SEND, topo);
    return result;
  }

  // this is a subsequent phase past ghost phase, if I am a ghost, do nothing
  if (_iam_ghost[CCMI::REDUCE_OP] || MY_TASK == _root)
    return result;

  // we pass -2 here to account for local and ghost reductions
  index = getReduceAxis(curr_phase);
  color_axis = (_color + TORUS_DIMS - 1 - index) % TORUS_DIMS;
  curr_axis = color_axis; 
  
  //    checks if I am part of this phase
  //    it starts with everyone active and then those who dont share the root's
  //    coordinate of the previos dim. For example, assume 3D rect XYZ of 2x2x2.
  //    The XY plane on Z=1 send their data to XY plane at Z=0. In the next
  //    round, only those on XY plane at Z=0 are active, and those at Z=1, are
  //    dropped out.

  total = REDUCE_GHOST_PHASE+1;
  for (i = 0; i < (unsigned) index; i++)
  {
    color_axis = (_color + TORUS_DIMS - 1 - i) % TORUS_DIMS;
    // this means this isnt my phase, I have nothing to do in this phase
    if (COORD(_self_coord, color_axis) != COORD(_root_coord, color_axis))
      return PAMI_SUCCESS;
    total += (_dim_sizes[color_axis] - 1);
  }
  
  // we subtract total visited phases from current phase to land on a phase
  // number that is relative to the axis we are working with currently
  //  curr_phase -= 2;
  curr_phase -= total;
  
  size_t head = 0;
  size_t tail = _dim_sizes[curr_axis] - 1;
  size_t my_coord = COORD(_self_coord, curr_axis) - COORD((*_ll), curr_axis);
  size_t root_coord = COORD(_root_coord, curr_axis) - COORD((*_ll), curr_axis);
  uint8_t tlink = MESH;

  // if root is at head of this dim
  if (root_coord == head)
  {
    // if this line is a torus, data is moving in a positive direction,
    // and this is my phase accrd to coord
    if (_torus_link[curr_axis] && dir == POSITIVE)
    {
      if (my_coord == (curr_phase + 1) % _dim_sizes[curr_axis])
      {
        //printf("I am here %zu in phase %u\n", MY_TASK, curr_phase);
        COORD(dst, curr_axis) = (my_coord + 1) % _dim_sizes[curr_axis];
	COORD(dst, curr_axis) += COORD((*_ll), curr_axis);
	tlink = POSITIVE;
      }
    }
    // Otherwise, either data moves in a negative direction, or this dim
    // isn't a torus line. 
    else if (my_coord == tail - curr_phase)
    {
      //      printf("I am here2  %zu in phase %u\n", MY_TASK, curr_phase);
      COORD(dst, curr_axis) = (my_coord - 1 + _dim_sizes[curr_axis]) %
	                       _dim_sizes[curr_axis];
      COORD(dst, curr_axis) += COORD((*_ll), curr_axis);
      //doing a wrap
      if (_torus_link[curr_axis])
	tlink = NEGATIVE;      
    }
  }

    // if root is at tail of this dim
  else if (root_coord == tail)
  {
    // if this line is a torus and data is moving in a negative direction,
    if (_torus_link[curr_axis] && dir == NEGATIVE)
    {
      if (my_coord == (tail - 1 - curr_phase + _dim_sizes[curr_axis]) %
          _dim_sizes[curr_axis])
      {
        COORD(dst, curr_axis) = (my_coord - 1 + _dim_sizes[curr_axis]) %
          _dim_sizes[curr_axis];
	COORD(dst, curr_axis) += COORD((*_ll), curr_axis);
	tlink = NEGATIVE;
      }
    }
    
    // Data either moves in positive direction, or this line
    // isn't a torus line.
    else if (my_coord == curr_phase)
    {
      COORD(dst, curr_axis) = my_coord + 1;
      COORD(dst, curr_axis) += COORD((*_ll), curr_axis);
    }
  }

  else // root is in the middle of this dim
  {
    // if torus dim, then data moves in one direction
    if (_torus_link[curr_axis])
    {
      // positive direction, the node after the root recvs nothing
      if (dir == POSITIVE &&
          my_coord == (curr_phase + root_coord + 1) % _dim_sizes[curr_axis])
      {
        COORD(dst, curr_axis) = (my_coord + 1) % _dim_sizes[curr_axis];
	COORD(dst, curr_axis) += COORD((*_ll), curr_axis);
	tlink = POSITIVE;
      }
      // negative direction, the node before the root recv nothing
      else if (my_coord == (root_coord - 1 - curr_phase +
                            _dim_sizes[curr_axis]) %
	                    _dim_sizes[curr_axis])
      {
        COORD(dst, curr_axis) = (my_coord - 1 + _dim_sizes[curr_axis]) %	  
	  _dim_sizes[curr_axis];
	COORD(dst, curr_axis) += COORD((*_ll), curr_axis);
      }
    }
    
    else // data gets reduced to root of this dim from - and + directions
    {
      // if I am before the root
      if (my_coord < root_coord &&
          my_coord == curr_phase)
      {
        COORD(dst, curr_axis) = my_coord + 1;
	COORD(dst, curr_axis) += COORD((*_ll), curr_axis);
      }
      
      //if I am after the root
      if (my_coord > root_coord &&
          my_coord == tail + root_coord - curr_phase)
        
      {
        COORD(dst, curr_axis) = my_coord - 1;
	COORD(dst, curr_axis) += COORD((*_ll), curr_axis); 
      }
    }
  }  
  
  NET2TASK(&dst, &dst_id, &type);
  if (dst_id != MY_TASK) {    
    //printf("%p: done with getdst topo and dst is %u, color %d, phase %d dir %d\n", this, dst_id, _color, phase, tlink);
    
    //if dimensions are larger than 2, mesh will find the correct
    //path
    if (_torus_link[curr_axis])
      if(__global.mapping.torusSize(curr_axis) > 2) 
	tlink = MESH;

    if (tlink == MESH) 
      new (topo) PAMI::Topology(dst_id);
    else if (tlink == POSITIVE)
      new (topo) PAMI::Topology(&_self_coord, &dst, 
				&_self_coord, &tlink);
    else // tlink == NEGATIVE
      new (topo) PAMI::Topology(&dst, &_self_coord,
				&_self_coord, &tlink);
  }  
  
  return PAMI_SUCCESS;
}


template <CCMI::Schedule::TRLocalAlgorithm TL>
inline pami_result_t
CCMI::Schedule::TorusRectT<TL>::setupReduceGhostTopology(unsigned phase,
							int data_movement, 
							PAMI::Topology * topo)
{
  pami_network type;
  pami_task_t partner_id = 0;
  pami_coord_t partner = _self_coord;
  size_t ref_coord, root_coord;
  unsigned int color_axis = _color % TORUS_DIMS;
  size_t dim_size = _dim_sizes[color_axis];
  uint8_t tlink = MESH;

  ref_coord = COORD(_self_coord, color_axis);
  root_coord = COORD(_root_coord, color_axis);

  if (_torus_link[color_axis]) // if this dim or axis is a torus
  {
    //+ve colors
    if (_color < TORUS_DIMS)
    {
      ref_coord = (root_coord + 1) % dim_size;
      tlink = POSITIVE;
    }
    else //-ve colors
    {
      ref_coord = (root_coord - 1 + dim_size) % dim_size;
      tlink = NEGATIVE;
    }
  }
  else
  {
    tlink = MESH;
    ref_coord = root_coord + 1;
    if (ref_coord >= dim_size + COORD((*_ll), color_axis))
    {
      ref_coord = root_coord - 1;
    }
  }

  //fprintf(stderr, "ghost dump self_coord %ld ref_coord %ld data %d color %d\n",
  //  COORD(_self_coord, color_axis), ref_coord, data_movement,
  //  _color);

  // if I am a ghost node
  if (COORD(_self_coord, color_axis) == root_coord)
  {
    if (MY_TASK != _root)
      _iam_ghost[CCMI::REDUCE_OP] = 1;

    if (data_movement == SEND && MY_TASK != _root)
    {
      COORD(partner, color_axis) = ref_coord;
      NET2TASK(&partner, &partner_id, &type);
      //printf("Ghost: I am %zu and sending to %u at phase %d color %d dir %d\n", MY_TASK, partner_id, phase, _color, tlink);	        

      if (partner_id != MY_TASK) { 
	if (_torus_link[color_axis])
	  if(__global.mapping.torusSize(color_axis) > 2) 
	    tlink = MESH;
	
	if (tlink == MESH) 
	  new (topo) PAMI::Topology(partner_id);
	//torus links used
	else if (tlink == POSITIVE)
	  new (topo) PAMI::Topology(&_self_coord, &partner, 
				    &_self_coord, &tlink);
	else // tlink == NEGATIVE
	  new (topo) PAMI::Topology(&partner, &_self_coord,
				    &_self_coord, &tlink);
      }
    }
  }
    
  // The nodes that are different from the root in one dimension (not
  // the leading dimension of the color) are the ghosts. The nodes
  // just before them have to recv data from the ghosts.
  if (COORD(_self_coord, color_axis) == ref_coord &&
      data_movement == RECV)
  {
    COORD(partner, color_axis) = COORD(_root_coord, color_axis);
    unsigned i, is_root = 1;
    for (i = 0; i < _glb_ndims && is_root; i++)
      if (COORD(partner, i) != COORD(_root_coord, i))
        is_root = 0;
    
    // node that is adjacent to root does not recv
    if (is_root)
      return PAMI_SUCCESS;    

    COORD(partner, color_axis) = root_coord;
    
    NET2TASK(&partner, &partner_id, &type);
    if (partner_id != MY_TASK)
    {
      new (topo) PAMI::Topology(partner_id);
      //printf("Ghost: I am %zu and recving From %u at phase %d color %d\n", MY_TASK, partner_id, phase, _color);

    }
  }
  return PAMI_SUCCESS;
}




#endif // __pami_target_socklinux__
#endif
