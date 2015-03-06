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
 * \file test/tools/sched_test.cc
 * \brief Program to build schedules and display (or graph) them.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/stat.h>
//#define _GNU_SOURCE // not needed, it seems...
#include <getopt.h>

#include "fakeTorusMapping.h"
#include "algorithms/schedule/Rectangle.h"
#include "algorithms/schedule/JaynomialTree.h"
#include "algorithms/schedule/BinomialTree.h"
#include "algorithms/schedule/BinomialTree.cc"
#include "algorithms/schedule/TreeSchedule.h"
#include "algorithms/schedule/GiSchedule.h"
#include "algorithms/schedule/RingSchedule.h"
#include "algorithms/schedule/LockboxBarrierSchedule.h"

char *edges[] = {
  "red", "blue", "green", "yellow", "purple",
  "cyan", "gray", "orange", "magenta4", "brown",

  "red4", "blue4", "green4", "yellow4", "violet",
  "cyan4", "black", "orange4", "lightblue", "gold",
};

#define UNDEFINED	((unsigned)-1)
static char *dbg_ops(unsigned op)
{
  switch(op)
  {
  case CCMI_PT_TO_PT_SUBTASK: return "PT_TO_PT_SUBTASK";
  case CCMI_LINE_BCAST_XP: return "LINE_BCAST_XP";
  case CCMI_LINE_BCAST_XM: return "LINE_BCAST_XM";
  case CCMI_LINE_BCAST_YP: return "LINE_BCAST_YP";
  case CCMI_LINE_BCAST_YM: return "LINE_BCAST_YM";
  case CCMI_LINE_BCAST_ZP: return "LINE_BCAST_ZP";
  case CCMI_LINE_BCAST_ZM: return "LINE_BCAST_ZM";
  case CCMI_COMBINE_SUBTASK: return "COMBINE_SUBTASK";
  case CCMI_GI_BARRIER: return "GI_BARRIER";
  case CCMI_LOCKBOX_BARRIER: return "LOCKBOX_BARRIER";
  case CCMI_TREE_BARRIER: return "TREE_BARRIER";
  case CCMI_TREE_ALLREDUCE: return "TREE_ALLREDUCE";
  case CCMI_TREE_BCAST: return "TREE_BCAST";
  case CCMI_REDUCE_RECV_STORE: return "REDUCE_RECV_STORE";
  case CCMI_REDUCE_RECV_NOSTORE: return "REDUCE_RECV_NOSTORE";
  case CCMI_BCAST_RECV_STORE: return "BCAST_RECV_STORE";
  case CCMI_BCAST_RECV_NOSTORE: return "BCAST_RECV_NOSTORE";
  case CCMI_LOCALCOPY: return "LOCALCOPY";
  case UNDEFINED: return "RECV";
  default: return "???";
  }
}

typedef enum
{
  RECTANGLE_SCHED,
  BINOMIAL_SCHED,
  JAYNOMIAL_SCHED,
  TREE_SCHED,
  GI_SCHED,
  RING_SCHED,
  LOCKBOX_SCHED,
} sched_type;

typedef enum
{
  REDUCE_OP = CCMI::REDUCE_OP,
  BARRIER_OP = CCMI::BARRIER_OP,
  ALLREDUCE_OP = CCMI::ALLREDUCE_OP,
  BROADCAST_OP = CCMI::BROADCAST_OP,
} coll_type;

#define UNDEFINED_RANK	((unsigned)-1)

struct params
{
  int flag, dig, dsp, recv, quell, quiet;
  int verify;
  coll_type op;
  char *coll_str;
  sched_type type;
  char *type_str;
  CCMI::TorusCollectiveMapping *map;
  CCMI::Schedule::Schedule *sched;
  const CCMI::Schedule::Rectangle *rect;
  CCMI::Schedule::Color color;
  unsigned root;
  unsigned *ranks;
  unsigned nranks;
  unsigned rank0;
  char *title;
};

unsigned index2rank(struct params *pm, unsigned idx)
{
  if(idx < pm->nranks)
  {
    return(pm->ranks ? pm->ranks[idx] : idx + pm->rank0);
  }
  return UNDEFINED_RANK;
}

unsigned rank2index(struct params *pm, unsigned rank)
{
  unsigned x;

  if(pm->ranks)
  {
    for(x = 0; x < pm->nranks; ++x)
    {
      if(rank == pm->ranks[x])
      {
        return x;
      }
    }
  }
  else if(rank < pm->nranks)
  {
    return rank;
  }
  return UNDEFINED_RANK;
}

