/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2011-2018 by Luca Bellonda and individual contributors  *
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
#include "testmixedcontent.h"
#include "comparexml.h"
#include "qxmleditconfig.h"
#include "mainwindow.h"
#include "utils.h"
#include "fakeuidelegate.h"


#define INPUT_M1    "../test/data/mixed_content/m1/m1_input.xml"
#define OUTPUT_M1_1    "../test/data/mixed_content/m1/m1_out_1.xml"
#define OUTPUT_M1_2    "../test/data/mixed_content/m1/m1_out_2.xml"
#define OUTPUT_M1_3    "../test/data/mixed_content/m1/m1_out_3.xml"
#define OUTPUT_M1_4    "../test/data/mixed_content/m1/m1_out_4.xml"
#define OUTPUT_M1_5    "../test/data/mixed_content/m1/m1_out_5.xml"
#define OUTPUT_M1_6    "../test/data/mixed_content/m1/m1_out_6.xml"
#define OUTPUT_M1_7    "../test/data/mixed_content/m1/m1_out_7.xml"
#define OUTPUT_M1_8    "../test/data/mixed_content/m1/m1_out_8.xml"
#define OUTPUT_M1_9    "../test/data/mixed_content/m1/m1_out_9.xml"
#define OUTPUT_M1_10    "../test/data/mixed_content/m1/m1_out_10.xml"
#define OUTPUT_M1_11    "../test/data/mixed_content/m1/m1_out_11.xml"
#define OUTPUT_M1_12    "../test/data/mixed_content/m1/m1_out_12.xml"
#define OUTPUT_M1_13    "../test/data/mixed_content/m1/m1_out_13.xml"
#define OUTPUT_M1_14    "../test/data/mixed_content/m1/m1_out_14.xml"
#define OUTPUT_M1_15    "../test/data/mixed_content/m1/m1_out_15.xml"
#define OUTPUT_M1_16    "../test/data/mixed_content/m1/m1_out_16.xml"

#define INPUT_M3    "../test/data/mixed_content/m3/m3_input.xml"
#define OUTPUT_M3_1    "../test/data/mixed_content/m3/m3_out_1.xml"
#define OUTPUT_M3_2    "../test/data/mixed_content/m3/m3_out_2.xml"
#define OUTPUT_M3_3    "../test/data/mixed_content/m3/m3_out_3.xml"
#define OUTPUT_M3_4    "../test/data/mixed_content/m3/m3_out_4.xml"
#define OUTPUT_M3_5    "../test/data/mixed_content/m3/m3_out_5.xml"

#define INPUT_M6    "../test/data/mixed_content/m6/m6_input.xml"
#define OUTPUT_M6_1    "../test/data/mixed_content/m6/m6_out_1.xml"
#define OUTPUT_M6_2    "../test/data/mixed_content/m6/m6_out_2.xml"
#define OUTPUT_M6_3    "../test/data/mixed_content/m6/m6_out_3.xml"
#define OUTPUT_M6_4    "../test/data/mixed_content/m6/m6_out_4.xml"
#define OUTPUT_M6_5    "../test/data/mixed_content/m6/m6_out_5.xml"


#define TEST_M1    "M1"
#define TEST_M3    "M3"
#define TEST_M6    "M6"


TestMixedContent::TestMixedContent()
{
    _showXML = false ;
}

