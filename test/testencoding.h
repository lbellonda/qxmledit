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


#ifndef TESTENCODING_H
#define TESTENCODING_H

#include "testbase.h"

class TestEncodingParams;
class App;
class Regola;
class Element;

class TestEncoding : public TestBase
{
    bool existsIBM500();
    bool testAskForWrite();
    bool is8BitEncodingHonoredForStreamWriter(const QString &encoding, bool &isError);
    bool testWriteStreamQtBug();
    bool testFileMode();
    bool testRecognizeEncoding(const QString &file, const QString &expectedEncoding);
    bool testReadAndWriteEncoding();
    bool testReadAndWriteAnEncoding(const QString &fileInput, const QString &encoding);
    bool testResultsForEncoding(const QString &expected, App*app);
    bool testAllReadEncoding();
    bool testReadAndWriteChangedEncoding();
    QString describeParams(TestEncodingParams *classa);
    bool testComplexWithParams(TestEncodingParams *p);
    bool testWriteEncoding();
    bool testNewWithEncoding();
    bool checkLabel(App *app, const QString &encoding, const QString &msg, const QString &p);
    Regola* regolaFromFile(const QString &fileInput, const QString &encoding);
    bool testNewWithSchema();
    bool deleteFirstElement(App *app, TestEncodingParams *p);
    bool cutFirstElement(App *app, TestEncodingParams *p);
    bool insertEncodingAsSecond(App *app, const QString &encoding);
    bool insertEncodingAtIndex(App *app, const QString &encoding, const int position);
    bool insertCommentAtIndex(App *app, const int position);
    bool insertElementAtIndex(App *app, const int position);
    QString mkEncodingDecl(const QString &enc);
    Element *selectFirstElement(App *app, TestEncodingParams *p);
    Element *selectTopElement(App *app, TestEncodingParams *p, const int pos);
    bool testReadChangeAndWriteAnEncoding(const QString &fileInput, const QString &fileCfr, const QString &finalEncoding);

    bool handleUncomment(App *app, TestEncodingParams *p);
    bool handleEdit(App *app, TestEncodingParams *p);
    bool handleDelete(App *app, TestEncodingParams *p);
    bool handleCut(App *app, TestEncodingParams *p);
    bool handleProlog(App *app, TestEncodingParams *p);
    bool handleInsertEncoding(App *app, TestEncodingParams *p);
    bool handleInsert(App *app, TestEncodingParams *p);
    bool handleMoveUp(App *app, TestEncodingParams *p);
    bool handleMoveDown(App *app, TestEncodingParams *p);
    bool checkClipbpoardStringToUtf8C(const QString &str);
    bool checkClipbpoardStringToUtf8(const QString &str);
    bool checkClipbpoardStringToJava(const QString &str);
    bool testExportClipboard();

    bool testSkeleton( const QString &id, const QString &fileStart, const QString &newEncoding,
                                     const int indentationValue, const bool sortAttributesAlpha,
                                     const QXmlEditData::EIndentAttributes valueSetting, const int valueCols );

public:
    TestEncoding();
    ~TestEncoding();

    bool testFast();
    bool testUnitTests();
    bool testFunctionalTests();
    bool testLoadSave();

};

#endif // TESTENCODING_H
