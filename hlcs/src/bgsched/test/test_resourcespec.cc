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
/* (C) Copyright IBM Corp.  2010, 2011                              */
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

#include <bgsched/bgsched.h>
#include <bgsched/allocator/Allocator.h>
#include <bgsched/allocator/LiveModel.h>
#include <bgsched/allocator/ResourceSpec.h>

#include <utility/include/Log.h>

#include <execinfo.h>
#include <iostream>
#include <stdexcept>
#include <string>
#include <strings.h>

using namespace bgsched;
using namespace bgsched::allocator;
using namespace log4cxx;
using namespace std;

LOG_DECLARE_FILE( "testallocator" );

// Obtain a backtrace and print it to stderr.
void
print_trace (void)
{
    void *array[20];
    size_t size;
    char **strings;
    size_t i;

    size = backtrace (array, 20);
    strings = backtrace_symbols (array, size);

    cerr << "Obtained " << size << " stack frames." << endl;

    for (i = 0; i < size; i++)
        cerr << strings[i] << endl;

    cerr.flush();
    free (strings);
}

void
dumpResourceSpecInfo(
        const ResourceSpec& resourceSpec,
        std::ostream& os)
{
    Shape::ConstPtr shape = resourceSpec.getShape();
    ResourceSpec::DrainedMidplanes drainedMidplanes = resourceSpec.getDrainedMidplanes();
    ResourceSpec::ExtendedOptions  extendedOptions = resourceSpec.getExtendedOptions();

    os << "Resource spec information . . . . :" << endl;
    if (shape->isSmall()) {
        // Small block shape
        os << "  Small block shape . . . . . . . : " << "Dimensions (compute nodes) A:"
           << shape->getNodeSize(Dimension::A) << " B:" << shape->getNodeSize(Dimension::B)
           << " C:" << shape->getNodeSize(Dimension::C) << " D:" << shape->getNodeSize(Dimension::D)
           << " E:" << shape->getNodeSize(Dimension::E) << endl;
        os << "  Compute nodes . . . . . . . . . : " << resourceSpec.getNodeCount() << endl;
    } else {
        // Large block shape
        os << "  Large block shape . . . . . . . : " << "Dimensions (midplanes) A:"
           << shape->getMidplaneSize(Dimension::A) << " B:" << shape->getMidplaneSize(Dimension::B)
           << " C:" << shape->getMidplaneSize(Dimension::C) << " D:" << shape->getMidplaneSize(Dimension::D) << endl;
        os << "  Compute nodes . . . . . . . . . : " << resourceSpec.getNodeCount() << endl;
        os << "  Can rotate shape  . . . . . . . : ";
        if (resourceSpec.canRotateShape()) {
            os << "Yes" << endl;
        } else {
            os << "No" << endl;
        }
        os << "  Can use pass-through  . . . . . : ";
        if (resourceSpec.canUsePassthrough()) {
            os << "Yes" << endl;
        } else {
            os << "No" << endl;
        }
        os << "  Connectivity spec . . . . . . . :";
        for (Dimension dim = Dimension::A; dim <= Dimension::D; ++dim) {
            if (ResourceSpec::ConnectivitySpec::Torus == resourceSpec.getConnectivitySpec(dim)) {
                os << " Torus";
            } else {
                if (ResourceSpec::ConnectivitySpec::Mesh == resourceSpec.getConnectivitySpec(dim)) {
                    os << " Mesh";
                } else {
                    os << " Either";
                }
            }
        }
        os << endl;
    }
    if (drainedMidplanes.size() == 0){
        os << "  Excluded (drained) midplanes  . : No" << endl;
    } else {
        // Iterate over the list of drained midplanes
        os << "  Excluded (drained) midplanes  . : Yes" << endl;
        for (ResourceSpec::DrainedMidplanes::iterator iter = drainedMidplanes.begin(); iter != drainedMidplanes.end(); ++iter) {
            os << "  Excluding (draining) midplane . : " << *iter << endl;
        }
    }
    if (extendedOptions.empty()){
        os << "  Extended options  . . . . . . . : No" << endl;
    } else {
        // Iterate over the extended/options map
        os << "  Extended options  . . . . . . . : Yes" << endl;
        for (ResourceSpec::ExtendedOptions::iterator iter = extendedOptions.begin(); iter != extendedOptions.end(); ++iter) {
            os << "  Key/value . . . . . . . . . . . : " << iter->first << "/" << iter->second << endl;
        }
    }
}

