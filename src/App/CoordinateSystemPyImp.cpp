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

#include "App/CoordinateSystem.h"

// inclusion of the generated files (generated out of CoordinateSystemPy.xml)
#include "CoordinateSystemPy.h"
#include "CoordinateSystemPy.cpp"

#include <Base/MatrixPy.h>
#include <Base/VectorPy.h>

using namespace App;

// returns a string which represents the object e.g. when printed in python
std::string CoordinateSystemPy::representation(void) const
{
    return std::string("<CoordinateSystem object>");
}


PyObject* CoordinateSystemPy::setOrigin( PyObject * arg )
{

  PyObject *obj;
  if (!PyArg_ParseTuple(arg, "O!", &(Base::VectorPy::Type), &obj)) // I do not understand the "O!" string
  {
    std::string error = std::string("tried to convert argument to 'Vector' but failed!");
    throw Py::TypeError(error);
  }
  
  Base::VectorPy* v = static_cast<Base::VectorPy*>(obj);
  
  getCoordinateSystemPtr()->setOrigin( Base::Vector3f( v->getVectorPtr()->x,
                                                       v->getVectorPtr()->y,
                                                       v->getVectorPtr()->z ) );
  
  // Return nothing Py_None
  Py_Return;
}

PyObject* CoordinateSystemPy::setOrientation(PyObject * arg )
{
  PyObject *obj;
  if (!PyArg_ParseTuple(arg, "O!", &(Base::MatrixPy::Type), &obj)) // I do not understand the "O!" string
  {
    std::string error = std::string("tried to convert argument to 'Matrix' but failed!");
    throw Py::TypeError(error);
  }
  
  Base::MatrixPy* m = static_cast<Base::MatrixPy*>(obj);
  
  getCoordinateSystemPtr()->setOrientation( *(m->getMatrixPtr()) );
  
  // Return nothing Py_None
  Py_Return;
  
}

PyObject* CoordinateSystemPy::transformToGlobal(PyObject * arg )
{
  // This code has been influenced by from src/Base/VectorPyImp.cpp
 
  PyObject *obj;
  if (!PyArg_ParseTuple(arg, "O!", &(Base::VectorPy::Type), &obj)) // I do not understand the "O!" string
  {
    std::string error = std::string("tried to convert argument to 'Vector' but failed!");
    throw Py::TypeError(error);
  }
       
  Base::VectorPy* vec = static_cast<Base::VectorPy*>(obj);

  // return vector
  Base::Vector3f v_out;
  
  // this might get simpler if the C++ member excepts Vector3d?
  v_out = (getCoordinateSystemPtr()->transformToGlobal( Base::Vector3f(vec->getVectorPtr()->x, 
                                                                       vec->getVectorPtr()->y,
                                                                       vec->getVectorPtr()->z) ));
  
  return new Base::VectorPy(v_out);
  
}

PyObject* CoordinateSystemPy::transformFromGlobal(PyObject * arg )
{
  // This code has been influenced by from src/Base/VectorPyImp.cpp
 
  PyObject *obj;
  if (!PyArg_ParseTuple(arg, "O!", &(Base::VectorPy::Type), &obj)) // I do not understand the "O!" string
  {
    std::string error = std::string("tried to convert argument to 'Vector' but failed!");
    throw Py::TypeError(error);
  }
       
  Base::VectorPy* vec = static_cast<Base::VectorPy*>(obj);

  // return vector
  Base::Vector3f v_out;
  
  // get the transformed vector
  v_out = (getCoordinateSystemPtr()->transformFromGlobal( Base::Vector3f(vec->getVectorPtr()->x, 
                                                                         vec->getVectorPtr()->y,
                                                                         vec->getVectorPtr()->z) )); 
  
  return new Base::VectorPy(v_out);
  
}


// What do the following two functions do? 
PyObject *CoordinateSystemPy::getCustomAttributes(const char* /*attr*/) const
{
    return 0;
}

int CoordinateSystemPy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0; 
}


