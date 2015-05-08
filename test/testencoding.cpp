/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2014 by Luca Bellonda and individual contributors       *
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


#include "testencoding.h"
#include "app.h"
#include "comparexml.h"
#include "regola.h"
#include "editprocessinginstruction.h"
#include "charencodingdialog.h"

#define DEFAULT_ENCODING    "UTF-8"

TestEncoding::TestEncoding()
{
}

TestEncoding::~TestEncoding()
{
}

///--------
#define INPUT_FILE_EMPTY        "../test/data/encoding/empty.xml"
#define INPUT_FILE_UTF8         "../test/data/encoding/utf8.xml"
#define INPUT_FILE_ISO885915    "../test/data/encoding/iso8859-15.xml"
#define INPUT_FILE_CP1250       "../test/data/encoding/wincp1250.xml"
#define CHANGE_FILE_UTF8         "../test/data/encoding/changeutf8.xml"
#define CHANGE_FILE_ISO885915    "../test/data/encoding/changeiso8859.xml"

///--------
///
bool TestEncoding::testUnitTests()
{
    if(!testRecognizeEncoding(INPUT_FILE_EMPTY, DEFAULT_ENCODING)) {
        return false;
    }
    if(!testRecognizeEncoding(INPUT_FILE_UTF8, "UTF-8")) {
        return false;
    }
    if(!testRecognizeEncoding(INPUT_FILE_ISO885915, "ISO-8859-15")) {
        return false;
    }
    if(!testExportClipboard()) {
        return false ;
    }
    return true;
}

bool TestEncoding::testFunctionalTests()
{
    if(!testReadAndWriteEncoding()){
        return false;
    }
    if(!testAllReadEncoding()){
        return false;
    }
    if(!testNewWithEncoding()){
        return false;
    }
    if(!testNewWithSchema() ) {
        return false;
    }
    if(!testReadAndWriteChangedEncoding()){
        return false;
    }
    if(!testWriteEncoding()){
        return false;
    }
    return true;
}


//-------------------------------------------------------


bool TestEncoding::testRecognizeEncoding(const QString &file, const QString &expectedEncoding)
{
    _testName = "testRecognizeEncoding";

    App app;
    if(!app.init(true)) {
        return error("init all");
    }
    if( !app.mainWindow()->loadFile(file)) {
        return error(QString("Opening file '%1'").arg(file));
    }
    if( app.mainWindow()->getRegola()->originalEncoding() != expectedEncoding ) {
        return error(QString("Original encoding, exprected:'%1' found:'%2'").arg(expectedEncoding).arg(app.mainWindow()->getRegola()->originalEncoding()));
    }
    if( app.mainWindow()->getRegola()->encoding() != expectedEncoding ) {
        return error(QString("Original Eencoding, exprected:'%1' found:'%2'").arg(expectedEncoding).arg(app.mainWindow()->getRegola()->encoding()));
    }
    if(!checkLabel(&app, expectedEncoding, "1", "") ){
        return false;
    }
    return true;
}

//---------------------------------------------------------


bool TestEncoding::testReadAndWriteEncoding()
{
    if(!testReadAndWriteAnEncoding(INPUT_FILE_UTF8, "UTF-8")){
        return false;
    }
    if(!testReadAndWriteAnEncoding(INPUT_FILE_ISO885915, "ISO-8859-15")){
        return false;
    }
    if(!testReadAndWriteAnEncoding(INPUT_FILE_CP1250, "WINDOWS-1250")){
        return false;
    }
    return true;
}

bool TestEncoding::testReadAndWriteAnEncoding(const QString &fileInput, const QString &encoding)
{
    _testName = QString("testReadAndWriteEncoding %1").arg(encoding);
    App app;
    if(!app.init(true)) {
        return error("init app");
    }
    if(!app.mainWindow()->loadFile(fileInput)){
        return error( QString("error reading file open:'%1'").arg(fileInput) );
    }
    QBuffer buffer;
    app.mainWindow()->getRegola()->setIndentation(0);
    if(!app.mainWindow()->getRegola()->write(&buffer, false)) {
        return error( "error writing");
    }
    CompareXML compare;
    if( !compare.compareBufferWithFile(&buffer, fileInput)) {
        return error(QString("error comparing '%1' cause:%2").arg(fileInput).arg(compare.errorString()));
    }
/*    QFile file(fileInput);
    if( file.open(QIODevice::ReadOnly)) {
        QByteArray dataFile = file.readAll();
        if( file.error() != QFile::NoError) {
            file.close();
            return error(QString("error reading final '%1'").arg(fileInput));
        }
        file.close();
        if(dataFile != buffer.data()) {
            return error(QString("error comparing '%1'").arg(fileInput));
        }
    } else {
        return error(QString("error opening final file '%1'").arg(fileInput));
    }*/
    return true;
}

bool TestEncoding::testNewWithSchema()
{
    _testName = QString("testNewWithSchema");
    App app;
    if(!app.init(true)) {
        return error("init app");
    }
    app.data()->setAutoInsertPrologEncoding("ISO-8859-15");
    app.data()->setAutoInsertProlog(true);
    app.mainWindow()->getEditor()->onActionNewUsingXMLSchema("");
    if( app.mainWindow()->getRegola()->encoding() != "ISO-8859-15" ) {
        return error(QString("Econdign differs, expected:'%1' found='%2'").arg("ISO-8859-15").arg(app.mainWindow()->getRegola()->encoding()));
    }
    return true;
}

// snippet is a string carried as payload, so the original encoding is UTF8, but when
// decoded, it is really a memory full unicode string.
Regola* TestEncoding::regolaFromFile(const QString &fileInput, const QString &encoding)
{
    QString data ;
    if( loadFileAsString(fileInput, encoding, &data) ) {
        QDomDocument document ;
        if(document.setContent(data)) {
            Regola * regola = new Regola(document, "");
            return regola;
        }
    }
    return NULL ;
}


