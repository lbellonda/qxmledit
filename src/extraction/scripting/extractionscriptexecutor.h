/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2019 by Luca Bellonda and individual contributors       *
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

#ifndef EXTRACTIONSCRIPTCONTAINER_H
#define EXTRACTIONSCRIPTCONTAINER_H

#include <QObject>
#include "extractionscriptfilter.h"
/**
 * @brief Holds the execution scripts set sorted by type
 */
class ExtractionScriptContainer : public QObject
{
    Q_OBJECT

    QList<ExtractionScriptFilter*> _textEvents;
    QList<ExtractionScriptFilter*> _elementEvents;

public:
    explicit ExtractionScriptContainer(QObject *parent = NULL);
    ~ExtractionScriptContainer();

    void reset();
    bool init();
    QList<ExtractionScriptFilter*> allTextEvents();
    QList<ExtractionScriptFilter*> allElementEvents();
    void addTextEventHandler(ExtractionScriptFilter *filter);
    void addElementEventHandler(ExtractionScriptFilter *filter);

signals:

public slots:
};

#endif // EXTRACTIONSCRIPTCONTAINER_H
