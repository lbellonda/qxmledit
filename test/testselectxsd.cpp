/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2014 by Luca Bellonda and individual contributors       *
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


#include "testselectxsd.h"
#include "app.h"
#include "xsdeditor/xschema.h"
#include <stdio.h>

#define FILE_1  "../test/data/xsd/select/one.xml"
#define FILE_2  "../test/data/xsd/select/two.xsd"

TestSelectXsd::TestSelectXsd()
{
}

TestSelectXsd::~TestSelectXsd()
{
}

//---------------------

bool TestSelectXsd::testForward()
{
    _testName = "testForward";
    App app;
    if(!loadFile(app, FILE_1)) {
        return false;
    }
    ElementLoadInfoMap loadInfoMap;
    Regola *regola = app.mainWindow()->getRegola();
    regola->getAsText(&loadInfoMap);
    QList<int> pos;
    pos << 1;
    if(!findElementPos(regola, pos, "*1", "root", &loadInfoMap)) {
        return false;
    }
    pos.clear();
    pos << 1 << 0 ;
    if(!findElementPos(regola, pos, "*1*0", "a", &loadInfoMap)) {
        return false;
    }
    return true;
}

bool TestSelectXsd::testBack()
{
    _testName = "testBack";
    App app;
    if(!loadFile(app, FILE_2)) {
        return false;
    }
    ElementLoadInfoMap loadInfoMap;
    Regola *regola = app.mainWindow()->getRegola();
    QString text = regola->getAsText(&loadInfoMap);
    XSDSchema schema(NULL);
    XSDLoadContext loadContext;
    loadContext.setLoadKeys(true);
    if(!schema.readFromString(&loadContext, text, true, app.data()->xsdNetworkAccessManager(), "") ) {
        return error("unable to load schema from string");
    }
    if(!findSchemaObject("*1", "", SchemaTypeSchema, loadContext.inputKeyMap())) {
        return false;
    }
    if(!findSchemaObject("*1*2", "Plane", SchemaTypeElement, loadContext.inputKeyMap())) {
        return false;
    }
    return true;
}

//--------------------


bool TestSelectXsd::findSchemaObject(const QString &searchKey, const QString &expectedName, const ESchemaType expectedType, QHash<QString, XSchemaObject*> *loadInfoMap)
{
    XSchemaObject *searchedElement = (*loadInfoMap)[searchKey];
    if( NULL == searchedElement ) {
        return error(QString("Unable to find an object with key:'%1'', map is:%2").arg(searchKey).arg(dumpLoadInfoMap(loadInfoMap)));
    }

    if( searchedElement->getType() != expectedType ) {
       return error(QString( "Object type differs type:'%1' expected type:%2, expected key:'%3', expected name='%4', found name='%5' map is:%6")
                    .arg(searchedElement->getType())
                    .arg(expectedType)
                    .arg(searchKey)
                    .arg(expectedName)
                    .arg(searchedElement->name())
                    .arg(dumpLoadInfoMap(loadInfoMap)));
    }
    if( searchedElement->name() != expectedName ) {
        return error(QString( "Object name differs type:'%1' expected type:%2, expected key:'%3', expected name='%4', found name='%5' map is:%6")
                     .arg(searchedElement->getType())
                     .arg(expectedType)
                     .arg(searchKey)
                     .arg(expectedName)
                     .arg(searchedElement->name())
                     .arg(dumpLoadInfoMap(loadInfoMap)));
    }

    return true;
}


bool TestSelectXsd::findElementPos(Regola *regola, QList<int>pos, const QString &expectedKey, const QString &expectedName, ElementLoadInfoMap *loadInfoMap)
{
    Element *element = regola->findElementByArray(pos);
    if( NULL == element ) {
        return error(QString( "Unable to find element at pos, expected key:'%1', expected name='%2' ").arg(expectedKey).arg(expectedName));
    }
    if( !element->isElement()) {
        return error(QString( "Element at pos, expected key:'%1', expected name='%2' is not an element").arg(expectedKey).arg(expectedName));
    }
    if( element->tag() != expectedName ) {
       return error(QString( "Element at pos, found:'%3' expected key:'%1', expected name='%2' is not an element").arg(expectedKey).arg(expectedName).arg(element->tag()));
    }
    // trova elemento per chiave di ricerca e controlla che corrisponda a quello selezionagto.
    Element *searchedElement = loadInfoMap->dataMap[expectedKey];
    if( NULL == searchedElement ) {
        return error(QString("Unable to find element with key:'%1'', map is:%2").arg(expectedKey).arg(dumpInfoMap(loadInfoMap)));
    }
    if(searchedElement != element) {
       return error(QString("Element returned is '%3', key:'%1'', map is:%2").arg(expectedKey).arg(dumpInfoMap(loadInfoMap)).arg(searchedElement->tag()));
    }
    return true;
}

bool TestSelectXsd::loadFile(App &app, const QString &inputFileName)
{
    if(!app.init() ) {
        return error("init app");
    }
    if(!app.mainWindow()->loadFile(inputFileName) ) {
        return error(QString("Error loading file:%1").arg(inputFileName));
    }
    return true;
}


QString TestSelectXsd::dumpInfoMap(ElementLoadInfoMap *loadInfoMap)
{
    QString result ="\n";
    result += QString("Size of map:%1\n").arg(loadInfoMap->dataMap.size());
    int index = 0 ;
    foreach( QString key, loadInfoMap->dataMap.keys()) {
        Element *e = loadInfoMap->dataMap[key];
        QString tag;
        if( NULL != e ) {
            tag = e->tag();
        }
        result += QString(" Entry '%1', key:'%2' -> '%3'\n").arg(index).arg(key).arg(tag);
        index++;
    }
    result += QString("**End of map\n");
    return result;
}

QString TestSelectXsd::dumpLoadInfoMap(QHash<QString, XSchemaObject*> *loadInfoMap)
{
    QString result ="\n";
    size_t bufSize = 3*sizeof(void*);
    char ptrBuffer[bufSize];

    ::snprintf(ptrBuffer, bufSize, "%p", loadInfoMap);
    result += QString("Map %2: size of load map:%1\n").arg(loadInfoMap->size()).arg(ptrBuffer);
    int index = 0 ;
    foreach( QString key, loadInfoMap->keys()) {
        XSchemaObject *e = loadInfoMap->value(key);
        QString name;
        if( NULL != e ) {
            name = e->name();
        }
        result += QString(" Entry '%1', key:'%2' -> '%3'\n").arg(index).arg(key).arg(name);
        index++;
    }
    result += QString("**End of map\n");
    return result;
}
