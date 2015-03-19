/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2011 by Luca Bellonda and individual contributors       *
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
#include "app.h"
#include "testcomment.h"
#include "comparexml.h"
#include "config.h"
#include "mainwindow.h"
#include "utils.h"
#include "fakeuidelegate.h"

#define TEST_FILE_COMMENT_INPUT  "../test/data/comment_input.xml"
#define TEST_FILE_COMMENT_RESULT  "../test/data/comment_result.xml"
#define TEST_FILE_UNCOMMENT_INPUT  "../test/data/uncomment_input.xml"
#define TEST_FILE_COMMENT_MULTI_INPUT   "../test/data/uncomment_multi_input.xml"
#define TEST_FILE_COMMENT_MULTI_RESULT  "../test/data/uncomment_multi_result.xml"
#define TEST_FILE_COMMENT_UNDOABLE  "../test/data/uncomment_undoable.xml"


TestComment::TestComment()
{
}

bool TestComment::testMakeComment()
{
    App app;
    if(!app.initNoWindow() ) {
        return false;
    }

    MainWindow mainWindow(false, qApp, app.data());

    mainWindow.loadFile(QString(TEST_FILE_COMMENT_INPUT));

    QList<int> selection;
    selection.append(1);
    selection.append(0);
    selection.append(1);
    selection.append(1);
    Element *elementToTransform = mainWindow.getRegola()->findElementByArray(selection);
    mainWindow.getRegola()->transformInComment(&mainWindow, NULL, elementToTransform) ;

    QByteArray resultData = mainWindow.getRegola()->writeMemory();
    QDomDocument document1;
    QDomDocument document2;
    CompareXML compare;
    if(!compare.loadFileIntoDocument(TEST_FILE_COMMENT_RESULT, document1)) {
        return false;
    }
    QBuffer outputData(&resultData);
    if(!compare.loadFileIntoDocument(&outputData, document2)) {
        return false;
    }
    bool result = compare.compareDomDocuments(document1, document2);

    return result ;
}

bool TestComment::testUndoComment()
{
    return testUndoCommentInternal(QString(TEST_FILE_COMMENT_RESULT), TEST_FILE_COMMENT_INPUT);
}

bool TestComment::testUndoCommentMulti()
{
    return testUndoCommentInternal(QString(TEST_FILE_COMMENT_MULTI_INPUT), TEST_FILE_COMMENT_MULTI_RESULT);
}

bool TestComment::testUndoCommentInternal(const QString &fileInput, const QString &fileResult)
{
    App app;
    if(!app.initNoWindow() ) {
        return false;
    }
    MainWindow mainWindow(false, qApp, app.data());

    mainWindow.loadFile(fileInput);

    QList<int> selection;
    selection.append(1);
    selection.append(0);
    selection.append(1);
    selection.append(1);
    Element *elementToTransform = mainWindow.getRegola()->findElementByArray(selection);
    FakeUIDelegate uiDelegate;

    if(!mainWindow.getRegola()->generateFromComment(mainWindow.getMainTreeWidget(), &uiDelegate, elementToTransform)) {
        return false;
    }
    QByteArray resultData = mainWindow.getRegola()->writeMemory();
    QDomDocument document1;
    QDomDocument document2;
    CompareXML compare;
    if(!compare.loadFileIntoDocument(fileResult, document1)) {
        return false;
    }
    QBuffer outputData(&resultData);
    if(!compare.loadFileIntoDocument(&outputData, document2)) {
        return false;
    }
    bool result = compare.compareDomDocuments(document1, document2);

    return result ;
}

bool TestComment::testUndoUndoableComment()
{
    App app;
    if(!app.initNoWindow() ) {
        return false;
    }
    MainWindow mainWindow(false, qApp, app.data());

    mainWindow.loadFile(QString(TEST_FILE_COMMENT_UNDOABLE));

    QList<int> selection;
    selection.append(1);
    selection.append(0);
    selection.append(1);
    selection.append(1);
    Element *elementToTransform = mainWindow.getRegola()->findElementByArray(selection);
    FakeUIDelegate uiDelegate;

    if(!mainWindow.getRegola()->generateFromComment(mainWindow.getMainTreeWidget(), &uiDelegate, elementToTransform)) {
        return false;
    }

    return true ;
}
