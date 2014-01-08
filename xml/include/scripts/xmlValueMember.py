
# begin_generated_IBM_copyright_prolog                             
#                                                                  
# This is an automatically generated copyright prolog.             
# After initializing,  DO NOT MODIFY OR MOVE                       
# ================================================================ 
#                                                                  
# Licensed Materials - Property of IBM                             
#                                                                  
# Blue Gene/Q                                                      
#                                                                  
# (C) Copyright IBM Corp.  2011, 2011                              
#                                                                  
# US Government Users Restricted Rights -                          
# Use, duplication or disclosure restricted                        
# by GSA ADP Schedule Contract with IBM Corp.                      
#                                                                  
# This software is available to you under the                      
# Eclipse Public License (EPL).                                    
#                                                                  
# ================================================================ 
#                                                                  
# end_generated_IBM_copyright_prolog                               
class XmlValueMember:
    """
    An XmlValueMember object.
    """
    
    def __init__(self, name, attrs):
        """
        The constructor.
        
        \param name
                 the name of the value set member.
        \param attrs
                 the XML attributes for the value set member.
        """

        self._typeName = name
        self._name = attrs['name']
        self._value = None
        self._comment = None
        
        if 'value' in attrs.keys():
            self._value = attrs['value']
        
        if 'comment' in attrs.keys():
            self._comment = attrs['comment'].capitalize()
            if self._comment[-1] != '.':
                self._comment += '.'
