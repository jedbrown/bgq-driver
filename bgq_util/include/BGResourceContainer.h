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
/* (C) Copyright IBM Corp.  2006, 2011                              */
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

#ifndef _BG_RESOURCE_CONTAINER_H_
#define _BG_RESOURCE_CONTAINER_H_

#include <string>
#include <map>
#include <sstream>
#include <vector>

#include "bgq_util/include/string_tokenizer.h"

/*!
 * ABC for a container of resources
 */
template <typename BGResourceNodeContents>
class BGResourceContainer {
	
public:
	
	class BGResourceNode {
	public:
		BGResourceNode(const std::string& loc):location(loc){};
		BGResourceNodeContents* getContents() { return &contents; };
		void setContents( const BGResourceNodeContents& newcontents ) { contents = newcontents; };
		friend class BGResourceContainer;
	protected:
		std::string location; //<! Location of resource
	private:
		BGResourceNodeContents contents;
	};
	
	
	virtual ~BGResourceContainer(){};
	/*!
	 * \brief Return the node representing the resource.
	 * This will create a new node of none exists for this location, as well
	 * as the parent nodes to the root if they don't already exist.
	 */
	virtual BGResourceNode* getResourceNode (const std::string& location) = 0;
	
	/*!
	 * \brief Return the node representing the resource.
	 * 
	 * \param[in] currnode - Node to start the search with, if NULL, will search whole container.
	 * 
	 * \param[out] returns node representing the specific location, or NULL if none was found.
	 */
	virtual BGResourceNode* findResourceNode(const std::string& location) const = 0;

	
	/*!
	 * \brief Remove a node from the container.
	 */
	virtual int removeResource (const std::string& location, bool (*test)(BGResourceNodeContents*) ) = 0;
	
	/*!
	 * \brief Return all nodes related to this location.
	 * This will return a map of all nodes in order from the root to the node 
	 * listed by location
	 */
	virtual std::vector< std::pair< std::string, void*> > getAllResourceNodes (const std::string& location, bool getSubtree = true) = 0;
	
	/*! 
	 * \brief To std::string method.
	 * Need this in addition to the output operator because you can't have
	 * virtual friends.
	 */
	virtual std::string toString() const = 0;
	
	
	/*! 
	 * \brief Output operator.
	 * Print the contents of the container to the stream.
	 */
	friend std::ostream& operator<< (std::ostream& os, const BGResourceContainer* stuff )
	{
		if( &stuff == NULL )
			return os << "Container is NULL";
		else
			return os << stuff->toString();
	};
	
};

/*!
 * \brief Imlpementation for a simple, flat resource map.
 * This treats each location string as an individual resource unrelated to any other.
 */
template <typename BGResourceNodeContents>
class BGResourceMap 
:public BGResourceContainer<BGResourceNodeContents> {

protected:
	/*! This degenerate subclass keeps the compiler from complaining about the nested templates */
	class BGResourceMapNode 
	:public BGResourceContainer<BGResourceNodeContents>::BGResourceNode {
	public:
		BGResourceMapNode(const std::string& loc)
		:BGResourceContainer<BGResourceNodeContents>::BGResourceNode(loc){};
		
		friend class BGResourceMap;
	};
	
protected:
	std::map< std::string, void* > resource_map; //<! Map of resources

public:
	/*!
	 * \brief Return the node representing the resource.
	 * This will create a new node of none exists for this location, as well
	 * as the parent nodes to the root if they don't already exist.
	 */
	BGResourceMapNode* 
	getResourceNode(const std::string& location) {
		BGResourceMapNode* node = (BGResourceMapNode*)resource_map[location];
		if( node == NULL )
		{
			node = new BGResourceMapNode(location);
			resource_map[location] = node;
		}
		return node;
	};

	/*!
	 * \brief Remove a node from the map.
	 */
	int removeResource(const std::string& location, bool (*test)(BGResourceNodeContents*) )
	{
		BGResourceMapNode* deleteme = findResourceNode(location);
		if( (*test != NULL) && deleteme != NULL )
			if( !(*test)(deleteme->getContents()) )
				return -1;
		resource_map.erase(location);
		return 0;
	};

	virtual std::string toString() const
	{
		std::ostringstream oss;
		std::map< std::string, void* >::const_iterator node;
		for(	node = resource_map.begin();
				node != resource_map.end();
				node++ )
		{
			oss << "location: " << node->first << " contents: " << ((BGResourceMapNode*)(node->second))->getContents() << std::endl;
		}
		return oss.str();
	};


	/*!
	 * \brief Return all nodes related to this location.
	 * For the map implementation, this returns a vector of length zero or one.
	 */
	std::vector< std::pair<std::string, void*> > getAllResourceNodes(const std::string& location, bool getSubtree = true)
	{
		std::vector< std::pair<std::string, void*> > nodes;
		std::map<std::string, void* >::const_iterator node = resource_map.find(location);
		if( node != resource_map.end() )
			nodes.push_back(std::make_pair(node->first, node->second));
		return nodes;
	};

	BGResourceMapNode* findResourceNode(const std::string& location) const
	{
		std::map<std::string, void* >::const_iterator node = resource_map.find(location);
		if (node == resource_map.end() )
			return NULL;
		else
			return (BGResourceMapNode*)(node->second);
	};

};


