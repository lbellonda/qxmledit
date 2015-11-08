/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2015 by Luca Bellonda and individual contributors       *
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


#include "testreplica.h"
#include "modules/replica/replicacommand.h"
#include "modules/replica/replicamanager.h"
#include "modules/replica/replicasettingsdialog.h"
#include "app.h"

#define BASE_PATH "../test/data/xml/replica/"
#define INPUT1  BASE_PATH "input1.xml"
#define OUTPUT1  BASE_PATH "output1.xml"

TestReplica::TestReplica()
{
}

TestReplica::~TestReplica()
{
}

bool TestReplica::testFast()
{
    //enumAlpha();
    if(!testUnitCommand()) {
        return false;
    }
    return testApply();
}

bool TestReplica::testUnit()
{
    _testName = "testUnit" ;
    if(!testUnitCommand()) {
        return false;
    }
    if(!testCheckParams() ) {
        return false;
    }
    if(!testUnitReplica()) {
        return false;
    }
    if(!testCommandDialog()) {
        return false;
    }
    return true;
}

bool TestReplica::testUnitReplica()
{
    _testName = "testUnitReplica" ;
    if(!testCheckParams()) {
        return false;
    }
    if(!testFormat()) {
        return false;
    }
    if(!testFormatId()) {
        return false;
    }
    if(!testWidth()) {
        return false;
    }
    return true;
}

bool TestReplica::testFormatNumber( const QString &expected, const int index, const bool isPadded, const int totalWidth)
{
    _testName = "testFormatNumber" ;
    ReplicaManager manager;
    QString result = manager.formatNumber(index, isPadded, totalWidth);
    if( result != expected ) {
        return error(QString("Fmt expected:'%1' found:'%2' index:%3, padded:%4, totalWidth:%5")
                     .arg(expected).arg(result).arg(index).arg(isPadded).arg(totalWidth));
    }
    return true ;
}

bool TestReplica::testFormatString( const QString &expected, const int index, const bool isPadded, const int totalWidth)
{
    _testName = "testFormatString" ;
    ReplicaManager manager;
    QString result = manager.formatAlpha(index, isPadded, totalWidth);
    if( result != expected ) {
        return error(QString("Fmt expected:'%1' found:'%2' index:%3, padded:%4, totalWidth:%5")
                     .arg(expected).arg(result).arg(index).arg(isPadded).arg(totalWidth));
    }
    return true ;
}

bool TestReplica::testFormatId( ReplicaCommand *cmd, const QString &expected, const int startIndex, const int totalCount)
{
    ReplicaManager manager;
    QString result = manager.makeId( cmd, startIndex, totalCount);
    if( result != expected ) {
        return error(QString("Fmt expected:'%1' found:'%2' index:%3, format:%4, totalCount:%5")
                     .arg(expected).arg(result).arg(startIndex).arg(cmd->format()).arg(totalCount));
    }
    return true ;
}

bool TestReplica::checkTestFormatId( const QString &expected, const ReplicaCommand::EFormat format, const int startIndex, const int totalCount)
{
    ReplicaCommand cmd;
    cmd.setFormat(format);
    return testFormatId( &cmd, expected, startIndex, totalCount);
}

void TestReplica::enumAlpha()
{
    ReplicaManager manager;
    for( int i= 0; i < 100 ; i++ ) {
        QString result = manager.formatAlpha(i, false, 200);
        printf("%d %s\n", i, result.toLatin1().data());
    }
    printf("****\n");
    for( int i= 0; i < 100 ; i++ ) {
        QString result = manager.formatAlpha(i, true, 10);
        printf("%d %s\n", i, result.toLatin1().data());
    }
    printf("****\n");
}

