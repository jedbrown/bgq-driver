/* begin_generated_IBM_copyright_prolog                             */
/*                                                                  */
/* This is an automatically generated copyright prolog.             */
/* After initializing,  DO NOT MODIFY OR MOVE                       */
/* ================================================================ */
/*                                                                  */
/* Licensed Materials - Property of IBM                             */
/*                                                                  */
/* (C) Copyright IBM Corp.  2011, 2012                              */
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
#ifndef __api_extension_c_collsel_CollselXMLParser_h__
#define __api_extension_c_collsel_CollselXMLParser_h__

#include <fstream>
#include <cstring>
#include <locale>
#include <algorithm>
#include <iostream>
#include <vector>

#include "boost/config.hpp"
#include "boost/property_tree/detail/rapidxml.hpp"
#include "CollselData.h"
#include "CollselXMLWriter.h"

namespace PAMI
{
  using  namespace boost::property_tree::detail::rapidxml;

  template <class Ch = char>
  class XMLParser
  {
      /// Text elements should be put in separate keys,
      /// not concatenated in parent data.
      static const int no_concat_text  = 0x1;
      /// Comments should be omitted.
      static const int no_comments     = 0x2;
      /// Whitespace should be collapsed and trimmed.
      static const int trim_whitespace = 0x4;

      inline bool validate_flags(int flags)
      {
        return (flags & ~(no_concat_text | no_comments | trim_whitespace)) == 0;
      }

      int parse_algorithm_mapping(xml_node<Ch> *collectives_node,
                                  CollselData  *data)
      {
        int ret = 0;
        PAMI::AlgoMap *algo_map = data->get_algorithm_map();

        for (xml_node<Ch> *child = collectives_node->first_node();
             child; child = child->next_sibling())
        {
          // Parse collective ID
          xml_node<Ch> *tmp_node = child->first_node();
          char *end, *value = tmp_node->value();

          int coll_id = (int) strtol(value, &end, 10);
          if(value == end || *end != '\0' || coll_id >= PAMI_XFER_COUNT)
          {
            fprintf(stderr, "Invalid collective ID: %s\n", value);
            ret = 1;
            goto invalid_data;
          }

          for (xml_node<Ch> *algo = tmp_node->next_sibling();
               algo; algo = algo->next_sibling())
          {
            // Parse the algorithm ID and name
            tmp_node = algo->first_node();
            value = tmp_node->value();
            int algo_id = (int) strtol(value, &end, 10);
            if(value == end || *end != '\0')
            {
              fprintf(stderr, "Invalid algorithm ID: %s\n", value);
              ret = 1;
              goto invalid_data;
            }
            tmp_node = tmp_node->next_sibling();
            // Allocate memory from memory pool & copy value to it
            char *algo_name = data->mempool_allocate(tmp_node->value_size() + 1);
            strcpy(algo_name, tmp_node->value());
            algo_map[coll_id][algo_id] = algo_name;

            //printf("Collective[%d][%d]: = %s\n", coll_id, algo_id, algo_map[coll_id][algo_id]);
          }
        }
      invalid_data:
        return ret;
      }

      /** 
       * Convert the comma separated characters to numeric form.
       * strtok followed by strtol/atoi is expensive
       */
      void to_numeric(unsigned char *nalgo, const char *algo, int algo_sz)
      {
        unsigned char val = 0;
        int i = 0, j = 1;
        for(; i < algo_sz; ++i)
        {
          if(algo[i] == ',')
          {
            nalgo[j++] = val;
            val = 0;
            continue;
          }
          val = 10 * val + algo[i] - '0';
        }
        nalgo[j] = val;
        // Store array size as first element
        nalgo[0] = j;
      }

