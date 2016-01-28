/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2012-2016 by Luca Bellonda and individual contributors  *
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

#ifndef TESTBASE_H
#define TESTBASE_H

#include "qtincludes.h"
#include <QString>
#include <QHash>

#define FILE_TEST_BASE_SIMPLE   "../test/data/test_base.xml"
#define TEST_BASE_DATA   "../test/data/"

class Element;
class Regola;
class QAction;

class TestBase
{
protected:
    bool _isError ;
    QString _errorString;
    QString _testName;
    QString _origName;

    bool error(const QString &testName, const QString &msg);
    bool error(const QString &msg);
    bool error(const TestBase &inner);
    static bool False();
    void resetError();
private:
    static QList<bool> _boolArray;

public:
    TestBase();
    virtual ~TestBase();

    bool isError();
    QString errorString();

protected:
    bool fireAction(QMainWindow *window, const QString &actionName);
    bool fireAction(QList<QMenu*> menus, const QString &actionName);
    bool toggleAction(QMainWindow *window, const QString &actionName);
    bool toggleAction(QList<QMenu*> menus, const QString &actionName);
    void insertItems(QSet<QString> *items, ... );

    bool compareStringSets(const QString &msg, QSet<QString> &referenceSet, QSet<QString> &compareSet);
    QString dumpStringSet(QSet<QString> &set, const QString &name);
    QString stringSetErrorString(const QString &explain, QSet<QString> &referenceSet, QSet<QString> &compareSet);
    QString listIntToString(const QList<int> &reference);
    bool stringSetError(const QString &explain, QSet<QString> &referenceSet, QSet<QString> &compareSet);
    bool compareListInts(const QString &msg, QList<int> &reference, QList<int> &compare);
    bool loadFileIntoClipboard(const QString &fileInput, const QString &encoding);
    bool loadFileAsString(const QString &fileInput, const QString &encoding, QString *result);
    bool loadIOAsString(QIODevice *device, const QString &encoding, QString *result);
    bool checkOneWidgetEnabled( QWidget *editor, const QString &childName, const bool expected);
    QAction *findAction(QList<QMenu*> menus, const QString &actionName);
    bool clickButton(QWidget *widget, const QString &actionName);
    bool setTextWidget(QWidget *widget, const QString &actionName, const QString &text);
    bool checkActionEnabled(QMainWindow *window, const QString &actionName, bool &boolResult);

    void buildRegola(Regola *regola, const QList<Element*> &elements);
    QList<bool> &boolArray();

    Element * getRoot(Regola *regola);
    bool checkRoot(Regola *regola);

    template <typename T>
    bool compareListGeneric(const QString &msg, QList<T> &reference, QList<T> &compare);
    bool compareStringList(const QString &msg, QStringList reference, QStringList compare);

    //---
    Element * makeElement(const QString &tag, const QString &attr);
    QHash<QString, QString> unpackAttrib(const QString &attribs);
    Element * addAttrs(Element *el, const QString &attribs);
    QString dumpHash(QHash<QString,QString> hash);
    bool testHash(QHash<QString,QString> expected, QHash<QString,QString> found);
    bool compare(Regola *regola, const QString &id, const QString &fileResult);
    bool readFromFile(const QString &file, QString &result);

};

#endif // TESTBASE_H
