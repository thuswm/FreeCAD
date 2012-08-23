/***************************************************************************
 *   Copyright (c) 2012 Michael Thuswaldner <thuswm[at]gmail.com>          *
 *                                                                         *
 *   This file is part of the FreeCAD CAx development system.              *
 *                                                                         *
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Library General Public           *
 *   License as published by the Free Software Foundation; either          *
 *   version 2 of the License, or (at your option) any later version.      *
 *                                                                         *
 *   This library  is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                  *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this library; see the file COPYING.LIB. If not,    *
 *   write to the Free Software Foundation, Inc., 51 Franklin Street,      *
 *   Fifth Floor, Boston, MA  02110-1301, USA                              *
 *                                                                         *
 ***************************************************************************/

#include "PreCompiled.h"

#ifndef _PreComp_
#endif

#include "CoordinateSystem.h"
#include "CoordinateSystemPy.h"

using namespace App;

PROPERTY_SOURCE(App::CoordinateSystem, App::DocumentObject)

/*!
 * Constructor.
 */
CoordinateSystem::CoordinateSystem()
{
  
    ADD_PROPERTY_TYPE(id,(0),"Coordinate System",App::Prop_None,"The id number of the coordinate system");
    ADD_PROPERTY_TYPE(name,("coord"),"Coordinate System",App::Prop_None,"The name of the coordinate system");
    ADD_PROPERTY_TYPE(origin,(Base::Vector3f()),"Coordinate System",App::Prop_None,"Position of the local origo given in global coordinates"); 
    ADD_PROPERTY_TYPE(orientation,(Base::Matrix4D(1.,0.,0.,0., 0.,1.,0.,0., 0.,0.,1.,0., 0.,0.,0.,1.)),"Coordinate System",App::Prop_None,"The transformation matrix"); 
   
}

/*!
 * Destructor ... is it needed?
 */
CoordinateSystem::~CoordinateSystem()
{
}

/*! 
 * Will not be used for a start. Maybe later when I understand what it is used for.
 */
/*
short
CoordinateSystem::mustExecute() const
{
}
*/

/*!
 * There are no calculations to be performed at execution.
 */
App::DocumentObjectExecReturn*
CoordinateSystem::execute(void)
{
  return DocumentObject::StdReturn;
}

/*!
 * I do not fully understand this member function but apparently it needs 
 * to be implemented in order for the python I/F to work. 
 * 
 * The only thing it does is to return a python object.
 */
PyObject*
CoordinateSystem::getPyObject(void)
{
  //std::cout << "referencing CoordinateSystem" << std::endl;
  if (PythonObject.is(Py::_None()))
  {
    // ref counter is set to 1
    PythonObject = Py::Object(new CoordinateSystemPy(this),true);  
  }
  
  return Py::new_reference_to(PythonObject); 

}

/*!
 * setOrigin sets the origin of the local coordinate system. The 
 * vector v provides the coordinate in the Global coordiante system 
 * for the origin of the local coordinate system.
 */
void
CoordinateSystem::setOrigin( Base::Vector3f v )
{
  origin.setValue( v );
}

/*!
 * setOrientation sets the transformation matrix for the local 
 * coordinate system. This means that each column of the 3x3 
 * transformation matrix m can be viewed as one of the three local
 * coordinate axes representedd in the global coordinate system
 * 
 * [m] = [<x-axis> <y-axis> <z-axis>]
 * 
 * Note that the length of each axis under normal circumstances 
 * should be normated to 1. If not, you will get a scale effect on the
 * transformation which might be confusing.
 */
void
CoordinateSystem::setOrientation( Base::Matrix4D m )
{
  orientation.setValue( m );
}

/*!
 * transformToGlobal transforms local coordinates, given in vector v,
 * to the global coordinate system.
 */
Base::Vector3f
CoordinateSystem::transformToGlobal( Base::Vector3f v )
{
  Base::Matrix4D o = orientation.getValue();
  return o*v;
}

/*!
 * transformFromGlobal transforms global coordinates, given in vector v,
 * to the local coordinate system.
 */
Base::Vector3f
CoordinateSystem::transformFromGlobal( Base::Vector3f v )
{
  Base::Matrix4D o = orientation.getValue();
  Base::Matrix4D inv = o;
  
  inv.inverse();
  
  return inv*v;

}