void dot_send(struct params *pm,
              char *subgraph, unsigned op,
              unsigned src, unsigned dst, unsigned ph)
{
  char *e = "magenta";
  if(ph < (sizeof(edges) / sizeof(edges[0]))) e = edges[ph];
  if(!pm->recv && (op & LINE_BCAST_MASK))
  {
    unsigned coords[CCMI_TORUS_NDIMS];
    unsigned r = src;
    if(subgraph)
    {
      printf("\"%s%d\"", subgraph, src);
    }
    else
    {
      printf("%d", src);
    }
    do
    {
      pm->map->Rank2Torus(coords, r);
      switch(op)
      {
      case CCMI_LINE_BCAST_XP: ++coords[CCMI_X_DIM]; break;
      case CCMI_LINE_BCAST_XM: --coords[CCMI_X_DIM]; break;
      case CCMI_LINE_BCAST_YP: ++coords[CCMI_Y_DIM]; break;
      case CCMI_LINE_BCAST_YM: --coords[CCMI_Y_DIM]; break;
      case CCMI_LINE_BCAST_ZP: ++coords[CCMI_Z_DIM]; break;
      case CCMI_LINE_BCAST_ZM: --coords[CCMI_Z_DIM]; break;
      }
      (void) pm->map->Torus2Rank(coords, &r);
      if(subgraph)
      {
        printf(" -> \"%s%d\"", subgraph, r);
      }
      else
      {
        printf(" -> %d", r);
      }
    } while(r != dst);
    printf(" [ label=%d,color=\"%s\" ]", ph, e);
  }
  else
  {
    if(subgraph)
    {
      printf("\"%s%d\" -> \"%s%d\""
             " [ label=%d,color=\"%s\" ]",
             subgraph, src, subgraph, dst, ph, e);
    }
    else
    {
      printf("%d -> %d [ label=%d,color=\"%s\" ]",
             src, dst, ph, e);
    }
  }
  printf(";\n");
}

static char *dbg_colors(unsigned c)
{
  switch(c)
  {
  case CCMI::Schedule::XP_Y_Z: return "A";
  case CCMI::Schedule::YP_Z_X: return "B";
  case CCMI::Schedule::ZP_X_Y: return "C";
  case CCMI::Schedule::XN_Y_Z: return "D";
  case CCMI::Schedule::YN_Z_X: return "E";
  case CCMI::Schedule::ZN_X_Y: return "F";
  default: return "???";
  }
}

/* mapping must already be setup */
unsigned get_root(struct params *pm, char *s)
{
  unsigned coords[CCMI_TORUS_NDIMS];
  int i;
  if(strchr(s, ','))
  {
    i = sscanf(s, "(%d,%d,%d,%d)", &coords[CCMI_X_DIM], &coords[CCMI_Y_DIM], &coords[CCMI_Z_DIM], &coords[CCMI_T_DIM]);
  }
  else
  {
    i = sscanf(s, "(%d %d %d %d)", &coords[CCMI_X_DIM], &coords[CCMI_Y_DIM], &coords[CCMI_Z_DIM], &coords[CCMI_T_DIM]);
  }
  if(i != 4)
  {
    return((unsigned)-1);
  }
  if(coords[CCMI_X_DIM] < pm->rect->x0) coords[CCMI_X_DIM] = pm->rect->x0;if(coords[CCMI_X_DIM] >= pm->rect->xs) coords[CCMI_X_DIM] = pm->rect->xs - 1;
  if(coords[CCMI_Y_DIM] < pm->rect->y0) coords[CCMI_Y_DIM] = pm->rect->y0;if(coords[CCMI_Y_DIM] >= pm->rect->ys) coords[CCMI_Y_DIM] = pm->rect->ys - 1;
  if(coords[CCMI_Z_DIM] < pm->rect->z0) coords[CCMI_Z_DIM] = pm->rect->z0;if(coords[CCMI_Z_DIM] >= pm->rect->zs) coords[CCMI_Z_DIM] = pm->rect->zs - 1;
  if(coords[CCMI_T_DIM] < pm->rect->t0) coords[CCMI_T_DIM] = pm->rect->t0;if(coords[CCMI_T_DIM] >= pm->rect->ts) coords[CCMI_T_DIM] = pm->rect->ts - 1;
  unsigned rank;
  (void) pm->map->Torus2Rank(coords, &rank);
  return rank;
}

void print_sizes(sched_type type, coll_type op)
{
  if(type == JAYNOMIAL_SCHED)
  {
    printf("Sizeof(JaynomialTreeSchedule) = %d\n", sizeof(CCMI::Schedule::JaynomialTreeSchedule));
  }
  else if(type == BINOMIAL_SCHED)
  {
    printf("Sizeof(BinomialTreeSchedule) = %d\n", sizeof(CCMI::Schedule::BinomialTreeSchedule));
  }
  else if(type == TREE_SCHED)
  {
    printf("Sizeof(TreeSchedule) = %d\n", sizeof(CCMI::Schedule::TreeSchedule));
  }
  else if(type == GI_SCHED)
  {
    printf("Sizeof(GiSchedule) = %d\n", sizeof(CCMI::Schedule::GiSchedule));
  }
  else if(type == RING_SCHED)
  {
    printf("Sizeof(RingSchedule) = %d\n", sizeof(CCMI::Schedule::RingSchedule));
  }
  else if(type == LOCKBOX_SCHED)
  {
    printf("Sizeof(LockboxBarrierSchedule) = %d\n", sizeof(CCMI::Schedule::LockboxBarrierSchedule));
  }
  else /* if (type == RECTANGLE_SCHED) */
  {
    printf("Sizeof(OneColorRectangle) = %d\n", sizeof(CCMI::Schedule::OneColorRectangle));
#ifdef HAVE_NEW_RECT_SCHED
    if(op == ALLREDUCE_OP || op == BROADCAST_OP)
    {
      printf("Sizeof(OneColorRectBcastSched) = %d\n", sizeof(CCMI::Schedule::OneColorRectBcastSched));
    }
    if(op == ALLREDUCE_OP || op == REDUCE_OP)
    {
      printf("Sizeof(OneColorRectRedSched) = %d\n", sizeof(CCMI::Schedule::OneColorRectRedSched));
    }
    if(op == ALLREDUCE_OP)
    {
      printf("Sizeof(OneColorRectAllredSched) = %d\n", sizeof(CCMI::Schedule::OneColorRectAllredSched));
    }
//		if (op == BARRIER_OP) {
//			printf("Sizeof(OneColorRect???) = %d\n", sizeof(CCMI::Schedule::OneColorRect???));
//		}
#endif /* HAVE_NEW_RECT_SCHED */
  }
}