bool TestMixedContent::test_m1()
{
    M1Case1Test m1;
    if(! internalTest(TEST_M1, INPUT_M1, INPUT_M1, m1, 0)) {
        return false;
    }
    if(! internalTest(TEST_M1, INPUT_M1, OUTPUT_M1_1, m1, 1)) {
        return false;
    }
    if(! internalTest(TEST_M1, INPUT_M1, OUTPUT_M1_2, m1, 2)) {
        return false;
    }
    if(! internalTest(TEST_M1, INPUT_M1, OUTPUT_M1_3, m1, 3)) {
        return false;
    }
    if(! internalTest(TEST_M1, INPUT_M1, OUTPUT_M1_4, m1, 4)) {
        return false;
    }
    if(! internalTest(TEST_M1, INPUT_M1, OUTPUT_M1_5, m1, 5)) {
        return false;
    }
    if(! internalTest(TEST_M1, INPUT_M1, OUTPUT_M1_6, m1, 6)) {
        return false;
    }
    if(! internalTest(TEST_M1, INPUT_M1, OUTPUT_M1_7, m1, 7)) {
        return false;
    }
    if(! internalTest(TEST_M1, INPUT_M1, OUTPUT_M1_8, m1, 8)) {
        return false;
    }
    if(! internalTest(TEST_M1, INPUT_M1, OUTPUT_M1_9, m1, 9)) {
        return false;
    }
    if(! internalTest(TEST_M1, INPUT_M1, OUTPUT_M1_10, m1, 10)) {
        return false;
    }
    if(! internalTest(TEST_M1, INPUT_M1, OUTPUT_M1_11, m1, 11)) {
        return false;
    }
    if(! internalTest(TEST_M1, INPUT_M1, OUTPUT_M1_12, m1, 12)) {
        return false;
    }
    if(! internalTest(TEST_M1, INPUT_M1, OUTPUT_M1_13, m1, 13)) {
        return false;
    }
    if(! internalTest(TEST_M1, INPUT_M1, OUTPUT_M1_14, m1, 14)) {
        return false;
    }
    if(! internalTest(TEST_M1, INPUT_M1, OUTPUT_M1_15, m1, 15)) {
        return false;
    }
    if(! internalTest(TEST_M1, INPUT_M1, OUTPUT_M1_16, m1, 16)) {
        return false;
    }
    return true ;
}

bool TestMixedContent::test_m3()
{
    M3Case1Test m3;
    if(! internalTest(TEST_M3, INPUT_M3, INPUT_M3, m3, 0)) {
        return false;
    }
    if(! internalTest(TEST_M3, INPUT_M3, OUTPUT_M3_1, m3, 1)) {
        return false;
    }
    if(! internalTest(TEST_M3, INPUT_M3, OUTPUT_M3_2, m3, 2)) {
        return false;
    }
    if(! internalTest(TEST_M3, INPUT_M3, OUTPUT_M3_3, m3, 3)) {
        return false;
    }
    if(! internalTest(TEST_M3, INPUT_M3, OUTPUT_M3_4, m3, 4)) {
        return false;
    }
    if(! internalTest(TEST_M3, INPUT_M3, OUTPUT_M3_5, m3, 5)) {
        return false;
    }
    return true ;
}

bool TestMixedContent::test_m6()
{
    M6Case1Test m6;
    if(! internalTest(TEST_M6, INPUT_M6, INPUT_M6, m6, 0)) {
        return false;
    }
    if(! internalTest(TEST_M6, INPUT_M6, OUTPUT_M6_1, m6, 1)) {
        return false;
    }
    if(! internalTest(TEST_M6, INPUT_M6, OUTPUT_M6_2, m6, 2)) {
        return false;
    }
    if(! internalTest(TEST_M6, INPUT_M6, OUTPUT_M6_3, m6, 3)) {
        return false;
    }
    if(! internalTest(TEST_M6, INPUT_M6, OUTPUT_M6_4, m6, 4)) {
        return false;
    }
    if(! internalTest(TEST_M6, INPUT_M6, OUTPUT_M6_5, m6, 5)) {
        return false;
    }
    return true ;
}



bool TestMixedContent::internalTest(const QString &testName, const QString &inputFileName, const QString &outputFileName, MCTestTransformer &transformer, const int operationType)
{
    if(! execTest(inputFileName, outputFileName, transformer, operationType)) {
        fprintf(stderr, "%s\n", QString("TestMixedContent: Test %1 failed, operation: %2 ").arg(testName).arg(operationType).toLatin1().data());
        return false;
    }
    return true ;
}