/*!
 * \brief Implementation of a hierarchal resource tree.
 * This creates a hierarchy of nodes based on parsing the resource 
 * location string.
 */
template <typename BGResourceNodeContents>
class BGResourceTree
:public BGResourceContainer<BGResourceNodeContents> {
	
protected:
	class BGResourceTreeNode 
	:public BGResourceContainer<BGResourceNodeContents>::BGResourceNode {
	public:
		BGResourceTreeNode(const std::string& loc)
		:BGResourceContainer<BGResourceNodeContents>::BGResourceNode(loc){};
		
		friend class BGResourceTree;
		
	private:
		BGResourceTreeNode* parent; //<! parent node
		std::map<std::string, void*> children; //<! Child nodes by location
		
		BGResourceTreeNode()
		:parent(NULL){};		
	};
	
public:
	BGResourceTree( unsigned int max_depth = 5 )
	:depth(max_depth),loc_delims("-_") 
	{
		root = new BGResourceTreeNode(""); // Create root with empty location
		//next_node->location = loctok[i];
		root->parent = NULL;
	};
	/*!
	 * \brief Return the node representing the resource.
	 * This will create a new node of none exists for this location, as well
	 * as the parent nodes to the root if they don't already exist.
	 */
	BGResourceTreeNode* getResourceNode(const std::string& location) {
		BGResourceTreeNode* node = root;

		StringTokenizer loctok;
		/*int num_toks =*/ loctok.tokenize(location, loc_delims, NULL, depth );

		for (unsigned i=0; i<loctok.size(); i++) 
		{
			std::map< std::string, void* >::iterator next_node_ptr = node->children.find(loctok[i]);
			BGResourceTreeNode* next_node;
			if (next_node_ptr == node->children.end()) // If this node doesn't really exist, create a new one
			{
				next_node = new BGResourceTreeNode(loctok[i]);
				next_node->parent = node;
				node->children[loctok[i]] = next_node;
			}
			else
			{
				next_node = (BGResourceTreeNode*)(next_node_ptr->second);
			}
			node = next_node;
		}		
		
		return node;

	};

	
	/*!
	 * \brief Remove a node from the tree.
	 * This will remove the node for this location if it has no children.
	 * This also removes the parents leading to the root of the tree if
	 * they pass the test function and have no other children.
	 */
	int removeResource(const std::string& location, bool (*test)(BGResourceNodeContents*) )
	{
		BGResourceTreeNode* deleteme = findResourceNode( location );
		if( deleteme == NULL || deleteme == root )
			return -1;
		
		BGResourceTreeNode* parent = deleteme->parent;
		int rc = 0;
		if( (*test) != NULL )
		{
			if( (*test)(deleteme->getContents()) )
				rc = removeNode(deleteme);
		
			int prc = 0;
			while( parent != NULL && parent != root && prc >= 0 )
			{
				if( (*test)(parent->getContents()) )
				{
					deleteme = parent;
					parent = deleteme->parent;
					prc = removeNode(deleteme);
				}
				else 
					break;
			}
		}
		else
			rc = removeNode(deleteme);
		
		return rc;
	};
	
	int removeNode( BGResourceTreeNode* node )
	{
		if( node->children.size() != 0 )
			return -1;
		node->parent->children.erase(node->location);
		delete( node );
		return 0;
	};


	/*!
	 * \brief Return all nodes related to this location.
	 * This will return a vector of all nodes in order from the root to the node 
	 * representing the specific location (or as close as it gets) and the
	 * subtree for which the location is the root.
	 */
	std::vector< std::pair<std::string, void*> > getAllResourceNodes(const std::string& location, bool getSubtree = true )
	{
		BGResourceTreeNode* node = root;
		std::vector< std::pair<std::string, void*> > nodes;
		StringTokenizer loctok;
		/*int num_toks =*/ loctok.tokenize(location, loc_delims, NULL, depth );

		// This will get the path from the root to the node that represents the location
		unsigned i = 0;
		for( i=0; i<loctok.size(); i++)
		{
			std::map< std::string, void* >::iterator next_node = node->children.find(loctok[i]);
			if( next_node == node->children.end())
			{
				break;
			}
			else
			{
				node = (BGResourceTreeNode*)(next_node->second);
				nodes.push_back(std::make_pair(next_node->first,node));
			}
		}
		
		// Also need to return the subtree with the location as the root
		if( i == loctok.size() && getSubtree ) // This means we reached the node for this location, so it's in the tree
			DFSget( nodes, node );
		
		return nodes;
	}

	/*!
	 * \brief Return all nodes in the subtree of this location.
	 * This will return a vector of all nodes in order from the node 
	 * representing the specific location (or as close as it gets) and the
	 * subtree for which the location is the root.
	 */
	std::vector< std::pair<std::string, void*> > getSubTreeResourceNodes(const std::string& location)
	{
		BGResourceTreeNode* node = root;
		std::vector< std::pair<std::string, void*> > nodes;
		StringTokenizer loctok;
		/*int num_toks =*/ loctok.tokenize(location, loc_delims, NULL, depth );

		// This will traverse the path from the root to the node that represents the location
		unsigned i = 0;
		for( i=0; i<loctok.size(); i++)
		{
			std::map< std::string, void* >::iterator next_node = node->children.find(loctok[i]);
			if( next_node == node->children.end())
			{
				break;
			}
			else
			{
				node = (BGResourceTreeNode*)(next_node->second);
			}
		}
		
		// Return the subtree with the location as the root
		if( i == loctok.size() ) // This means we reached the node for this location, so it's in the tree
			DFSget( nodes, node );
		
		return nodes;
	}

	BGResourceTreeNode* findResourceNode(const std::string& location) const
	{
		
		BGResourceTreeNode* currnode = root;
			
		StringTokenizer loctok;
		/*int num_toks =*/ loctok.tokenize(location, loc_delims, NULL, depth );

		for (unsigned i=0; i<loctok.size(); i++) {
			std::map< std::string, void* >::const_iterator next_node = currnode->children.find(loctok[i]);
			if (next_node == currnode->children.end()) {
				// This means we ran out of nodes before finding the exact location
				return NULL;
			} else {
				currnode = (BGResourceTreeNode*)(next_node->second);
			}
		}
		return currnode;
	}


	std::string toString() const
	{
		std::ostringstream oss;
		DFSprint( oss, root, 0 );
		return oss.str();
	}
	
private:
	BGResourceTreeNode* root; //<! Root of the tree
	unsigned int depth; //<! Maximum tree depth (passed on tokenization of location string)
	const char* loc_delims; //<! Location delimiters (may have more than one)


	void DFSget(std::vector< std::pair<std::string, void*> >& nodes, BGResourceTreeNode* subtree) const
	{
	  for( 	std::map< std::string, void* >::const_iterator next_node = subtree->children.begin(); 
	  		next_node != subtree->children.end(); 
	  		next_node++) 
	  {
			BGResourceTreeNode* node = (BGResourceTreeNode*)(next_node->second);
			nodes.push_back(std::make_pair(next_node->first,node));
			DFSget( nodes, node ); // do the children
	  }
	}
	   
	void DFSprint(std::ostream& os, BGResourceTreeNode* subtree, unsigned depth) const
	{
	  DFSprintNode(os, subtree, depth); // print this node
	  
	  for( 	std::map< std::string, void* >::const_iterator next_node = subtree->children.begin(); 
	  		next_node != subtree->children.end(); 
	  		next_node++) 
	  {
	      DFSprint( os, (BGResourceTreeNode*)(next_node->second), depth+1); // do the children
	  }
	}
	   
	void DFSprintNode(std::ostream& os, BGResourceTreeNode* node, unsigned depth) const
	{
	    for( unsigned b = 0; b<=depth; b++)
	       os << " ";
		os << "location: " << node->location << " contents: " << node->getContents() << std::endl;
	}
};