/* TODO: delete
bool TestBase::loadFileIntoTheClipboard(const QString &fileInput, const QString &encoding)
{
    QFile file;
    if( file.open(QIODevice::ReadOnly)) {
        QByteArray dataFile = file.readAll();
        if( file.error() != QFile::NoError) {
            file.close();
            return error(QString("error reading final '%1'").arg(fileInput));
        }
        file.close();
        QTextCodec *codec = QTextCodec::codecForName(encoding);
        QString data = codec->toUnicode(dataFile);
        QApplication::clipboard()->setText(file);
        return true;
    }
    return error(QString("error opening final '%1'").arg(fileInput));
}*/

/****
 *General usage
 *Test type|start enc|final ENC|Undo|Redo|input| out |
 * --------|---------|---------|----------------------
 * CHANGE  | E1 UTF8 |   E2    |UTF8| E2 | F1  | R1  |
 * SAME    | E2      |   E2    | E2 | E2 | F2  | R1  |
 * REMOVE  | E2      |  none   | E2 |none| F2  | R2  |
 * INSERT  | none    |   E2    |none| E2 | F3  | F1  |
*/

class TestEncodingParams {
public:
    enum EOpenType {
        //O_NEW=0, NEW IS TESTED APART
        O_OPEN=0,
        O_NEWFROMCLIPBOARD,
        O_NEWFROMTEMPLATE,
        O_NEWFROMSNIPPET,
        //O_NEWWITHSCHEMA, already tested
        O_MAX
    };

    enum ESaveType {
        S_SAVE=0,
        S_SAVEAS,
        S_SAVECOPY,
        S_TOSTRING,
        S_TOCLIPBOARD,
        S_MAX
    };

    enum EEditType {
        E_EDIT=0,
        E_DELETE,
        E_INSERT,
        E_MOVEUP,
        E_MOVEDOWN,
        //E_INSERTSNIPPET,
        //E_UNCOMMENT,
        E_CUT,
        //E_PASTE,
        E_INSERT_PROLOG,
        E_INSERT_ENCODING,
        E_MAX
    };

    enum ETestType {
        T_CHANGE=0,
        T_INSERT,
        T_REMOVE,
        T_SAME,
        T_MAX
    };

    //EOpenType openType;
    //ESaveType saveType;
    EEditType editType;
    ETestType testType;
    bool useUndo;
    bool useRedo;
    QString inputFileName;
    QString resultFileName;
    QString startEncoding;
    QString editEncoding;
    bool notCompareFile;
    bool doNotTest ;

    TestEncodingParams( const ETestType pTestType, const EEditType pEditType, const QString &pStartEncoding, const QString & pEditEncoding,
                        const QString &pInputFile, const QString &pResultFile, const bool pUseUndo, const bool pUseRedo ) {
        //openType = pOpenType ;
        testType = pTestType ;
        editType = pEditType ;
        startEncoding = pStartEncoding ;
        editEncoding = pEditEncoding ;
        inputFileName = pInputFile;
        resultFileName = pResultFile;
        useUndo = pUseUndo;
        useRedo = pUseRedo;
        notCompareFile = false ;
        doNotTest = false ;
    }

};

#define READ_ALL_NOENC      "../test/data/encoding/read_all_noenc.xml"
#define READ_ALL_UTF8       "../test/data/encoding/read_all_utf8.xml"
#define READ_ALL_ISO8859    "../test/data/encoding/read_all_iso8859.xml"

bool TestEncoding::testResultsForEncoding(const QString &expected, App *app)
{
    if( expected != app->mainWindow()->getRegola()->encoding()) {
        return error( QString("Cfr1 expected:'%1', found:'%2'.").arg(expected).arg(app->mainWindow()->getRegola()->encoding()));
    }
    if( !checkLabel(app, expected, "2", "") ) {
        return false;
    }
    if( expected != app->mainWindow()->getRegola()->encoding() ) {
        return error( QString("Cfr2 expected:'%1', found:'%2'.").arg(expected).arg(app->mainWindow()->getRegola()->encoding()));
    }
    return true;
}

// tests all the load methods
bool TestEncoding::testAllReadEncoding()
{
    _testName = "testAllReadEncoding";

    {
        App app;
        if(!app.init()) {
            return error("init");
        }
        if(!testResultsForEncoding(DEFAULT_ENCODING, &app) ) {
            return false;
        }
    }
    QList<QPair<QString,QString> > filesAndEncodings;
    QPair<QString,QString> encNothing("UTF-8", READ_ALL_NOENC);
    QPair<QString,QString> encUtf8("UTF-8", READ_ALL_UTF8);
    QPair<QString,QString> encISO8859("ISO-8859-15", READ_ALL_ISO8859);
    filesAndEncodings << encNothing;
    filesAndEncodings << encUtf8;
    filesAndEncodings << encISO8859;
    QPair<QString,QString> xtest;
    foreach( xtest, filesAndEncodings ) {
        for( int pOpenType = 0 ; pOpenType < TestEncodingParams::O_MAX ; pOpenType++ ) {
            App app;
            if(!app.init(true)) {
                return error("init");
            }
            switch(pOpenType){
            default:
                return error( QString("open method not recognized, state:%1").arg(pOpenType) );

            case TestEncodingParams::O_OPEN:
                if(!app.mainWindow()->loadFile(xtest.second)) {
                    return error( QString("opening input file :'%1'").arg(xtest.second) );
                }
                break;
            case TestEncodingParams::O_NEWFROMCLIPBOARD:
                //carica il file binario nella clipboard
                if(!loadFileIntoClipboard(xtest.second, xtest.first)) {
                    return error( QString("unable to put file into the clipboard :'%1'").arg(xtest.second) );
                }
                if( !fireAction(app.mainWindow(), "actionNewFromClipboard")) {
                    return error( QString("unable to fire actionNewFromClipboard for '%1'").arg(xtest.second));
                }
                break;
            case TestEncodingParams::O_NEWFROMTEMPLATE:
                {
                    QString templateName ;
                    if(READ_ALL_NOENC == xtest.second ) {
                        templateName = ":/templates/template_noenc";
                    } else if(READ_ALL_UTF8 == xtest.second ) {
                        templateName = ":/templates/template_utf8";
                    } else if(READ_ALL_ISO8859 == xtest.second ) {
                        templateName = ":/templates/templateiso8859-15";
                    }
                    if(!app.mainWindow()->createDocumentFromResources(templateName)) {
                        return error( QString("error creating template '%1'").arg(templateName));
                    }
                }
                break;
            case TestEncodingParams::O_NEWFROMSNIPPET:
                {
                    Regola* newRegola = regolaFromFile(xtest.second, xtest.first);
                    if( NULL == newRegola ) {
                        return error( QString("unable to load snippet for '%1'").arg(xtest.second));
                    }
                    app.mainWindow()->createDocumentFromSnippet(newRegola);
                }
                break;
            }
            if(!testResultsForEncoding(xtest.first, &app) ) {
                return false;
            }
        }
    }
    return true;
}