bool TestReplica::testFormat()
{
    _testName = "testFormat" ;

    if( !testFormatNumber( "1", 1, false, 1) ) {
        return false;
    }
    if( !testFormatNumber( "105", 105, false, 1) ) {
        return false;
    }
    if( !testFormatNumber( "105", 105, false, 100) ) {
        return false;
    }
    if( !testFormatNumber( "105", 105, true, 3) ) {
        return false;
    }
    if( !testFormatNumber( "1", 1, true, 1) ) {
        return false;
    }
    if( !testFormatNumber( "015", 15, true, 3) ) {
        return false;
    }
    if( !testFormatNumber( "0015", 15, true, 4) ) {
        return false;
    }
    //---
    if( !testFormatString( "A", 0, false, 1) ) {
        return false;
    }
    if( !testFormatString( "B", 1, false, 1) ) {
        return false;
    }
    if( !testFormatString( "AB", 26+1, false, 1) ) {
        return false;
    }
    if( !testFormatString( "AC", 26+2, false, 100) ) {
        return false;
    }
    if( !testFormatString( "AEC", 2+(26*4), true, 3) ) {
        return false;
    }
    if( !testFormatString( "A", 0, true, 1) ) {
        return false;
    }
    if( !testFormatString( "B", 1, true, 1) ) {
        return false;
    }
    if( !testFormatString( "AAAB", 1, true, 4) ) {
        return false;
    }
    if( !testFormatString( "AAD", 3, true, 3) ) {
        return false;
    }
    if( !testFormatString( "AADC",  2+(26*3), true, 4) ) {
        return false;
    }
    return true;
}

bool TestReplica::testFormatId()
{
    _testName = "testFormatId" ;

    if( !checkTestFormatId( "1", ReplicaCommand::NumberUnpadded, 1, 1) ) {
        return false;
    }
    if( !checkTestFormatId( "105", ReplicaCommand::NumberUnpadded, 105, 1000) ) {
        return false;
    }
    if( !checkTestFormatId( "1", ReplicaCommand::NumberPadded, 1, 1) ) {
        return false;
    }
    if( !checkTestFormatId( "000015", ReplicaCommand::NumberPadded, 15, 6) ) {
        return false;
    }
    if( !checkTestFormatId( "0000000015", ReplicaCommand::NumberPadded, 15, 10) ) {
        return false;
    }
    //---
    if( !checkTestFormatId( "A", ReplicaCommand::AlphaUnpadded, 0, 1) ) {
        return false;
    }
    if( !checkTestFormatId( "B", ReplicaCommand::AlphaUnpadded, 1, 1) ) {
        return false;
    }
    if( !checkTestFormatId( "AB", ReplicaCommand::AlphaUnpadded, 26+1, 1) ) {
        return false;
    }
    if( !checkTestFormatId( "AC", ReplicaCommand::AlphaUnpadded, 26+2, 100) ) {
        return false;
    }
    if( !checkTestFormatId( "AEC", ReplicaCommand::AlphaPadded, 2+(26*4), 3) ) {
        return false;
    }
    if( !checkTestFormatId( "A", ReplicaCommand::AlphaPadded, 0, 1) ) {
        return false;
    }
    if( !checkTestFormatId( "B", ReplicaCommand::AlphaPadded, 1, 1) ) {
        return false;
    }
    if( !checkTestFormatId( "AAAB", ReplicaCommand::AlphaPadded, 1, 4) ) {
        return false;
    }
    if( !checkTestFormatId( "AAD", ReplicaCommand::AlphaPadded, 3, 3) ) {
        return false;
    }
    if( !checkTestFormatId( "AADC", ReplicaCommand::AlphaPadded, 2+(26*3), 4) ) {
        return false;
    }
    return true;
}

/*
 * num siblings
 * alpha or number
 * total expected
 */
bool TestReplica::testWidth( const int numSiblings, const bool isAlpha, const int expected)
{
    ReplicaCommand cmd;
    if( isAlpha ) {
        cmd.setFormat(ReplicaCommand::AlphaPadded);
    } else {
        cmd.setFormat(ReplicaCommand::NumberPadded);
    }
    ReplicaManager manager;
    int totalWidthPattern = manager.totalWidth(numSiblings, manager.base(&cmd));
    if( totalWidthPattern != expected ) {
        return error(QString("Test width expected:%1 found:%2 alpha:%3").arg(expected).arg(totalWidthPattern).arg(isAlpha));
    }
    return true;
}

