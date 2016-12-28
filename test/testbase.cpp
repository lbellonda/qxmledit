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

#include "testbase.h"
#include "regola.h"
#include "comparexml.h"
#include <QTextCodec>
#include <QClipboard>
#include <QAction>
#include <QTest>
#include "app.h"

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

bool TestBase::error(const TestBase &inner)
{
    if( !inner._testName.isEmpty() ) {
        _testName += "/";
        _testName += inner._testName;
    }
    const QString &newErr = const_cast<TestBase&>(inner).errorString();
    return error(newErr);
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

bool TestBase::setComboWidget(QWidget *widget, const QString &actionName, const QString &text)
{
    QList<QComboBox*> texts = widget->findChildren<QComboBox*>(actionName);
    if(texts.size()>0) {
        texts.at(0)->setCurrentText(text);
        return true ;
    }
    return false;
}

bool TestBase::setCheckBoxWidget(QWidget *widget, const QString &actionName, const bool isChecked)
{
    QList<QCheckBox*> texts = widget->findChildren<QCheckBox*>(actionName);
    if(texts.size()>0) {
        texts.at(0)->setChecked(isChecked);
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

bool TestBase::fireActionIfNotChecked(QMainWindow *window, const QString &actionName)
{
    QList<QMenu*> menus = window->menuBar()->findChildren<QMenu*>();
    return fireActionIfNotChecked(menus, actionName);
}

bool TestBase::fireActionIfNotChecked(QList<QMenu*> menus, const QString &actionName)
{
    foreach(QMenu* menu, menus ) {
        QList<QAction*> actions = menu->actions();
        foreach( QAction* action, actions ) {
            QString aName = action->objectName();
            if( aName == actionName ) {
                if(!action->isChecked()) {
                    action->trigger();
                }
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

void TestBase::insertItems(QSet<QString> *items, ... )
{
    va_list ap;
    va_start( ap, items);
    const char* str = va_arg( ap, const char *);
    while( str != NULL ) {
        QString data(str);
        items->insert(data);
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

bool TestBase::compareStringList(const QString &msg, QStringList reference, QStringList compare)
{
    QList<QString> s1 = reference;
    QList<QString> s2 = compare;
    return compareListGeneric<QString>(msg, s1, s2);
}

template <typename T>
bool TestBase::compareListGeneric(const QString &msg, QList<T> &reference, QList<T> &compare)
{
    QString errorMessage = msg ;
    errorMessage += "\n";
    bool isOk = true;
    if( reference.count() != compare.count() ) {
        isOk = false;
        errorMessage += QString("arguments count differs %1 (ref) vs %2 (cand)\n").arg(reference.count()).arg(compare.count());
    }
    int maxop = reference.count();
    for( int i = 0 ; i < maxop ; i ++ ) {
        bool isRef = false;
        bool isCompare = false;
        T r ;
        T c;
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
            errorMessage += QString("\nItem %1 only reference:'%2'\n").arg(i).arg(r);
            isOk = false;
        }
        if(isRef && !isCompare) {
            errorMessage += QString("\nItem %1 only compare :'%2'\n").arg(i).arg(c);
            isOk = false;
        }
        if(isRef && isCompare) {
            errorMessage += QString("\nItem %1 reference:'%2'' compare:'%3'\n").arg(i).arg(r).arg(c);
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
        // now normalize crlf from text mode to string.
        *result = result->replace("\r\n", "\n");
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

bool TestBase::compareFiles(const QString &file1, const QString &file2)
{
    QString string1;
    QString string2;
    if(!loadFileAsString(file1, "UTF-8", &string1)) {
        return error(QString("Unable to load file 1 '%1'").arg(file1));
    }
    if(!loadFileAsString(file2, "UTF-8", &string2)) {
        return error(QString("Unable to load file 2 '%1'").arg(file2));
    }
    if(string1 != string2) {
        return error(QString("Strings differ len1:%1, len2:%2\n document (1) is:\n%3\n reference (2) is:\n%4\n---end\n")
                     .arg(string1.length()).arg(string2.length()).arg(string1).arg(string2) );
    }
    return true ;
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


Element * TestBase::makeElement(const QString &tag, const QString &attr)
{
    Element *el = new Element(tag, "", NULL, NULL);
    addAttrs(el, attr);
    return el;
}

QHash<QString, QString> TestBase::unpackAttrib(const QString &attribs)
{
    QHash<QString, QString> decoded ;
    QStringList pairs = attribs.split(",", QString::SkipEmptyParts);
    foreach( QString pair, pairs ) {
        QStringList attr = pair.split("=", QString::KeepEmptyParts);
        decoded.insert(attr.at(0), attr.at(1));
    }
    return decoded;
}


Element * TestBase::addAttrs(Element *el, const QString &attribs)
{
    QHash<QString, QString> decoded = unpackAttrib(attribs);
    //
    foreach( QString key, decoded.keys()) {
        el->addAttribute(key, decoded[key]);
    }
    return el;
}

QString TestBase::dumpHash(QHash<QString,QString> hash)
{
    QString res;
    res += QString("Size:%1\n").arg(hash.size());
    foreach( QString key, hash.keys() ) {
        res += QString("%1:%2\n").arg(key).arg(hash[key]);
    }
    res+= "\n\n";
    return res;
}

bool TestBase::testHash(QHash<QString,QString> expected, QHash<QString,QString> found)
{
    if( found.keys().size() != expected.keys().size()) {
        return error(QString("Size differs. dump found:\n%1\ndump expected:\n%2").arg(dumpHash(found)).arg(dumpHash(expected)));
    }
    foreach( QString key, expected.keys() ) {
        if(!found.contains(key)) {
           return error(QString("Size differs. dump found:\n%1\ndump expected:\n%2").arg(dumpHash(found)).arg(dumpHash(expected)));
        }
        if( found[key] != expected[key] ) {
            return error(QString("Size differs. dump found:\n%1\ndump expected:\n%2").arg(dumpHash(found)).arg(dumpHash(expected)));
        }
    }
    return true;
}

bool TestBase::compare(Regola *regola, const QString &id, const QString &fileResult)
{
    QByteArray resultData = regola->writeMemory();
    QDomDocument document1;
    QDomDocument document2;
    CompareXML compare;
    if(!compare.loadFileIntoDocument(fileResult, document1)) {
        return error(QString("id: %1, load file result %2").arg(id).arg(fileResult));
    }
    QBuffer outputData(&resultData);
    if(!compare.loadFileIntoDocument(&outputData, document2)) {
        return error(QString("id %1 load modified data").arg(id));
    }
    bool result = compare.compareDomDocuments(document1, document2);
    if( !result ) {
        compare.dumpErrorCause();
        return error(QString("Step: %1 comparing file with regola: %2").arg(id).arg(compare.errorString()));
    }
    return true ;
}

bool TestBase::compare(Element *e1, Element *e2)
{
    if(e1->getType() != e2->getType() ) {
        return error(QString("type differ: %1/%2").arg(e1->getType()).arg(e2->getType()));
    }
    if(e1->tag() != e2->tag() ) {
        return error(QString("tag differ: %1/%2").arg(e1->tag()).arg(e2->tag()));
    }
    QList<Attribute*> aa = e1->getAttributesList();
    QList<Attribute*> bb = e1->getAttributesList();
    QHash<QString, QString> reference ;
    QHash<QString, QString> compare ;
    foreach(Attribute *a1, aa) {
        reference.insert(a1->name, a1->value);
    }
    foreach(Attribute *b1, bb) {
        compare.insert(b1->name, b1->value);
    }
    QString errorMessage;
    bool isOk = true;
    if( reference.keys().size() != compare.keys().size() ) {
        isOk = false;
        errorMessage += QString("arguments count differs %1/%2\n").arg(reference.keys().size()).arg(compare.keys().size());
    }
    foreach( const QString &key, reference.keys() ) {
        if(!compare.contains(key)) {
            errorMessage += QString("\nItem %1 only reference:%2\n").arg(key).arg(reference[key]);
            isOk = false;
        } else {
            compare.remove(key);
        }
    }
    foreach( const QString &key, compare.keys() ) {
        errorMessage += QString("\nItem %1 only compare:%2\n").arg(key).arg(compare[key]);
        isOk = false;
    }
    if(!isOk) {
        QString err = "items differs\n";
        err += errorMessage;
        return error(err);
    }
    return true;
}

bool TestBase::readFromFile(const QString &file, QString &result)
{
    bool ok = false ;
    QFile data(file);
    if (data.open(QFile::ReadOnly | QFile::Text)) {
        ok = true ;
        QTextStream streamIn(&data);
        result = streamIn.readAll();
        if(data.error() != QFile::NoError) {
            ok = false;
        }
        data.close();
    }
    return ok;
}

void TestBase::dumpTree(QTreeWidget *tree)
{
    printf("Dump tree begin\n");
    int items = tree->topLevelItemCount();
    printf("top level items count:%d\n", items);
    for( int i = 0 ; i < items ; i ++ ) {
        dumpItem( i+1, 0, tree->topLevelItem(i));
    }
    printf("\n\nDump tree end\n");
}

void TestBase::dumpItem(const int index, const int level, QTreeWidgetItem *item)
{
    for( int i = 0 ; i < level ; i ++ ) {
        printf(" ");
    }
    printf("%d- ", index);
    QString text = item->text(0);
    printf( "len:%d '%s'\n", text.length(), text.toLatin1().data());
    int itemsLeft = item->childCount();
    for( int i = 0 ; i < itemsLeft ; i ++ ) {
        dumpItem(i, index+1, item->child(i));
    }
}

bool TestBase::testSkeletonBase( const QString &id, const QString &fileStart, const QString &fileFinal, bool (*apply)(TestBase *arg, MainWindow *, Element *), QList<int> selPath, const TestSelSpec selSpec)
{
    QString oldName = _testName ;
    _testName = _testName.append(id);
    App app;
    if(!app.init() ) {
        return error("init app failed");
    }
    if( !app.mainWindow()->loadFile(fileStart) ) {
        return error(QString("unable to load input file: '%1' ").arg(fileStart));
    }
    Regola *regola = app.mainWindow()->getRegola();
    Element *selectedElement = NULL ;
    selectedElement = app.mainWindow()->getRegola()->findElementByArray(selPath);
    if(NULL == selectedElement) {
        return error("no element selected");
    }
    switch(selSpec) {
    default:
        return error(QString("Unknown selection type:%1").arg(selSpec));
        break;
    case SelOnlySelection:
        app.mainWindow()->getEditor()->setCurrentItem(selectedElement);
        break;
    case SelOnlyBookmark:
        regola->addBookmark(selectedElement);
        break;
    case SelSelectionAndBookmark:
        regola->addBookmark(selectedElement);
        app.mainWindow()->getEditor()->setCurrentItem(selectedElement);
        break;
    }
    if(!(*apply)(this, app.mainWindow(), selectedElement)) {
        return error("method returned false");
    }
    if(!compare(regola, "do", fileFinal)){
        return false;
    }
    app.mainWindow()->getRegola()->undo();
    if(!compare(regola, "op undo", fileStart)){
        return false;
    }
    app.mainWindow()->getRegola()->redo();
    if(!compare(regola, "op redo", fileFinal)){
        return false;
    }
    app.mainWindow()->getRegola()->undo();
    if(!compare(regola, "op undo", fileStart)){
        return false;
    }
    _testName = oldName ;
    return true;
}

bool TestBase::testSkeletonBaseNoOp( const QString &id, const QString &fileStart, bool (*apply)(TestBase *arg, MainWindow *, Element *, void* aptr), QList<int> selPath, const TestSelSpec selSpec, void* aptr )
{
    QString oldName = _testName ;
    _testName = _testName.append(id);
    App app;
    if(!app.init() ) {
        return error("init app failed");
    }
    if( !app.mainWindow()->loadFile(fileStart) ) {
        return error(QString("unable to load input file: '%1' ").arg(fileStart));
    }
    Regola *regola = app.mainWindow()->getRegola();
    Element *selectedElement = NULL ;
    selectedElement = app.mainWindow()->getRegola()->findElementByArray(selPath);
    if(NULL == selectedElement) {
        return error("no element selected");
    }
    switch(selSpec) {
    default:
        return error(QString("Unknown selection type:%1").arg(selSpec));
        break;
    case SelOnlySelection:
        app.mainWindow()->getEditor()->setCurrentItem(selectedElement);
        break;
    case SelOnlyBookmark:
        regola->addBookmark(selectedElement);
        break;
    case SelSelectionAndBookmark:
        regola->addBookmark(selectedElement);
        app.mainWindow()->getEditor()->setCurrentItem(selectedElement);
        break;
    }
    if(!(*apply)(this, app.mainWindow(), selectedElement, aptr)) {
        return error("method returned false");
    }
    _testName = oldName ;
    return true;
}
