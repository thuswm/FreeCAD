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
 *   This library is distributed in the hope that it will be useful,       *
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

#ifndef VIEW_PROVIDER_COORDSYS_H
#define VIEW_PROVIDER_COORDSYS_H

#include <Base/Vector3D.h>
#include "ViewProviderDocumentObject.h"
#include <QObject>

class SoFontStyle;
class SoText2;
class SoBaseColor;
class SoTranslation;
class SoTransform;
class SoCoordinate3;
class SoIndexedLineSet;
class SoEventCallback;
class SoMarkerSet;

namespace Gui
{

/*!
 * CoordSysVis is the Gui part of my coordinate system implementation.
 */
class GuiExport ViewProviderCoordinateSystem : public ViewProviderDocumentObject
{
  PROPERTY_HEADER(Gui::ViewProviderCoordinateSystem);

  public:
    /// Constructor
    ViewProviderCoordinateSystem();
    ~ViewProviderCoordinateSystem();
    
    /// Display properties
    App::PropertyColor          TextColor;
    App::PropertyColor          LineColor;
    App::PropertyInteger        FontSize;

    void                        attach(App::DocumentObject *);
    void                        updateData(const App::Property*);
    std::vector<std::string>    getDisplayModes(void) const;
    void                        setDisplayMode(const char* ModeName);

    static void                 measureDistanceCallback(void * ud, SoEventCallback * n);

protected:
    void                        onChanged(const App::Property* prop);
    void                        inverse3x3( SbMatrix * m );
    std::string                 convertMatrix( SbMatrix m);
    std::string                 convertMatrix( Base::Matrix4D m);
    void                        Matrix4DtoSbMatrix( Base::Matrix4D m_in, SbMatrix &m_out );
    void                        test();

private:
    SoTransform                 * cTransform;
    SoFontStyle                 * pFont;
    
    SoText2                     * pNameLabel;
    SoText2                     * pXLabel;
    SoText2                     * pYLabel;
    SoText2                     * pZLabel;
    
    SoBaseColor                 * pTextColor;
    
    SoTranslation               * pTranslation;
    SoTranslation               * xTranslation;
    SoTranslation               * yTranslation;
    SoTranslation               * zTranslation;
    
    SoBaseColor                 * pColor;
    SoCoordinate3               * pCoords;
    SoIndexedLineSet            * pLines;
};

} //namespace Gui


#endif //  VIEW_PROVIDER_COORDSYS_H