bool TestReplica::testWidth()
{
    _testName = "testWidth" ;
    if( !testWidth( 10, false, 2) ) {
        return false;
    }
    if( !testWidth( 105, false, 3) ) {
        return false;
    }
    if( !testWidth( 1234, false, 4) ) {
        return false;
    }
    if( !testWidth( 10, true, 1) ) {
        return false;
    }
    if( !testWidth( (26*26)+25, true, 3) ) {
        return false;
    }
    if( !testWidth( 10*(26*26*26)+3*(26*26)+25, true, 4) ) {
        return false;
    }
    return true;
}

bool TestReplica::testAction()
{
    _testName = "testAction" ;
    if( !testApply() ) {
        return false;
    }
    return true;
}

static int fromBool(const bool b)
{
    if( b ) {
        return 1;
    }
    return 0;
}

static QString fillCommand(ReplicaCommand *cmd,
                           const ReplicaCommand::EFormat format,
                        const bool overwrite,
                        const bool recursive,
                        const QString &attributeName,
                        const int startNumber,
                        const bool atEnd,
                        const QString &separator)
{
    cmd->setFormat(format);
    cmd->setOverwrite(overwrite);
    cmd->setRecursive(recursive);
    cmd->setAttributeName(attributeName);
    cmd->setStartNumber(startNumber);
    cmd->setAtEnd(atEnd);
    cmd->setSeparator(separator);
    QString resultFile = QString("o_%1_%2_%3_%4_%5_%6_%7.xml").arg(format).arg(fromBool(overwrite)).arg(fromBool(recursive))
            .arg(attributeName).arg(startNumber).arg(fromBool(atEnd)).arg(separator);
    return resultFile ;
}


bool TestReplica::testApply()
{
    _testName = "testApply" ;
    ReplicaCommand command;
    QList<int> selection ;
    selection << 1 << 1 << 1 ;
    QList<bool> bools;
    bools << false << true ;
    QStringList seps;
    seps << "_" << "" ;

    int index = 0 ;
    int format = ReplicaCommand::NumberUnpadded ;
    bool overwrite = false ;
     bool recursive = false;
    QString attributeName = "attrib" ;
    for( int startNumber = 0 ; startNumber < 2 ; startNumber ++ ) {
        foreach( bool atEnd, bools ) {
            foreach( QString separator, seps ) {
                QString resultFileName = fillCommand(&command, (ReplicaCommand::EFormat)format, overwrite, recursive, attributeName, startNumber, atEnd, separator) ;
                QString resultFile = QString(BASE_PATH).append(resultFileName);
                if( !baseApply( QString("%1").arg(index), INPUT1, resultFile, selection, &command ) ) {
                    return false;
                }
                index ++ ;
            }
        }
    }
    //test cases reduced
    //TODO: add more formatting, but specific

    //0/1_0_1/0_attrib_0_0/1_-;
    //for( int format = ReplicaCommand::NumberUnpadded ; format < ReplicaCommand::AlphaPadded ; format++ ) {
    QList<int> fmts;
    fmts << ReplicaCommand::NumberUnpadded << ReplicaCommand::AlphaUnpadded ;
    foreach( int format,  fmts ) {
        bool overwrite = true ;
        foreach( bool recursive, bools ) {
            QString attributeName = "attrib" ;
            int startNumber = 0 ;
            foreach( bool atEnd, bools ) {
                QString separator = "-";
                QString resultFileName = fillCommand(&command, (ReplicaCommand::EFormat)format, overwrite, recursive, attributeName, startNumber, atEnd, separator) ;
                QString resultFile = QString(BASE_PATH).append(resultFileName);
                if( !baseApply( QString("%1").arg(index), INPUT1, resultFile, selection, &command ) ) {
                    return false;
                }
                index ++ ;
            }
        }
    }

    {
        ReplicaCommand::EFormat format = ReplicaCommand::NumberPadded ;
        bool overwrite = true ;
        bool recursive = true ;
        QString attributeName = "attrib" ;
        int startNumber = 1000 ;
        bool atEnd = false;
        QString separator = "-";
        QString resultFileName = fillCommand(&command, format, overwrite, recursive, attributeName, startNumber, atEnd, separator) ;
        QString resultFile = QString(BASE_PATH).append(resultFileName);
        if( !baseApply( QString("%1").arg(index), INPUT1, resultFile, selection, &command ) ) {
            return false;
        }
        index ++ ;
    }
    {
        ReplicaCommand::EFormat format = ReplicaCommand::AlphaPadded ;
        bool overwrite = true ;
        bool recursive = true ;
        QString attributeName = "attrib" ;
        int startNumber = 26*2+4 ;
        bool atEnd = false;
        QString separator = "-";
        QString resultFileName = fillCommand(&command, format, overwrite, recursive, attributeName, startNumber, atEnd, separator) ;
        QString resultFile = QString(BASE_PATH).append(resultFileName);
        if( !baseApply( QString("%1").arg(index), INPUT1, resultFile, selection, &command ) ) {
            return false;
        }
        index ++ ;
    }
    if(!testFormatNumber()) {
        return false;
    }
    if(!testFormatAlpha()) {
        return false;
    }
    return true ;
}