/**
  definition of tests:
  delete
        -> with -> without (enc ) -> *
        -> with w1 -> with w2 (enc1, enc2 ) -> enc
        -> without -> with ( pi, enc) -> enc
  insert
   with enc1-> enc2
        enc1 -> nothing, e2
        nothing -> enc
  uncomment
        comment-> enc
  edit
    e1 -> e2
    e1 -> nothing
    nothing -> e2

  paste
    as insert

  insert snippet
    as insert
*/

#define ENC_1       "UTF-8"
#define ENC_2       "ISO-8859-15"
#define ENC_NONE    "UTF-8"
#define IN_FILE_1   "../test/data/encoding/infile_1.xml"
#define IN_FILE_2   "../test/data/encoding/infile_2.xml"
#define IN_FILE_3   "../test/data/encoding/infile_3.xml"
#define IN_FILE_3_EDIT  "../test/data/encoding/infile_3_edit.xml"
#define RIS_FILE_1   "../test/data/encoding/risfile_1.xml"
#define RIS_FILE_2   "../test/data/encoding/risfile_2.xml"

// only two sample encodings are needed
bool TestEncoding::testReadAndWriteChangedEncoding()
{
    _testName = "testReadAndWriteChangedEncoding";
    QList<bool> blu;
    blu << true;
    blu << false;
    QList<bool> blr;
    blr << true;
    blr << false;
    //from each test I expect: input encoding and input file
    // edit mode
    // output encoding when saved
    for( int pEditType = 0 ; pEditType < TestEncodingParams::E_MAX ; pEditType++ ) {
        for( int pTestType = 0 ; pTestType < TestEncodingParams::T_MAX ; pTestType++ ) {
            foreach( bool useUndo, blu ) {
                foreach( bool useRedo, blr ) {
                    /*if( !useUndo && useRedo ) {
                        // skip this case
                        continue;
                    }*/
                    QString startEncoding ;
                    QString endEncoding ;
                    QString fileInput ;
                    QString fileOutput ;

                    switch(pTestType) {
                    default:
                        return error( QString("test method not recognized, :%1").arg(pTestType) );

                    case TestEncodingParams::T_CHANGE:
                        startEncoding = ENC_1 ;
                        endEncoding = ENC_2;
                        fileInput = IN_FILE_1;
                        fileOutput = RIS_FILE_1;
                        break;
                    case TestEncodingParams::T_INSERT:
                        startEncoding = ENC_NONE ;
                        endEncoding = ENC_2;
                        fileInput = IN_FILE_3;
                        fileOutput = RIS_FILE_1;
                        if( TestEncodingParams::E_EDIT == pEditType ) {
                            fileInput = IN_FILE_3_EDIT;
                        }

                        break;
                    case TestEncodingParams::T_REMOVE:
                        startEncoding = ENC_2 ;
                        endEncoding = ENC_NONE;
                        fileInput = IN_FILE_2;
                        fileOutput = RIS_FILE_2;
                        break;
                    case TestEncodingParams::T_SAME:
                        startEncoding = ENC_2 ;
                        endEncoding = ENC_2;
                        fileInput = IN_FILE_2;
                        fileOutput = RIS_FILE_1;
                        break;
                    }
                    if( useUndo && !useRedo ) {
                        fileOutput = fileInput;
                    }
                    TestEncodingParams testParam((TestEncodingParams::ETestType)pTestType, (TestEncodingParams::EEditType)pEditType, startEncoding, endEncoding, fileInput, fileOutput, useUndo, useRedo);
                    if( !testComplexWithParams(&testParam)) {
                        return false;
                    }
                }
            }
        }
    }
    return true;
}

/*****************************************************
            QString startEncoding ;
            QString endEncoding ;
            QString fileInput = MOD_INPUT;
            QString fileOutput = MOD_OUTPUT;
            switch(pEditType) {
            default:
                return error( QString("edit method not recognized, :%1").arg(describeParams(pEditType)) );
            }
            case TestEncodingParams::E_PASTE:
                {

                }
                break;
            case TestEncodingParams::E_INSERTSNIPPET:
                break;
            case TestEncodingParams::E_UNCOMMENT:
                break;
            case TestEncodingParams::E_EDIT:
                break;
            case TestEncodingParams::E_DELETE:
                {
                    //
                    TestEncodingParams test1(pSaveType, pEditType, startEncoding, endEncoding, IN_WITH1, OUT_WITHOUT);
                    if(!testComplex(&test1)) {
                        return false;
                    }
                    TestEncodingParams test2(pSaveType, pEditType, startEncoding, endEncoding, IN_WITH1, OUT_WITH2);
                    if(!testComplex(&test2)) {
                        return false;
                    }
                    TestEncodingParams test3(pSaveType, pEditType, startEncoding, endEncoding, IN_WITHOUTDEL, OUT_WITH1);
                    if(!testComplex(&test3)) {
                        return false;
                    }
                }
                break;
            }

            QString startEncoding = "ISO-8859-15";
            QString endEncoding = "windows-1250";
            QString fileInput = MOD_INPUT;
            QString fileOutput = MOD_OUTPUT;
            TestEncodingParams test(pOpenType, pSaveType, pEditType, startEncoding, endEncoding, IN_WITH, OUT_WITHOUT);
            if(!testComplex(&test)) {
                return false;
            }
        }
    }
    return true ;
}
*****************************************************/

