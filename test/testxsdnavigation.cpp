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


#include "testxsdnavigation.h"
#include "app.h"
#include "xsdeditor/navigationtree.h"
#include <QTest>

#include "xsdeditor/widgets/xsdgenericeditor.h"
#include "ui_xsdwindow.h"

#define  FILE_BASE_XSD  "../test/data/xsd/navigation.xsd"

//---------------------------

bool TestXSDWindow::testCopyElementActionExecute(XSchemaObject *object)
{
    return copyElementActionExecute(object);
}

XSDItem *TestXSDWindow::selectItemByIndexAndZoom(const QList<int> &selectionPath)
{
    selectItemByIndex(selectionPath);
    on_cmdObjZoomIn_clicked();
    ui->cmdShowBase->setChecked(true);
    on_cmdShowBase_clicked();
    return _mainItem ;
}

XSDItem *TestXSDWindow::selectItemByIndex(const QList<int> &selectionPath)
{
    XSDItem *item = selectItemByIndexInternal(selectionPath);
    if( NULL != item ) {
        _scene->clearSelection();
        item->graphicItem()->setSelected(true);
    }
    return item ;
}

XSDItem *TestXSDWindow::selectItemByIndexInternal(const QList<int> &selectionPath)
{
    QList<int> sel;
    sel = QList<int>::fromVector(selectionPath.toVector());
    XSDItem *root = _mainItem;
    if( sel.size()>0) {
        int index = sel.first();
        sel.removeFirst();
        if( 0 == index ) {
            if( !sel.isEmpty() ) {
                return selectItemByIndexInternal( root, sel );
            } else {
                return root ;
            }
        } else {
            return NULL ;
        }
    }
    return NULL;
}

XSDItem *TestXSDWindow::selectItemByIndexInternal( XSDItem *parent, QList<int> &selectionPath)
{
    int pos = selectionPath.first();
    selectionPath.removeFirst();
    RChildren *itemChildren = parent->rChildren();
    if(itemChildren->childrenSize()>pos) {
        XSDItem *item = parent->rChildren()->childAt(pos)->item();
        if( selectionPath.isEmpty()) {
            return item;
        }
        return selectItemByIndexInternal( item, selectionPath);
    }
    return NULL;
}

QString TestXSDWindow::getAsHTML(const bool isSimple)
{
    QString htmlText = _printManager.getAsHTML(false, true, isSimple, false);
    return htmlText;
}

//---------------------------


NavigationTree *TestXSDWindow::navigation()
{
    return ui->navigation;
}


TestXSDNavigation::TestXSDNavigation()
{
    xsdEditor = NULL ;
}

TestXSDNavigation::~TestXSDNavigation()
{
}

//---------------------------

bool TestXSDNavigation::checkTopNavigation(QTreeWidgetItem *topItem)
{
    if( topItem->childCount()!=0) {
        return false;
    }
    return true;
}

bool TestXSDNavigation::checkTopElements(QTreeWidgetItem *topItem)
{
    if( topItem->childCount()!=1) {
        return error(QString("top elements count expected 1 found:%1").arg(topItem->childCount()));
    }
    QTreeWidgetItem *item = topItem->child(0);
    if(item->text(0)!= "e1") {
        return error(QString("top elements expected 'e1' found:'%1'").arg(item->text(0)));
    }
    return true;
}

bool TestXSDNavigation::checkTopTypes(QTreeWidgetItem *topItem)
{
    if( topItem->childCount()!=2) {
        return false;
    }
    QTreeWidgetItem *item = topItem->child(0);
    if(item->text(0)!= "Model") {
        return false;
    }
    item = topItem->child(1);
    if(item->text(0)!= "Plane") {
        return false;
    }
    return true;
}

bool TestXSDNavigation::checkTopAttributes(QTreeWidgetItem *topItem)
{
    if( topItem->childCount()!=1) {
        return false;
    }
    QTreeWidgetItem *item = topItem->child(0);
    if(item->text(0)!= "a1") {
        return false;
    }
    return true;
}

bool TestXSDNavigation::checkTopAttributeGroups(QTreeWidgetItem *topItem)
{
    if( topItem->childCount()!=1) {
        return false;
    }
    QTreeWidgetItem *item = topItem->child(0);
    if(item->text(0)!= "agg3") {
        return false;
    }
    return true;
}

