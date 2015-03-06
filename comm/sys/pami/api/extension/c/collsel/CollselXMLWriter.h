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
#ifndef __api_extension_c_collsel_XMLWriter_h__
#define __api_extension_c_collsel_XMLWriter_h__
#include "boost/property_tree/detail/rapidxml.hpp"
#include <ostream>
#include <sstream>
#include <fstream>
#include <iterator>
#include "CollselData.h"

extern char **environ;

namespace PAMI
{
  using  namespace boost::property_tree::detail::rapidxml;

  const char* PAMI_TUNE_TAG     = "pami_tune";
  const char* PAMI_ENV_TAG      = "pami_environment";
  const char* COLLSEL_TAG       = "collective_selection";
  const char* VERSION_TAG       = "version";
  const char* COLLECTIVES_TAG   = "collectives";
  const char* COLLECTIVE_TAG    = "collective";
  const char* COLLECTIVE_ID_TAG = "collective_id";
  const char* ALGO_TAG          = "algorithm";
  const char* ALGO_ID_TAG       = "id";
  const char* ALGO_NAME_TAG     = "name";
  const char* GEOMETRIES_TAG    = "geometries";
  const char* PPN_TAG           = "ppn";
  const char* GEOMETRY_TAG      = "geometry";
  const char* GEOMETRY_SIZE_TAG = "geometry_size";
  const char* COLL_TAG          = "coll";
  const char* COLL_ID_TAG       = "coll_id";
  const char* MESSAGE_TAG       = "message";
  const char* SIZE_MIN_TAG      = "size_min";
  const char* SIZE_MAX_TAG      = "size_max";
  const char* ALGORITHMS_TAG    = "algorithms";
  const char* OP_ATTR           = "op";
  const char* DT_ATTR           = "dt";

  template <class Iter = std::back_insert_iterator<std::string>, class Ch = char>
  class XMLWriter
  {

  public:
    XMLWriter():tmp_str(new char[STR_MAX_SZ])
    {
    }

    ~XMLWriter()
    {
      delete[] tmp_str;
    }

    int write_xml(const std::string &filename,
                  PAMI::GeometryShapeMap &collsel_map,
                  PAMI::AlgoMap *algo_map,
                  const std::locale &loc = std::locale())
    {
      std::basic_ofstream<Ch> stream(filename.c_str(), std::ios::out | std::ios::trunc);
      if (!stream.is_open ())
      {
         fprintf(stderr,"Error opening file %s\n", filename.c_str());
         return -1;
      }

      stream.imbue(loc);


      // Add the XML declaration
      xml_node<>* decl = doc.allocate_node(node_declaration);
      decl->append_attribute(doc.allocate_attribute("version", "1.0"));
      decl->append_attribute(doc.allocate_attribute("encoding", "utf-8"));
      doc.append_node(decl);

      //Add root node
      xml_node<>* root = doc.allocate_node(node_element, PAMI_TUNE_TAG);
      doc.append_node(root);

      // Create a comma separated list of MP_* environment variables
      char **env;
      std::ostringstream sstream;
      bool flag = true;
      for (env = environ; *env != NULL; env++)
      {
        if(strncmp(*env,"MP_",3) == 0)
        {
          if(flag)
          {
            sstream << *env;
            flag = false;
          }
          else
          {
            sstream << "," << *env;
          }
        }
      }
      // Save the PAMI/MPI environment variables
      xml_node<>* environment = doc.allocate_node(node_element, PAMI_ENV_TAG,
           doc.allocate_string(sstream.str().c_str(), strlen(sstream.str().c_str())+1) );
      root->append_node(environment);

      xml_node<>* collsel = doc.allocate_node(node_element, COLLSEL_TAG);
      root->append_node(collsel);
      collsel->append_node(doc.allocate_node(node_element, VERSION_TAG, "1.0"));
      // Save the collective algorithm ID <-> pami_algorithm_t mapping
      xml_node<>* collectives = doc.allocate_node(node_element, COLLECTIVES_TAG);
      append_algo_map_data(collectives, algo_map);
      collsel->append_node(collectives);
      // Save the collective selection data
      append_ppn_data(collsel, collsel_map);
      std::string xml_as_string;
      write(std::back_inserter(xml_as_string), doc);

      //XMLWriter::write(std::back_inserter(xml_as_string), doc, XMLWriter::write_no_linefeed);
      stream << xml_as_string;
      stream.flush();
      stream.close();

      return 0;
    }

  private:
    // Copy characters to output iterator
    inline Iter copy_characters(const Ch *start, const Ch *end, Iter out)
    {
      while (start != end)
        *out++ = *start++;

      return out;
    }