void
testcase_smallshape(
        Allocator &my_alloc,
        uint32_t computeNodes
        )
{
    // Create compute node shape
    try {
        Shape shape = my_alloc.createSmallShape(computeNodes);
        LOG_INFO_MSG("Small shape with compute node size " << computeNodes << " was created.");

        // Create ResourceSpec based on the shape
        ResourceSpec resourceSpec(shape);
        // Print out the resource spec details
        dumpResourceSpecInfo(resourceSpec, std::cout);
    }
    catch(...)
    {
        LOG_ERROR_MSG("testcase_smallshape() caught exception");
        return;
    }
}

void
testcase_largeshape(
        Allocator &my_alloc,
        uint32_t a,
        uint32_t b,
        uint32_t c,
        uint32_t d
        )
{
    // Create midplane shape
    try {
        Shape shape = my_alloc.createMidplaneShape(a, b, c, d);
        LOG_INFO_MSG("Large shape with dimensions " << a << ", " << b << ", " << c << ", " << d << " was created.");

        // Create ResourceSpec based on the shape (allow rotation and pass-through)
        ResourceSpec resourceSpec(shape, true, true);
        // Set the connectivity spec
        for (Dimension dim = Dimension::A; dim <= Dimension::D; ++dim) {
            resourceSpec.setConnectivitySpec(dim, ResourceSpec::ConnectivitySpec::Either);
        }
        // Print out the resource spec details
        dumpResourceSpecInfo(resourceSpec, std::cout);
    }
    catch(...)
    {
        LOG_ERROR_MSG("testcase_largeshape() caught exception");
        return;
    }
}

void
testcase_drainedMidplanes(
        Allocator &my_alloc,
        uint32_t a,
        uint32_t b,
        uint32_t c,
        uint32_t d
        )
{
    // Create midplane shape
    try {
        Shape shape = my_alloc.createMidplaneShape(a, b, c, d);
        LOG_INFO_MSG("Large shape with dimensions " << a << ", " << b << ", " << c << ", " << d << " was created.");

        // Create ResourceSpec based on the shape (allow rotation and pass-through)
        ResourceSpec resourceSpec(shape, true, true);
        // Set the connectivity spec
        for (Dimension dim = Dimension::A; dim <= Dimension::D; ++dim) {
            resourceSpec.setConnectivitySpec(dim, ResourceSpec::ConnectivitySpec::Torus);
        }
        // Add drained midplanes
        resourceSpec.addDrainedMidplane("R00-M0");
        resourceSpec.addDrainedMidplane("R01-M0");
        resourceSpec.addDrainedMidplane("R01-M0");
        resourceSpec.removeDrainedMidplane("R01-M0");
        resourceSpec.removeDrainedMidplane("R01-M0");

        // Print out the resource spec details
        dumpResourceSpecInfo(resourceSpec, std::cout);
    }
    catch(...)
    {
        LOG_ERROR_MSG("testcase_drainedMidplanes() caught exception");
        return;
    }
}

