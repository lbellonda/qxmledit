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

#ifndef TESTBASE_H
#define TESTBASE_H

#include "qtincludes.h"
#include <QString>
#include <QHash>
#include "testhelpers/collect.h"

#define FILE_TEST_BASE_SIMPLE   "../test/data/test_base.xml"
#define TEST_BASE_DATA   "../test/data/"

class Element;
class Regola;
class QAction;
class MainWindow;
class App;

class TestBase
{
public:
    bool error(const QString &msg);

protected:
    bool _isError ;
    QString _errorString;
    QString _testName;
    QString _subTestName;
    QString _origName;

    QList<Element*> gc;

    bool error(const QString &testName, const QString &msg);
    bool error(const TestBase &inner);
    static bool False();
    void resetError();

    enum TestSelSpec {
        SelOnlySelection,
        SelOnlyBookmark,
        SelSelectionAndBookmark
    };

private:
    static QList<bool> _boolArray;

public:
    TestBase();
    virtual ~TestBase();

    bool isError();
    QString errorString();

    //-- utility
    bool setTextWidget(QWidget *widget, const QString &actionName, const QString &text);
    bool setComboWidget(QWidget *widget, const QString &actionName, const QString &text);
    bool setCheckBoxWidget(QWidget *widget, const QString &actionName, const bool isChecked);

protected:
    bool fireAction(QMainWindow *window, const QString &actionName);
    bool fireAction(QList<QMenu*> menus, const QString &actionName);
    bool fireActionIfNotChecked(QList<QMenu*> menus, const QString &actionName);
    bool fireActionIfNotChecked(QMainWindow *window, const QString &actionName);
    bool toggleAction(QMainWindow *window, const QString &actionName);
    bool toggleAction(QList<QMenu*> menus, const QString &actionName);
    void insertItems(QSet<QString> *items, ... );

    bool compareFiles(const QString &file1, const QString &file2);
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
    bool compare(Element *e1, Element *e2);
    bool readFromFile(const QString &file, QString &result);
    bool writeToFile(const QString &file, const QString &dataString);

    static void dumpTree(QTreeWidget *tree);
    static void dumpItem(const int index, const int level, QTreeWidgetItem *item);

    bool testSkeletonBase(const QString &id, const QString &fileStart, const QString &fileFinal, bool (*apply)(TestBase *, MainWindow *, Element *), QList<int> selPath, const TestSelSpec selSpec = SelOnlySelection);
    bool testSkeletonBaseNoOp( const QString &id, const QString &fileStart, bool (*apply)(TestBase *arg, MainWindow *, Element *, void* aptr), QList<int> selPath, const TestSelSpec selSpec, void* aptr );

    bool compareXMLBase(Regola *regola, const QString &step, const QString &fileResult);
    Element *selectAnItem(App &app, QList<int> selectionPath);
    bool assertEquals(const QString &msg, const QString &expected, const QString &current);
    bool checkBoolSetting(const QString &setting, const bool expected);
    QString normalizeCR(QString in) const ;
    QString loadTextFile(const QString &filePath) const;
};

#define DELTELMS(object) foreach( Element *var, object) { delete var; } object.clear();

#endif // TESTBASE_H
