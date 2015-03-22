/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2012 by Luca Bellonda and individual contributors       *
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

#include "testbase.h"
#include "regola.h"
#include <QTextCodec>
#include <QClipboard>
#include <QAction>
#include <QTest>


QList<bool> TestBase::_boolArray;

TestBase::TestBase()
{
    _isError = false;
}

TestBase::~TestBase()
{
}

bool TestBase::error(const QString &testName, const QString &msg)
{
    _errorString = QString("%1: %2").arg(testName).arg(msg);
    _isError = true ;
    return false ;
}

bool TestBase::error(const QString &msg)
{
    return error( _testName, msg);
}

QString TestBase::errorString()
{
    return _errorString;
}

bool TestBase::isError()
{
    return _isError;
}

void TestBase::resetError()
{
    _errorString = "" ;
    _isError = false ;
}

bool TestBase::False()
{
    return false ;
}

bool TestBase::clickButton(QWidget *widget, const QString &actionName)
{
    QList<QAbstractButton*> buttons = widget->findChildren<QAbstractButton*>(actionName);
    if(buttons.size()>0) {
        buttons.at(0)->setFocus();
        QTest::mouseClick(buttons.at(0), Qt::LeftButton);
        //buttons.at(0)->click();
        return true ;
    }
    return false;
}

bool TestBase::setTextWidget(QWidget *widget, const QString &actionName, const QString &text)
{
    QList<QLineEdit*> texts = widget->findChildren<QLineEdit*>(actionName);
    if(texts.size()>0) {
        texts.at(0)->setText(text);
        return true ;
    }
    return false;
}

bool TestBase::fireAction(QMainWindow *window, const QString &actionName)
{
    QList<QMenu*> menus = window->menuBar()->findChildren<QMenu*>();
    return fireAction(menus, actionName);
}

bool TestBase::fireAction(QList<QMenu*> menus, const QString &actionName)
{
    foreach(QMenu* menu, menus ) {
        QList<QAction*> actions = menu->actions();
        foreach( QAction* action, actions ) {
            QString aName = action->objectName();
            if( aName == actionName ) {
                action->trigger();
                return true;
            }
        }
    }
    return false;
}

QAction *TestBase::findAction(QList<QMenu*> menus, const QString &actionName)
{
    foreach(QMenu* menu, menus ) {
        QList<QAction*> actions = menu->actions();
        foreach( QAction* action, actions ) {
            QString aName = action->objectName();
            if( aName == actionName ) {
                return action;
            }
        }
    }
    return NULL;
}

bool TestBase::checkActionEnabled(QMainWindow *window, const QString &actionName, bool &boolResult)
{
    QList<QMenu*> menus = window->menuBar()->findChildren<QMenu*>();
    QAction *action = findAction(menus, actionName);
    if( NULL == action ) {
        return false;
    }
    boolResult = action->isEnabled();
    return true ;
}



bool TestBase::toggleAction(QMainWindow *window, const QString &actionName)
{
    QList<QMenu*> menus = window->menuBar()->findChildren<QMenu*>();
    return toggleAction(menus, actionName);
}

bool TestBase::toggleAction(QList<QMenu*> menus, const QString &actionName)
{
    foreach(QMenu* menu, menus ) {
        QList<QAction*> actions = menu->actions();
        foreach( QAction* action, actions ) {
            QString aName = action->objectName();
            if( aName == actionName ) {
                action->toggle();
                return true;
            }
        }
    }
    return false;
}

void TestBase::insertItems(QSet<QString> &items, ... )
{
    va_list ap;
    va_start( ap, items);
    const char* str = va_arg( ap, const char *);
    while( str != NULL ) {
        QString data(str);
        items.insert(data);
        str = va_arg( ap, const char *);
    }
    va_end(ap);
}

QString TestBase::dumpStringSet(QSet<QString> &set, const QString &name)
{
    QStringList lst = set.toList();
    QString ret = QString("%1: %2 items >>'%3'<<").arg(name).arg(set.count()).arg(lst.join(","));
    return ret ;
}

QString TestBase::stringSetErrorString(const QString &explain, QSet<QString> &referenceSet, QSet<QString> &compareSet)
{
    return QString("%1:\n%2\n%3\n").arg(explain).arg(dumpStringSet(referenceSet, "reference" )).arg(dumpStringSet(compareSet, "compare"));
}

bool TestBase::stringSetError(const QString &explain, QSet<QString> &referenceSet, QSet<QString> &compareSet)
{
    return error(stringSetErrorString(explain, referenceSet, compareSet));
}

bool TestBase::compareStringSets(const QString &msg, QSet<QString> &referenceSet, QSet<QString> &compareSet)
{
    QString errorMessage = msg ;
    errorMessage += "\n";
    bool isOk = true;
    if( referenceSet.count() != compareSet.count() ) {
        isOk = false;
        errorMessage += "arguments count differs \n";
    }
    QSet<QString> notFound;
    QSet<QString> data;
    data.unite(compareSet);
    foreach( QString key, referenceSet) {
        if( !data.contains(key)) {
            notFound.insert(key);
            isOk = false;
        } else {
            data.remove(key);
        }
    }
    if(data.count()>0) {
        isOk = false;
    }
    if(!isOk) {
        QString err = errorMessage;
        err += QString("Unexpected items: %1\n").arg(dumpStringSet(data, "unexpected"));
        err += QString("Not found items: %1\n").arg(dumpStringSet(notFound, "not found"));
        err+= stringSetErrorString("items differs", referenceSet, compareSet);
        return error(err);
    }
    return isOk ;
}


