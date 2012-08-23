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
# include <sstream>
# include <QApplication>
# include <Inventor/SoPickedPoint.h>
# include <Inventor/events/SoMouseButtonEvent.h>
# include <Inventor/nodes/SoSeparator.h>
# include <Inventor/nodes/SoBaseColor.h>
# include <Inventor/nodes/SoFontStyle.h>
# include <Inventor/nodes/SoPickStyle.h>
# include <Inventor/nodes/SoText2.h>
# include <Inventor/nodes/SoTranslation.h>
# include <Inventor/nodes/SoTransform.h>
# include <Inventor/nodes/SoCoordinate3.h>
# include <Inventor/nodes/SoIndexedLineSet.h>
# include <Inventor/nodes/SoMarkerSet.h>
# include <Inventor/nodes/SoDrawStyle.h>
# include <Inventor/actions/SoGetMatrixAction.h>
#endif

#include "ViewProviderCoordinateSystem.h"
#include "SoFCSelection.h"
#include "Application.h"
#include "Document.h"
#include "View3DInventorViewer.h"

#include <App/PropertyGeo.h>
#include <App/PropertyStandard.h>
#include <App/MeasureDistance.h>
#include <Base/Console.h>

#include <boost/lexical_cast.hpp>
#include <iostream>

using namespace Gui;

/*!
 * Struct container for transformation data. Used to clean up the code.
 */
struct Transform_struct
{
  SbVec3f    t;  // translation
  SbRotation r;  // rotation, quaternion
  SbVec3f    s;  // scale
  SbRotation so; // scale orientation
};


PROPERTY_SOURCE(Gui::ViewProviderCoordinateSystem, Gui::ViewProviderDocumentObject)

/*!
 * Constructor. 
 */
ViewProviderCoordinateSystem::ViewProviderCoordinateSystem() 
{
    // set properties
    ADD_PROPERTY(TextColor,(1.0f,1.0f,1.0f));
    ADD_PROPERTY(LineColor,(0.0f,1.0f,0.0f));
    ADD_PROPERTY(FontSize,(18));

    // initialize Coin objects
    cTransform = new SoTransform();
    
    pFont = new SoFontStyle();
    pFont->ref();
    pNameLabel = new SoText2();
    pNameLabel->justification = SoText2::LEFT;
    pNameLabel->ref();

    pColor = new SoBaseColor();
    pColor->ref();
    pTextColor = new SoBaseColor();
    pTextColor->ref();
    pTranslation = new SoTranslation();
    pTranslation->ref();
    pTranslation->translation = SbVec3f(0.1,0.1,0.1);

    // set all properties touched - whatever that means
    TextColor.touch();
    FontSize.touch();
    LineColor.touch();

    // Create axes as lines
    // ====================
    
    // coordinates for the line vertices.
    // basically the centre and the tips of the coordinate axes
    static const SbVec3f verts[4] =
    {
        SbVec3f(0,0,0), SbVec3f(1.,0,0),
        SbVec3f(0,1.,0), SbVec3f(0,0,1.)
    };

    // indexes used to create the lines
    static const int32_t lines[9] =
    {
        0,1,-1,
        0,2,-1,
        0,3,-1
    };
    
    // prepare the vertices for lines
    pCoords = new SoCoordinate3();
    pCoords->ref();
    pCoords->point.setNum(4);
    pCoords->point.setValues(0, 4, verts);

    // prepare the line set
    pLines  = new SoIndexedLineSet();
    pLines->ref();
    pLines->coordIndex.setNum(9);
    pLines->coordIndex.setValues(0, 9, lines);
    
    
    // Create the axes labels
    // ======================
    // is there a more efficient way to do this?
    // x-axis
    xTranslation = new SoTranslation();
    xTranslation->ref();
    xTranslation->translation = SbVec3f(1.05-0.1,-0.1,-0.1);
    
    pXLabel = new SoText2();
    pXLabel->ref();
    pXLabel->string.setValue("x");
    
    // y-axis
    yTranslation = new SoTranslation();
    yTranslation->ref();
    yTranslation->translation = SbVec3f(-1.05,1.05,0);
    
    pYLabel = new SoText2();
    pYLabel->ref();
    pYLabel->string.setValue("y");
    
    // z-axis
    zTranslation = new SoTranslation();
    zTranslation->ref();
    zTranslation->translation = SbVec3f(0,-1.05,1.05);
    
    pZLabel = new SoText2();
    pZLabel->ref();
    pZLabel->string.setValue("z");
    
    // reference to icon
    //sPixmap = "view-coordiateSystem";
    
}

