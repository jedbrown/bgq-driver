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
/* (C) Copyright IBM Corp.  2013                                    */
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

#include "server/job/class_route/Coordinates.h"
#include "server/job/class_route/Dimension.h"
#include "server/job/class_route/Mapping.h"
#include "server/job/class_route/Rectangle.h"

#include "common/Environment.h"
#include "common/JobInfo.h"
#include "common/logging.h"
//#include "common/SubBlock.h"

#include <log4cxx/consoleappender.h>
#include <log4cxx/patternlayout.h>
#include <log4cxx/basicconfigurator.h>

#include <utility/include/Log.h>
#include <utility/include/Properties.h>
#include <utility/include/version.h>

#include <fcntl.h> // need open()

#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>
#include <boost/scoped_array.hpp>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

#include <boost/numeric/conversion/cast.hpp>

#include <iostream>
#include <set>
#include <string>
#include <vector>

#include <errno.h>
#include <stdio.h>
#include <strings.h>
#include <time.h>

//using namespace runjob;
using namespace std;

#define NUMDIMENSION     6

///
/// \brief Blue Gene/Q coordinate structure
///
/// This structure takes 32-bits on any 32/64 bit system. The a, b, c,
/// and d fields are the same size and in the same location as the MU
/// descriptor structure. The thread/core fields are sized for 16 cores
/// with 4 hardware threads each, though the reserved bit can be stolen
/// for the 17th core if it needs a rank. The e dimension is sized to the
/// current node-layout maximum, though the MU hardware supports the full
/// 6 bits.
///
/// \see MUHWI_Destination_t
///
typedef struct
{
    /*
    uint32_t e        : 1; ///< Torus 'e' dimension, two nodes per node card
    uint32_t reserved : 1; ///< Reserved - possibly to identify the 17th core
    uint32_t a        : 6; ///< Torus 'a' dimension
    uint32_t b        : 6; ///< Torus 'b' dimension
    uint32_t c        : 6; ///< Torus 'c' dimension
    uint32_t d        : 6; ///< Torus 'd' dimension
    uint32_t t        : 6; ///< Hardware thread id, 4 threads per core
        */
    uint32_t e        ; ///< Torus 'e' dimension, two nodes per node card
    uint32_t reserved ; ///< Reserved - possibly to identify the 17th core
    uint32_t a        ; ///< Torus 'a' dimension
    uint32_t b        ; ///< Torus 'b' dimension
    uint32_t c        ; ///< Torus 'c' dimension
    uint32_t d        ; ///< Torus 'd' dimension
    uint32_t t        ; ///< Hardware thread id, 4 threads per core
} BG_CoordinateMapping_t;


///
/// \brief Blue Gene/Q job coordinates structure
///
/// This structure defines the coordinates of a job. The corner structure identifies the
/// corner coordinate of the job. The shape structure defines the dimensions of the job
/// relative to the corner. The value in the coordinate core field indicates the first
/// core index in the node. The core value in the shape core field indicates
/// the number of cores of this node that are contained in this job.
///

typedef struct
{
    struct
    {
            uint8_t a   ; ///< Torus 'a' coordinate
            uint8_t b   ; ///< Torus 'b' coordinate
            uint8_t c   ; ///< Torus 'c' coordinate
            uint8_t d   ; ///< Torus 'd' coordinate
            uint8_t e   ; ///< Torus 'e' coordinate
            uint8_t core;
    } corner;
    struct
    {
            uint8_t a   ; ///< Torus 'a' dimension
            uint8_t b   ; ///< Torus 'b' dimension
            uint8_t c   ; ///< Torus 'c' dimension
            uint8_t d   ; ///< Torus 'd' dimension
            uint8_t e   ; ///< Torus 'e' dimension
            uint8_t core;
    } shape;
    uint8_t isSubBlock; ///< true if job occupies a proper subset of its block
} BG_JobCoords_t;

using namespace log4cxx;
using namespace std;

LOG_DECLARE_FILE("tester");

