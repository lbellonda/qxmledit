/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2012-2018 by Luca Bellonda and individual contributors  *
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


#ifndef XSDGRAPHICSCONFIGURATION_H
#define XSDGRAPHICSCONFIGURATION_H

#include <QColor>

class XSDGraphicsBackgroundConfiguration
{
public:
    enum EGradientType {
        GradientSolid,
        GradientLinear,
        GradientRadial
    };

    enum EGradientDirection {
        DirectionVertical,
        DirectionHorizontal,
        DirectionDiagonal
    };

    enum EGradientUnits {
        GradientUnitsPct,
        GradientUnitsPixels
    };

    QColor  _mainColor;
    QColor  _alternateColor;
    EGradientType _gradientType;
    EGradientDirection _gradientDirection;
    int _gradientHeight;
    EGradientUnits _gradientUnits;

public:
    XSDGraphicsBackgroundConfiguration();
    ~XSDGraphicsBackgroundConfiguration();

    QColor mainColor();
    void setMainColor(const QColor value);
    QColor alternateColor();
    void setAlternateColor(const QColor value);
    EGradientType gradientType();
    void setGradientType(const EGradientType value);
    EGradientDirection gradientDirection();
    void setGradientDirection(const EGradientDirection value);
    int gradientHeight();
    void setGradientHeight(const int value);
    EGradientUnits gradientUnits();
    void setGradientUnits(const EGradientUnits value);

    void copyTo(XSDGraphicsBackgroundConfiguration*other);

    void save();
    void load();
    void reset();
};

class XSDGraphicsConfiguration
{
    XSDGraphicsBackgroundConfiguration _background;
public:
    XSDGraphicsConfiguration();
    ~XSDGraphicsConfiguration();

    void save();
    void load();

};

#endif // XSDGRAPHICSCONFIGURATION_H