void
testcase_extendedOptions(
        Allocator &my_alloc,
        uint32_t a,
        uint32_t b,
        uint32_t c,
        uint32_t d
        )
{
    // Create midplane shape
    try {
        Shape shape = my_alloc.createMidplaneShape(a, b, c, d);
        LOG_INFO_MSG("Large shape with dimensions " << a << ", " << b << ", " << c << ", " << d << " was created.");
        // Create ResourceSpec based on the shape (allow rotation and pass-through)
        ResourceSpec resourceSpec(shape, true, true);
        // Set the connectivity spec
        for (Dimension dim = Dimension::A; dim <= Dimension::D; ++dim) {
            resourceSpec.setConnectivitySpec(dim, ResourceSpec::ConnectivitySpec::Either);
        }
        // Add extended options
        resourceSpec.addExtendedOption("Tiger", "Woods");
        resourceSpec.addExtendedOption("Jesse", "James");

        // Print out the resource spec details
        dumpResourceSpecInfo(resourceSpec, std::cout);

        // Modify/remove extended options
        resourceSpec.addExtendedOption("Jesse", "Ventura"); // Should replace previous entry
        resourceSpec.removeExtendedOption("Tiger");   // Should remove previous entry
        resourceSpec.removeExtendedOption("NoEntry"); // Should be ignored

        // Print out the resource spec details
        dumpResourceSpecInfo(resourceSpec, std::cout);
    }
    catch(...)
    {
        LOG_ERROR_MSG("testcase_extendedOptions() caught exception");
        return;
    }
}

void
testcase_InvalidDimension(
        Allocator &my_alloc,
        uint32_t a,
        uint32_t b,
        uint32_t c,
        uint32_t d
        )
{
    // Create midplane shape
    try {
        Shape shape = my_alloc.createMidplaneShape(a, b, c, d);
        LOG_INFO_MSG("Large shape with dimensions " << a << ", " << b << ", " << c << ", " << d << " was created.");

        // Create ResourceSpec based on the shape
        ResourceSpec resourceSpec(shape, true);
        // Attempt to set invalid connectivity spec
        for (Dimension dim = Dimension::A; dim != Dimension(); ++dim) {
            resourceSpec.setConnectivitySpec(dim, ResourceSpec::ConnectivitySpec::Torus);
        }

        // Print out the resource spec details
        dumpResourceSpecInfo(resourceSpec, std::cout);
    }
    catch(...)
    {
        LOG_ERROR_MSG("testcase_InvalidDimension() caught exception");
        return;
    }
}

void
testcase_InvalidConnectivity(
        Allocator &my_alloc
        )
{
    // Create midplane shape
    try {
        Shape shape = my_alloc.createMidplaneShape(1, 1, 1, 1);
        LOG_INFO_MSG("Large shape with dimensions 1, 1, 1, 1 was created.");

        // Create ResourceSpec based on the shape
        ResourceSpec resourceSpec(shape, true);
        // Attempt to set invalid connectivity spec (single midplane must be Torus in all dims)
        for (Dimension dim = Dimension::A; dim <= Dimension::D; ++dim) {
            resourceSpec.setConnectivitySpec(dim, ResourceSpec::ConnectivitySpec::Mesh);
        }

        // Print out the resource spec details
        dumpResourceSpecInfo(resourceSpec, std::cout);
    }
    catch(...)
    {
        LOG_ERROR_MSG("testcase_InvalidConnectivity() caught exception");
        return;
    }
}


void
testcase_InvalidConnectivity2(
        Allocator &my_alloc
        )
{
    // Create midplane shape
    try {
        Shape shape = my_alloc.createMidplaneShape(1, 4, 1, 1);
        LOG_INFO_MSG("Large shape with dimensions 1, 4, 1, 1 was created.");

        // Create ResourceSpec based on the shape
        ResourceSpec resourceSpec(shape, true);
        // Attempt to set invalid connectivity spec (any dimension that is equal to the size of the
        // machine in that dimension must also be Torus)
        resourceSpec.setConnectivitySpec(Dimension::B, ResourceSpec::ConnectivitySpec::Mesh);

        // Print out the resource spec details
        dumpResourceSpecInfo(resourceSpec, std::cout);
    }
    catch(...)
    {
        LOG_ERROR_MSG("testcase_InvalidConnectivity2() caught exception");
        return;
    }
}

