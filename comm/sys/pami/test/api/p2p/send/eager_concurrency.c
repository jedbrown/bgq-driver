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
 * \file test/api/p2p/send/eager_concurrency.c
 *
 * \brief Multpiple processes send and recveive to/from all the rest.
 */
#include <unistd.h>

#define ITERATIONS	-1	/* Use our variable */
#define MINBUFSIZE	-1
#define BUFSIZE		-1
#include "send_concurrency.h" /* includes send_general.h */

int ITERATIONS = 100;
size_t MINBUFSIZE = 128;
size_t BUFSIZE = 128;

int main(int argc, char **argv) {
        int e = 0, x, y, n;
        size_t z;
        unsigned *results;
        int *nets;
        size_t *protocol;
        bool verify = false;
        bool graph = false;
        bool local = false;
        const char *dflt = "DEFAULT_NETWORK";
        pami_result_t result;

        extern int optind;
        extern char *optarg;

        while ((x = getopt(argc, argv, "d:e:gi:ls:v")) != EOF) {
                switch(x) {
                case 'd':
                        dflt = optarg;
                        break;
                case 'e':
                        BUFSIZE = strtoul(optarg, NULL, 0);
                        break;
                case 'g':
                        graph = true;
                        break;
                case 'i':
                        ITERATIONS = strtoul(optarg, NULL, 0);
                        break;
                case 'l':
                        local = true;
                        break;
                case 's':
                        MINBUFSIZE = strtoul(optarg, NULL, 0);
                        break;
                case 'v':
                        verify = true;
                        break;
                default:
usage:
                        fprintf(stderr, "Usage: %s [-v][-i iter][-s start][-e end] [network...]\n", argv[0]);
                        exit(1);
                        break;
                }
        }

        optind -= 1;
        argc -= optind;
        if (argc < 1) goto usage;
        if (MINBUFSIZE > BUFSIZE) {
                fprintf(stderr, "-s value (%zu) must be less than (or equal) -e value (%zu)\n", MINBUFSIZE, BUFSIZE);
                goto usage;
        }
        nets = (int *)malloc(argc * sizeof(*nets));
        protocol = (size_t *)malloc(argc * sizeof(*protocol));
        assert(nets);
        assert(protocol);

        pami_client_t client;
        pami_context_t context;

        result = PAMI_Client_create ("test", &client, NULL, 0);
        if (result != PAMI_SUCCESS) {
                fprintf (stderr, "Error. Unable to initialize pami client. result = %d\n", result);
                return 1;
        }

        result = PAMI_Context_createv(client, NULL, 0, &context, 1);
        if (result != PAMI_SUCCESS) {
                fprintf (stderr, "Error. Unable to create pami context. result = %d\n", result);
                return 1;
        }
        pami_configuration_t configuration;
        configuration.name = PAMI_CLIENT_TASK_ID;
        result = PAMI_Client_query(client, &configuration,1);
        if (result != PAMI_SUCCESS) {
                fprintf (stderr, "Error. Unable query configuration (%d). result = %d\n", configuration.name, result);
                return 1;
        }
        size_t task_id = configuration.value.intval;
        configuration.name = PAMI_CLIENT_NUM_TASKS;
        result = PAMI_Client_query(client, &configuration,1);
        if (result != PAMI_SUCCESS) {
                fprintf (stderr, "Error. Unable query configuration (%d). result = %d\n", configuration.name, result);
                return 1;
        }
        size_t num_tasks = configuration.value.intval;

        send_concurrency_init(BUFSIZE, num_tasks);
        pami_task_t ranks[64];
        size_t nranks;
        pami_task_t me;
        if (local) {
                /*nranks = setup_localpeers(ranks, 64, &me); */
                /* assume all local... */
                for (nranks = 0; nranks < num_tasks; ++nranks) ranks[nranks] = nranks;
                me = task_id;
        } else {
                nranks = num_tasks;
                me = task_id;
        }

        for (x = 0; x < argc; ++x) {
                const char *net = x ? argv[x + optind] : dflt;
                nets[x] = find_netw(net);
                if (nets[x] < 0) {
                        fprintf(stderr, "Invalid network: %s\n", net);
                        PAMI_abortf("%s<%d>\n", __FILE__, __LINE__);
                }
                con_setup_netw(nets[x], context, &protocol[x]);
        }

        /* Count number of buffer sizes being tested */
        for (z = MINBUFSIZE, n = 0; z <= BUFSIZE; z = NEXT_BUFSIZE(z)) ++n;

        results = (unsigned *)malloc(argc * n * sizeof(unsigned));
        if (me == 0) {
                /*PAMI_Coord_t addr; */
                size_t orig, *_orig;

                fprintf(stdout, "#\n");
                fprintf(stdout, "# PAMI_Send() eager concurrency test\n");
                for (orig = 0; orig < nranks; ++orig) {
                        if (local) {
                                _orig = &ranks[orig];
                        } else {
                                _orig = &orig;
                        }
                        /*rank2network(*_orig, PAMI_TORUS_NETWORK, &addr); */
                        /*fprintf(stdout, "# Rank %zu (%zu,%zu,%zu,%zu)\n", *_orig, */
                                /*addr.torus.x, addr.torus.y, addr.torus.z, addr.torus.t); */
                        fprintf(stdout, "# Rank %zu\n", *_orig);
                }
                /*fprintf(stdout, "# Clock MHz = %d, Iterations = %d\n", hw.clockMHz, ITERATIONS); */
                fprintf(stdout, "# Clock MHz = %d, Iterations = %d\n", 0, ITERATIONS);
                if (verify) {
                        fprintf(stdout, "# WARNING! Data verification is ON. Performance numbers are not realistic!\n");
                }
                fprintf(stdout, "#\n");
                fflush(stdout);
        }

        for (x = 0; x < argc; ++x) {
                if (0 && task_id == 0) {
                        fprintf(stdout, "# Running test on %s network...\n", name_netw(nets[x]));
                        fflush(stdout);
                }
                e |= con_test(protocol[x], client, context, me, (local ? ranks : NULL), nranks,
                                results + (x * n), (x == 0 && argc > 1 ? false : verify));
        }
        if (me == 0) {
                if (!graph) {
                        fprintf(stdout, "#         ");
                        for (x = 0; x < argc; ++x) {
                                fprintf(stdout, "%*s", 20 + (x ? 4 : 0), name_netw(nets[x]));
                        }
                        fprintf(stdout, "\n#    bytes");
                        for (x = 0; x < argc; ++x) {
                                fprintf(stdout, "    cycles  bytes/cy");
                                if (x) {
                                        fprintf(stdout, "   %%");
                                }
                        }
                        fprintf(stdout, "\n");
                }
                for (z = MINBUFSIZE, y = 0; z <= BUFSIZE; z = NEXT_BUFSIZE(z), ++y) {
                        fprintf(stdout, "%10zd", z);
                        for (x = 0; x < argc; ++x) {
                                int t = *(results + x * n + y);
                                if (graph) {
                                        printf(" %10d", t);
                                } else {
                                        double BW = (double)z / t;
                                        printf("%10d%10.6f", t, BW);
                                        if (x) {
                                                int t0 = *(results + 0 * n + y);
                                                double pct = (double)(t - t0) / t0;
                                                printf("%4.0f", pct * 100.0);
                                        }
                                }
                        }
                        fprintf(stdout, "\n");
                }
                fflush(stdout);
        }

        result = PAMI_Client_destroy(&client);
        if (result != PAMI_SUCCESS) {
                fprintf (stderr, "Error. Unable to finalize pami client. result = %d\n", result);
                return 1;
        }
        return e;
}