QString TestEncoding::describeParams(TestEncodingParams *classa)
{
    QString result = QString("config params: testType:%1, editType:%2, startEncoding:%3, editEncoding:%4, useUndo:%5, useRedo:%6, in:%7, out:%8, not test:%9")
        .arg(classa->testType)//1
        .arg(classa->editType)//2
        .arg(classa->startEncoding)//3
        .arg(classa->editEncoding)//4
        .arg(classa->useUndo)//5
        .arg(classa->useRedo) //6
        .arg(classa->inputFileName)//7
        .arg(classa->resultFileName) //8
        .arg(classa->doNotTest); //9
    return result;
}

bool TestEncoding::testComplexWithParams(TestEncodingParams *p)
{
    App app;
    if(!app.init(true)) {
        return error("init app");
    }

    if(!app.mainWindow()->loadFile(p->inputFileName) ) {
        return error( QString("error reading file open, state:%1").arg(describeParams(p)) );
    }

    // primo controllo su sincronizazione operazione di edit
    if(app.mainWindow()->getRegola()->encoding() != p->startEncoding ) {
        return error( QString("start encoding, found:'%1'', expected '%2' params:%3").arg(app.mainWindow()->getRegola()->encoding()).arg(p->startEncoding).arg(describeParams(p)) );
    }
    if(!checkLabel(&app, p->startEncoding, "start", describeParams(p) ) ){
        return false;
    }
    switch(p->editType) {
    default:
        return error( QString("error unexpected edit type params:%1").arg(describeParams(p)) );
     /*case TestEncodingParams::E_PASTE:
        if( !handlePaste(&app, p)) {
            return false;
        }
        break;*/
     /*case TestEncodingParams::E_INSERTSNIPPET:
        if( !handleInsertSnippet(&app, p)) {
            return false;
        }
        break;
     case TestEncodingParams::E_UNCOMMENT:
        if( !handleUncomment(&app, p)) {
            return false;
        }
        break;*/
    case TestEncodingParams::E_INSERT:
       if( !handleInsert(&app, p)) {
           return false;
       }
       break;
    case TestEncodingParams::E_EDIT:
        if( !handleEdit(&app, p)) {
            return false;
        }
        break;
     case TestEncodingParams::E_DELETE:
        if( !handleDelete(&app, p)) {
            return false;
        }
        break;
     case TestEncodingParams::E_CUT:
        if( !handleCut(&app, p)) {
            return false;
        }
        break;
       case TestEncodingParams::E_INSERT_PROLOG:
        if( !handleProlog(&app, p)) {
            return false;
        }
        break;
        case TestEncodingParams::E_INSERT_ENCODING:
        if( !handleInsertEncoding(&app, p)) {
            return false;
        }
        break;
        case TestEncodingParams::E_MOVEUP:
        if( !handleMoveUp(&app, p)) {
            return false;
        }
        break;
        case TestEncodingParams::E_MOVEDOWN:
        if( !handleMoveDown(&app, p)) {
            return false;
        }
        break;
    }
    if( p->doNotTest) {
        return true;
    }
    // primo controllo su sincronizazione operazione di edit
    if(app.mainWindow()->getRegola()->encoding() != p->editEncoding ) {
        return error( QString("end encoding, found:'%1'', expected '%2' params:%3").arg(app.mainWindow()->getRegola()->encoding()).arg(p->editEncoding).arg(describeParams(p)) );
    }
    if(!checkLabel(&app, p->editEncoding, "edit", describeParams(p) ) ){
        return false;
    }

    //undo
    if( p->useUndo ) {
        app.mainWindow()->getRegola()->undo();
        // primo controllo su sincronizazione operazione di edit
        if(app.mainWindow()->getRegola()->encoding() != p->startEncoding ) {
            return error( QString("start redo encoding, found:'%1'', expected '%2' params:%3").arg(app.mainWindow()->getRegola()->encoding()).arg(p->startEncoding).arg(describeParams(p)) );
        }
        if(!checkLabel(&app, p->startEncoding, "undo", describeParams(p) ) ){
            return false;
        }
        if( p->useRedo ) {
            app.mainWindow()->getRegola()->redo();
            // primo controllo su sincronizazione operazione di edit
            if(app.mainWindow()->getRegola()->encoding() != p->editEncoding ) {
                return error( QString("start redo encoding, found:'%1'', expected '%2' params:%3").arg(app.mainWindow()->getRegola()->encoding()).arg(p->startEncoding).arg(describeParams(p)) );
            }
            if(!checkLabel(&app, p->editEncoding, "redo", describeParams(p) ) ){
                return false;
            }
        }
    }
    QBuffer regola ;
    if(!app.mainWindow()->getRegola()->write(&regola, true)) {
        return error( QString("Writing regola to memory for the final compare params:%1").arg(describeParams(p)) );
    }
    if( !p->notCompareFile) {
        CompareXML compare;
        if(!compare.compareBufferWithFile(&regola, p->resultFileName)) {
            return error( QString("Finale compare params:%1, msg:%2").arg(describeParams(p)).arg(compare.errorString()) );
        }
    }

    return true;
}