bool TestXSDNavigation::checkTopGroups(QTreeWidgetItem *topItem)
{
    if( topItem->childCount()!=1) {
        return false;
    }
    QTreeWidgetItem *item = topItem->child(0);
    if(item->text(0)!= "group1") {
        return false;
    }
    return true;
}
bool TestXSDNavigation::checkTopReferences(QTreeWidgetItem *topItem)
{
    if( topItem->childCount()!=1) {
        return false;
    }
    QTreeWidgetItem *item = topItem->child(0);
    if(item->text(0)!= "r1") {
        QVariant variant = item->data(0, Qt::UserRole);
        XSchemaObject*object = (XSchemaObject*) variant.value<void *>();
        if( NULL != object ) {
            switch( object->getType() ) {
            case SchemaTypeRedefine:
            case SchemaTypeImport:
            case SchemaTypeInclude:
                return true;
            default:
                return false;
            }
        }
        return false;
    }
    return true;
}

bool TestXSDNavigation::checkInnerElements(QTreeWidgetItem *topItem)
{
    if( topItem->childCount()!=4) {
        return false;
    }
    QTreeWidgetItem *item = topItem->child(0);
    if(item->text(0)!= "e2 (Model)") {
        return false;
    }
    item = topItem->child(1);
    if(item->text(0)!= "e3 (Model)") {
        return false;
    }
    item = topItem->child(2);
    if(item->text(0)!= "e4 (Plane)") {
        return false;
    }
    item = topItem->child(3);
    if(item->text(0)!= "sample-inner (group1)") {
        return false;
    }
    return true;
}

//---------------------------

bool TestXSDNavigation::openXsdViewer(App *app)
{
    xsdEditor = new TestXSDWindow(app->data(), app->mainWindow()) ;
    QString xmlAsString = app->mainWindow()->getRegola()->getAsText();
    xsdEditor->setFileName("test");
    xsdEditor->setTitle("test");
    xsdEditor->loadStringImmediate(xmlAsString);
    int index = 0 ;
    while(index < 120) {
        index ++ ;
        QTest::qWait(10);
        if( NULL != xsdEditor->schema() ) {
            break;
        }
    }
    if( xsdEditor->isInError() ) {
        return error("data not loaded");
    }
    if( NULL == xsdEditor->schema() ) {
        return error("data not loaded:2 ");
    }
    return true;
}

bool TestXSDNavigation::checkNavigation()
{
    NavigationTree *navigationTree = xsdEditor->navigation();
    //dumpTree(navigationTree);
    // get top level elements and check them
    QTreeWidgetItem *topNavigation = navigationTree->topLevelItem(0);
    if(!checkTopNavigation(topNavigation)) {
        return error("top navigation");
    }
    QTreeWidgetItem *topElements = navigationTree->topLevelItem(2);
    if(!checkTopElements(topElements)) {
        return false;
    }
    QTreeWidgetItem *topGroups = navigationTree->topLevelItem(4);
    if(!checkTopGroups(topGroups)) {
        return error("top groups");
    }
    QTreeWidgetItem *topTypes = navigationTree->topLevelItem(6);
    if(!checkTopTypes(topTypes)) {
        return error("top types");
    }
    QTreeWidgetItem *topAttributes = navigationTree->topLevelItem(8);
    if(!checkTopAttributes(topAttributes)) {
        return error("top attributes");
    }
    QTreeWidgetItem *topAttributeGroups = navigationTree->topLevelItem(10);
    if(!checkTopAttributeGroups(topAttributeGroups)) {
        return error("top attributeGroups");
    }
    QTreeWidgetItem *topReferences = navigationTree->topLevelItem(12);
    if(!checkTopReferences(topReferences)) {
        return error("top references");
    }
    QTreeWidgetItem *topInnerElements = navigationTree->topLevelItem(14);
    if(!checkInnerElements(topInnerElements)) {
        return error("inner elements");
    }

    return true;
}

//---------------------------


bool TestXSDNavigation::testNavigation()
{
    _testName = "testNavigation";
    App app;
    if(!app.init() ) {
        return error("init");
    }
    if(!app.mainWindow()->loadFile(QString(FILE_BASE_XSD))) {
        return error("load file");
    }
    if( !openXsdViewer(&app) ) {
        return false;
    }
    if(! checkNavigation() ) {
        return false;
    }
    return true ;
}

//---------------------------