char *rank2string(unsigned rank)
{
  static char buf[BUFSIZ];  // CAUTION! static return value!
  sprintf(buf, "[%d]", rank);
  return buf;
}

void do_src_pes(struct params *pm, unsigned ph, int &count)
{
  unsigned dstpes[8];
  unsigned tasks[8];
  unsigned ndst;
  unsigned i;

  memset(tasks, UNDEFINED, sizeof(tasks));
  pm->sched->getSrcPeList(ph, dstpes, ndst, tasks);
  if(!pm->dig && !pm->verify && (!pm->quell || ndst) && !count)
  {
    printf("%2d %3d", ph, pm->map->rank());
    ++count;
  }
  for(i = 0; i < ndst; ++i)
  {
    ++count;
    if(pm->dig)
    {
      if(pm->recv)
      {
        dot_send(pm, NULL,
                 tasks[i], dstpes[i],
                 pm->map->rank(), ph);
      }
    }
    else if(pm->verify)
    {
      printf("{\n"
             "'rank' => '%d',\n"
             "'phase' => '%d',\n"
             "'dst' => undef,\n"
             "'src' => '%d',\n"
             "'op' => '%s',\n"
             "},\n",
             pm->map->rank(), ph,
             dstpes[i], dbg_ops(tasks[i]));
    }
    else
    {
      printf(" <%d,%s>", dstpes[i], dbg_ops(tasks[i]));
    }
  }
}

void do_dst_pes(struct params *pm, unsigned ph, int &count)
{
  unsigned dstpes[8];
  unsigned tasks[8];
  unsigned ndst;
  unsigned i;

  pm->sched->getDstPeList(ph, dstpes, ndst, tasks);
  if(pm->dig && !ndst) return;
  if(!pm->dig && !pm->verify && (!pm->quell || ndst) && !count)
  {
    printf("%2d %3d", ph, pm->map->rank());
    ++count;
  }
  for(i = 0; i < ndst; ++i)
  {
    ++count;
    if(pm->dig)
    {
      if(!pm->recv)
      {
        dot_send(pm, NULL, tasks[i],
                 pm->map->rank(), dstpes[i], ph);
      }
    }
    else if(pm->verify)
    {
      printf("{\n"
             "'rank' => '%d',\n"
             "'phase' => '%d',\n"
             "'dst' => '%d',\n"
             "'src' => undef,\n"
             "'op' => '%s',\n"
             "},\n",
             pm->map->rank(), ph,
             dstpes[i], dbg_ops(tasks[i]));
    }
    else
    {
      printf(" (%d,%s)", dstpes[i], dbg_ops(tasks[i]));
    }
  }
}