bool TestEncoding::checkLabel(App *app, const QString &encoding, const QString &msg, const QString &p)
{
    QLabel *lbl = app->mainWindow()->getEditor()->findChild<QLabel*>("encoding");
    if( NULL == lbl ) {
        return error( QString("unable to find the label params:%1").arg(p) );
    }
    QString expected = app->mainWindow()->getEditor()->labelForEncoding(encoding);
    if( lbl->text() != expected ) {
        return error( QString("Label encoding, type:%1 found:'%2', expected '%3' params:%4").arg(msg).arg(lbl->text()).arg(expected).arg(p) );
    }
    return true;
}

QString TestEncoding::mkEncodingDecl(const QString &enc)
{
    QString result = QString("<?xml version='1.0' encoding='%1' ?>").arg(enc);
    return result;
}

bool TestEncoding::handleMoveUp(App *app, TestEncodingParams *p)
{
    p->notCompareFile = true;
    switch(p->testType) {
    default:
        return error( QString("error unexpected test type params:%1").arg(describeParams(p)) );
    case TestEncodingParams::T_CHANGE:
        {
            if(!insertEncodingAtIndex(app, ENC_2, 1)){
                return false;
            }
            Element *e = selectTopElement(app, p, 1);
            if( NULL == e ) {
                return error( QString("unable to find an element, params:%1").arg(describeParams(p)) );
            }
            app->mainWindow()->getEditor()->onActionMoveUp();
        }
        break;
    case TestEncodingParams::T_INSERT:
        {
            if(!insertEncodingAtIndex(app, ENC_2, 1)){
                return false;
            }
            Element *e = selectTopElement(app, p, 1);
            if( NULL == e ) {
                return error( QString("unable to find an element, params:%1").arg(describeParams(p)) );
            }
            app->mainWindow()->getEditor()->onActionMoveUp();
        }
        break;
    case TestEncodingParams::T_REMOVE:
        {
            app->mainWindow()->getEditor()->removeAllElements();
            if(!insertElementAtIndex(app, 0)){
                return false;
            }
            if(!insertEncodingAtIndex(app, ENC_2, 0)){
                return false;
            }
            Element *e = selectTopElement(app, p, 1);
            if( NULL == e ) {
                return error( QString("unable to find an element, params:%1").arg(describeParams(p)) );
            }
            app->mainWindow()->getEditor()->onActionMoveUp();
        }
        break;
    case TestEncodingParams::T_SAME:
        {
            if(!insertEncodingAtIndex(app, ENC_2, 1)){
                return false;
            }
            Element *e = selectTopElement(app, p, 1);
            if( NULL == e ) {
                return error( QString("unable to find an element, params:%1").arg(describeParams(p)) );
            }
            app->mainWindow()->getEditor()->onActionMoveUp();
        }
        break;
    }
    return true;
}

bool TestEncoding::handleMoveDown(App *app, TestEncodingParams *p)
{
    p->notCompareFile = true;
    switch(p->testType) {
    default:
        return error( QString("error unexpected test type params:%1").arg(describeParams(p)) );
    case TestEncodingParams::T_CHANGE:
        {
            if(!insertEncodingAtIndex(app, ENC_2, 1)){
                return false;
            }
            Element *e = selectTopElement(app, p, 0);
            if( NULL == e ) {
                return error( QString("unable to find an element, params:%1").arg(describeParams(p)) );
            }
            app->mainWindow()->getEditor()->onActionMoveDown();
        }
        break;
    case TestEncodingParams::T_INSERT:
        {
            if(!insertEncodingAtIndex(app, ENC_2, 1)){
                return false;
            }
            Element *e = selectTopElement(app, p, 0);
            if( NULL == e ) {
                return error( QString("unable to find an element, params:%1").arg(describeParams(p)) );
            }
            app->mainWindow()->getEditor()->onActionMoveDown();
        }
        break;
    case TestEncodingParams::T_REMOVE:
        {
            app->mainWindow()->getEditor()->removeAllElements();
            if(!insertElementAtIndex(app, 0)){
                return false;
            }
            if(!insertEncodingAtIndex(app, ENC_2, 0)){
                return false;
            }
            Element *e = selectTopElement(app, p, 0);
            if( NULL == e ) {
                return error( QString("unable to find an element, params:%1").arg(describeParams(p)) );
            }
            app->mainWindow()->getEditor()->onActionMoveDown();
        }
        break;
    case TestEncodingParams::T_SAME:
        {
            if(!insertEncodingAtIndex(app, ENC_2, 1)){
                return false;
            }
            Element *e = selectTopElement(app, p, 0);
            if( NULL == e ) {
                return error( QString("unable to find an element, params:%1").arg(describeParams(p)) );
            }
            app->mainWindow()->getEditor()->onActionMoveUp();
        }
        break;
    }
    return true;
}

