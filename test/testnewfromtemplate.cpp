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


#include "testnewfromtemplate.h"
#include "app.h"
#include "comparexml.h"
#include "snippet_interface.h"
#include "testhelpers/testsnippethandler.h"

#define FILE_XSD    "../src/resources/templates/templateXsd.xml"
#define FILE_FO    "../src/resources/templates/templateFo.xml"
#define FILE_XSLT    "../src/resources/templates/templateXsl.xml"
#define FILE_POM   "../src/resources/templates/templateMavenPOM.xml"
#define FILE_TEST_SNIPPET "../test/data/snippet_base.xml"
#define FILE_TEST_SNIPPET_NOPRE "../test/data/snippet_base_nopre.xml"
#define FILE_TEST_SNIPPET_RESULT "../test/data/test_snippet_result.xml"
#define FILE_TEST_SNIPPET_RESULT_NOPRE "../test/data/test_snippet_result.nopre.xml"

#define ANAME_FO    "actionNewXSLFODocument"
#define ANAME_XSLT  "actionNewXSLTSheet"
#define ANAME_XSD   "actionNewXSDDocument"
#define ANAME_POM   "actionNewMavenPOM"

TestNewFromTemplate::TestNewFromTemplate()
{
}

bool TestNewFromTemplate::testFast()
{
    return testBadXML();
}

bool TestNewFromTemplate::testCreateFromTemplate()
{
    _testName = "testNewFromTemplate" ;
    if( ! testNewXSD() ) {
        return false;
    }
    if( ! testNewFO() ) {
        return false;
    }
    if( ! testNewXSLT() ) {
        return false;
    }
    if( ! testNewPOM() ) {
        return false;
    }
    if( ! testMixed() ) {
        return false;
    }
    if(! testEmptyFileName()) {
        return false;
    }
    if(! testBadXML()) {
        return false;
    }
    return true ;
}

bool TestNewFromTemplate::testEmptyFileName()
{
    _testName = "testEmptyFileName" ;
    App app;
    if(!app.init() ) {
        return error("init window");
    }
    if( app.mainWindow()->createDocumentFromResources("") ) {
        return error("Create fromresource empty path");
    }
    return true ;
}

bool TestNewFromTemplate::testBadXML()
{
    _testName = "testBadXML" ;
    App app;
    if(!app.init() ) {
        return error("init window");
    }
    if( app.mainWindow()->createDocumentFromResources(":/templates/badTemplate.xml") ) {
        return error("Create from resource bad template");
    }
    return true ;
}

bool TestNewFromTemplate::testNewXSD()
{
    return testNewFile(FILE_XSD, "test xsd", ANAME_XSD);
}

bool TestNewFromTemplate::testNewFO()
{
    return testNewFile(FILE_FO, "test fo", ANAME_FO);
}

bool TestNewFromTemplate::testNewXSLT()
{
    return testNewFile(FILE_XSLT, "test xslt", ANAME_XSLT);
}

bool TestNewFromTemplate::testNewPOM()
{
    return testNewFile(FILE_POM, "test pom", ANAME_POM);
}

bool TestNewFromTemplate::testMixed()
{
    if(!testNewFile(FILE_XSD, "test xsd/mixed", ANAME_XSD) ) {
        return false;
    }
    if(!testNewFile(FILE_FO, "test fo/mixed", ANAME_FO) ) {
        return false;
    }
    if(!testNewFile(FILE_XSLT, "test xslt/mixed", ANAME_XSLT)) {
        return false;
    }
    if(!testNewFile(FILE_XSD, "test xsd/mixed", ANAME_XSD)) {
        return false;
    }
    if(!testNewFile(FILE_FO, "test fo/mixed", ANAME_FO) ) {
        return false;
    }
    if(!testNewFile(FILE_XSLT, "test xslt/mixed", ANAME_XSLT) ) {
        return false;
    }
    return true;
}

bool TestNewFromTemplate::testNewFile(const QString &fileName, const QString testName, const QString &actionName)
{
    _testName = testName ;
    App app;
    if(!app.initNoWindow() ) {
        return error("init window");
    }
    MainWindow mainWindow(false, app.data());
    bool found = fireAction( &mainWindow, actionName );
    if(!found ) {
        return error(QString("action '%1' not found").arg(actionName));
    }
    if( !mainWindow.getRegola()->fileName().isEmpty()) {
        return error("after loading the file name is not empty");
    }

    QByteArray resultData = mainWindow.getRegola()->writeMemory();
    QBuffer outputData(&resultData);

    QDomDocument document1;
    QDomDocument document2;
    CompareXML compare;
    if(!compare.loadFileIntoDocument(fileName, document1)) {
        return error(QString("loading compare file '%1'").arg(fileName));
    }
    if(!compare.loadFileIntoDocument(&outputData, document2)) {
        return error("loading data from memory");
    }
    bool result = compare.compareDomDocuments(document1, document2);
    if(!result ) {
        return error( QString("File '%1' is different ").arg(fileName) );
    }
    return true ;

}