/*!
 * Destructor of the view provider. Unreferences all Coin objects.
 */
ViewProviderCoordinateSystem::~ViewProviderCoordinateSystem()
{
    pFont->unref();
    pNameLabel->unref();
    pXLabel->unref();
    pYLabel->unref();
    pZLabel->unref();
    
    xTranslation->unref();
    yTranslation->unref();
    zTranslation->unref();
    
    pColor->unref();
    pTextColor->unref();
    pTranslation->unref();
    pCoords->unref();
    pLines->unref();
}

/*!
 * This member function gets called when a property is changed.
 */
void
ViewProviderCoordinateSystem::onChanged(const App::Property* prop)
{
    // check which property is changed and implement the change
    if (prop == &TextColor) 
    {
        const App::Color& c = TextColor.getValue();
        pTextColor->rgb.setValue(c.r,c.g,c.b);
    }
    else if (prop == &LineColor) 
    {
        const App::Color& c = LineColor.getValue();
        pColor->rgb.setValue(c.r,c.g,c.b);
    }
    else if (prop == &FontSize) 
    {
        pFont->size = FontSize.getValue();
    }
    else 
    {
        ViewProviderDocumentObject::onChanged(prop);
    }
}

/*!
 * Returns the display mode?
 */
std::vector<std::string>
ViewProviderCoordinateSystem::getDisplayModes(void) const
{
    // add modes
    std::vector<std::string> StrList;
    StrList.push_back("Base");
    return StrList;
}

/*!
 * Sets the display mode.
 */
void
ViewProviderCoordinateSystem::setDisplayMode(const char* ModeName)
{
    if (strcmp(ModeName, "Base") == 0)
    {
      setDisplayMaskMode("Base");
    }
    
    ViewProviderDocumentObject::setDisplayMode(ModeName);
}

/*!
 */
void
ViewProviderCoordinateSystem::attach(App::DocumentObject* pcObject)
{
    // call the base class member
    ViewProviderDocumentObject::attach(pcObject);

    // Set pick style for geometry
    SoPickStyle* ps = new SoPickStyle();
    ps->style = SoPickStyle::UNPICKABLE;

    // draw axes
    SoSeparator *lineSep = new SoSeparator();
    SoDrawStyle* style = new SoDrawStyle();
    style->lineWidth = 2.0f;
    lineSep->addChild(ps);
    lineSep->addChild(style);
    lineSep->addChild(pColor);
    lineSep->addChild(pCoords);
    lineSep->addChild(pLines);
    

    // Making the whole Coordinate System object selectable by 3d view can
    // become cumbersome since it has influence to any raypick action.
    // Thus, it's only selectable by its text label
    
    // This class is locally defined in FreeCAD
    // This node does the complete highlighting and selection together with the viewer
    SoFCSelection* textsep = new SoFCSelection(); 
    
    textsep->objectName = pcObject->getNameInDocument();
    textsep->documentName = pcObject->getDocument()->getName();
    textsep->subElementName = "Main";
    
    textsep->addChild(pTranslation);
    textsep->addChild(pTextColor);
    textsep->addChild(pFont);
    textsep->addChild(pNameLabel);
    
    SoSeparator* axes = new SoSeparator();
    axes->addChild(xTranslation);
    axes->addChild(pXLabel);
    axes->addChild(yTranslation);
    axes->addChild(pYLabel);
    axes->addChild(zTranslation);
    axes->addChild(pZLabel);
    
    
    // root (top) separator for the whole coordinate system object
    SoSeparator* sep = new SoSeparator();
    sep->addChild(cTransform);
    sep->addChild(lineSep);
    sep->addChild(textsep);
    sep->addChild(axes);
    
    // Adds a new display mask mode.
    addDisplayMaskMode(sep, "Base");
}

/*!
 * Gets called if some of the property hade bin changed
 */