void printHelp()
{
    printf("Parameters:\n");
    printf("  --shape arg                         - 5D shape in compute nodes, e.g. 1x4x4x1x2\n");
    printf("  --np arg                            - Positive number of ranks in the entire job\n");
    printf("  --ranksPerNode arg                  - Number of ranks per node: 1, 2, 4, 8, 16, 32, or 64\n");
    printf("  --mapping arg                       - Path to mapping file\n");
    printf("  --verbose 1|2|3|4|5|6|7             - Set the logging verbose level (1..7) (Defaults to INFO)\n");
    printf("                                         1=OFF,2=FATAL,3=ERROR,4=WARN,5=INFO,6=DEBUG,7=TRACE\n");
    printf("  --properties                        - Set the bg.properties file name\n");
    printf("  --help                              - Print this help text\n");
}


int PreCalculateMapOffsets(const char* mapFilename, const uint32_t* dimsize, uint32_t* dimoffset)
{
    int x;
    int y;
    uint32_t lastmultiplier = 1;
    int hitcount[NUMDIMENSION];
    memset(hitcount, 0, sizeof(hitcount));

    if((strlen(mapFilename) >= NUMDIMENSION) && (!isspace(mapFilename[NUMDIMENSION])) && (mapFilename[NUMDIMENSION] != 0))
    {
        // Not a pre-calculated mapping
        LOG_INFO_MSG( "Not a pre-calculated mapping" );
        return -1;
    }

    for(x=0; x<NUMDIMENSION; x++)
    {
        if(((mapFilename[x] >= 'A') && (mapFilename[x] <= 'E')) ||
           (mapFilename[x] == 'T'))
        {
            y = (mapFilename[x] - 'A') & 0x1f;
            if(y == 'T'-'A')
                y = 5;
            hitcount[y]++;
            if(hitcount[y] > 1)
            {
                LOG_ERROR_MSG( "Hit count > 1" );
                return -2;
            }
        }
        else
        {
            LOG_ERROR_MSG( "Not A,B,C,D,E,T" );
            return -3;
        }
    }

    for(x=NUMDIMENSION; x>0; )
    {
        x--;
        y = (mapFilename[x] - 'A') & 0x1f;
        if(y == 'T'-'A')
            y = 5;
        dimoffset[y] = lastmultiplier;
        lastmultiplier = lastmultiplier * dimsize[y];
    }
    return 0;
}