bool TestMixedContent::execTest(const QString &inputFileName, const QString &outputFileName, MCTestTransformer &transformer, const int operationType)
{
    App app;
    if(!app.initNoWindow() ) {
        return false;
    }
    MainWindow mainWindow(false, app.data());

    mainWindow.loadFile(inputFileName);

    QList<int> selection;
    selection.append(1);
    selection.append(0);
    selection.append(2);
    Element *elementToTransform = mainWindow.getRegola()->findElementByArray(selection);

    if(NULL == elementToTransform) {
        fprintf(stderr, "%s\n", QString("Test:%1 No selected element").arg(operationType).toLatin1().data());
        return false;
    }

    if(elementToTransform ->tag() != "text") {
        fprintf(stderr, "%s\n", QString("Test:%1 Unexpected tag: %2").arg(operationType).arg(elementToTransform->tag()).toLatin1().data());
        return false;
    }

    if(!transformer.preDialog(elementToTransform, operationType)) {
        fprintf(stderr, "%s\n", QString("Test:%1 preDialog").arg(operationType).toLatin1().data());
        return false;
    }

    EditElement editElementDialog(&mainWindow);
    editElementDialog.setTarget(elementToTransform) ;
    if(!transformer.exec(&editElementDialog, operationType)) {
        fprintf(stderr, "%s\n", QString("Test:%1 Exec failed").arg(operationType).toLatin1().data());
        return false ;
    }
    editElementDialog.accept();

    if(!transformer.postDialog(elementToTransform, operationType)) {
        fprintf(stderr, "%s\n", QString("Test:%1 postDialog").arg(operationType).toLatin1().data());
        return false;
    }

    QByteArray resultData = mainWindow.getRegola()->writeMemory();
    QDomDocument document1;
    QDomDocument document2;
    CompareXML compare;
    if(!compare.loadFileIntoDocument(outputFileName, document1)) {
        fprintf(stderr, "%s\n", (QString("Test:%1 loadFile %2").arg(operationType).arg(outputFileName)).toLatin1().data());
        return false;
    }
    if(_showXML) {
        QString res(resultData);
        Utils::message(res);
    }
    QBuffer outputData(&resultData);
    if(!compare.loadFileIntoDocument(&outputData, document2)) {
        fprintf(stderr, "%s\n", (QString("Test:%1 loadFile data").arg(operationType)).toLatin1().data());
        return false;
    }
    bool result = compare.compareDomDocuments(document1, document2);

    return result ;
}

/************* specialized test classes ***************************/

Element *MCTestTransformer::elementAt(Element *parent, const int index)
{
    Element *element = parent->getChildItems()->at(index);
    return element;
}

bool MCTestTransformer::deleteElement(Element *parent, const int index)
{
    Element *element = elementAt(parent, index);
    if(NULL == element) {
        return false ;
    }
    element->autoDelete(true);
    return true;
}

bool MCTestTransformer::updateElement(Element *parent, const int index, const QString &newTag)
{
    Element *element = elementAt(parent, index);
    if(NULL == element) {
        return false ;
    }
    element->_tag = newTag;
    return true;
}


void MCTestTransformer::selectAndDelete(EditElement *subject, const int index)
{
    subject->sendSelect(index);
    subject->sendDeleteCommand();
}

void MCTestTransformer::selectAndModify(EditElement *subject, const int index, const QString newData)
{
    subject->sendSelect(index);
    subject->sendModifyCommand(newData);
}

void MCTestTransformer::selectAndAdd(EditElement *subject, const int index, const QString newData)
{
    subject->sendSelect(index);
    subject->sendAddCommand(newData);
}

void MCTestTransformer::selectAndMoveUp(EditElement *subject, const int index)
{
    subject->sendSelect(index);
    subject->sendMoveUpCommand();
}

void MCTestTransformer::selectAndMoveDown(EditElement *subject, const int index)
{
    subject->sendSelect(index);
    subject->sendMoveDownCommand();
}

//-----M1