bool TestReplica::baseApply(const QString &id,
                            const QString &fileIn, const QString &fileCompare,
                                   QList<int> selection,
                                    ReplicaCommand *command )
{
    App app;
    if(!app.init()) {
        return error(QString("%1: init app ").arg(id));
    }
    if( !app.mainWindow()->loadFile(fileIn) ) {
        return error(QString("unable to load input file: '%1' ").arg(fileIn));
    }
    Regola *regola = app.mainWindow()->getRegola();
    Element *selectedElement = regola->findElementByArray(selection);
    if( NULL == selectedElement ) {
        return error(QString("Null element: %1").arg(id));
    }
    ReplicaManager replica;
    replica.apply( NULL, regola, selectedElement, command );

    if(!compare( regola, id, fileCompare )) {
        return false ;
    }
    return true;
}

bool TestReplica::compareAttributes(Element *element, const QString &attributeName, const QString &expected )
{
    QString value = element->getAttributeValue(attributeName);
    if( value != expected ) {
        return error( QString("Value differs. Expected:'%2', value:'%1'").arg(value).arg(expected));
    }
    return true ;
}

bool TestReplica::testFormatNumber()
{
    _testName = "testFormatNumber";
    ReplicaCommand command;
    ReplicaCommand::EFormat format = ReplicaCommand::NumberPadded ;
    bool overwrite = true ;
    bool recursive = false ;
    QString attributeName = "attrib" ;
    int startNumber = 0 ;
    bool atEnd = false;
    QString separator = "-";
    fillCommand(&command, format, overwrite, recursive, attributeName, startNumber, atEnd, separator) ;

    App app;
    if(!app.init()) {
        return error(QString("init app "));
    }
    Regola *regola = app.mainWindow()->getRegola();
    Element *selectedElement = regola->newElement();
    selectedElement->setTag("root");
    regola->setRootElement(selectedElement);

    // add
    for( int i = 0 ; i < 1100 ; i ++ ) {
        Element * theNewElement = regola->newElement();
        theNewElement->setTag("this");;
        selectedElement->addChild(theNewElement);
    }
    ReplicaManager replica;
    replica.apply(NULL, regola, selectedElement->getChildAt(0), &command );

    if(!compareAttributes(selectedElement->getChildAt(500), "attrib", "0500")) {
        return false;
    }
    if(!compareAttributes(selectedElement->getChildAt(1000), "attrib", "1000")) {
        return false;
    }
    if(!compareAttributes(selectedElement->getChildAt(0), "attrib", "0000")) {
        return false;
    }
    if(!compareAttributes(selectedElement->getChildAt(4), "attrib", "0004")) {
        return false;
    }
    if(!compareAttributes(selectedElement->getChildAt(23), "attrib", "0023")) {
        return false;
    }
    if(!compareAttributes(selectedElement->getChildAt(750), "attrib", "0750")) {
        return false;
    }

    return true;
}