void
ViewProviderCoordinateSystem::updateData(const App::Property* prop)
{
    // test
#if 0
    test();
#endif    
    
    // what does this do?
    if( prop->getTypeId() == App::PropertyVector::getClassTypeId() ) 
    { 
    }
    
    // if the Label has been changed
    if(strcmp(prop->getName(),"Label") == 0)
    {
      // - get name from App object
      const char* s = static_cast<const App::PropertyString*>(prop)->getValue();
        
      // set label
      pNameLabel->string.setValue(s);
    }


    // if the origin has changed
    if(strcmp(prop->getName(),"origin") == 0)
    {
       // - get the origin vector from the App object
      Base::Vector3f v = static_cast<const App::PropertyVector*>(prop)->getValue();
      
      // - get the current transformation matrix
      SbMatrix cur, cur_inv;
      cTransform->getTranslationSpaceMatrix( cur, cur_inv );
      
      // - replace the translation part of the matrix
      Transform_struct trans;
      cur.getTransform( trans.t, trans.r, trans.s, trans.so );
      
      cur.setTransform( SbVec3f( v.x, v.y, v.z ), trans.r, trans.s, trans.so );
      
      // - set the transformation matrix in the transformation nodes
      cTransform->setMatrix( cur );
    }
    
    // if the orientation has been changed
    if(strcmp(prop->getName(),"orientation") == 0)
    {
      
      // - get the orientation matrix from the App object
      Base::Matrix4D m = static_cast<const App::PropertyMatrix*>(prop)->getValue();
      
      SbMatrix mt, mc, ic;
      Matrix4DtoSbMatrix( m, mt );
      
      // - get current translation
      Transform_struct trans_cur;
      cTransform->getTranslationSpaceMatrix( mc, ic );
      mc.getTransform( trans_cur.t, trans_cur.r, trans_cur.s, trans_cur.so );
      
      // - get new rotation
      Transform_struct trans_new;
      mt.getTransform( trans_new.t, trans_new.r, trans_new.s, trans_new.so );
      
      // - set currnet translation
      mt.setTransform( trans_cur.t, trans_new.r, trans_cur.s, trans_cur.so );
      
      // - Invert the upper 3x3 matrix
      inverse3x3( &mt );
 
      // - check that axes are orthogonal
      
      // - set the transformation matrix in the transformation nodes
      cTransform->setMatrix( mt );
    }

    // call base class member
    ViewProviderDocumentObject::updateData(prop);
}

/*!
 * This function returns a matrix where the upper left 3x3 matrix is inverted. The rest 
 * of the 4x4 matrix is unchanged.
 * 
 * Throws an exception if the determinant of the upper left 3x3 matrix is close to zero.
 */
void
ViewProviderCoordinateSystem::inverse3x3( SbMatrix * m )
{
  
  // calculate the determinant for upper left 3x3
  float det = m->det3();
  
  // check determinant
  if( abs(det) < 0.01 )
  {
    std::string error = std::string("Transformation matrix is not invertable (det3 == 0)!");   
    error += std::string("SbMatrix:") + "\n" + convertMatrix( *m ) + "\n";

    throw Base::Exception(error);
  }
  
  // make a copy of 4th row and 4th column
  SbMatrix cm = *m;
  
  // set 4th row and column to 0. except for element (4,4). Set it to 1.
  SbMat &f(*m);
  
  f[0][3] = 0;
  f[1][3] = 0;
  f[2][3] = 0;
  f[3][3] = 1.;
  f[3][2] = 0;
  f[3][1] = 0;
  f[3][0] = 0;
  
  // invert matrix
  m->inverse();
  
  // write back 4th row and column
  SbMat &g(cm);
  f[0][3] = g[0][3];
  f[1][3] = g[1][3];
  f[2][3] = g[2][3];
  f[3][3] = g[3][3];
  f[3][2] = g[3][2];
  f[3][1] = g[3][1];
  f[3][0] = g[3][0];
  
  // return
  
}

/*!
 * Used for debugging
 * Converts a SbMatrix to a std::string
 */