#if 0
/*!
 * \brief Implementation of a hybrid hierarchal resource tree.
 * This creates a hybrid container which treats the begining of the 
 * location string as independent resources and the remainder as a 
 * hierarchy of nodes based on parsing the resource 
 * location string.
 */
template <typename BGResourceNodeContents>
class BGResourceHTree  
:public BGResourceMap <BGResourceTree<BGResourceNodeContents> > {
	
protected:
	class BGResourceHTreeNode :public BGResourceContainer<BGResourceNodeContents>::BGResourceNode {
	public:
		BGResourceHTreeNode(const std::string& loc)
		:BGResourceContainer<BGResourceNodeContents>::BGResourceNode(loc){};
		
		friend class BGResourceHTree;
		
	private:
		std::string location; //<! Location of resource
		BGResourceTree<BGResourceNodeContents>* child_root; //<! Root of heirarchal part
		
		BGResourceHTreeNode()
		:location(""),child_root(NULL){};		
	};
	
public:
	BGResourceHTree( unsigned int prefix = 3, unsigned int max_depth = 5 )
	:prefix_len(prefix),depth(max_depth),loc_delims("-") 
	{};
	
	/*!
	 * \brief Return the node representing the resource.
	 * This will create a new node of none exists for this location, as well
	 * as the parent nodes to the root if they don't already exist.
	 */
	BGResourceContainer<BGResourceNodeContents>::BGResourceNode* getResourceNode (const std::string& location)
	{
		BGResourceMap<BGResourceTree<BGResourceNodeContents> >::BGResourceMapNode* mn = BGResourceMap<BGResourceTree<BGResourceNodeContents> >::getResourceNode( location.substr(0,prefix_len) );
		if( location.length() <= prefix )
			return mn;
		BGResourceTree<BGResourceNodeContents>* tree = (BGResourceTree<BGResourceNodeContents>*)(mn->getContents());
		if( tree == NULL )
		{
			tree = new BGResourceTree<BGResourceNodeContents>(max_depth);
			mn->setContents( tree );
		}
		BGResourceTree<BGResourceNodeContents>::BGResourceTreeNode* tn = tree->getResourceNode( location.substr(prefix) );
		return tn;
	}
	
	/*!
	 * \brief Return the node representing the resource.
	 * 
	 * \param[in] currnode - Node to start the search with, if NULL, will search whole container.
	 * 
	 * \param[out] returns node representing the specific location, or NULL if none was found.
	 */
	virtual BGResourceContainer::BGResourceNode* findResourceNode(const std::string& location)
	{
		BGResourceMap<BGResourceTree<BGResourceNodeContents> >::BGResourceMapNode* mn = BGResourceMap<BGResourceTree<BGResourceNodeContents> >::findResourceNode( location.substr(0,prefix_len) );
		if( location.length() <= prefix || mn == NULL )
			return mn;
		BGResourceTree<BGResourceNodeContents>* tree = (BGResourceTree<BGResourceNodeContents>*)(mn->getContents());
		if( tree == NULL )
			return NULL;
		BGResourceTree<BGResourceNodeContents>::BGResourceTreeNode* tn = tree->findResourceNode( location.substr(prefix) );
		return tn;
	}

	
	/*!
	 * \brief Remove a node from the container.
	 */
	virtual int removeResource (const std::string& location, bool (*test)(BGResourceNodeContents*) )
	{
		BGResourceMap<BGResourceTree<BGResourceNodeContents> >::BGResourceMapNode* mn = BGResourceMap<BGResourceTree<BGResourceNodeContents> >::findResourceNode( location.substr(0,prefix_len) );
		if( mn == NULL )
			return -1;
		BGResourceTree<BGResourceNodeContents>* tree = (BGResourceTree<BGResourceNodeContents>*)(mn->getContents());
		if( location.length() > prefix ) // Remove nodes from tree only
		{
			if( tree == NULL )
				return -1;
			else
				return tree->removeResource( location.substr(prefix), test );
		}
		else // Remove map node and tree 
		{
			if( tree != NULL )
				delete( tree );
			
			return BGResourceMap<BGResourceTree<BGResourceNodeContents> >::removeResource( location.substr(0,prefix_len), test );
		}
	}
	
	/*!
	 * \brief Return all nodes related to this location.
	 * This will return a map of all nodes in order from the root to the node 
	 * listed by location
	 */
	virtual std::vector< std::pair< std::string, void*> > getAllResourceNodes (const std::string& location, bool getSubtree = true)
	{
		std::vector< std::pair<std::string, void*> > nodes;
		BGResourceMap<BGResourceTree<BGResourceNodeContents> >::BGResourceMapNode* mn = BGResourceMap<BGResourceTree<BGResourceNodeContents> >::findResourceNode( location.substr(0,prefix_len) );
		if( mn == NULL )
			return nodes;
		BGResourceTree<BGResourceNodeContents>* tree = (BGResourceTree<BGResourceNodeContents>*)(mn->getContents());
		if( tree != NULL )
		{
			nodes = tree->getAllResourceNodes( location.substr(prefix) );
		}
		// Add the map node
		nodes.push_back(std::make_pair(location.substr(0,prefix), mp));
		return nodes;
		
		
	}
	
	/*! 
	 * \brief To std::string method.
	 * Need this in addition to the output operator because you can't have
	 * virtual friends.
	 */
	virtual std::string toString() const
	{
		return BGResourceMap<BGResourceTree<BGResourceNodeContents> >::toString();
	}
	
	
private:
	unsigned prefix_len;
	unsigned depth;
	std::string loc_delims;
	
};
#endif

#endif
