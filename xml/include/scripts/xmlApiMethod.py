
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
class XmlApiMethod:
    """
    """
    
    def __init__(self, name, attrs):
        """
        """
        
        self._typeName = name
        self._name = attrs['name']
        self._comment = None
        self._type = None
        
        if 'comment' in attrs.keys():
            self._comment = attrs['comment'].capitalize()
            if self._comment[-1] != '.':
                self._comment += '.'
                
        if 'type' in attrs.keys():
            self._type = attrs['type']
            
    def getNameCapFirst(self):
        return self._name[0].upper() + self._name[1:]
    
    def getDecl(self, indent, namespace):
        """
        """
        
        text  = indent + "/*!\n"
        if self._comment != None:
            text += indent + " * " + self._comment + "\n"
        else:
            text += indent + " * " + self._name + " method.\n"
        text += indent + " *\n"
        text += indent + " * @param " + self.getNameCapFirst() + "Request\n"
        text += indent + " *           request object\n"
        text += indent + " * @param " + self.getNameCapFirst() + "Reply\n"
        text += indent + " *           reply object\n"
        text += indent + " */\n"
        text += indent + "virtual void " + self._name + "(const " + namespace + "::" + self.getNameCapFirst() + "Request& request, " + namespace + "::" + self.getNameCapFirst() + "Reply& reply) = 0;\n"

        return text
    
    def getImpl(self, indent, namespace):
        """
        """
        
        text  = indent + "/*!\n"
        if self._comment != None:
            text += indent + " * " + self._comment + "\n"
        else:
            text += indent + " * " + self._name + " method.\n"
        text += indent + " *\n"
        text += indent + " * @param " + self.getNameCapFirst() + "Request\n"
        text += indent + " *           request object\n"
        text += indent + " * @param " + self.getNameCapFirst() + "Reply\n"
        text += indent + " *           reply object\n"
        text += indent + " */\n"
        text += indent + "virtual void " + self._name + "(const " + namespace + "::" + self.getNameCapFirst() + "Request& request, " + namespace + "::" + self.getNameCapFirst() + "Reply& reply)\n"
        text += indent + "{\n"
        text += indent + "  sendReceive(" + namespace + "::" + self.getNameCapFirst() + "Request::getClassName(), request, " + namespace + "::" + self.getNameCapFirst() + "Reply::getClassName(), reply);\n"
        text += indent + "}\n"
    
        return text
        
