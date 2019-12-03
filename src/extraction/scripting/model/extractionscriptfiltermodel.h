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

#ifndef EXTRACTIONSCRIPTFILTERMODEL_H
#define EXTRACTIONSCRIPTFILTERMODEL_H

#include "xmlEdit.h"
#include "extractionscripteventmodel.h"
#include <QSet>

class ExtractionScriptFilterModel
{
    QString _id;
    bool _enabled;
    QString _name;
    QString _description;
    QHash<int, ExtractionScriptEventModel*> _events;
    QSet<ExtractionScriptEventModel*> _eventsSet;

    bool readFromDom(const QDomElement &element);
    bool readFromSerializedXmlString(const QString &codedValue);

public:
    ExtractionScriptFilterModel();
    ~ExtractionScriptFilterModel();
    bool enabled() const;
    void setEnabled(bool enabled);
    QString id() const;
    void setId(const QString &id);
    QString name() const;
    void setName(const QString &name);
    QString description() const;
    void setDescription(const QString &description);
    void addEventModel(ExtractionScriptEventModel *newModel);

    QSet<ExtractionScriptEventModel*> events();

    static ExtractionScriptFilterModel* fromXMLString(const QString &codedValue);
};

#endif // EXTRACTIONSCRIPTFILTERMODEL_H
