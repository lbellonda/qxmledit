/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2014-2018 by Luca Bellonda and individual contributors  *
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


#include "testlengthattributes.h"
#include "element.h"
#include "testhelpers/editelementtest.h"
#include "app.h"
#include <QTest>


TestLengthAttributes::TestLengthAttributes()
{
}


TestLengthAttributes::~TestLengthAttributes()
{
}


bool TestLengthAttributes::test()
{
    _testName = "test";
    if(!testAttributMaxLen()) {
        return false;
    }
    /*TODO: test loading from file ? test tag if(!testAttributMaxTag()) {
        return false;
    }*/
    return true;
}

QString TestLengthAttributes::makeString(const int len, const QString &body, const QString &end)
{
    int times = (len /body.length())+1;
    QString result = "" ;
    for( int i = 0 ; i < times ; i ++ ) {
        result += body ;
    }
    result += end ;
    return result;
}

bool TestLengthAttributes::testAttributMaxLen()
{
    _testName = "testAttributMaxLen";
    App app;
    if(!app.init()) {
        return error("init");
    }

    QString newValue = makeString(250000, "1", "2");
    QString orig = makeString(250000, "1", "2");
    QString origTag = makeString(250000, "1", "2");
    //QString newValue = makeString(4, "1", "2");
    //QString orig = makeString( 4, "1", "2");
    //QString origTag = makeString(4, "1", "2");
    orig += "a" ;
    origTag += "b" ;
    Regola regola;
    Element element(newValue, "", &regola, NULL);
    element.setAttribute("attr", newValue);
    EditElementTest editElement(app.mainWindow());
    editElement.setTarget(&element);

    QTableWidget * attrTable = editElement.findChild<QTableWidget*>("attrTable");
    if( NULL == attrTable ) {
        return error("null attr table");
    }
    QTableWidgetItem *item = attrTable->item(0, editElement.columnForEditAttributes());
    if( NULL == item ) {
        return error("null attr item");
    }
    AttributeColumnItemDelegate *delegate = editElement.attributeDelegate();
    if( NULL == delegate ) {
        return error("null attr delegate");
    }

    attrTable->editItem(item);

    QWidget *editor = delegate->lastEditor();
    if( NULL == editor ) {
        return error("null editor");
    }
    QLineEdit *lineEdit = qobject_cast<QLineEdit*>(editor);
    if( NULL != lineEdit ) {
        lineEdit->deselect();
    }

    QTest::keyClick( editor, Qt::Key_Right );
    QTest::keyClick( editor, Qt::Key_End );
    QTest::keyClicks( editor, "a" );
    QTest::keyClick( editor, Qt::Key_Tab );

    QLineEdit *edit = qobject_cast<QLineEdit*>(editor);
    if( NULL == edit ) {
        return error("editor not line edit");
    }
    QString value1 = edit->text();

    //---- tag
    QLineEdit *tagEdit = editElement.findChild<QLineEdit*>("editTag");
    if( NULL == tagEdit ) {
        return error("editor not edit tag");
    }
    tagEdit->setFocus();

    QTest::keyClick( tagEdit, Qt::Key_End );
    QTest::keyClicks( tagEdit, "b" );

    editElement.accept();

    Attribute *attr = element.getAttribute("attr");
    if(NULL == attr ) {
        return error("null attribute on exit");
    }
    QString value = attr->value;
    //check for attribute value
    if(value != orig) {
        return error(QString("After edit: expecting %1, but found:%2, values='%3' vs '%4'")
                .arg(orig.length()).arg(value.length()).arg(value).arg(orig));
    }
    if(value1 != orig) {
        return error(QString("After edit 1: expecting %1, but found:%2, values='%3' vs '%4'")
                .arg(orig.length()).arg(value1.length()).arg(value).arg(orig));
    }
    QString nowTag = element.tag();
    if(nowTag != origTag ) {
        return error(QString("After edit tag: expecting %1, but found:%2, values='%3' vs '%4'")
                .arg(origTag.length()).arg(nowTag.length()).arg(orig).arg(value));
    }

    return true;
}