void
testcase_InvalidConnectivity3(
        Allocator &my_alloc
        )
{
    // Create midplane shape
    try {
        Shape shape = my_alloc.createMidplaneShape(1, 3, 1, 1);
        LOG_INFO_MSG("Large shape with dimensions 1, 3, 1, 1 was created.");

        // Create ResourceSpec based on the shape and disallow pass-through and rotation
        ResourceSpec resourceSpec(shape, false, false);
        // Attempt to set invalid connectivity spec (any dimension that is less than the size of the
        // machine in that dimension must be Mesh unless pass-through is allowed)
        resourceSpec.setConnectivitySpec(Dimension::B, ResourceSpec::ConnectivitySpec::Torus);

        // Print out the resource spec details
        dumpResourceSpecInfo(resourceSpec, std::cout);
    }
    catch(...)
    {
        LOG_ERROR_MSG("testcase_InvalidConnectivity3() caught exception");
        return;
    }
}

void
testcase_Invalid4x4Connectivity(
        Allocator &my_alloc
        )
{
    // Create midplane shape
    try {
        Shape shape = my_alloc.createMidplaneShape(2, 4, 2, 2);
        LOG_INFO_MSG("Large shape with dimensions 2, 4, 2, 2 was created.");

        // Create ResourceSpec based on the shape
        ResourceSpec resourceSpec(shape, true);
        // Attempt to set invalid connectivity spec (full block must be Torus in all dims)
        for (Dimension dim = Dimension::A; dim <= Dimension::D; ++dim) {
            resourceSpec.setConnectivitySpec(dim, ResourceSpec::ConnectivitySpec::Mesh);
        }

        // Print out the resource spec details
        dumpResourceSpecInfo(resourceSpec, std::cout);
    }
    catch(...)
    {
        LOG_ERROR_MSG("testcase_Invalid4x4Connectivity() caught exception");
        return;
    }
}

void
testcase_SmallConnectivity(
        Allocator &my_alloc,
        uint32_t computeNodes
        )
{
    // Create compute node shape
    try {
        Shape shape = my_alloc.createSmallShape(computeNodes);
        LOG_INFO_MSG("Small shape with compute node size " << computeNodes << " was created.");

        // Create ResourceSpec based on the shape
        ResourceSpec resourceSpec(shape);
        // Attempt to set connectivity spec (ignored for small block shapes)
        for (Dimension dim = Dimension::A; dim <= Dimension::D; ++dim) {
            resourceSpec.setConnectivitySpec(dim, ResourceSpec::ConnectivitySpec::Torus);
        }
        // Print out the resource spec details
        dumpResourceSpecInfo(resourceSpec, std::cout);
    }
    catch(...)
    {
        LOG_ERROR_MSG("testcase_SmallConnectivity() caught exception");
        return;
    }
}