std::string
ViewProviderCoordinateSystem::convertMatrix( SbMatrix m)
{
    // get rows from out matrix
    float* row1 = m[0];
    float* row2 = m[1];
    float* row3 = m[2];
    float* row4 = m[3];
  
    std::string ms;
   
    // row 1
    ms += boost::lexical_cast<std::string>(row1[0]) + "  ";
    ms += boost::lexical_cast<std::string>(row1[1]) + "  ";
    ms += boost::lexical_cast<std::string>(row1[2]) + "  ";
    ms += boost::lexical_cast<std::string>(row1[3]) + "\n";
    
    // row 2
    ms += boost::lexical_cast<std::string>(row2[0]) + "  ";
    ms += boost::lexical_cast<std::string>(row2[1]) + "  ";
    ms += boost::lexical_cast<std::string>(row2[2]) + "  ";
    ms += boost::lexical_cast<std::string>(row2[3]) + "\n";
    
    // row 3
    ms += boost::lexical_cast<std::string>(row3[0]) + "  ";
    ms += boost::lexical_cast<std::string>(row3[1]) + "  ";
    ms += boost::lexical_cast<std::string>(row3[2]) + "  ";
    ms += boost::lexical_cast<std::string>(row3[3]) + "\n";
    
    // row 4
    ms += boost::lexical_cast<std::string>(row4[0]) + "  ";
    ms += boost::lexical_cast<std::string>(row4[1]) + "  ";
    ms += boost::lexical_cast<std::string>(row4[2]) + "  ";
    ms += boost::lexical_cast<std::string>(row4[3]);
    
    return ms;
}


/*!
 * Used for debugging.
 * Converts a Base::Matrix4D to a std::string
 */
std::string
ViewProviderCoordinateSystem::convertMatrix( Base::Matrix4D m)
{
    // get rows from out matrix
    double* row1 = m[0];
    double* row2 = m[1];
    double* row3 = m[2];
    double* row4 = m[3];
  
    std::string ms;
   
    // row 1
    ms += boost::lexical_cast<std::string>(row1[0]) + "  ";
    ms += boost::lexical_cast<std::string>(row1[1]) + "  ";
    ms += boost::lexical_cast<std::string>(row1[2]) + "  ";
    ms += boost::lexical_cast<std::string>(row1[3]) + "\n";
    
    // row 2
    ms += boost::lexical_cast<std::string>(row2[0]) + "  ";
    ms += boost::lexical_cast<std::string>(row2[1]) + "  ";
    ms += boost::lexical_cast<std::string>(row2[2]) + "  ";
    ms += boost::lexical_cast<std::string>(row2[3]) + "\n";
    
    // row 3
    ms += boost::lexical_cast<std::string>(row3[0]) + "  ";
    ms += boost::lexical_cast<std::string>(row3[1]) + "  ";
    ms += boost::lexical_cast<std::string>(row3[2]) + "  ";
    ms += boost::lexical_cast<std::string>(row3[3]) + "\n";
    
    // row 4
    ms += boost::lexical_cast<std::string>(row4[0]) + "  ";
    ms += boost::lexical_cast<std::string>(row4[1]) + "  ";
    ms += boost::lexical_cast<std::string>(row4[2]) + "  ";
    ms += boost::lexical_cast<std::string>(row4[3]);
    
    return ms;
}

/*!
 * Converts a Matrix4D to a SbMatrix.
 * This function exists since the implementation differences between the two matrices are a 
 * bit unfortunate...
 */
void
ViewProviderCoordinateSystem::Matrix4DtoSbMatrix( Base::Matrix4D m_in, SbMatrix &m_out )
{
  // get rows from in matrix
  double* in_row1 = m_in[0];
  double* in_row2 = m_in[1];
  double* in_row3 = m_in[2];
  double* in_row4 = m_in[3];
  
  // get rows from out matrix
  float* out_row1 = m_out[0];
  float* out_row2 = m_out[1];
  float* out_row3 = m_out[2];
  float* out_row4 = m_out[3];
  
  // copy data - row 1
  for(int i=0; i<4; i++)
  {
    out_row1[i] = float(in_row1[i]);
  }
  
  // copy data - row 2
  for(int i=0; i<4; i++)
  {
    out_row2[i] = float(in_row2[i]);
  }
  
  // copy data - row 3
  for(int i=0; i<4; i++)
  {
    out_row3[i] = float(in_row3[i]);
  }
  
  // copy data - row 4
  for(int i=0; i<4; i++)
  {
    out_row4[i] = float(in_row4[i]);
  }
  
}


/*!
 * For test only.
 */
void
ViewProviderCoordinateSystem::test()
{
}
// ----------------------------------------------------------------------------