int GenerateCoordinates(
        const char *mapFilename,
        const BG_JobCoords_t* jobcoord,
        BG_CoordinateMapping_t* mycoord,
        uint32_t numProcesses,
        uint32_t np,
        size_t tmpStorageSize,
        void* tmpStorage,
        BG_CoordinateMapping_t map[],
        uint32_t* myRank,
        uint32_t* mpmdFound
        )
{
    uint32_t x;
    uint32_t y;
    uint32_t dimoffset[NUMDIMENSION];
    uint32_t dimsize[NUMDIMENSION];
    uint32_t coord[NUMDIMENSION];
    uint64_t nodeOffset;
    bool     calculatedMap = false;
    bool     comment;
    bool     hasDigits;
    int fd        = -1;
    size_t offset = 0;
    size_t size   = 0;

    size_t buffersize = tmpStorageSize;
    char* buffer = (char*)tmpStorage;
    char line[256];

    if ((myRank != NULL) && (mycoord == NULL)) {
        LOG_ERROR_MSG( "(myRank != NULL) && (mycoord == NULL)" );
        return -1;
    }

    if ((jobcoord == NULL) || (mapFilename == NULL)) {
        LOG_ERROR_MSG( "(jobcoord == NULL) || (mapFilename == NULL)" );
        return -2;
    }

    if (mpmdFound)
        *mpmdFound = 0;

    dimsize[0] = jobcoord->shape.a;
    dimsize[1] = jobcoord->shape.b;
    dimsize[2] = jobcoord->shape.c;
    dimsize[3] = jobcoord->shape.d;
    dimsize[4] = jobcoord->shape.e;
    dimsize[5] = numProcesses;

    // Determine if this is a calculate map
    if (PreCalculateMapOffsets(mapFilename, dimsize, dimoffset) == 0)
        calculatedMap = true;

    if (!calculatedMap) {
        fd = open(mapFilename, 0,0);
        if (fd<0) {
            LOG_ERROR_MSG( "Mapping file " << mapFilename << " open failed with errno: " << strerror(errno) );
            return -3;
        }
        offset = 0;
        size = read(fd, buffer, buffersize);
    }

    for(x=0; x<np; x++) {
        if(calculatedMap) {
            for(y=0; y<NUMDIMENSION; y++)
                coord[y] = (x / dimoffset[y]) % dimsize[y];
        } else {
            do {
                y=0;
                comment = false;
                hasDigits = false;

                if (buffer[offset] == 0)
                    break;

                while (buffer[offset]) {
                    int value = buffer[offset];
                    offset++;
                    if ((isdigit(value)) && (comment == false))
                        hasDigits = true;
                    if (value == '#') {
                        if ((!comment) && (mpmdFound != NULL) && (strncmp(&buffer[offset-1], "#mpmd", 5) == 0)) {
                            *mpmdFound = 1;
                        }
                        if ((!comment) && (strncmp(&buffer[offset-1], "#mapping", 8) == 0)) {
                            char* str = &buffer[offset+7];
                            while(isspace(*str))
                                str++;
                            if (PreCalculateMapOffsets(str, dimsize, dimoffset) == 0)
                                calculatedMap = true;
                            else {
                                // invalid value
                                if(fd >= 0)
                                {
                                    close(fd);
                                    fd = -1;
                                }
                                LOG_ERROR_MSG( "Invalid value found in mapping file." << " Line: " << std::string(line));
                                return -4;
                            }
                            for (int yy=0; yy<NUMDIMENSION; yy++)
                                coord[yy] = (x / dimoffset[yy]) % dimsize[yy];

                            hasDigits = true;
                        }
                        value = 0;
                        comment = true;
                        //                        line[y++] = value;
                        //                        break;
                    } else if((isalpha(value)) && (comment == false)) {
                        if (fd >= 0) {
                            close(fd);
                            fd = -1;
                        }
                        LOG_ERROR_MSG( "isalpha(value)) && (comment == false) " << " Line: " << std::string(line));
                        return -5;
                    } else if (value == '\n') {
                        line[y] = 0;
                        break;
                    }
                    // line[y++] = value;
                    line[y++] = (char) value;
                    if ((size_t)y >= sizeof(line)) {
                        if (fd >= 0) {
                            close(fd);
                            fd = -1;
                        }
                        LOG_ERROR_MSG( "(size_t)y >= sizeof(line) " << " Line: " << std::string(line));
                        return -6;
                    }
                }
            }
            while(!hasDigits);

            if (!hasDigits) {
                if (fd >= 0) {
                    close(fd);
                    fd = -1;
                }
                LOG_ERROR_MSG( "!hasDigits: indicates mapping file does not have enough entries to match job processes (np) specified. " << " Line: " << std::string(line));
                return -7;
            }

            if (!calculatedMap) {
                char* ptr = line;
                for (y=0; y<NUMDIMENSION; y++) {
                    //coord[y] = strtoull(ptr, &ptr, 10);
                    coord[y] = (uint32_t) strtoull(ptr, &ptr, 10);

                }
            }
            if (offset > buffersize/2) {
                memcpy(&buffer[0], &buffer[buffersize/2], size-buffersize/2);
                offset -= buffersize/2;
                size   -= buffersize/2;
                size   += read(fd, &buffer[size], buffersize/2);
            }
        }

        uint32_t nodeSize=1;
        nodeOffset = 0;
        for (y=0; y<NUMDIMENSION; y++) {
            nodeOffset = nodeOffset*dimsize[y] + coord[y];
            nodeSize   = nodeSize*dimsize[y];
        }
        if (nodeOffset >= nodeSize) {
            if (fd >= 0) {
                close(fd);
                fd = -1;
            }
            LOG_ERROR_MSG( "nodeOffset >= nodeSize " << std::string(line));
            return -9;
        }
        if (map != NULL) {
            map[x].a = coord[0];
            map[x].b = coord[1];
            map[x].c = coord[2];
            map[x].d = coord[3];
            map[x].e = coord[4];
            map[x].t = coord[5];
            map[x].reserved = 0;
        }
        if (myRank != NULL) {
            // mycoord is relative to the job, not the entire block.
            if ((coord[0] == mycoord->a) &&
                    (coord[1] == mycoord->b) &&
                    (coord[2] == mycoord->c) &&
                    (coord[3] == mycoord->d) &&
                    (coord[4] == mycoord->e) &&
                    (coord[5] == mycoord->t))
            {
                *myRank = x;
                if (map == NULL)   // map[] isn't being generated, perform early exit.
                {
                    if (fd >= 0) {
                        close(fd);
                        fd = -1;
                    }
                    return 0;
                }
            }
        }
    }
    if (fd >= 0) {
        close(fd);
        fd = -1;
    }
    return 0;
}

