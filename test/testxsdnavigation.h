/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2013 by Luca Bellonda and individual contributors       *
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


#ifndef TESTXSDNAVUIGATION_H
#define TESTXSDNAVUIGATION_H

#include "testbase.h"
#include "xsdeditor/xsdwindow.h"

class NavigationTree;
class App;

class TestXSDWindow: public XSDWindow {
    Q_OBJECT

    XSDItem *selectItemByIndex(const QList<int> &selectionPath);
    XSDItem *selectItemByIndexInternal(const QList<int> &selectionPath);
    XSDItem *selectItemByIndexInternal( XSDItem *parent, QList<int> &selectionPath);

public:
    TestXSDWindow(QXmlEditData *appData, QWidget *parent) : XSDWindow(appData, parent)
    {

    }
    virtual ~TestXSDWindow(){ this->close();}
    NavigationTree *navigation();
    bool testCopyElementActionExecute(XSchemaObject *object);
    XSDItem *selectItemByIndexAndZoom(const QList<int> &selectionPath);
};

class TestXSDNavigation : public TestBase
{
    TestXSDWindow *xsdEditor;

    bool checkTopNavigation(QTreeWidgetItem *topItem);
    bool checkTopElements(QTreeWidgetItem *topItem);
    bool checkTopTypes(QTreeWidgetItem *topItem);
    bool checkTopAttributes(QTreeWidgetItem *topItem);
    bool checkTopAttributeGroups(QTreeWidgetItem *topItem);
    bool checkTopReferences(QTreeWidgetItem *topItem);
    bool checkInnerElements(QTreeWidgetItem *topItem);
    bool checkTopGroups(QTreeWidgetItem *topItem);

    bool openXsdViewer(App *app);
    bool checkNavigation();
public:
    TestXSDNavigation();
    ~TestXSDNavigation();

    bool testNavigation();
    bool testPanel();

};

#endif // TESTXSDNAVUIGATION_H
