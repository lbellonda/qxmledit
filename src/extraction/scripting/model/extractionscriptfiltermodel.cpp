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

#include "extractionscriptfiltermodel.h"
#include "xmlutils.h"
#include "utils.h"

static int instancesCount = 0;

//
#define ATTR_ENABLED    "enabled"
#define ELM_EVENT       "event"
#define ELM_FILTER      "filter"
//

ExtractionScriptFilterModel::ExtractionScriptFilterModel()
{
    _enabled = false;
    instancesCount ++;
}

ExtractionScriptFilterModel::~ExtractionScriptFilterModel()
{
    _events.clear();
    foreach(ExtractionScriptEventModel *event, _eventsSet.values()) {
        delete event;
    }
    _eventsSet.clear();
    instancesCount--;
    Q_ASSERT(instancesCount > -1);
}

bool ExtractionScriptFilterModel::enabled() const
{
    return _enabled;
}

void ExtractionScriptFilterModel::setEnabled(bool enabled)
{
    _enabled = enabled;
}

QString ExtractionScriptFilterModel::name() const
{
    return _name;
}

void ExtractionScriptFilterModel::setName(const QString &name)
{
    _name = name;
}

QString ExtractionScriptFilterModel::id() const
{
    return _id;
}

void ExtractionScriptFilterModel::setId(const QString &id)
{
    _id = id;
}

QString ExtractionScriptFilterModel::description() const
{
    return _description;
}

void ExtractionScriptFilterModel::setDescription(const QString &description)
{
    _description = description;
}

void ExtractionScriptFilterModel::addEventModel(ExtractionScriptEventModel *newModel)
{
    if(NULL != newModel) {
        _eventsSet.insert(newModel);
        _events.insert(newModel->eventType(), newModel);
    }
}

QSet<ExtractionScriptEventModel*> ExtractionScriptFilterModel::events()
{
    return _eventsSet;
}

ExtractionScriptFilterModel* ExtractionScriptFilterModel::fromXMLString(const QString &codedValue)
{
    ExtractionScriptFilterModel* model = new ExtractionScriptFilterModel();
    if(NULL != model) {
        if(model->readFromSerializedXmlString(codedValue)) {
            return model;
        }
        delete model ;
    }
    return NULL ;
}

bool ExtractionScriptFilterModel::readFromSerializedXmlString(const QString &codedValue)
{
    QDomDocument document;
    if(document.setContent(codedValue)) {
        int nodi = document.childNodes().size();
        for(int i = 0 ; i < nodi ; i ++) {
            QDomNode childNode = document.childNodes().item(i) ;
            if(childNode.isElement()) {
                QDomElement element = childNode.toElement();
                if(element.tagName() == ELM_FILTER) {
                    if(readFromDom(element)) {
                        return true ;
                    }
                }
            }
        }
    }
    return false;
}

bool ExtractionScriptFilterModel::readFromDom(const QDomElement &element)
{
    _enabled = XmlUtils::readFromBool(element.attribute(ATTR_ENABLED), false);
    int nodi = element.childNodes().size();
    for(int i = 0 ; i < nodi ; i ++) {
        QDomNode childNode = element.childNodes().item(i) ;
        if(childNode.isElement()) {
            QDomElement child = childNode.toElement();
            if(ELM_EVENT == child.tagName()) {
                ExtractionScriptEventModel *event = new ExtractionScriptEventModel();
                if(!event->scanEventFromDom(child)) {
                    delete event ;
                }
                addEventModel(event);
            }
        }
    }
    return true;
}