unsigned countIncludedNodes(
        const runjob::JobInfo& info,
        const runjob::server::job::class_route::Rectangle* world
        )
{
    // Need to calculate how many nodes will be included in the np rectangle
    // this requires calculating the coordinates for ranks 0 through np, and
    // counting the number of unique a,b,c,d,e coordinates in the resulting set
    std::set<runjob::server::job::class_route::Coordinates> result;

    BG_JobCoords_t coordinates;
    coordinates.shape.a = boost::numeric_cast<uint8_t>( world->size(runjob::server::job::class_route::Dimension::A) );
    coordinates.shape.b = boost::numeric_cast<uint8_t>( world->size(runjob::server::job::class_route::Dimension::B) );
    coordinates.shape.c = boost::numeric_cast<uint8_t>( world->size(runjob::server::job::class_route::Dimension::C) );
    coordinates.shape.d = boost::numeric_cast<uint8_t>( world->size(runjob::server::job::class_route::Dimension::D) );
    coordinates.shape.e = boost::numeric_cast<uint8_t>( world->size(runjob::server::job::class_route::Dimension::E) );

    unsigned buf[2048];

    const unsigned np = info.getNp().get();
    boost::scoped_array<BG_CoordinateMapping_t> output(
            new BG_CoordinateMapping_t[np]
            );

    const std::string& map = info.getMapping();

    const int rc = GenerateCoordinates(
            map.c_str(),
            &coordinates,
            NULL, // rank's coordinates
            info.getRanksPerNode(),
            np,
            sizeof(buf),
            buf,
            output.get(),
            NULL, // rank
            NULL  // mpmd Found
            );

    if ( rc ) {
        LOG_ERROR_MSG("Could not generate mapping: rc=" <<  boost::lexical_cast<std::string>(rc));
        exit(-1);
    }

    for ( unsigned i = 0; i < np; ++i ) {
        LOG_TRACE_MSG(
                "(" <<
                output[i].a << "," <<
                output[i].b << "," <<
                output[i].c << "," <<
                output[i].d << "," <<
                output[i].e << "," <<
                output[i].t << ") " << i
                );

        result.insert(
                runjob::server::job::class_route::Coordinates(
                    output[i].a,
                    output[i].b,
                    output[i].c,
                    output[i].d,
                    output[i].e
                    )
                );
    }

    return static_cast<unsigned>(result.size());
}

void Np(const runjob::JobInfo& info,
        runjob::server::job::class_route::Rectangle* world
        )

{
    const unsigned size = world->size();
    LOG_INFO_MSG( "size           : " << size << " node" << (size == 1 ? "" : "s") );

    const unsigned ranksPerNode = info.getRanksPerNode();
    LOG_INFO_MSG( "ranks per node : " << ranksPerNode );

    const unsigned np_ranks = info.getNp().get();
    LOG_INFO_MSG( "np             : " << np_ranks << " rank" << (np_ranks == 1 ? "" : "s") );

    const unsigned includeCount = countIncludedNodes(info, world);
    const unsigned excludeCount = size - includeCount;

    LOG_INFO_MSG( "exclude        : " << excludeCount << " node" << (excludeCount == 1 ? "" : "s") );
    LOG_INFO_MSG( "np             : " << includeCount << " node" << (includeCount == 1 ? "" : "s") );

}