bool TestEncoding::handleInsert(App *app, TestEncodingParams *p)
{
    switch(p->testType) {
    default:
        return error( QString("error unexpected test type params:%1").arg(describeParams(p)) );
    case TestEncodingParams::T_CHANGE:
    {
        Element *pi = new Element(app->mainWindow()->getRegola(), Element::ET_PROCESSING_INSTRUCTION, NULL);
        pi->setPITarget("xml");
        pi->setPIData(QString("version='1.0' encoding='%1'").arg(ENC_2));
        p->notCompareFile = true ;
        app->mainWindow()->getEditor()->removeAllElements();
        app->mainWindow()->getRegola()->addProcessingInstruction(app->mainWindow(), app->mainWindow()->getMainTreeWidget(), pi);
    }
        break;
    case TestEncodingParams::T_INSERT:
    {
        app->mainWindow()->getEditor()->removeAllElements();
        Element *pi = new Element(app->mainWindow()->getRegola(), Element::ET_PROCESSING_INSTRUCTION, NULL);
        pi->setPITarget("xml");
        pi->setPIData(QString("version='1.0' encoding='%1'").arg(ENC_2));
        app->mainWindow()->getRegola()->addProcessingInstruction(app->mainWindow(), app->mainWindow()->getMainTreeWidget(), pi);
        Element *e = new Element( "root", "", app->mainWindow()->getRegola(), NULL);
        app->mainWindow()->getRegola()->addTopElement(e);
    }
        break;
    case TestEncodingParams::T_REMOVE:
    {
        QList<int> sel;
        sel << 0;
        Element *selectedElement = app->mainWindow()->getRegola()->findElementByArray(sel);
        if( NULL != selectedElement ) {
            app->mainWindow()->getEditor()->setCurrentItem(selectedElement);
        } else {
            return error( QString("unable to find an element, params:%1").arg(describeParams(p)) );
        }
        app->mainWindow()->getRegola()->deleteElement(selectedElement, true);
    }
        break;
    case TestEncodingParams::T_SAME:
    {
        app->mainWindow()->getEditor()->removeAllElements();
        Element *pi = new Element(app->mainWindow()->getRegola(), Element::ET_PROCESSING_INSTRUCTION, NULL);
        pi->setPITarget("xml");
        pi->setPIData(QString("version='1.0' encoding='%1'").arg(ENC_2));
        app->mainWindow()->getRegola()->addProcessingInstruction(app->mainWindow(), app->mainWindow()->getMainTreeWidget(), pi);
        p->notCompareFile = true ;
        if( p->useUndo) {
            p->doNotTest = true;
        }
    }
        break;
    }
    return true;
}

static bool editHook(QWidget *const parentWindow, QTreeWidgetItem *item, UIDelegate *uiDelegate, Element* element);
static bool editHook2(QWidget *const parentWindow, QTreeWidgetItem *item, UIDelegate *uiDelegate, Element* element);

bool editHook(QWidget *const /*parentWindow*/, QTreeWidgetItem * /*item*/, UIDelegate * /*uiDelegate*/, Element* element)
{
    if(NULL == element) {
        return false;
    }
    element->setPITarget("xml");
    element->setPIData(QString("version='1.0' encoding='%1'").arg(ENC_2));
    /*EditProcessingInstruction editElementDialog(parentWindow);
    editElementDialog.setTarget(element) ;
    editElementDialog.accept();*/

    return true ;
}

bool editHook2(QWidget *const /*parentWindow*/, QTreeWidgetItem * /*item*/, UIDelegate * /*uiDelegate*/, Element* element)
{
    if(NULL == element) {
        return false;
    }
    element->setPITarget("xxml");
    element->setPIData("");

    return true ;
}

bool TestEncoding::handleEdit(App *app, TestEncodingParams *p)
{
    p->notCompareFile = true;

    QList<int> sel;
    sel << 0;
    Element *selectedElement = app->mainWindow()->getRegola()->findElementByArray(sel);
    if( NULL != selectedElement ) {
        app->mainWindow()->getEditor()->setCurrentItem(selectedElement);
    } else {
        return error( QString("unable to find an element, params:%1").arg(describeParams(p)) );
    }
    app->mainWindow()->getRegola()->setEditHook(&editHook);
    switch(p->testType) {
    default:
        return error( QString("error unexpected test type params:%1").arg(describeParams(p)) );
    case TestEncodingParams::T_CHANGE:
        {
            app->mainWindow()->getEditor()->onActionEdit();
        }
        break;
    case TestEncodingParams::T_INSERT:
        {
        app->mainWindow()->getEditor()->onActionEdit();
        }
        break;
    case TestEncodingParams::T_REMOVE:
        {
            app->mainWindow()->getRegola()->setEditHook(&editHook2);
            app->mainWindow()->getEditor()->onActionEdit();
        }
        break;
    case TestEncodingParams::T_SAME:
        {
            app->mainWindow()->getEditor()->onActionEdit();
        }
        break;
    }
    return true;
}

bool TestEncoding::handleUncomment(App *app, TestEncodingParams *p) {
    p->notCompareFile = true;
    switch(p->testType) {
    default:
        return error( QString("error unexpected test type params:%1").arg(describeParams(p)) );
    case TestEncodingParams::T_CHANGE:
        {
            app->mainWindow()->getEditor()->removeAllElements();
            QApplication::clipboard()->setText(mkEncodingDecl(ENC_2));
            app->mainWindow()->getEditor()->onActionPaste();
        }
        break;
    case TestEncodingParams::T_INSERT:
        {
            app->mainWindow()->getEditor()->removeAllElements();
            QApplication::clipboard()->setText(mkEncodingDecl(ENC_2));
            app->mainWindow()->getEditor()->onActionPaste();
        }
        break;
    case TestEncodingParams::T_REMOVE:
        {
            app->mainWindow()->getEditor()->removeAllElements();
            QApplication::clipboard()->setText("<root />");
            app->mainWindow()->getEditor()->onActionPaste();
        }
        break;
    case TestEncodingParams::T_SAME:
        {
            app->mainWindow()->getEditor()->removeAllElements();
            QApplication::clipboard()->setText(mkEncodingDecl(ENC_2));
            app->mainWindow()->getEditor()->onActionPaste();
        }
        break;
    }
    return true;
}

