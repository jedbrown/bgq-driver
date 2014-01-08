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
/* (C) Copyright IBM Corp.  2010, 2012                              */
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
//#include <stdio.h>
//#include <ctype.h>
//#include <unistd.h>
//#include <fcntl.h>
//#include <stdlib.h>

#include <stdint.h>
#include <string.h>
#include "spi/include/kernel/location.h"

#define NUMDIMENSION     6

inline int MUSPI_GenerateCoordinates(const char *mapFilename, const BG_JobCoords_t* jobcoord, BG_CoordinateMapping_t* mycoord, uint32_t numProcesses, uint32_t np, 
                                     size_t tmpStorageSize, void* tmpStorage, BG_CoordinateMapping_t map[], uint32_t *myRank)
{
    uint32_t x;
    uint32_t y;
    uint32_t lastmultiplier = 1;
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
    
    if((myRank != NULL) && (mycoord == NULL))
        return -1;
    
    if((jobcoord == NULL) || (mapFilename == NULL))
        return -1;
    
    dimsize[0] = jobcoord->shape.a;
    dimsize[1] = jobcoord->shape.b;
    dimsize[2] = jobcoord->shape.c;
    dimsize[3] = jobcoord->shape.d;
    dimsize[4] = jobcoord->shape.e;
    dimsize[5] = numProcesses;
    
    // Determine if this is a calculate map
    if(strlen(mapFilename) == NUMDIMENSION)
    {
        for(x=NUMDIMENSION; x>0; )
        {
            x--;
            y = (mapFilename[x] - 'A') & 0x1f;
            if(y == 'T'-'A')
                y = 5;
            if(y > 'E'-'A' + 1)
                break;
            
            dimoffset[y] = lastmultiplier;
            lastmultiplier = lastmultiplier * dimsize[y];
        }
        if(x==0)
            calculatedMap = true;
    }
    
    if(!calculatedMap)
    {
        fd = open(mapFilename, 0,0);
        if(fd<0)
            return -1;
        offset = 0;
        size = read(fd, buffer, buffersize);
    }
    
    for(x=0; x<np; x++)
    {
        if(calculatedMap)
        {
            for(y=0; y<NUMDIMENSION; y++)
                coord[y] = (x / dimoffset[y]) % dimsize[y];
        }
        else
        {
            do
            {
                y=0;
                comment = false;
                hasDigits = false;
                while(buffer[offset])
                {
                    int value = buffer[offset];
                    offset++;
                    if((isdigit(value)) && (comment == false))
                        hasDigits = true;
                    if(value == '#')
                    {
                        value = 0;
                        comment = true;
                        line[y++] = 0;
                        while(buffer[offset++] != '\n')  // zoom ahead
                        {
                        }
                        break;
                    }
                    else if((isalpha(value)) && (comment == false))
                    {
                        if(fd >= 0)
                        {
                            close(fd);
                            fd = -1;
                        }
                        return -1;
                    }
                    else if(value == '\n')
                    {
                        line[y] = 0;
                        break;
                    }
                    line[y++] = value;
                }
            }
            while(!hasDigits);
            
            char* ptr = line;
            for(y=0; y<NUMDIMENSION; y++)
            {
                coord[y] = strtoull(ptr, &ptr, 10);
                
            }
            if(offset > buffersize/2)
            {
                memcpy(&buffer[0], &buffer[buffersize/2], size-buffersize/2);
                offset -= buffersize/2;
                size   -= buffersize/2;
                size   += read(fd, &buffer[size], buffersize/2);
            }
        }
        
        uint32_t nodeSize=1;
        nodeOffset = 0;
        for(y=0; y<NUMDIMENSION; y++)
        {
            nodeOffset = nodeOffset*dimsize[y] + coord[y];
            nodeSize   = nodeSize*dimsize[y];
        }
        if(nodeOffset >= nodeSize)
        {
            if(fd >= 0)
            {
                close(fd);
                fd = -1;
            }
            return -1;
        }
        if (map != NULL) 
        {
            map[x].a = coord[0];
            map[x].b = coord[1];
            map[x].c = coord[2];
            map[x].d = coord[3];
            map[x].e = coord[4];
            map[x].t = coord[5];
            map[x].reserved = 0;
        }
        if(myRank != NULL)
        {
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
                    if(fd >= 0)
                    {
                        close(fd);
                        fd = -1;
                    }
                    return 0;
                }
            }
        }
    }
    if(fd >= 0)
    {
        close(fd);
        fd = -1;
    }
    return 0;
}