      int parse_collective_selection_data(xml_node<Ch> *geometries_node,
                                          CollselData  *data)
      {
        int ret = 0;
        PAMI::GeometryShapeMap &collsel_data = data->get_datastore();
        xml_node<Ch> *tmp_node = geometries_node->first_node();// ppn 

        char *end, *value = tmp_node->value();
        unsigned ppn, geo_size, coll_id;
        size_t size_min = 0, size_max = 0;

        ppn = (unsigned) strtoul(value, &end, 10);
        if(value == end || *end != '\0')
        {
          fprintf(stderr, "Invalid ppn: %s\n", value);
          ret = 1;
          goto invalid_data;
        }
        // Iterate over all geometry sizes
        for (xml_node<Ch> *child = tmp_node->next_sibling();
             child; child = child->next_sibling())
        {
          tmp_node = child->first_node(); // geometry_size
          value = tmp_node->value();
          geo_size = (unsigned) strtoul(value, &end, 10);
          if(value == end || *end != '\0')
          {
            fprintf(stderr, "Invalid geometry size: %s\n", value);
            ret = 1;
            goto invalid_data;
          }
          // Iterate over all collectives
          for (xml_node<Ch> *coll = tmp_node->next_sibling();
               coll; coll = coll->next_sibling())
          {
            tmp_node = coll->first_node(); // coll_id 
            value = tmp_node->value();
            coll_id = (unsigned) strtoul(value, &end, 10);
            if(value == end || *end != '\0' || coll_id >= PAMI_XFER_COUNT)
            {
              fprintf(stderr, "Invalid collective ID: %s\n", value);
              ret = 1;
              goto invalid_data;
            }
            // Iterate over all collectives
            for (xml_node<Ch> *msg = tmp_node->next_sibling();
                 msg; msg = msg->next_sibling())
            {
              tmp_node = msg->first_node(); // size_min
              value = tmp_node->value();
              size_min = (size_t) strtoul(value, &end, 10);
              if(value == end || *end != '\0')
              {
                fprintf(stderr, "Invalid message size: %s\n", value);
                ret = 1;
                goto invalid_data;
              }
              tmp_node = tmp_node->next_sibling(); // size_max
              value = tmp_node->value();
              size_max = (size_t) strtoul(value, &end, 10);
              if(value == end || *end != '\0')
              {
                fprintf(stderr, "Invalid message size: %s\n", value);
                ret = 1;
                goto invalid_data;
              }
              tmp_node = tmp_node->next_sibling(); // algorithms
              // Allocate memory from pool & copy numeric algo IDs
              // We might end up allocating a few extra bytes here if id > 10
              // Allocate an extra byte to store the size (at offset 0) of the array
              unsigned char *algo_list = (unsigned char *)
                data->mempool_allocate(((tmp_node->value_size() + 1) / 2) + 1);
              to_numeric(algo_list, tmp_node->value(), tmp_node->value_size());

              collsel_data[ppn][geo_size][coll_id][size_min] = algo_list;
              collsel_data[ppn][geo_size][coll_id][size_max] = algo_list;
            }
          }
        }
      invalid_data:
        return ret;
      }

      void read_xml_node(xml_node<Ch> *node,
                         CollselData  *data,
                         int flags)
      {
        switch (node->type())
          {
              // Element nodes
            case node_element:
            {
              xml_node<Ch> *child;
              if(strcmp(node->name(), COLLECTIVES_TAG) == 0)
              {
                if(parse_algorithm_mapping(node, data))
                  return;
                child = node->next_sibling();
              }
              else if(strcmp(node->name(), GEOMETRIES_TAG) == 0)
              {
                if(parse_collective_selection_data(node, data))
                  return;
                child = node->next_sibling();
              }
              else
                child = node->first_node();

              // Iterate children
              for (;child; child = child->next_sibling())
                read_xml_node(child, data, flags);
            }
            break;

            default:
              // Skip other node types
              break;
          }
      }

      int read_xml_file(std::basic_istream<Ch> &stream,
                        CollselData *data,
                        int flags,
                        const char *filename)
      {
        // Load data into vector
        stream.unsetf(std::ios::skipws);
        std::vector<Ch> v(std::istreambuf_iterator<Ch>(stream.rdbuf()),
                          std::istreambuf_iterator<Ch>());

        if (!stream.good())
          {
            fprintf(stderr, "Error reading from file: %s\n", filename);
            return 1;
          }

        v.push_back(0); // zero-terminate

        try
          {
            // Parse using appropriate flags
            const int f_tws = parse_normalize_whitespace
                              | parse_trim_whitespace;
            const int f_c = parse_comment_nodes;
            // Some compilers don't like the bitwise or in the template arg.
            const int f_tws_c = parse_normalize_whitespace
                                | parse_trim_whitespace
                                | parse_comment_nodes;
            xml_document<Ch> doc;

            if (flags & no_comments)
              {
                if (flags & trim_whitespace)
                  doc.BOOST_NESTED_TEMPLATE parse<f_tws>(&v.front());
                else
                  doc.BOOST_NESTED_TEMPLATE parse<0>(&v.front());
              }
            else
              {
                if (flags & trim_whitespace)
                  doc.BOOST_NESTED_TEMPLATE parse<f_tws_c>(&v.front());
                else
                  doc.BOOST_NESTED_TEMPLATE parse<f_c>(&v.front());
              }


            // Create data structure from nodes
            for (xml_node<Ch> *child = doc.first_node();
                 child; child = child->next_sibling())
              {
                read_xml_node(child, data, flags);
              }

          }
        catch (parse_error &e)
          {
            long line = static_cast<long>(
                          std::count(&v.front(), e.where<Ch>(), Ch('\n')) + 1);
            fprintf(stderr, "XML parser error: %s %s:%ld", e.what(), filename, line);
            return 1;
          }

        return 0;
      }
    public:
      int read_xml(const char  *filename,
                   CollselData *data,
                   int flags = 0,
                   const std::locale &loc = std::locale())
      {

        std::basic_ifstream<char> stream(filename, std::ios::in );

        if (!stream.is_open ())
          {
            fprintf(stderr, "Error opening file %s\n", filename);
            return 1;
          }

        stream.imbue(loc);

        // Reset the stream
        stream.clear(); // Clear eofbit
        stream.seekg(0, std::ios::beg); // Set stream position to beginning
        flags = flags | trim_whitespace | no_comments;


        //PAMI::GeometryShapeMap &ppn_map = collsel_data.get_datastore();
        read_xml_file(stream, data, flags, filename);

        stream.close();

        return 0;
      }

  };

}
#endif