bool TestEncoding::handleDelete(App *app, TestEncodingParams *p)
{
    switch(p->testType) {
    default:
        return error( QString("error unexpected test type params:%1").arg(describeParams(p)) );
    case TestEncodingParams::T_CHANGE:
    {
        insertEncodingAsSecond(app, ENC_2);
        deleteFirstElement(app, p);
        p->notCompareFile = true ;
    }
        break;
    case TestEncodingParams::T_INSERT:
    {
        insertCommentAtIndex(app, 0);
        insertEncodingAsSecond(app, ENC_2);
        deleteFirstElement(app, p);
        p->notCompareFile = true ;
    }
        break;
    case TestEncodingParams::T_REMOVE:
    {
        deleteFirstElement(app, p);
    }
        break;
    case TestEncodingParams::T_SAME:
    {
        insertEncodingAsSecond(app, ENC_2);
        deleteFirstElement(app, p);
        p->notCompareFile = true ;
    }
        break;
    }
    return true;
}

bool TestEncoding::handleCut(App *app, TestEncodingParams *p)
{
    switch(p->testType) {
    default:
        return error( QString("error unexpected test type params:%1").arg(describeParams(p)) );
    case TestEncodingParams::T_CHANGE:
    {
        insertEncodingAsSecond(app, ENC_2);
        cutFirstElement(app, p);
        p->notCompareFile = true ;
    }
        break;
    case TestEncodingParams::T_INSERT:
    {
        insertCommentAtIndex(app, 0);
        insertEncodingAsSecond(app, ENC_2);
        cutFirstElement(app, p);
        p->notCompareFile = true ;
    }
        break;
    case TestEncodingParams::T_REMOVE:
    {
        cutFirstElement(app, p);
    }
        break;
    case TestEncodingParams::T_SAME:
    {
        insertEncodingAsSecond(app, ENC_2);
        cutFirstElement(app, p);
        p->notCompareFile = true ;
    }
        break;
    }
    return true;
}

bool TestEncoding::handleProlog(App *app, TestEncodingParams *p)
{
    switch(p->testType) {
    default:
        return error( QString("error unexpected test type params:%1").arg(describeParams(p)) );
    case TestEncodingParams::T_CHANGE:
        {
            Element *selectedElement = selectFirstElement(app, p);
            if( NULL == selectedElement ) {
                return false;
            }
            QString oldValue = selectedElement->getPITarget();
            selectedElement->setPITarget("x");
            app->data()->setInsertPrologEncoding(ENC_2);
            app->mainWindow()->getEditor()->insertProlog();
            selectedElement->setPITarget(oldValue);
            p->notCompareFile = true ;
        }
        break;
    case TestEncodingParams::T_INSERT:
        {
            app->data()->setInsertPrologEncoding(ENC_2);
            app->mainWindow()->getEditor()->insertProlog();
        }
        break;
    case TestEncodingParams::T_REMOVE:
        {
            p->doNotTest = true;
        }
        break;
    case TestEncodingParams::T_SAME:
        {
            Element *selectedElement = selectFirstElement(app, p);
            if( NULL == selectedElement ) {
                return false;
            }
            QString oldValue = selectedElement->getPITarget();
            selectedElement->setPITarget("x");
            app->data()->setInsertPrologEncoding(ENC_2);
            app->mainWindow()->getEditor()->insertProlog();
            selectedElement->setPITarget(oldValue);
            p->notCompareFile = true ;
        }
        break;
    }
    return true;
}

bool TestEncoding::handleInsertEncoding(App *app, TestEncodingParams *p) {
    switch(p->testType) {
    default:
        return error( QString("error unexpected test type params:%1").arg(describeParams(p)) );
    case TestEncodingParams::T_CHANGE:
        {
            app->mainWindow()->getEditor()->setEncoding(ENC_2);
        }
        break;
    case TestEncodingParams::T_INSERT:
        {
            app->mainWindow()->getEditor()->setEncoding(ENC_2);
        }
        break;
    case TestEncodingParams::T_REMOVE:
        {
            p->doNotTest = true;
        }
        break;
    case TestEncodingParams::T_SAME:
        {
            app->mainWindow()->getEditor()->setEncoding(ENC_2);
        }
        break;
    }
    return true;
}

bool TestEncoding::deleteFirstElement(App *app, TestEncodingParams *p)
{
    Element *selectedElement = selectFirstElement(app, p);
    if( NULL == selectedElement ) {
        return false ;
    }
    app->mainWindow()->getRegola()->deleteElement(selectedElement, true);
    return true;
}

Element *TestEncoding::selectFirstElement(App *app, TestEncodingParams *p)
{
    return selectTopElement(app, p, 0);
}

Element *TestEncoding::selectTopElement(App *app, TestEncodingParams *p, const int pos)
{
    QList<int> sel;
    sel << pos;
    Element *selectedElement = app->mainWindow()->getRegola()->findElementByArray(sel);
    if( NULL != selectedElement ) {
        app->mainWindow()->getEditor()->setCurrentItem(selectedElement);
    } else {
        error( QString("unable to find first element to delete it, params:%1").arg(describeParams(p)) );
    }
    return selectedElement;
}

bool TestEncoding::cutFirstElement(App *app, TestEncodingParams *p)
{
    Element *element = selectFirstElement(app, p);
    if( NULL == element ) {
        return false ;
    }
    app->mainWindow()->getEditor()->onActionCut();
    return true;
}
bool TestEncoding::insertEncodingAsSecond(App *app, const QString &encoding)
{
    return insertEncodingAtIndex(app, encoding, 1);
}

bool TestEncoding::insertEncodingAtIndex(App *app, const QString &encoding, const int position)
{
    Element *pi = new Element(app->mainWindow()->getRegola(), Element::ET_PROCESSING_INSTRUCTION, NULL);
    pi->setPITarget("xml");
    pi->setPIData(QString("version='1.0' encoding='%1'").arg(encoding));
    app->mainWindow()->getRegola()->pasteInternals(app->mainWindow()->getEditor()->getMainTreeWidget(), NULL, pi, position);
    return true;
}

