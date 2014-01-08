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

#include <string>
#include <sstream>
#include <iostream>

#include "BGResourceContainer.h"


	struct TestResource
	{
		std::string name;
		
		TestResource( )
		:name("noname"){};
		
		TestResource( std::string aname )
		:name(aname){};
		
		TestResource( const TestResource& oldone )
		:name(oldone.name){};
		
		friend std::ostream& operator<< (std::ostream& os, const TestResource& stuff )
		{
			return os << stuff.name;
		}
		
		// Tests whether resource can be removed
		static bool removeCheck( TestResource* resource )
		{
			return true;
		}
	};

/*!
main does it all
*/
int main( int argc, char* argv[] )
{

	BGResourceContainer< unsigned >* testContainer1;
	
	testContainer1 = new BGResourceMap< unsigned >();
	for( unsigned i=0; i<10; i++)
	{
		std::ostringstream loc;
		loc << "R00-M0-N00-J0" << i;
		BGResourceContainer< unsigned >::BGResourceNode* node = testContainer1->getResourceNode( loc.str() );
		node->setContents( i );
	}
	
	std::cout << "testContainer1 BGResourceMap< unsigned >" << std::endl;
	std::cout << testContainer1;
	
	BGResourceContainer< TestResource >* testContainer;
	
	testContainer = new BGResourceMap< TestResource >();
	for( unsigned i=0; i<10; i++)
	{
		std::ostringstream loc;
		loc << "R00-M0-N00-J0" << i;
		BGResourceContainer< TestResource >::BGResourceNode* node = testContainer->getResourceNode( loc.str() );
		TestResource tr(loc.str());
		node->setContents( tr );
	}
	
	std::cout << "testContainer BGResourceMap< TestResource >" << std::endl;
	std::cout << testContainer;
	
	TestResource* oneResource = testContainer->getResourceNode("R00-M0-N00-J01")->getContents();
	std::cout << "Resource at R00-M0-N00-J01 = " << oneResource << std::endl;
	testContainer->removeResource("R00-M0-N00-J01", TestResource::removeCheck);
	std::cout << "testContainer BGResourceMap< TestResource >" << std::endl;
	std::cout << testContainer;
	
	testContainer = new BGResourceTree< TestResource >();
	for( unsigned m=0; m<2; m++)
	{
	for( unsigned n=0; n<3; n++)
	{
	for( unsigned j=0; j<10; j++)
	{
		std::ostringstream loc;
		loc << "R00-M" << m << "-N0" << n << "-J0" << j;
		BGResourceContainer< TestResource >::BGResourceNode* node = testContainer->getResourceNode( loc.str() );
		TestResource tr(loc.str());
		node->setContents( tr );
	}
	}
	}
	
	std::cout << "testContainer BGResourceTree< TestResource >" << std::endl;
	std::cout << testContainer;

	oneResource = testContainer->getResourceNode("R00-M1-N00-J05")->getContents();
	std::cout << "Resource at R00-M1-N00-J05 = " << *oneResource << std::endl;
	testContainer->removeResource("R00-M1-N00-J05", TestResource::removeCheck);
	std::cout << "testContainer BGResourceMap< TestResource >" << std::endl;
	std::cout << testContainer;
	
	
}
