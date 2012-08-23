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

#ifndef COORDSYS_H
#define COORDSYS_H

#include <Base/Vector3D.h>

#include "DocumentObject.h"
#include "PropertyStandard.h"
#include "PropertyGeo.h"
#include "PropertyUnits.h"

namespace App
{

/*!
 * This is the App part of the Coordinate System implementation.
 */
class AppExport CoordinateSystem : public DocumentObject
{
  PROPERTY_HEADER(App::CoordinateSystem);
  
  public:
    /// Constructor
    CoordinateSystem();
    ~CoordinateSystem();
    
    /** @name Properties */
    //@{
    App::PropertyInteger   id;
    App::PropertyString    name;
    App::PropertyVector    origin;   
    App::PropertyMatrix    orientation;
    //@}

    /** @name Actions */
    //@{
    /// recalculate the object
    App::DocumentObjectExecReturn* execute(void);
    //@}

    /// returns the type name of the ViewProvider
    const char* getViewProviderName(void) const
    { 
      return "Gui::ViewProviderCoordinateSystem";
    }
    
    PyObject* getPyObject(void);
    
    void setOrigin( Base::Vector3f v );
    void setOrientation( Base::Matrix4D m );
    
    Base::Vector3f transformToGlobal( Base::Vector3f v );
    Base::Vector3f transformFromGlobal( Base::Vector3f v );

};

} //namespace App


#endif
