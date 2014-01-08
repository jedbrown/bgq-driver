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
#include "common/SubBlock.h"

#include <boost/program_options/errors.hpp>

namespace runjob {

SubBlock::SubBlock() throw() :
    _corner(),
    _cornerSet( false ),
    _shape(),
    _shapeSet( false )
{

}

SubBlock::SubBlock(
        const Corner& corner,
        const Shape& shape
        ) :
    _corner(),
    _cornerSet( false ),
    _shape(),
    _shapeSet( false )
{
    this->setCorner( corner );
    this->setShape( shape );
}

void
SubBlock::setCorner(
        const Corner& corner
        )
{
    _corner = corner;
    _cornerSet = true;
    this->validate();
}

void
SubBlock::setShape(
        const Shape& shape
        )
{
    _shape = shape;
    _shapeSet = true;
    this->validate();
}

bool
SubBlock::isValid() const
{
    return _corner.isValid() && _shape.isValid();
}

void
SubBlock::validate()
{
    if ( !_shapeSet || !_cornerSet ) return;

    if ( _corner.isValid() && _shape.isValid() ) {
        // if we have a compute core location, shape will be ignored
        if ( _corner.isCoreSet() ) {
            _shape = Shape( "1x1x1x1x1" );
        }
    } else if ( _corner.isValid() && !_shape.isValid() ) {
        // corner but no shape, if we have a compute node location, this is an error
        if ( !_corner.isCoreSet() ) {
            throw boost::program_options::error(
                    "--corner provided but missing --shape"
                    );
        } else {
            _shape = Shape( "1x1x1x1x1" );
        }
    } else if ( !_corner.isValid() && _shape.isValid() ) {
        throw boost::program_options::error(
                "--shape provided but missing --corner"
                );
    } else {
        // valid
    }
}

} // runjob
