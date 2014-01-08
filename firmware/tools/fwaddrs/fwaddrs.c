/* begin_generated_IBM_copyright_prolog                             */
/*                                                                  */
/* This is an automatically generated copyright prolog.             */
/* After initializing,  DO NOT MODIFY OR MOVE                       */
/*  --------------------------------------------------------------- */
/*                                                                  */
/* Licensed Materials - Property of IBM                             */
/* Blue Gene/Q                                                      */
/* (C) Copyright IBM Corp.  2010, 2012                              */
/* US Government Users Restricted Rights - Use, duplication or      */
/*   disclosure restricted by GSA ADP Schedule Contract with IBM    */
/*   Corp.                                                          */
/*                                                                  */
/* This software is available to you under the Eclipse Public       */
/* License (EPL).                                                   */
/*                                                                  */
/*  --------------------------------------------------------------- */
/*                                                                  */
/* end_generated_IBM_copyright_prolog                               */


#include <firmware/include/Firmware_Interface.h>
#include <stdio.h>
#include <string.h>

/** A global instance of the Firmware_Interface_t structure. */
Firmware_Interface_t fwi;

/** A flag indicating that the output needs to be assembler. */
int printAsm = 0;

/*!
 * Prints an offset value for a field in the firmware interface structure.
 *
 * @param name
 *          the name of the field.
 * @param address
 *          a pointer containing the address of the field in the global fwi variable.
 *          It is important that the pointer is pointing to a field in the fwi global variable
 *          since pointer arithmetic is used to calculate the offset value.
 */
void printAddress(char *name, void *address)
{
  // Calcluate the offset into the structure for the field.
  int offset = (address - (void *)&fwi);

  if(printAsm)
  {
    // Create an assembler definition name.
    char asmName[80];
    strcpy(asmName, "_FIRMWARE_");
    strcat(asmName, name);
    strcat(asmName, ",");

    // Print the assembler definition for the field.
    printf(".set %-36s %d\n", asmName, offset);
  }
  else
  {
    // Print a generic message indicating the offset for the field.
    printf("%-26s &fwi + 0x%08X (%d) bytes\n", name, offset, offset);
  }
}

/*!
 * Parses command line arguments.
 *
 * @param argc
 *          the number of arguments.
 * @param argv
 *          the array of argument strings.
 */
void parseArgs(int argc, char *argv[])
{
  if(argc > 1)
  {
    if(strcmp(argv[1], "-a") == 0)
    {
      printAsm = 1;
    }
  }
}

/*!
 * The main entry point for the program.
 *
 * @param argc
 *          the number of arguments.
 * @param argv
 *          the array of argument strings.
 * @return 0 indicating success.
 */
int main(int argc, char *argv[])
{
  // Parse command line arguments.
  parseArgs(argc, argv);

  printf("\n");

  if(printAsm)
  {
    // Add an assembler comment if we're outputting assembler.
    printf("/* BG/Q Firmware interface offsets. */\n");
  }

  // Print field offsets.
  printAddress("exit", (void *)&(fwi.exit));
  printAddress("terminate", (void *)&(fwi.terminate));
  printAddress("getPersonality", (void *)&(fwi.getPersonality));
  printAddress("writeRASEvent", (void *)&(fwi.writeRASEvent));
  printAddress("writeRASString", (void *)&(fwi.writeRASString));
  printAddress("putn", (void *)&(fwi.putn));
  printAddress("installInterruptVector", (void *)&(fwi.installInterruptVector));

  printf("\n");

  return 0;
}