bool TestReplica::testFormatAlpha()
{
    _testName = "testFormatAlpha";
    ReplicaCommand command;
    ReplicaCommand::EFormat format = ReplicaCommand::AlphaPadded ;
    bool overwrite = true ;
    bool recursive = false ;
    QString attributeName = "attrib" ;
    int startNumber = 0 ;
    bool atEnd = false;
    QString separator = "-";
    fillCommand(&command, format, overwrite, recursive, attributeName, startNumber, atEnd, separator) ;

    App app;
    if(!app.init()) {
        return error(QString("init app "));
    }
    Regola *regola = app.mainWindow()->getRegola();
    Element *selectedElement = regola->newElement();
    selectedElement->setTag("root");
    regola->setRootElement(selectedElement);

    // add
    for( int i = 0 ; i < (3*26*26)+10 ; i ++ ) {
        Element * theNewElement = regola->newElement();
        theNewElement->setTag("this");;
        selectedElement->addChild(theNewElement);
    }
    ReplicaManager replica;
    replica.apply( NULL, regola, selectedElement->getChildAt(0), &command );

    if(!compareAttributes(selectedElement->getChildAt(0), "attrib", "AAA")) {
        return false;
    }
    if(!compareAttributes(selectedElement->getChildAt(1), "attrib", "AAB")) {
        return false;
    }
    if(!compareAttributes(selectedElement->getChildAt(26*26), "attrib", "BAA")) {
        return false;
    }
    if(!compareAttributes(selectedElement->getChildAt(26), "attrib", "ABA")) {
        return false;
    }
    if(!compareAttributes(selectedElement->getChildAt((3*26*26)+4), "attrib", "DAE")) {
        return false;
    }

    return true;
}

bool TestReplica::checkAndRaiseError( const QString &id, ReplicaCommand *cmd, const ReplicaCommand::EErrorCode expected)
{
    const ReplicaCommand::EErrorCode value = cmd->check();
    if( value != expected ) {
        return error(QString("Id: %1 expected error: %2, but found: %3").arg(id).arg(expected).arg(value));
    }
    QString errMsg = cmd->errorMessage(value);
    if( expected == ReplicaCommand::NoError ) {
        if( !errMsg.isEmpty() ) {
            return error(QString("Id: %1 expected no error message, but found :'%2'").arg(id).arg(errMsg));
        }
    } else {
        if( errMsg.isEmpty() ) {
            return error(QString("Id: %1 expected an error message for:%2, but found none.").arg(id).arg(errMsg));
        }
    }
    return true ;
}


static void defaultCommand(ReplicaCommand *command)
{
    ReplicaCommand::EFormat format = ReplicaCommand::AlphaPadded ;
    bool overwrite = true ;
    bool recursive = false ;
    QString attributeName = "attrib" ;
    int startNumber = 0 ;
    bool atEnd = false;
    QString separator = "-";
    fillCommand(command, format, overwrite, recursive, attributeName, startNumber, atEnd, separator) ;
}

bool TestReplica::testCheckParams()
{
    _testName = "testCheckParams" ;
    {
        ReplicaCommand command;
        defaultCommand(&command) ;
        if( !checkAndRaiseError( "0", &command, ReplicaCommand::NoError) ) {
            return false;
        }
    }
    {
        ReplicaCommand command;
        defaultCommand(&command) ;
        command.setAttributeName("@!@# sdfl");
        if( !checkAndRaiseError( "0", &command, ReplicaCommand::AttributeNameError) ) {
            return false;
        }
    }
    {
        ReplicaCommand command;
        defaultCommand(&command) ;
        command.setStartNumber(-1);
        if( !checkAndRaiseError( "0", &command, ReplicaCommand::StartNumberError) ) {
            return false;
        }
    }
    {
        ReplicaCommand command;
        defaultCommand(&command) ;
        command.setFormat((ReplicaCommand::EFormat)122112);
        if( !checkAndRaiseError( "0", &command, ReplicaCommand::IllegalFormatError) ) {
            return false;
        }
    }
    return true ;
}

