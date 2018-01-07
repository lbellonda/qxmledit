/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2011-2018 by Luca Bellonda and individual contributors  *
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
#include "xsdplugin.h"
#include "regola.h"
#include "utils.h"
#include "xsdeditor/xsdwindow.h"

#define XSD_PLUGIN_VERSION "1.0.0-ALPHA"
#define XSD_PLUGIN_NAME "XsdPlugin"

void xsdTest(QXmlEditData *data);

XsdPlugin::XsdPlugin()
{
}

XsdPlugin::~XsdPlugin()
{
}

QString XsdPlugin::name()
{
    return XSD_PLUGIN_NAME;
}

QString XsdPlugin::description()
{
    return tr("XSD Viewer");
}

QString XsdPlugin::version()
{
    return XSD_PLUGIN_VERSION ;
}

bool XsdPlugin::go(QWidget * parent, QXmlEditData *appData, Regola *data, Element *selectedElement, Element ** selectedOut)
{
    XSDWindow xsdEditor(appData, parent) ;

    ElementLoadInfoMap loadMap;
    QString xmlAsString = data->getAsText(&loadMap);
    QEventLoop eventLoop;
    xsdEditor.EVENTLOOP = &eventLoop ;

    if(NULL != selectedOut) {
        *selectedOut = NULL ;
    }
    try {
        // bad method to reverse scan a map
        QString keySel;
        if(NULL != selectedElement) {
            foreach(QString key, loadMap.dataMap.keys()) {
                Element *value = loadMap.dataMap[key];
                if(value == selectedElement) {
                    keySel = key ;
                    break;
                }
            }
            xsdEditor.setSelectedObjectKey(keySel);
        }
        xsdEditor.setWindowModality(Qt::WindowModal);
        xsdEditor.show();
        xsdEditor.setFileName(data->fileName());
        xsdEditor.setTitle(data->fileName());
        xsdEditor.loadString(xmlAsString);
        // TODO handle event loop inside the window.
        bool result = (eventLoop.exec() > 0) ;
        if(loadMap.dataMap.contains(xsdEditor.selectedExitKey())) {
            *selectedOut = loadMap.dataMap[xsdEditor.selectedExitKey()];
        }
        return result ;
    } catch(XsdException *ex) {
        Utils::error(tr("Error in XSD Plugin. \n%1").arg(ex->cause()));
    } catch(...) {
        Utils::error(tr("Unknown exception."));
    }
    return false;
}

bool XsdPlugin::goTest(QXmlEditData *data, QWidget */*parent*/)
{
    qDebug("Balsamiq plugin test called");
    xsdTest(data);
    return true;
}