int
main(int argc, const char** argv)
{
    // iterate through args first to get -properties
    string properties;
    for (int i = 1; i < argc; ++i) {
        if (!strcasecmp(argv[i], "-properties")) {
            if (++i == argc) {
                cerr << "Please give a file name after " << argv[i-1] << endl;
                exit(EXIT_FAILURE);
            } else {
                properties = argv[i];
            }
        } else if (!strcasecmp(argv[i], "-help") || !strcasecmp(argv[i], "-h")) {
            cerr << argv[0] << ":" << endl;
            cerr << "\t -properties <file>     : Set bg.properties file name" << endl;
            cerr << "\t -help | -h             : Print this help text" << endl;
            exit(EXIT_SUCCESS);
        }
    }

    // init library
    init( properties );
    LOG_INFO_MSG( "test_resourcespec running testcases (Note: requires 4x4 configuration)" );

    try
    {
        LOG_TRACE_MSG( "Creating allocator framework" );
        Allocator my_alloc;
        // Test out small block shapes in resource spec
        LOG_INFO_MSG( "=== ResourceSpec with 2 compute nodes Shape, should complete OK ===" );
        testcase_smallshape(my_alloc, 2);
        LOG_INFO_MSG( "=== ResourceSpec with 32 compute nodes Shape, should complete OK ===" );
        testcase_smallshape(my_alloc, 32);
        LOG_INFO_MSG( "=== ResourceSpec with 33 compute nodes Shape, should complete OK ===" );
        testcase_smallshape(my_alloc, 33);
        LOG_INFO_MSG( "=== ResourceSpec with 64 compute nodes Shape, should complete OK ===" );
        testcase_smallshape(my_alloc, 64);
        LOG_INFO_MSG( "=== ResourceSpec with 65 compute nodes Shape, should complete OK ===" );
        testcase_smallshape(my_alloc, 65);
        LOG_INFO_MSG( "=== ResourceSpec with 128 compute nodes Shape, should complete OK ===" );
        testcase_smallshape(my_alloc, 128);
        LOG_INFO_MSG( "=== ResourceSpec with 129 compute nodes Shape, should complete OK ===" );
        testcase_smallshape(my_alloc, 129);
        LOG_INFO_MSG( "=== ResourceSpec with 256 compute nodes Shape, should complete OK ===" );
        testcase_smallshape(my_alloc, 256);
        LOG_INFO_MSG( "=== ResourceSpec with 257 compute nodes Shape, should FAIL ===" );
        testcase_smallshape(my_alloc, 257);
        LOG_INFO_MSG( "=== ResourceSpec with 512 compute nodes Shape, should FAIL ===" );
        testcase_smallshape(my_alloc, 512);
        // Test out large block shapes in resource spec
        LOG_INFO_MSG( "=== ResourceSpec with 1x1x1x1 midplane Shape (512), should complete OK ===" );
        testcase_largeshape(my_alloc, 1, 1, 1, 1);
        LOG_INFO_MSG( "=== ResourceSpec with 1x1x1x2 midplane Shape (1024), should complete OK ===" );
        testcase_largeshape(my_alloc, 1, 1, 1, 2);
        LOG_INFO_MSG( "=== ResourceSpec with 1x1x2x1 midplane Shape (1024), should complete OK ===" );
        testcase_largeshape(my_alloc, 1, 1, 2, 1);
        LOG_INFO_MSG( "=== ResourceSpec with 1x1x2x2 midplane Shape (2048), should complete OK ===" );
        testcase_largeshape(my_alloc, 1, 1, 2, 2);
        LOG_INFO_MSG( "=== ResourceSpec with 2x1x1x1 midplane Shape (1024), should FAIL ===" );
        testcase_largeshape(my_alloc, 2, 1, 1, 1);
        // Test out drained midplanes in resource spec
        LOG_INFO_MSG( "=== ResourceSpec with drained midplanes, should complete OK ===" );
        testcase_drainedMidplanes(my_alloc, 1, 1, 1, 1);
        LOG_INFO_MSG( "=== ResourceSpec with extended options, should complete OK ===" );
        testcase_extendedOptions(my_alloc, 1, 1, 1, 1);
        LOG_INFO_MSG( "=== ResourceSpec with bad connectivitySpec (invalid dimension), should FAIL ===" );
        testcase_InvalidDimension(my_alloc, 1, 1, 1, 1);
        LOG_INFO_MSG( "=== ResourceSpec with bad connectivitySpec (invalid connectivity), should FAIL ===" );
        testcase_InvalidConnectivity(my_alloc);
        LOG_INFO_MSG( "=== ResourceSpec with bad full block (4x4) connectivitySpec (invalid connectivity), should FAIL ===" );
        testcase_Invalid4x4Connectivity(my_alloc);
        LOG_INFO_MSG( "=== ResourceSpec with bad connectivitySpec (invalid connectivity), should FAIL ===" );
        testcase_InvalidConnectivity2(my_alloc);
        LOG_INFO_MSG( "=== ResourceSpec with bad connectivitySpec (invalid connectivity), should FAIL ===" );
        testcase_InvalidConnectivity3(my_alloc);
        LOG_INFO_MSG( "=== ResourceSpec with small block connectivitySpec (ignores connectivity), should complete OK ===" );
        testcase_SmallConnectivity(my_alloc, 32);
   }
    catch(...)
    {
        LOG_ERROR_MSG( "Exception caught" );
        print_trace();
        throw;
    }
}
