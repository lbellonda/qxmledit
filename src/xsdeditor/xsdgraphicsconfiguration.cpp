/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2012 by Luca Bellonda and individual contributors       *
 *    as indicated in the AUTHORS file                                    *
 *  lbellonda _at_ gmail.com                                              *
 *                                                                        *
 * This library is free software; you can redistribute it and/or          *
 * modify it under the terms of the GNU Library General Public            *
 * License as published by the Free Software Foundation; either           *
 * version 2 of the License, or (at your option) any later version.       *
 *                                                                        *
 * This library is distributed in the hope that it will be useful,        *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU      *
 * Library General Public License for more details.                       *
 *                                                                        *
 * You should have received a copy of the GNU Library General Public      *
 * License along with this library; if not, write to the                  *
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,       *
 * Boston, MA  02110-1301  USA                                            *
 **************************************************************************/


#include "xsdgraphicsconfiguration.h"
#include "utils.h"
#include "qxmleditconfig.h"


XSDGraphicsBackgroundConfiguration::XSDGraphicsBackgroundConfiguration()
{
    reset();
}

XSDGraphicsBackgroundConfiguration::~XSDGraphicsBackgroundConfiguration()
{

}

void XSDGraphicsBackgroundConfiguration::reset()
{
    _mainColor = QColor(220, 220, 220);
    _alternateColor = QColor(190, 190, 190);
    _gradientType = GradientLinear;
    _gradientDirection = DirectionVertical;
    _gradientHeight = 100 ;
    _gradientUnits = GradientUnitsPixels;
}

void XSDGraphicsBackgroundConfiguration::save()
{
    Config::saveColor(Config::KEY_XSD_DIAGRAM_BK_GRADIENTMAINCOLOR, _mainColor);
    Config::saveColor(Config::KEY_XSD_DIAGRAM_BK_GRADIENTALTCOLOR, _alternateColor);
    Config::saveInt(Config::KEY_XSD_DIAGRAM_BK_GRADIENTTYPE, _gradientType);
    Config::saveInt(Config::KEY_XSD_DIAGRAM_BK_GRADIENTDIRECTION, _gradientDirection);
    Config::saveInt(Config::KEY_XSD_DIAGRAM_BK_GRADIENTHEIGHT, _gradientHeight);
    Config::saveInt(Config::KEY_XSD_DIAGRAM_BK_GRADIENTUNITS, _gradientUnits);
}


void XSDGraphicsBackgroundConfiguration::load()
{
    XSDGraphicsBackgroundConfiguration theDefault;
    _mainColor = Config::getColor(Config::KEY_XSD_DIAGRAM_BK_GRADIENTMAINCOLOR, theDefault.mainColor());
    _alternateColor = Config::getColor(Config::KEY_XSD_DIAGRAM_BK_GRADIENTALTCOLOR, theDefault.alternateColor());
    _gradientType = (XSDGraphicsBackgroundConfiguration::EGradientType)Config::getInt(Config::KEY_XSD_DIAGRAM_BK_GRADIENTTYPE, theDefault.gradientType());
    _gradientDirection = (XSDGraphicsBackgroundConfiguration::EGradientDirection)Config::getInt(Config::KEY_XSD_DIAGRAM_BK_GRADIENTDIRECTION, theDefault.gradientDirection());
    _gradientHeight = Config::getInt(Config::KEY_XSD_DIAGRAM_BK_GRADIENTHEIGHT, theDefault.gradientHeight());
    _gradientUnits  = (XSDGraphicsBackgroundConfiguration::EGradientUnits)Config::getInt(Config::KEY_XSD_DIAGRAM_BK_GRADIENTUNITS, theDefault.gradientUnits());
}


QColor XSDGraphicsBackgroundConfiguration::mainColor()
{
    return _mainColor ;
}


void XSDGraphicsBackgroundConfiguration::setMainColor(const QColor value)
{
    _mainColor = value ;
}

QColor XSDGraphicsBackgroundConfiguration::alternateColor()
{
    return _alternateColor ;
}


void XSDGraphicsBackgroundConfiguration::setAlternateColor(const QColor value)
{
    _alternateColor = value ;
}

XSDGraphicsBackgroundConfiguration::EGradientType XSDGraphicsBackgroundConfiguration::gradientType()
{
    return _gradientType ;
}


void XSDGraphicsBackgroundConfiguration::setGradientType(const EGradientType value)
{
    _gradientType = value ;
}

XSDGraphicsBackgroundConfiguration::EGradientDirection XSDGraphicsBackgroundConfiguration::gradientDirection()
{
    return _gradientDirection ;
}


void XSDGraphicsBackgroundConfiguration::setGradientDirection(const EGradientDirection value)
{
    _gradientDirection = value ;
}

int XSDGraphicsBackgroundConfiguration::gradientHeight()
{
    return _gradientHeight ;
}


void XSDGraphicsBackgroundConfiguration::setGradientHeight(const int value)
{
    _gradientHeight = value ;
}

XSDGraphicsBackgroundConfiguration::EGradientUnits XSDGraphicsBackgroundConfiguration::gradientUnits()
{
    return _gradientUnits ;
}


void XSDGraphicsBackgroundConfiguration::setGradientUnits(const EGradientUnits value)
{
    _gradientUnits = value ;
}

void XSDGraphicsBackgroundConfiguration::copyTo(XSDGraphicsBackgroundConfiguration*other)
{
    other->setMainColor(mainColor());
    other->setAlternateColor(alternateColor());
    other->setGradientType(gradientType());
    other->setGradientDirection(gradientDirection());
    other->setGradientHeight(gradientHeight());
    other->setGradientUnits(gradientUnits());
}

//-----------------------------------------------------------

XSDGraphicsConfiguration::XSDGraphicsConfiguration()
{
}

XSDGraphicsConfiguration::~XSDGraphicsConfiguration()
{
}

void XSDGraphicsConfiguration::save()
{
    _background.save();
}


void XSDGraphicsConfiguration::load()
{
    _background.load();
}