bool TestEncoding::insertCommentAtIndex(App *app, const int position)
{
    Element *pi = new Element(app->mainWindow()->getRegola(), Element::ET_COMMENT, NULL);
    pi->setComment("test");
    app->mainWindow()->getRegola()->pasteInternals(app->mainWindow()->getEditor()->getMainTreeWidget(), NULL, pi, position);
    return true;
}

bool TestEncoding::insertElementAtIndex(App *app, const int position)
{
    Element *pi = new Element( "root", "", app->mainWindow()->getRegola(), NULL);
    app->mainWindow()->getRegola()->pasteInternals(app->mainWindow()->getEditor()->getMainTreeWidget(), NULL, pi, position);
    return true;
}

bool TestEncoding::testNewWithEncoding()
{
    _testName = "testNewWithSchemaEncoding" ;
    App app;
    if(!app.init(true)) {
        return error("init all");
    }
    app.data()->setAutoInsertProlog(false);
    app.data()->setAutoInsertPrologEncoding("");
    if( !fireAction(app.mainWindow(), "actionNew") ) {
        return error(QString("Firing new 1 "));
    }
    if( app.mainWindow()->getRegola()->encoding() != DEFAULT_ENCODING ) {
        return error(QString("Econding differs, expected:'%1' found='%2'").arg(DEFAULT_ENCODING).arg(app.mainWindow()->getRegola()->encoding()));
    }
    app.data()->setAutoInsertProlog(true);
    app.data()->setAutoInsertPrologEncoding("ISO-8859-15");
    if( !fireAction(app.mainWindow(), "actionNew") ) {
        return error(QString("Firing new 1 "));
    }
    if( app.mainWindow()->getRegola()->encoding() != "ISO-8859-15" ) {
        return error(QString("Econdign differs, expected:'%1' found='%2'").arg("ISO-8859-15").arg(app.mainWindow()->getRegola()->encoding()));
    }
    return true;
}

///-----------------------------------------
///
bool TestEncoding::testWriteEncoding()
{
    _testName = "testWriteEncoding" ;
    if(!testReadChangeAndWriteAnEncoding(CHANGE_FILE_UTF8, CHANGE_FILE_ISO885915, "ISO-8859-15")){
        return false;
    }
    return true;
}

bool TestEncoding::testReadChangeAndWriteAnEncoding(const QString &fileInput, const QString &fileCfr, const QString &finalEncoding)
{
    _testName = QString("testReadChangeAndWriteEncoding %1").arg(finalEncoding);
    App app;
    if(!app.init(true)) {
        return error("init app");
    }
    if(!app.mainWindow()->loadFile(fileInput)){
        return error( QString("error reading file open:'%1'").arg(fileInput) );
    }
    if(!app.mainWindow()->getEditor()->setEncoding(finalEncoding)) {
        return error( QString("error setting encoding :'%1'").arg(finalEncoding) );
    }

    if( app.mainWindow()->getRegola()->encoding() != finalEncoding ) {
        return error(QString("Econding differs, expected:'%1' found='%2'").arg(finalEncoding).arg(app.mainWindow()->getRegola()->encoding()));
    }
    // all save operations end in the same method
    QBuffer buffer;
    if(!app.mainWindow()->getRegola()->write(&buffer, false)) {
        return error( "error writing");
    }
    /*QFile file(fileCfr);
    if( file.open(QIODevice::ReadOnly)) {
        QByteArray dataFile = file.readAll();
        if( file.error() != QFile::NoError) {
            file.close();
            return error(QString("error reading final '%1'").arg(fileInput));
        }
        file.close();
        if(dataFile != buffer.data()) {
            return error(QString("error comparing '%1'").arg(fileInput));
        }
    } else {
        return error(QString("error opening final file '%1'").arg(fileInput));
    }*/
    CompareXML compare;
    if(!compare.compareBufferWithFile(&buffer, fileCfr)) {
        return error( QString("Finale compare file:%1, msg:%2").arg(fileCfr).arg(compare.errorString()) );
    }

    return true;
}

bool TestEncoding::checkClipbpoardStringToUtf8C(const QString &str)
{
    QApplication::clipboard()->setText("");
    CharEncodingDialog dlg(NULL);
    dlg.exportToUtf8CBytes(str);
    QString decoded = QApplication::clipboard()->text();
    QString expected = "\"\\x61\\xe2\\x80\\xa2\"";
    if( decoded != expected ) {
        return error( QString("Error in toutf8C, expected:'%1', found:'%2'").arg(expected).arg(decoded));
    }
    return true ;
}


bool TestEncoding::checkClipbpoardStringToUtf8(const QString &str)
{
    QApplication::clipboard()->setText("");
    CharEncodingDialog dlg(NULL);
    dlg.exportToUtf8Bytes(str);
    QString decoded = QApplication::clipboard()->text();
    QString expected = "0x61 0xe2 0x80 0xa2";
    if( decoded != expected ) {
        return error( QString("Error in toutf8, expected:'%1', found:'%2'").arg(expected).arg(decoded));
    }
    return true ;
}

bool TestEncoding::checkClipbpoardStringToJava(const QString &str)
{
    QApplication::clipboard()->setText("");
    CharEncodingDialog dlg(NULL);
    dlg.exportToJavaUnicodeChars(str);
    QString decoded = QApplication::clipboard()->text();
    QString expected = "\"a\\u2022\"";
    if( decoded != expected ) {
        return error( QString("Error in toJava, expected:'%1', found:'%2'").arg(expected).arg(decoded));
    }
    return true ;
}

bool TestEncoding::testExportClipboard()
{
    _testName = "testExportClipboard";
    QString src = "a" + QString::fromUtf8("\u2022");
    if(!checkClipbpoardStringToUtf8C(src)){
        return false ;
    }
    if(!checkClipbpoardStringToUtf8(src)){
        return false ;
    }
    if(!checkClipbpoardStringToJava(src)){
        return false ;
    }
    return true ;
}