void get_sched(struct params *pm, void *mem)
{
  if(pm->type == JAYNOMIAL_SCHED)
  {
    if(!pm->flag)
    {
      pm->sched = new(mem) CCMI::Schedule::JaynomialTreeSchedule((CCMI::CollectiveMapping *)pm->map, pm->rect->xs, pm->ranks);
    }
    else
    {
#ifdef HAVE_BINO_LINE_SCHED
      pm->sched = new(mem) CCMI::Schedule::JaynomialTreeSchedule(pm->map->x(), pm->rect->x0, pm->rect->x0 + pm->rect->xs - 1);
#endif /* HAVE_BINO_LINE_SCHED */
    }
  }
  else if(pm->type == BINOMIAL_SCHED)
  {
    if(!pm->flag)
    {
      pm->sched = new(mem) CCMI::Schedule::BinomialTreeSchedule((CCMI::CollectiveMapping *)pm->map, pm->nranks, pm->ranks);
    }
    else
    {
#ifdef HAVE_BINO_LINE_SCHED
      pm->sched = new(mem) CCMI::Schedule::BinomialTreeSchedule(pm->map->x(), pm->rect->x0, pm->rect->x0 + pm->rect->xs - 1);
#endif /* HAVE_BINO_LINE_SCHED */
    }
  }
  else if(pm->type == TREE_SCHED)
  {
    pm->sched = new(mem) CCMI::Schedule::TreeSchedule(pm->map, pm->nranks, pm->ranks);
  }
  else if(pm->type == GI_SCHED)
  {
    pm->sched = new(mem) CCMI::Schedule::GiSchedule(pm->map, pm->nranks, pm->ranks);
  }
  else if(pm->type == RING_SCHED)
  {
    pm->sched = new(mem) CCMI::Schedule::RingSchedule(pm->map, pm->nranks, pm->ranks);
  }
  else if(pm->type == LOCKBOX_SCHED)
  {
    pm->sched = new(mem) CCMI::Schedule::LockboxBarrierSchedule(pm->map, pm->nranks, pm->ranks);
  }
  else /* if (pm->type == RECTANGLE_SCHED) */
  {
#ifdef HAVE_NEW_RECT_SCHED
    if(pm->op == ALLREDUCE_OP)
    {
      pm->sched = new(mem) CCMI::Schedule::OneColorRectAllredSched(pm->map, pm->color, *pm->rect);
    }
    else if(pm->op == REDUCE_OP)
    {
      pm->sched = new(mem) CCMI::Schedule::OneColorRectRedSched(pm->map, pm->color, *pm->rect);
//		} else if (pm->op == BARRIER_OP) {
//			pm->sched = new(mem) CCMI::Schedule::OneColorRect???(pm->map, pm->color, *pm->rect);
    }
    else /* if (pm->op == BROADCAST_OP) */
    {
      pm->sched = new(mem) CCMI::Schedule::OneColorRectBcastSched(pm->map, pm->color, *pm->rect);
    }
#else /* ! HAVE_NEW_RECT_SCHED */
    pm->sched = new(mem) CCMI::Schedule::OneColorRectangle(pm->map, pm->color, *pm->rect);
#endif /* ! HAVE_NEW_RECT_SCHED */
  }
}

char *argv0;

void usage(int ret)
{
  fprintf(stderr,
          "Usage: %s [-d] [-B|-R|-T|-G|-O|-C] [-a|-b|-r|-e] [opts] <args>\n"
          "Where:\n"
          "-h|--help		This help string\n"
          "-d|--dot|--digraph	produce output for dot (sends)\n"
          "-v|--recv		Use recvs for dot output\n"
          "-R|--rectangle		Rectangle schedule\n"
          "-J|--jaynomial		Jaynomial schedule\n"
          "-O|--ring		Ring schedule\n"
          "-P|--lockbox		Lockbox barrier schedule\n"
          "-B|--binomial		Binomial schedule\n"
          "-T|--tree		Tree schedule\n"
          "-G|--gi		Global Interrupt schedule\n"
          "-a|--allreduce		Allreduce\n"
          "-b|--broadcast		Broadcast\n"
          "-e|--barrier		Barrier\n"
          "-r|--reduce		Reduce\n"
          "Opts:\n"
          "-L|--ranks file		Use <file> as source of mappings (list\n"
          "			of ranks). Cannot be used with --rectangle.\n"
          "			Each line in file is <x> <y> <z> <t>\n"
          "			\"file\" may also be a string {x y z t\\n...}\n"
          "-x|--noranks		(-B/-J) Do NOT use real ranks[]\n"
          "-s|--space|--dblspc	(!-d) Add space between ranks output\n"
          "-V|--verify		Format output for use with sched_verify and other programs\n"
          "-Q|--quell		Quell empty phases in output\n"
          "-t|--title string	Prepend <string> to labels/titles\n"
          "Rectangle args:\n"
          "	<rect>	File containing rectangle description.\n"
          "		File format: two lines, each with 4 numbers,\n"
          "		of <x0> <y0> <z0> <t0>\\n<xs> <ys> <zs> <ts>\n"
          "		Optional third line with whole partition size\n"
          "		makes mappings more realistic.\n"
          "		Also accepts eight (or 12) numbers enclosed in braces\n"
          "		instead of file name '{x0 y0 z0 t0 xs ys zs ts}'.\n"
          "		Negative xs,ys,zs indicate torus links on those axii.\n"
          "	<root>	Root node, rank or '(x,y,z,t)'\n"
          "	<color>	Color, 1-6 or A-F\n"
          "Bi/Jay-nomial, Tree, Ring, Lockbox and GI args:\n"
          "	<root>	Root node, index\n"
          "	<min>	Minimum index of range\n"
          "	<max>	Maximum index of range\n"
          "	-or-\n"
          "	-L <file>	Ranks list file\n"
          "	<root>		Root node, index into ranks list\n"
          "Examples:\n"
          "Basic schedule output for a Rectangle Broadcast:\n"
          "	%s -R -b '{0 0 0 0 4 4 2 1}' 0 A\n"
          "Display schedule output for a Tree allreduce using a rank file:\n"
          "	%s --tree --allreduce --ranks rankfile 0\n"
          "Display a graph of Binomial Reduce schedule:\n"
          "	%s -d -B -r 0 0 7 | dot -Tpng >file; gimp file\n"
          "Verify data in a Binomial Reduce schedule:\n"
          "	%s -V --binomial --reduce 0 0 7 | ./sched_verify -d\n"
          "Display Binomial Reduce graph with per-node subgraphs:\n"
          "	%s -V --binomial --allreduce 0 0 7 | ./digraph.pl -A\n"
          ,
          argv0, argv0, argv0, argv0, argv0, argv0);
  exit(ret);
}