//------------------------------------------------
bool TestNewFromTemplate::testCreateFromSnippet()
{
    _testName = "testNewFromSnippet" ;
    if( ! testNewFromSnippet(FILE_TEST_SNIPPET, "testCreateFromSnippet1", FILE_TEST_SNIPPET_RESULT) ) {
        return false;
    }
    if( ! testChooseSnippet(FILE_TEST_SNIPPET_NOPRE, "testCreateFromSnippet2", FILE_TEST_SNIPPET_RESULT_NOPRE) ) {
        return false;
    }
    return true ;
}

class FakeSnippetManager: public SnippetManager {
  //  Q_OBJECT
    QString _fileToLoad;
public:
    explicit FakeSnippetManager(const QString &newFile);
    virtual ~FakeSnippetManager();
    virtual Regola* chooseSnippets(ApplicationData *data, QWidget *parent);
};

FakeSnippetManager::FakeSnippetManager(const QString &newFile) : SnippetManager()
{
    _fileToLoad = newFile ;
}

FakeSnippetManager::~FakeSnippetManager()
{}

Regola* FakeSnippetManager::chooseSnippets(ApplicationData * data, QWidget * parent)
{
    /*
    QString errorMessage;
    Regola *result = Regola::loadFromFile(_fileToLoad, &errorMessage);
    if( NULL == result ) {
        printf( "Error loading snippet, cause:'%s'\n", errorMessage.toLatin1().data() );
    }
    return result;
*/
    TestSnippetHandler handler(_fileToLoad, data, false, parent);
    handler.setModal(true);
    if(handler.exec() == QDialog::Accepted) {
        return handler.regola();
    }
    return NULL;
}

bool TestNewFromTemplate::testNewFromSnippet(const QString &fileName, const QString &testName, const QString &resultFileName)
{
    _testName = testName ;
    App app;
    if(!app.initNoWindow() ) {
        return error("init window");
    }
    MainWindow mainWindow(false, app.data());
    if( !mainWindow.loadFile(QString(FILE_TEST_BASE_SIMPLE)) ) {
        return error(QString("opening test file: '%1'").arg(FILE_TEST_BASE_SIMPLE));
    }

    FakeSnippetManager *snippetManager = new FakeSnippetManager(fileName);
    mainWindow.setSnippetManager(snippetManager);
    bool found = fireAction( &mainWindow, "actionNewFromSnippet" );
    if(!found ) {
        return error(QString("action '%1' not found").arg("actionNewFromSnippet"));
    }
    if( !mainWindow.getRegola()->fileName().isEmpty()) {
        return error("after creating, the file name is not empty");
    }

    Element *newRoot = mainWindow.getRegola()->root();
    if( NULL == newRoot ) {
        return error("after creating, root is null");
    }
    if( newRoot->tag() != "site" ) {
        return error(QString("after creating, root tag is '%1', expected is 'site'").arg(newRoot->tag()));
    }
    CompareXML compare;
    bool result = compare.compareFileWithRegola(mainWindow.getRegola(), resultFileName);
    if(!result ) {
        return error( QString("File '%1' is different reason:%2").arg(resultFileName).arg(compare.errorString()) );
    }
    return true ;

}

//-----

bool TestNewFromTemplate::testChooseSnippet(const QString &fileName, const QString &testName, const QString &resultFileName)
{
    _testName = testName;
    _testName += "/testChooseSnippet" ;
    App app;
    if(!app.initNoWindow() ) {
        return error("init window");
    }
    MainWindow mainWindow(false, app.data());
    if( !mainWindow.loadFile(QString(FILE_TEST_BASE_SIMPLE)) ) {
        return error(QString("opening test file: '%1'").arg(FILE_TEST_BASE_SIMPLE));
    }

    TestSnippetHandler handler( fileName, app.data(), false, &mainWindow);
    bool isOk = false;
    int retCode = handler.exec() ;
    Regola *regola = handler.regola();
    if(retCode != QDialog::Accepted) {
        error(QString("Snippet returned invalid response :%d").arg(retCode));
    } else {
        if( NULL == regola ) {
            error(QString("Snippet returned NULL result"));
        } else {
            isOk = true;
        }
    }
    if( isOk ) {
        // compare the result.
        CompareXML compare;
        if( !compare.compareFileWithRegola( regola, resultFileName)) {
            error(QString("compare differs (file '%1') :%2").arg(resultFileName).arg(compare.errorString()));
            isOk = false ;
        }
    }
    if( NULL != regola ) {
        delete regola;
    }
    return isOk ;

}