    inline Iter write_node(Iter out, const xml_node<Ch> *node, int flags, int indent)
    {
      switch (node->type())
        {
          case node_document:
            out = write_children(out, node, flags, indent);
            break;
          case node_element:
            out = write_element(out, node, flags, indent);
            break;
          case node_data:
            out = write_data(out, node, flags, indent);
            break;
          case node_comment:
            out = write_comment(out, node, flags, indent);
            break;
          case node_declaration:
            out = write_xml_declaration(out, node, flags, indent);
            break;
          default:
            assert(0);
            break;
        }
      if (!(flags & write_no_linefeed))
        *out = Ch('\n'), ++out;

      return out;
    }

    inline Iter write_children(Iter out, const xml_node<Ch> *node, int flags, int indent)
    {
      for (xml_node<Ch> *child = node->first_node(); child; child = child->next_sibling())
        out = write_node(out, child, flags, indent);
      return out;
    }

    inline Iter write_data(Iter out, const xml_node<Ch> *node, int flags, int indent)
    {
      assert(node->type() == node_data);
      out = copy_characters(node->value(), node->value() + node->value_size(), out);
      return out;
    }

    inline Iter write_attributes(Iter out, const xml_node<Ch> *node, int flags)
    {
      for (xml_attribute<Ch> *attr = node->first_attribute(); attr; attr = attr->next_attribute())
        {
          if (attr->name() && attr->value())
            {
              *out = Ch(' '), ++out;
              out = copy_characters(attr->name(), attr->name() + attr->name_size(), out);
              *out = Ch('='), ++out;
              *out = Ch('"'), ++out;
              out = copy_characters(attr->value(), attr->value() + attr->value_size(), out);
              *out = Ch('"'), ++out;
            }
        }

      return out;
    }

    inline Iter write_element(Iter out, const xml_node<Ch> *node, int flags, int indent)
    {
      assert(node->type() == node_element);

      *out = Ch('<'), ++out;
      out = copy_characters(node->name(), node->name() + node->name_size(), out);
      out = write_attributes(out, node, flags);

      // Node is childless
      if (node->value_size() == 0 && !node->first_node())
        {
          *out = Ch('/'), ++out;
          *out = Ch('>'), ++out;
        }
      else
        {
          *out = Ch('>'), ++out;

          xml_node<Ch> *child = node->first_node();

          if (!child)
            {
              out = copy_characters(node->value(), node->value() + node->value_size(), out);
            }
          else if (child->next_sibling() == 0 && child->type() == node_data)
            {
              out = copy_characters(child->value(), child->value() + child->value_size(), out);
            }
          else
            {
              if (!(flags & write_no_linefeed))
                *out = Ch('\n'), ++out;

              out = write_children(out, node, flags, indent + 1);
            }

          *out = Ch('<'), ++out;
          *out = Ch('/'), ++out;
          out = copy_characters(node->name(), node->name() + node->name_size(), out);
          *out = Ch('>'), ++out;
        }

      return out;
    }

    inline Iter write_xml_declaration(Iter out, const xml_node<Ch> *node, int flags, int indent)
    {
      *out = Ch('<'), ++out;
      *out = Ch('?'), ++out;
      *out = Ch('x'), ++out;
      *out = Ch('m'), ++out;
      *out = Ch('l'), ++out;
      out = write_attributes(out, node, flags);
      *out = Ch('?'), ++out;
      *out = Ch('>'), ++out;

      return out;
    }

    inline Iter write_comment(Iter out, const xml_node<Ch> *node, int flags, int indent)
    {
      assert(node->type() == node_comment);
      *out = Ch('<'), ++out;
      *out = Ch('!'), ++out;
      *out = Ch('-'), ++out;
      *out = Ch('-'), ++out;
      out = copy_characters(node->value(), node->value() + node->value_size(), out);
      *out = Ch('-'), ++out;
      *out = Ch('-'), ++out;
      *out = Ch('>'), ++out;

      return out;
    }

    // Write XML to given output iterator.
    inline Iter write(Iter out, const xml_node<Ch> &node, int flags = 0)
    {
      return write_node(out, &node, flags, 0);
    }

    // Write XML to given output stream.
    inline std::basic_ostream<Ch> &write(std::basic_ostream<Ch> &out, const xml_node<Ch> &node, int flags = 0)
    {
      write(std::ostream_iterator<Ch>(out), node, flags);
      return out;
    }

    void append_algo_map_data(xml_node<Ch> *collectives_node, AlgoMap *algo_map)
    {
      AlgoMap::iterator iter;
      xml_node<>* algo_node, *collective_node = NULL;
      int tmp_str_sz, i;
      for (i = 0; i < PAMI_XFER_COUNT; ++i)
      {
        for (iter = algo_map[i].begin(); iter != algo_map[i].end(); iter++)
        {
          if(!collective_node)
          {
            collective_node = doc.allocate_node(node_element, COLLECTIVE_TAG);
            tmp_str_sz = snprintf(tmp_str, STR_MAX_SZ,"%u",i);
            collective_node->append_node(doc.allocate_node(node_element, COLLECTIVE_ID_TAG,
                      doc.allocate_string(tmp_str, tmp_str_sz+1)));
          }
          algo_node = doc.allocate_node(node_element, ALGO_TAG);
          tmp_str_sz = snprintf(tmp_str, STR_MAX_SZ,"%u",iter->first);
          algo_node->append_node(doc.allocate_node(node_element, ALGO_ID_TAG,
                    doc.allocate_string(tmp_str, tmp_str_sz+1)));
          algo_node->append_node(doc.allocate_node(node_element, ALGO_NAME_TAG,
                    doc.allocate_string(iter->second)));
          collective_node->append_node(algo_node);
        }
        if(collective_node)
        {
          collectives_node->append_node(collective_node);
          collective_node = NULL;
        }
      }
    }