bool M1Case1Test::exec(EditElement *subject, const int operationType)
{
    switch(operationType) {
    case 0:
        return true;
    case 1:
        selectAndDelete(subject, 0);
        return true;
    case 2:
        return true ;
    case 3:
    case 5:
        selectAndDelete(subject, 2);
        return true;
    case 4:
        selectAndDelete(subject, 0);
        return true;
    case 6:
    case 7:
        selectAndDelete(subject, 2);
        selectAndDelete(subject, 0);
        return true;
    case 8:
        selectAndModify(subject, 0, "new data");
        return true;
    case 9:
        return true;
    case 10:
        selectAndModify(subject, 2, "new data");
        return true;
    case 11:
        selectAndModify(subject, 0, "new data 1");
        selectAndModify(subject, 2, "new data 2");
        return true;
    case 12:
        selectAndAdd(subject, 0, "new data 1");
        selectAndMoveUp(subject, 1);
        return true;
    case 13:
        selectAndAdd(subject, 0, "new data 1");
        return true;
    case 14:
        selectAndAdd(subject, 0, "new data 1");
        selectAndMoveDown(subject, 1);
        return true ;
    case 15:
        selectAndAdd(subject, 2, "new data 1");
        return true ;
    case 16:
        selectAndAdd(subject, 0, "new data 1");
        selectAndMoveUp(subject, 1);
        selectAndAdd(subject, 1, "new data 2");
        selectAndAdd(subject, 2, "new data 3");
        selectAndMoveDown(subject, 3);
        selectAndAdd(subject, 5, "new data 4");
        return true ;
    }
    return false;
}

bool M1Case1Test::preDialog(Element * /*element*/, const int operationType)
{
    switch(operationType) {
    default:
        return true;
    }
}

bool M1Case1Test::postDialog(Element *element, const int operationType)
{
    switch(operationType) {
    case 4:
    case 6:
        return deleteElement(element, 0);
    case 2:
    case 5:
        return deleteElement(element, 1);
    case 9:
        updateElement(element, 1, "newTag");
        return true ;
    default:
        return true;
    }
}

//---------------------M3

bool M3Case1Test::exec(EditElement *subject, const int operationType)
{
    switch(operationType) {
    case 0:
        return true;
    case 1:
        selectAndDelete(subject, 0);
        return true;
    case 2:
        selectAndModify(subject, 0, "new data 1");
        return true ;
    case 3:
        selectAndAdd(subject, 0, "new data 1");
        selectAndMoveDown(subject, 0);
        return true;
    case 4:
        selectAndAdd(subject, 0, "new data 1");
        return true;
    case 5:
        selectAndAdd(subject, 0, "new data 1");
        selectAndMoveUp(subject, 1);
        selectAndAdd(subject, 1, "new data 2");
        return true;
    }
    return false;
}

bool M3Case1Test::preDialog(Element * /*element*/, const int operationType)
{
    switch(operationType) {
    default:
        return true;
    }
}

bool M3Case1Test::postDialog(Element * /*element*/, const int operationType)
{
    switch(operationType) {
    default:
        return true;
    }
}

//---------------------M3

bool M6Case1Test::exec(EditElement *subject, const int operationType)
{
    switch(operationType) {
    case 0:
        return true;
    case 1:
        return true;
    case 2:
        return true ;
    case 3:
        selectAndAdd(subject, 0, "new data 1");
        selectAndMoveUp(subject, 1);
        return true;
    case 4:
        selectAndAdd(subject, 0, "new data 1");
        return true;
    case 5:
        selectAndAdd(subject, 0, "new data 1");
        selectAndMoveUp(subject, 1);
        selectAndAdd(subject, 0, "new data 2");
        selectAndMoveDown(subject, 1);
        return true;
    }
    return false;
}

bool M6Case1Test::preDialog(Element * element, const int operationType)
{
    switch(operationType) {
    case 1:
        return deleteElement(element, 0);
    default:
        return true;
    }
}

bool M6Case1Test::postDialog(Element *element, const int operationType)
{
    switch(operationType) {
    case 2:
        updateElement(element, 0, "newTag");
        // fallthrough
    default:
        return true;
    }
}