void mk_title(struct params *pm, char *title, int len)
{
  char *s = title;
  int n;
  n = snprintf(s, len, "%d-node", pm->nranks);
  len -= n;
  s += n;
  if(pm->title && len > 0)
  {
    n = snprintf(s, len, " %s", pm->title);
    len -= n;
    s += n;
  }
  if(pm->type == RECTANGLE_SCHED && len > 0)
  {
    n = snprintf(s, len, " Color '%s'", dbg_colors(pm->color));
    len -= n;
    s += n;
  }
  if(len > 0)
  {
    n = snprintf(s, len, " Root %d %s %s", pm->root, pm->type_str, pm->coll_str);
    len -= n;
    s += n;
  }
  if(!pm->verify && len > 0)
  {
    n = snprintf(s, len, " (%s)", pm->recv ? "receives" : "sends");
    len -= n;
    s += n;
  }
}

int main(int argc, char **argv)
{
  int x;
  char *rank_list = NULL;
  FILE *fp;
  int start, nphases, max;
  int p;
  static struct params pm = {0};
  char title[BUFSIZ];
  unsigned __x, __y, __z, __t, __r;
  CCMI::Schedule::Rectangle _rect;
  static struct option lopts[] = {
    {"help",  no_argument,    NULL, 'h'},
    {"jaynomial", no_argument,    NULL, 'J'},
    {"binomial",  no_argument,    NULL, 'B'},
    {"rectangle", no_argument,    NULL, 'R'},
    {"tree",  no_argument,    NULL, 'T'},
    {"gi",    no_argument,    NULL, 'G'},
    {"ring",    no_argument,    NULL, 'O'},
    {"lockbox",   no_argument,    NULL, 'C'},
    {"allreduce", no_argument,    NULL, 'a'},
    {"broadcast", no_argument,    NULL, 'b'},
    {"barrier", no_argument,    NULL, 'e'},
    {"reduce",  no_argument,    NULL, 'r'},
    {"dot",   no_argument,    NULL, 'd'},
    {"digraph", no_argument,    NULL, 'd'},
    {"dblspc",  no_argument,    NULL, 's'},
    {"space", no_argument,    NULL, 's'},
    {"recv",  no_argument,    NULL, 'v'},
    {"noranks", no_argument,    NULL, 'x'},
    {"verify",  no_argument,    NULL, 'V'},
    {"quell", no_argument,    NULL, 'Q'},
    {"quiet", no_argument,    NULL, 'q'},
    {"ranks", required_argument,  NULL, 'L'},
    {"title", required_argument,  NULL, 't'},
    {NULL, 0, NULL, 0},
  };

  extern int optind;
  extern char *optarg;

  pm.op = REDUCE_OP;
  pm.coll_str = "Reduce";
  pm.type = RECTANGLE_SCHED;
  pm.type_str = "Rectangle";

  argv0 = argv[0];
  while((x = getopt_long(argc, argv,
                         "abBCdeGhJLO:qQrRst:TvxV", lopts, NULL)) != EOF)
  {
    switch(x)
    {
    case 'a':
      pm.op = ALLREDUCE_OP;
      pm.coll_str = "Allreduce";
      break;
    case 'b':
      pm.op = BROADCAST_OP;
      pm.coll_str = "Broadcast";
      break;
    case 'B':
      pm.type = BINOMIAL_SCHED;
      pm.type_str = "Binomial";
      break;
    case 'd':
      pm.dig++;
      break;
    case 'e':
      pm.op = BARRIER_OP;
      pm.coll_str = "Barrier";
      break;
    case 'G':
      pm.type = GI_SCHED;
      pm.type_str = "Global Interrupt";
      break;
    case 'O':
      pm.type = RING_SCHED;
      pm.type_str = "Ring";
      break;
    case 'C':
      pm.type = LOCKBOX_SCHED;
      pm.type_str = "Lockbox";
      break;
    case 'J':
      pm.type = JAYNOMIAL_SCHED;
      pm.type_str = "Jaynomial";
      break;
    case 'L':
      rank_list = optarg;
      break;
    case 'q':
      pm.quiet++;
      break;
    case 'Q':
      pm.quell++;
      break;
    case 'r':
      pm.op = REDUCE_OP;
      pm.coll_str = "Reduce";
      break;
    case 'R':
      pm.type = RECTANGLE_SCHED;
      pm.type_str = "Rectangle";
      break;
    case 's':
      pm.dsp++;
      break;
    case 't':
      pm.title = optarg;
      break;
    case 'T':
      pm.type = TREE_SCHED;
      pm.type_str = "Tree";
      break;
    case 'V':
      pm.verify++;
      break;
    case 'v':
      pm.recv++;
      break;
    case 'x':
      pm.flag++;
      break;
    default:
      fprintf(stderr," Unknown %c\n",x);
      usage(1);
    }
  }
  pm.rect = &_rect;
  if(pm.type != BINOMIAL_SCHED && pm.type != JAYNOMIAL_SCHED)
  {
    pm.flag = 0;
  }

  if(pm.type == BINOMIAL_SCHED  ||
     pm.type == TREE_SCHED      ||
     pm.type == JAYNOMIAL_SCHED ||
     pm.type == RING_SCHED ||
     pm.type == LOCKBOX_SCHED ||
     pm.type == GI_SCHED)
  {
    if(rank_list)
    {
      struct stat stb;
      int n;
      char *s;
      unsigned cc[CCMI_TORUS_NDIMS];

      if(argc - optind != 1)
      {
        fprintf(stderr," (argc(%d) - optind(%d) != 1)\n",argc, optind);
        usage(1);
      }
      if(*rank_list == '{')
      {
        ++rank_list;  // skip '{'
        stb.st_size = strlen(rank_list);
        if(rank_list[stb.st_size-1] == '}')
        {
          --stb.st_size;
        }
        pm.ranks = (unsigned *)rank_list;
      }
      else
      {
        fp = fopen(rank_list, "r");
        if(!fp)
        {
          perror(rank_list);
          exit(1);
        }
        fstat(fileno(fp), &stb);
        pm.ranks = (unsigned *)malloc(stb.st_size + 1);
        if(!pm.ranks)
        {
          perror("malloc");
          exit(1);
        }
        fread(pm.ranks, stb.st_size, 1, fp);
        fclose(fp);
      }
      // At first, we keep _rect.?s as "max value + 1"
      // This works fine later, as TorusCollectiveMapping is always
      // based at 0,0,0,0.
      _rect = (CCMI::Schedule::Rectangle)
      {
        (unsigned)-1, (unsigned)-1, (unsigned)-1, (unsigned)-1, 0, 0, 0, 0
      };
      n = 0;
      s = (char *)pm.ranks;
      s[stb.st_size] = '\0';
#define RANK_PACK(c)	(((c[CCMI_X_DIM]) << 24)|((c[CCMI_Y_DIM]) << 16)|((c[CCMI_Z_DIM]) << 8)|(c[CCMI_T_DIM]))
#define RANK_UNPACK(r, c)	{	\
        (c[CCMI_X_DIM]) = (((r) >> 24) & 0x00ff);		\
        (c[CCMI_Y_DIM]) = (((r) >> 16) & 0x00ff);		\
        (c[CCMI_Z_DIM]) = (((r) >> 8) & 0x00ff);		\
        (c[CCMI_T_DIM]) = ((r) & 0x00ff);			\
}
      while(*s)
      {
        x = sscanf(s, "%d %d %d %d\n", &cc[CCMI_X_DIM], &cc[CCMI_Y_DIM], &cc[CCMI_Z_DIM], &cc[CCMI_T_DIM]);
        if(x != 4)
        {
          fprintf(stderr, "Format error: %s\n", rank_list); exit(1);
        }
        while(*s && *s++ != '\n');
        if(cc[CCMI_X_DIM] < _rect.x0)
        {
          _rect.x0 = cc[CCMI_X_DIM];
        }
        if(cc[CCMI_Y_DIM] < _rect.y0)
        {
          _rect.y0 = cc[CCMI_Y_DIM];
        }
        if(cc[CCMI_Z_DIM] < _rect.z0)
        {
          _rect.z0 = cc[CCMI_Z_DIM];
        }
        if(cc[CCMI_T_DIM] < _rect.t0)
        {
          _rect.t0 = cc[CCMI_T_DIM];
        }
        if(cc[CCMI_X_DIM] >= _rect.xs)
        {
          _rect.xs = cc[CCMI_X_DIM] + 1;
        }
        if(cc[CCMI_Y_DIM] >= _rect.ys)
        {
          _rect.ys = cc[CCMI_Y_DIM] + 1;
        }
        if(cc[CCMI_Z_DIM] >= _rect.zs)
        {
          _rect.zs = cc[CCMI_Z_DIM] + 1;
        }
        if(cc[CCMI_T_DIM] >= _rect.ts)
        {
          _rect.ts = cc[CCMI_T_DIM] + 1;
        }
        pm.ranks[n++] = RANK_PACK(cc);
      }
      pm.nranks = n;
      // convert "max coord + 1" to relative size
      _rect.xs -= _rect.x0;
      _rect.ys -= _rect.y0;
      _rect.zs -= _rect.z0;
      _rect.ts -= _rect.t0;
      pm.map = new CCMI::TorusCollectiveMapping::TorusCollectiveMapping(
                                                   pm.rect->xs, pm.rect->ys, pm.rect->zs, pm.rect->ts,
                                                   pm.rect->x0, pm.rect->y0, pm.rect->z0, pm.rect->t0,
                                                   pm.rect->x0, pm.rect->y0, pm.rect->z0, pm.rect->t0);
      for(x = 0; x < n; ++x)
      {
        RANK_UNPACK(pm.ranks[x], cc);
        (void) pm.map->Torus2Rank(cc, &pm.ranks[x]);
      }
      if(isdigit(argv[optind][0]))
      {
        pm.root = strtoul(argv[optind], NULL, 0);
      }
      else
      {
        pm.root = get_root(&pm, argv[optind]);
        if(pm.root == ((unsigned)-1))
        {
          fprintf(stderr, "Invalid root node coords: %s\n", argv[optind + 1]);
          exit(1);
        }
      }
#undef RANK_PACK
#undef RANK_UNPACK
    }
    else  // not rank list
    {
      if(argc - optind != 3)
      {
        fprintf(stderr," (argc(%d) - optind(%d) != 3)\n",argc, optind);
        usage(1);
        /* NOTREACHED */
      }
      pm.root = strtoul(argv[optind++], NULL, 0); // root node rank
      _rect.x0 = strtoul(argv[optind++], NULL, 0);  // first node rank
      _rect.xs = strtoul(argv[optind++], NULL, 0);  // last node rank
      // convert max coord to relative size
      _rect.xs = _rect.xs - _rect.x0 + 1;
      _rect.y0 = _rect.z0 = _rect.t0 = 0;
      _rect.ys = _rect.zs = _rect.ts = 1;
      pm.map = new CCMI::TorusCollectiveMapping::TorusCollectiveMapping(
                                                   pm.rect->xs, pm.rect->ys, pm.rect->zs, pm.rect->ts,
                                                   pm.rect->x0, pm.rect->y0, pm.rect->z0, pm.rect->t0,
                                                   pm.rect->x0, pm.rect->y0, pm.rect->z0, pm.rect->t0);
      pm.nranks = _rect.xs;
      if(!pm.flag)
      {
        pm.ranks = (unsigned *)malloc(pm.nranks * sizeof(unsigned));
        if(!pm.ranks)
        {
          perror("malloc");
          exit(1);
        }
        for(__x = 0; __x < pm.nranks; ++__x)
        {
          pm.ranks[__x] = _rect.x0 + __x;
        }
      }
    }
    if(!pm.dig && !pm.verify && !pm.quiet)
    {
      printf("Root node is %d\n", pm.root);
    }
  }
  else /* if (pm.type == RECTANGLE_SCHED) */
  {
    unsigned xs, ys, zs, ts;

    if(argc - optind != 3)
    {
      fprintf(stderr,"b (argc(%d) - optind(%d) != 3)\n",argc, optind);
      usage(1);
    }
    // All rectangle formats use _rect.?s as size
    if(argv[optind][0] == '{')
    {
      x = sscanf(argv[optind], "{%d %d %d %d %d %d %d %d %d %d %d %d}",
                 &_rect.x0, &_rect.y0, &_rect.z0, &_rect.t0,
                 &_rect.xs, &_rect.ys, &_rect.zs, &_rect.ts,
                 &xs, &ys, &zs, &ts);
      if(x != 12)
      {
        x = sscanf(argv[optind], "{%d %d %d %d %d %d %d %d}",
                   &_rect.x0, &_rect.y0, &_rect.z0, &_rect.t0,
                   &_rect.xs, &_rect.ys, &_rect.zs, &_rect.ts);
        if(x != 8)
        {
          fprintf(stderr, "Rectangle format error: '%s'\n", argv[optind]);
          exit(1);
        }
        xs = _rect.x0 + abs((int)_rect.xs);
        ys = _rect.y0 + abs((int)_rect.ys);
        zs = _rect.z0 + abs((int)_rect.zs);
        ts = _rect.t0 + abs((int)_rect.ts);
      }
    }
    else
    {
      fp = fopen(argv[optind], "r");
      if(!fp)
      {
        perror(argv[optind]);
        exit(1);
      }
      x = fscanf(fp, "%d %d %d %d\n", &_rect.x0, &_rect.y0, &_rect.z0, &_rect.t0);
      if(x != 4)
      {
        fprintf(stderr, "Format error: %s\n", argv[optind]); exit(1);
      }
      x = fscanf(fp, "%d %d %d %d\n", &_rect.xs, &_rect.ys, &_rect.zs, &_rect.ts);
      if(x != 4)
      {
        fprintf(stderr, "Format error: %s\n", argv[optind]); exit(1);
      }
      x = fscanf(fp, "%d %d %d %d\n", &xs, &ys, &zs, &ts);
      if(x == EOF)
      {
        xs = _rect.x0 + abs((int)_rect.xs);
        ys = _rect.y0 + abs((int)_rect.ys);
        zs = _rect.z0 + abs((int)_rect.zs);
        ts = _rect.t0 + abs((int)_rect.ts);
      }
      else if(x != 4)
      {
        fprintf(stderr, "Format error: %s\n", argv[optind]); exit(1);
      }
      fclose(fp);
    }
    _rect.isTorusX = ((int)_rect.xs < 0);
    _rect.isTorusY = ((int)_rect.ys < 0);
    _rect.isTorusZ = ((int)_rect.zs < 0);
    _rect.isTorusT = 1;
    _rect.xs = abs((int)_rect.xs);
    _rect.ys = abs((int)_rect.ys);
    _rect.zs = abs((int)_rect.zs);
    _rect.ts = abs((int)_rect.ts);
    pm.map = new CCMI::TorusCollectiveMapping::TorusCollectiveMapping(
                                                 xs, ys, zs, ts,
                                                 0, 0, 0, 0,
                                                 pm.rect->x0, pm.rect->y0, pm.rect->z0, pm.rect->t0);
    pm.nranks = (_rect.xs * _rect.ys * _rect.zs * _rect.ts);
    if(isdigit(argv[optind + 1][0]))
    {
      pm.root = strtoul(argv[optind + 1], NULL, 0);
    }
    else
    {
      pm.root = get_root(&pm, argv[optind + 1]);
      if(pm.root == ((unsigned)-1))
      {
        fprintf(stderr, "Invalid root node: %s\n", argv[optind + 1]);
        exit(1);
      }
    }
    if(isdigit(argv[optind + 2][0]))
    {
      pm.color = (CCMI::Schedule::Color)strtoul(argv[optind + 2], NULL, 0);
    }
    else
    {
      pm.color = (CCMI::Schedule::Color)(argv[optind + 2][0] - 'A' + 1);
    }
    if(!pm.dig && !pm.verify && !pm.quiet)
    {
      int num, max, k;
      CCMI::Schedule::Color colors[10];
      CCMI::Schedule::OneColorRectangle::getColors(*pm.rect, num, max, colors);
      printf("Usable colors: {");
      for(k = 0; k < max; ++k)
      {
        if(k == num) printf(" ::");
        printf(" %s", dbg_colors(colors[k]));
      }
      printf("}, Using color %s, Root node is %d\n",
             dbg_colors(pm.color), pm.root);
    }
  }
  pm.rank0 = pm.map->rank();
  mk_title(&pm, title, sizeof(title));

  if(pm.dig)
  {
    printf("digraph \"%s\" {\n", title);
    printf("label=\"%s\";\n", title);
  }
  else if(pm.verify)
  {
    printf("$NUM_RANKS = %d;\n", pm.nranks);
    printf("$OP = \"%s\";\n", pm.coll_str);
    printf("$ALGORITHM = \"%s\";\n", pm.type_str);
    printf("$TITLE = \"%s\";\n", title);
    printf("$ROOT = %d;\n", pm.root);
    printf("$RECT = {\n"
           "'xs' => '%d',\n"
           "'ys' => '%d',\n"
           "'zs' => '%d',\n"
           "'ts' => '%d',\n"
           "'x0' => '%d',\n"
           "'y0' => '%d',\n"
           "'z0' => '%d',\n"
           "'t0' => '%d',\n"
           "};\n",
           pm.rect->xs, pm.rect->ys, pm.rect->zs, pm.rect->ts,
           pm.rect->x0, pm.rect->y0, pm.rect->z0, pm.rect->t0);
    printf("$SCHED = [\n");
  }
  else if(!pm.quiet)
  {
    print_sizes(pm.type, pm.op);
  }
  for(__r = 0; __r < pm.nranks; ++__r)
  {
    static unsigned _sched[256];

    pm.map->reset(index2rank(&pm, __r));
    __x = pm.map->x();
    __y = pm.map->y();
    __z = pm.map->z();
    __t = pm.map->t();
    if(__x < pm.rect->x0 || __x >= pm.rect->x0 + pm.rect->xs ||
       __y < pm.rect->y0 || __y >= pm.rect->y0 + pm.rect->ys ||
       __z < pm.rect->z0 || __z >= pm.rect->z0 + pm.rect->zs ||
       __t < pm.rect->t0 || __t >= pm.rect->t0 + pm.rect->ts)
    {
      continue;
    }

    if(pm.verify)
    {
      printf("{\n"
             "'x' => '%d',\n"
             "'y' => '%d',\n"
             "'z' => '%d',\n"
             "'t' => '%d',\n"
             "'rank' => '%d',\n",
             __x, __y, __z, __t,
             pm.map->rank());
    }
    else if(!pm.dig && pm.dsp)
    {
      printf("\n");
    }

    get_sched(&pm, _sched);
    pm.sched->init(pm.root, pm.op, start, nphases, max);
    if(pm.verify)
    {
      printf("'startph' => '%d',\n"
             "'nphases' => '%d',\n"
             "'phases' => [\n",
             start, nphases);
      for(p = 0; p < start; ++p)
      {
        printf("[ ],\n");
      }
    }
    for(p = 0; p < nphases; ++p)
    {
      int count = 0;
      if(pm.verify)
      {
        printf("[\n");
      }
      do_src_pes(&pm, p + start, count);
      do_dst_pes(&pm, p + start, count);
      if(pm.verify)
      {
        printf("],\n");
      }
      else if(!pm.dig && count)
      {
        printf("\n");
      }
    }
    if(pm.verify)
    {
      printf("],\n},\n");
    }
  }
  if(pm.verify)
  {
    printf("];\n");
  }
  else if(pm.dig)
  {
    printf("};\n");
  }
  exit(0);
}