int main(
        int argc,
        char *argv[]
        )
{
    bool isNp = false;
    bool isRanksPerNode = false;
    bool isMapping = false;
    bool isShape = false;
    bool isProperties = false;
    char* argVal;
    unsigned ranksPerNode = 0;
    unsigned np = 0;
    std::string mappingfile;
    std::string propertiesfile;
    std::string shape;
    unsigned _a = 0;
    unsigned _b = 0;
    unsigned _c = 0;
    unsigned _d = 0;
    unsigned _e = 0;

    // Print help text if no arguments passed
    if (argc == 1) {
        printHelp();
        return 0;
    }

    // Default logging to Info
    uint32_t verboseLevel = 5;

    // Iterate through args first to get -properties and -verbose
    for (int i = 1; i < argc; ++i) {
        if (strcasecmp(argv[i], "-properties")  == 0 || strcasecmp(argv[i], "--properties")  == 0) {
            isProperties = true;
            if (++i == argc) {
                printf("properties keyword specified without an argument value\n");
                exit(EXIT_FAILURE);
            } else {
                propertiesfile = argv[i];
            }
        } else if (strcasecmp(argv[i], "-verbose")  == 0 || strcasecmp(argv[i], "--verbose") == 0 || strcasecmp(argv[i], "-v")  == 0) {
            if (++i == argc) {
                printf("verbose keyword specified without an argument value\n");
                exit(EXIT_FAILURE);
            } else {
                try {
                    verboseLevel = boost::lexical_cast<uint32_t>(argv[i]);
                } catch (const boost::bad_lexical_cast& e) {
                    printf("verbose keyword specified with a bad value\n");
                    exit(EXIT_FAILURE);
                }
            }
        } else if (strcasecmp(argv[i], "-help")  == 0 || strcasecmp(argv[i], "--help")  == 0 || strcasecmp(argv[i], "-h")  == 0) {
            printHelp();
            exit(EXIT_SUCCESS);
        }
    }

    bgq::utility::Properties::Ptr properties;
    if (isProperties) {
        properties = bgq::utility::Properties::Ptr( new bgq::utility::Properties(propertiesfile) );
    } else {
        printf("--properties argument not specified, using default of /bgsys/local/etc/bg.properties\n");
        properties = bgq::utility::Properties::Ptr( new bgq::utility::Properties("/bgsys/local/etc/bg.properties") );
    }

    // initialize logging
    bgq::utility::initializeLogging( *properties );

    log4cxx::LoggerPtr logger(Logger::getLogger("ibm.tester"));

    // 1=OFF,2=FATAL,3=ERROR,4=WARN,5=INFO,6=DEBUG,7=TRACE
    switch (verboseLevel) {
      case 1:
        logger->setLevel(log4cxx::Level::getOff());
        break;
      case 2:
        logger->setLevel(log4cxx::Level::getFatal());
        break;
      case 3:
        logger->setLevel(log4cxx::Level::getError());
        break;
      case 4:
        logger->setLevel(log4cxx::Level::getWarn());
        break;
      case 5:
        logger->setLevel(log4cxx::Level::getInfo());
        break;
      case 6:
        logger->setLevel(log4cxx::Level::getDebug());
        break;
      case 7:
      case 0: // Conversion error on input - use default
      default:
        logger->setLevel(log4cxx::Level::getTrace());
        break;
    }

    int argNbr = 1;
    while (argNbr < argc) {
        char* argKey = argv[argNbr];
        argNbr++;
        if (argNbr < argc) {
            argVal = argv[argNbr];
        } else {
            argVal = NULL;
        }
        if (strcasecmp(argKey, "-shape") == 0 || strcasecmp(argKey, "--shape") == 0) {
            isShape = true;
            if (argVal != NULL) {
                shape = argVal;
                argNbr++;

                // split on x character
                typedef std::vector<std::string> Tokens;
                Tokens tokens;
                boost::split( tokens, shape, boost::is_any_of("x") );

                // ensure we found exactly 5 tokens
                if ( tokens.size() != 5 ) {
                    LOG_ERROR_MSG("shape argument is invalid, expected 5 tokens, found " <<  boost::lexical_cast<std::string>( tokens.size()));
                    return -1;
                }

                // iterate through tokens
                try {
                    for ( Tokens::iterator i = tokens.begin(); i != tokens.end(); ++i ) {
                        ssize_t index = std::distance( tokens.begin(), i );
                        switch( index ) {
                            case 0: _a = boost::lexical_cast<unsigned>( *i ); break;
                            case 1: _b = boost::lexical_cast<unsigned>( *i ); break;
                            case 2: _c = boost::lexical_cast<unsigned>( *i ); break;
                            case 3: _d = boost::lexical_cast<unsigned>( *i ); break;
                            case 4: _e = boost::lexical_cast<unsigned>( *i ); break;
                            default: BOOST_ASSERT( !"unhandled token" ); break;
                        }
                    }
                } catch ( const boost::bad_lexical_cast& e ) {
                    LOG_ERROR_MSG("shape argument is invalid: " <<  e.what());
                    return -1;
                }

                // validate each token size
                if ( _a == 0 || _b == 0 || _c == 0 || _d == 0 || _e == 0 ) {
                    // no dimension can be zero
                    LOG_ERROR_MSG("shape argument is invalid, 0 is not a valid size for any dimension");
                    return -1;
                } else if ( _e > 2 ) {
                    LOG_ERROR_MSG("shape argument is invalid, E dimension must be a positive value of size 2 or less");
                    return -1;
                }
            } else {
                printf("shape keyword specified without an argument value\n");
                return -1;
            }
        } else if (strcasecmp(argKey, "-ranksPerNode") == 0 || strcasecmp(argKey, "--ranksPerNode") == 0) {
            isRanksPerNode = true;
            if (argVal != NULL) {
                try {
                    ranksPerNode = boost::lexical_cast<unsigned>(argVal);
                    if (ranksPerNode == 1 || ranksPerNode == 2 || ranksPerNode == 4 || ranksPerNode == 8 || ranksPerNode == 16 || ranksPerNode == 32 || ranksPerNode == 64) {
                        // Value is OK
                    } else {
                        LOG_ERROR_MSG("ranksPerNode keyword specified with a bad value\n");
                        return -1;
                    }
                } catch (const boost::bad_lexical_cast& e) {
                    LOG_ERROR_MSG("ranksPerNode keyword specified with a bad value\n");
                    return -1;
                }
                argNbr++;
            } else {
                printf("ranksPerNode keyword specified without an argument value\n");
                return -1;
            }
        } else if (strcasecmp(argKey, "-np") == 0 || strcasecmp(argKey, "--np") == 0) {
            isNp = true;
            if (argVal != NULL) {
                try {
                    np = boost::lexical_cast<unsigned>(argVal);
                } catch (const boost::bad_lexical_cast& e) {
                    LOG_ERROR_MSG("np keyword specified with a bad value\n");
                    return -1;
                }
                argNbr++;
            } else {
                printf("np keyword specified without an argument value\n");
                return -1;
            }
        } else if (strcasecmp(argKey, "-mapping")  == 0 || strcasecmp(argKey, "--mapping")  == 0) {
            isMapping = true;
            if (argVal != NULL) {
                mappingfile = argVal;
                argNbr++;
            } else {
                printf("mapping keyword specified without an argument value\n");
                return -1;
            }
        } else if (strcasecmp(argKey, "-verbose") == 0 || strcasecmp(argKey, "--verbose") == 0 || strcasecmp(argKey, "-v") == 0) {
            // ignore, we've already parsed this guy
            ++argNbr;
        } else if (strcasecmp(argKey, "-properties") == 0 || strcasecmp(argKey, "--properties") == 0) {
            // ignore, we've already parsed this guy
            ++argNbr;
        } else {
            printf("parameter %s not recognized\n", argKey);
            printHelp();
            return 0;
        }
    }

    if (isShape == false || isMapping == false || isNp == false || isRanksPerNode == false) {
        LOG_ERROR_MSG( "Must specify --shape, --np, --ranksPerNode and --mapping arguments" );
        return -1;
    }
    runjob::JobInfo info;
    info.setNp(np);  // Set the number of ranks
    info.setRanksPerNode(ranksPerNode); // Set the ranks per node.
    try {
        info.setMapping(
                runjob::Mapping(
                        runjob::Mapping::Type::File,
                        mappingfile
                )
        );
    } catch ( const std::runtime_error& e ) {
        LOG_ERROR_MSG("Setting mapping file name failed with error: " <<  e.what());
        return -1;
    }

    CR_RECT_T world;
    *CR_RECT_LL(&world) = (CR_COORD_T) {{0,0,0,0,0}};

    /*
    if ( subBlock.isValid() ) {
        const Shape& shape = subBlock.shape();
        *CR_RECT_UR(&world) = (CR_COORD_T) {{ shape.a() - 1, shape.b() - 1, shape.c() - 1, shape.d() - 1, shape.e() - 1 }};
    } else {
        const block::Compute::Ptr block = _job->block();
        *CR_RECT_UR(&world) = (CR_COORD_T) {{ block->a() - 1, block->b() - 1, block->c() - 1, block->d() - 1, block->e() - 1 }};
    }
    */
    *CR_RECT_UR(&world) = (CR_COORD_T) {{ _a - 1, _b - 1, _c - 1, _d - 1, _e - 1 }};

    runjob::server::job::class_route::Rectangle* rect( new runjob::server::job::class_route::Rectangle(&world) );

    Np(info, rect);
}