    void append_ppn_data(xml_node<Ch> *collsel_node, GeometryShapeMap &collsel_map)
    {
      GeometryShapeMap::iterator iter;
      xml_node<>* geometries_node;
      int tmp_str_sz;
      for (iter = collsel_map.begin(); iter != collsel_map.end(); iter++)
      {
        geometries_node = doc.allocate_node(node_element, GEOMETRIES_TAG);
        tmp_str_sz = snprintf(tmp_str, STR_MAX_SZ,"%u",iter->first);
        geometries_node->append_node(doc.allocate_node(node_element, PPN_TAG,
                  doc.allocate_string(tmp_str, tmp_str_sz+1)));
        collsel_node->append_node(geometries_node);
        append_geometry_data(geometries_node, iter->second);
      }
    }

    void append_geometry_data(xml_node<Ch> *geometries_node, GeometrySizeMap &geo_map)
    {
      GeometrySizeMap::iterator iter;
      xml_node<>* geometry_node;
      int tmp_str_sz;
      for (iter = geo_map.begin(); iter != geo_map.end(); iter++)
      {
        geometry_node = doc.allocate_node(node_element, GEOMETRY_TAG);
        tmp_str_sz = snprintf(tmp_str, STR_MAX_SZ,"%u",iter->first);
        geometry_node->append_node(doc.allocate_node(node_element, GEOMETRY_SIZE_TAG,
                  doc.allocate_string(tmp_str, tmp_str_sz+1)));
        geometries_node->append_node(geometry_node);
        append_collective_data(geometry_node, iter->second);
      }
    }

    void append_collective_data(xml_node<Ch> *geometry_node, CollectivesMap &coll_map)
    {
      CollectivesMap::iterator iter;
      xml_node<>* coll_node;
      int tmp_str_sz;
      for (iter = coll_map.begin(); iter != coll_map.end(); iter++)
      {
        coll_node = doc.allocate_node(node_element, COLL_TAG);
        tmp_str_sz = snprintf(tmp_str, STR_MAX_SZ,"%u",iter->first);
        coll_node->append_node(doc.allocate_node(node_element, COLL_ID_TAG,
                  doc.allocate_string(tmp_str, tmp_str_sz+1)));
        geometry_node->append_node(coll_node);
        append_message_data(coll_node, iter->second);
      }
    }

    void append_message_data(xml_node<Ch> *coll_node, MessageSizeMap &msg_map)
    {
      MessageSizeMap::iterator iter, prev_iter;
      xml_node<>* msg_node;
      int tmp_str_sz;
      unsigned char *algolist;
      for (iter = msg_map.begin(); iter != msg_map.end(); iter++)
      {
        msg_node = doc.allocate_node(node_element, MESSAGE_TAG);
        tmp_str_sz = snprintf(tmp_str, STR_MAX_SZ,"%zu",iter->first);
        msg_node->append_node(doc.allocate_node(node_element, SIZE_MIN_TAG,
                  doc.allocate_string(tmp_str, tmp_str_sz+1)));
        algolist = iter->second;
        if(++iter != msg_map.end())
        {
          if(strcmp((const char*)algolist, (const char*)iter->second) == 0)
          {
            tmp_str_sz = snprintf(tmp_str, STR_MAX_SZ,"%zu",iter->first);
          }
          else
            --iter;
        }
        else
          --iter;
        msg_node->append_node(doc.allocate_node(node_element, SIZE_MAX_TAG,
                  doc.allocate_string(tmp_str, tmp_str_sz+1)));
        msg_node->append_node(doc.allocate_node(node_element, ALGORITHMS_TAG,
                  doc.allocate_string((const char *)iter->second, strlen((const char *)iter->second)+1)));
        coll_node->append_node(msg_node);
      }
    }

    // Temporary storage for integer to string conversions
    char  *tmp_str;
    //Create a new XML document
    xml_document<> doc;
    // Max tmp_str size
    static const int STR_MAX_SZ;
    // XML tag & attribute names
    static const int write_no_linefeed;   //Instruct to add linefeeds
  };

  template<class Iter,class Ch>   const int XMLWriter<Iter,Ch>::STR_MAX_SZ = 16;
  template<class Iter,class Ch>   const int XMLWriter<Iter,Ch>::write_no_linefeed = 0x1;   //Instruct to add linefeeds
}
#endif