bool TestBase::compareListInts(const QString &msg, QList<int> &reference, QList<int> &compare)
{
    QString errorMessage = msg ;
    errorMessage += "\n";
    bool isOk = true;
    if( reference.count() != compare.count() ) {
        isOk = false;
        errorMessage += "arguments count differs \n";
    }
    int maxop = reference.count();
    for( int i = 0 ; i < maxop ; i ++ ) {
        bool isRef = false;
        bool isCompare = false;
        int r =-1 ;
        int c = -1;
        if(i<reference.count() ) {
            isRef = true ;
            r = reference.at(i);
        }
        if(i<compare.count() ) {
            isCompare = true ;
            c = compare.at(i);
        }
        if(!isRef && !isCompare) {
            errorMessage += QString("\nItem %1 no ref, no compare\n").arg(i);
            isOk = false;
        }
        if(!isRef && isCompare) {
            errorMessage += QString("\nItem %1 only reference:%2\n").arg(i).arg(r);
            isOk = false;
        }
        if(isRef && !isCompare) {
            errorMessage += QString("\nItem %1 only compare :%2\n").arg(i).arg(c);
            isOk = false;
        }
        if(isRef && isCompare) {
            errorMessage += QString("\nItem %1 reference:%2 compare:%3\n").arg(i).arg(r).arg(c);
            if( r != c ) {
                isOk = false;
            }
        }
    }

    if(!isOk) {
        QString err = "items differs\n";
        err += errorMessage;
        return error(err);
    }
    return isOk ;
}

QString TestBase::listIntToString(const QList<int> &reference)
{
    bool isFirst = true ;
    QString result = "";
    foreach( int v, reference ) {
        if( isFirst ) {
            isFirst = false ;
        } else {
            result +=  "," ;
        }
        result += QString::number(v);
    }
    return result;
}


bool TestBase::loadFileIntoClipboard(const QString &fileInput, const QString &encoding)
{
    QFile file(fileInput);
    if( file.open(QIODevice::ReadOnly)) {
        QByteArray dataFile = file.readAll();
        if( file.error() != QFile::NoError) {
            file.close();
            return error(QString("error reading final '%1'").arg(fileInput));
        }
        file.close();
        QTextCodec *codec = QTextCodec::codecForName(encoding.toLatin1().data());
        QString data = codec->toUnicode(dataFile);
        QApplication::clipboard()->setText(data);
        return true;
    }
    return error(QString("error opening final '%1'").arg(fileInput));
}

bool TestBase::loadFileAsString(const QString &fileInput, const QString &encoding, QString *result)
{
    QFile file(fileInput);
    if( file.open(QIODevice::ReadOnly)) {
        QByteArray dataFile = file.readAll();
        if( file.error() != QFile::NoError) {
            file.close();
            return error(QString("error reading final '%1'").arg(fileInput));
        }
        file.close();
        QTextCodec *codec = QTextCodec::codecForName(encoding.toLatin1().data());
        *result = codec->toUnicode(dataFile);
        return true;
    }
    return error(QString("error opening final '%1'").arg(fileInput));
}


bool TestBase::loadIOAsString(QIODevice *device, const QString &encoding, QString *result)
{
    if( device->open(QIODevice::ReadOnly)) {
        QTextStream stream(device);
        QTextCodec *codec = QTextCodec::codecForName(encoding.toLatin1().data());
        stream.setCodec(codec);
        *result = stream.readAll();
        device->close();
        return true;
    }
    return error(QString("error opening data "));
}

bool TestBase::checkOneWidgetEnabled( QWidget *editor, const QString &childName, const bool expected)
{
    QWidget *widget = editor->findChild<QWidget*>(childName);
    if( NULL == widget ) {
        return error( QString("Cannot find child: '%1'").arg(childName) );
    }
    if( widget->isEnabled() != expected ) {
        return error(QString("Widget '%1' enabled expected:%2, but found %3").arg(childName).arg(expected).arg(widget->isEnabled()));
    }
    return true;
}

bool TestBase::checkRoot(Regola *regola)
{
    Element *regolaRoot = regola->root();
    Element *foundRoot = getRoot(regola);
    if( (NULL == foundRoot) && (NULL != regolaRoot) ) {
        return error("null root found and not null in rule");
    }
    if( (NULL != foundRoot) && (NULL == regolaRoot) ) {
        return error("not null found root and null regola");
    }
    if( (NULL == foundRoot) && (NULL == regolaRoot) ) {
        return true;
    }
    if( foundRoot->tag() != regolaRoot->tag()) {
        return error(QString("root tags differ: found:'%1', expected:'%2'").arg(foundRoot->tag()).arg(regolaRoot->tag()) );
    }
    return true;
}

Element * TestBase::getRoot(Regola *regola)
{
    foreach( Element *e, regola->getItems()) {
        if( e->isElement()) {
            return e ;
        }
    }
   return NULL;
}

void TestBase::buildRegola(Regola *regola, const QList<Element*> &elements)
{
    foreach( Element *element, elements ) {
        regola->insertElementForce(element);
    }
}

QList<bool> &TestBase::boolArray()
{
    if(_boolArray.isEmpty()) {
        _boolArray << false;
        _boolArray << true;
    }
    return _boolArray;
}