bool TestReplica::testUnitCommand()
{
    _testName = "testUnitCommand" ;
    if(!testCommandCompare()) {
        return false;
    }
    if(!testCommandClone()) {
        return false;
    }
    return true ;
}


bool TestReplica::testCommandCompare()
{
    _testName = "testCommandCompare" ;
    {
        ReplicaCommand c1;
        ReplicaCommand c2;
        if(!c1.compare(&c2)) {
            return error("cmp 1");
        }
    }
    {
        ReplicaCommand c1;
        ReplicaCommand c2;
        c2.setRecursive(!c1.recursive());
        if(c1.compare(&c2)) {
            return error("cmp 2");
        }
    }
    {
        ReplicaCommand c1;
        ReplicaCommand c2;
        c2.setAttributeName("a");
        if(c1.compare(&c2)) {
            return error("cmp 3");
        }
    }
    {
        ReplicaCommand c1;
        ReplicaCommand c2;
        c2.setOverwrite(!c1.overwrite());
        if(c1.compare(&c2)) {
            return error("cmp 4");
        }
    }
    {
        ReplicaCommand c1;
        ReplicaCommand c2;
        c2.setStartNumber(c1.startNumber()+1);
        if(c1.compare(&c2)) {
            return error("cmp 4");
        }
    }
    {
        ReplicaCommand c1;
        ReplicaCommand c2;
        c2.setAtEnd(!c1.atEnd());
        if(c1.compare(&c2)) {
            return error("cmp 5");
        }
    }
    {
        ReplicaCommand c1;
        ReplicaCommand c2;
        c2.setSeparator("a");
        if(c1.compare(&c2)) {
            return error("cmp 6");
        }
    }
    {
        ReplicaCommand c1;
        ReplicaCommand c2;
        c2.setFormat(static_cast<ReplicaCommand::EFormat>(c1.format()+1));
        if(c1.compare(&c2)) {
            return error("cmp 7");
        }
    }    return true;
}

class CC {
public:
    ReplicaCommand* c;
    CC(ReplicaCommand &newC) {
        c = newC.clone();
    }
    ~CC() {
        if( NULL != c ) {
            delete c;
            c = NULL ;
        }
    }
    bool verify(ReplicaCommand &b) {
        return b.compare(c);
    }

};

bool TestReplica::testCommandClone()
{
    _testName = "testCommandClone" ;
    {
        ReplicaCommand c1;
        CC c2(c1);
        if(!c2.verify(c1)) {
            return error("clone 1");
        }
    }
    {
        ReplicaCommand c1;
        c1.setRecursive(!c1.recursive());
        CC c2(c1);
        if(!c2.verify(c1)) {
            return error("clone 2");
        }
    }
    {
        ReplicaCommand c1;
        c1.setAttributeName(c1.attributeName()+"a");
        CC c2(c1);
        if(!c2.verify(c1)) {
            return error("clone 3");
        }
    }
    {
        ReplicaCommand c1;
        c1.setOverwrite(!c1.overwrite());
        CC c2(c1);
        if(!c2.verify(c1)) {
            return error("clone 4");
        }
    }
    {
        ReplicaCommand c1;
        c1.setStartNumber(c1.startNumber()+1);
        CC c2(c1);
        if(!c2.verify(c1)) {
            return error("clone 4");
        }
    }
    {
        ReplicaCommand c1;
        c1.setAtEnd(!c1.atEnd());
        CC c2(c1);
        if(!c2.verify(c1)) {
            return error("clone 5");
        }
    }
    {
        ReplicaCommand c1;
        c1.setSeparator(c1.separator()+"1");
        CC c2(c1);
        if(!c2.verify(c1)) {
            return error("clone 6");
        }
    }
    {
        ReplicaCommand c1;
        c1.setFormat(static_cast<ReplicaCommand::EFormat>(c1.format()+1));
        CC c2(c1);
        if(!c2.verify(c1)) {
            return error("clone 7");
        }
    }
    return true;
}


#define EL() \
    Element el("a", "", NULL, NULL );\
    el.addAttribute("b", "c");\
    el.addAttribute("d", "e");

