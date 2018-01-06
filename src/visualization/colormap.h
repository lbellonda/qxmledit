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
#ifndef COLORMAP_H
#define COLORMAP_H

#include <QObject>
#include <QColor>

class ColorMap : public QObject
{
    Q_OBJECT
protected:
    static const int Map0To1;
    static const int Map1To2;
    static const int Map2To3;

    uint *_values;
    QString _name;


    virtual bool initMap();
    void fillRange(const int start, const int end, uint *p, uint colorStart, uint colorEnd);
public:
    static const int MapElements;

    explicit ColorMap(const QString &name, QObject *parent = 0);
    virtual ~ColorMap();
    virtual uint backgroundColor();
    virtual uint emptyColor();

    uint *values();
    QString name();

signals:

public slots:

};

#endif // COLORMAP_H