#define DLG(dlg) EL()\
    ReplicaSettingsDialog dlg( &el, NULL );

bool TestReplica::checkCombo(QStringList values, const QString &comboName, const QString &expected)
{
    DLG(dlg);
    QComboBox *cbAttributes = dlg.findChild<QComboBox*>(comboName);
    if( NULL == cbAttributes ) {
        return error(QString("null combo %1").arg(comboName));
    }
    if( cbAttributes->count() != values.size() ) {
        return error(QString("Expected %1 items, found %2").arg(values.size()).arg(cbAttributes->count()));
    }
    for( int i = 0 ; i < values.size() ; i ++ ) {
        if( cbAttributes->itemData(i).toString() != values.at(i) ) {
            return error(QString("Expected at %1 '%2', found '%3'").arg(i).arg(values.at(i)).arg(cbAttributes->itemData(i).toString()));
        }
    }
    cbAttributes->setEditText(expected);
    dlg.accept();
    ReplicaCommand *cmd = dlg.result();
    if( NULL == cmd ) {
        return error("Null command");
    }
    bool ok = true ;
    if( cmd->attributeName() != expected) {
        ok = error( QString("Expected: '%1' found: '%2' ").arg(expected).arg(cmd->attributeName()));
    }
    delete cmd;

    return ok ;
}

#define TEST_BOOL(field, name)\
{\
    DLG(dlg);\
    QCheckBox *cb = dlg.findChild<QCheckBox*>(name);\
    if( NULL == cb ) {\
        return error(QString("null check %1").arg(name));\
    }\
    cb->setChecked(true);\
    dlg.accept();\
    ReplicaCommand *cmd = dlg.result();\
    if( NULL == cmd ) {\
        return error("Null command");\
    }\
    bool ok = true ;\
    if( cmd->field() != true) {\
        ok = error( QString("Expected: '%1' found: '%2' ").arg(true).arg(cmd->field()));\
    }\
    delete cmd;\
    if( !ok ) {\
        return false;\
    }\
    cb->setChecked(false);\
    dlg.accept();\
    cmd = dlg.result();\
    if( NULL == cmd ) {\
        return error("Null command");\
    }\
    ok = true ;\
    if( cmd->field() != false) {\
        ok = error( QString("Expected: '%1' found: '%2' ").arg(false).arg(cmd->field()));\
    }\
    delete cmd;\
    if( !ok ) {\
        return false;\
    }\
}

#define TEST_EDIT(field, name)\
{\
    DLG(dlg);\
    QLineEdit *cb = dlg.findChild<QLineEdit*>(name);\
    if( NULL == cb ) {\
        return error(QString("null edit %1").arg(name));\
    }\
    QString theValue = "" ;\
    cb->setText(theValue);\
    dlg.accept();\
    ReplicaCommand *cmd = dlg.result();\
    if( NULL == cmd ) {\
        return error("Null command");\
    }\
    bool ok = true ;\
    if( cmd->field() != theValue) {\
        ok = error( QString("Expected: '%1' found: '%2' ").arg(theValue).arg(cmd->field()));\
    }\
    delete cmd;\
    if( !ok ) {\
        return false;\
    }\
    theValue = "aaa";\
    cb->setText(theValue);\
    dlg.accept();\
    cmd = dlg.result();\
    if( NULL == cmd ) {\
        return error("Null command");\
    }\
    ok = true ;\
    if( cmd->field() != theValue) {\
        ok = error( QString("Expected: '%1' found: '%2' ").arg(theValue).arg(cmd->field()));\
    }\
    delete cmd;\
    if( !ok ) {\
        return false;\
    }\
}

bool TestReplica::testCommandDialog()
{
    _testName = "testCommandDialog" ;

    {
        QStringList values;
        values << "b" << "d" ;
        if( !checkCombo(values, "attributes", "aa") ) {
            return false;
        }
    }
    TEST_BOOL(atEnd, "cbAtEnd");
    TEST_BOOL(recursive, "cbApplyChildren");
    TEST_BOOL(overwrite, "cbOverWrite");
    TEST_EDIT(separator,"separator");
    return true ;
}

