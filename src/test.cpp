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

#include "test.h"
#include "modules/compare/compareengine.h"
#include "utils.h"

#define FILE_TEST ":/data/test1.xml"
#define FILE_TEST_FAIL ":/data/testf.xml"
#define FILE_TEST_RESULT ":/data/test2.xml"

void Test::executeTests()
{
    Test test;
    test.doTest();
}

Test::Test()
{
}

Test::~Test()
{
}

void Test::doTest()
{
    QFile file(FILE_TEST);
    if(file.open(QIODevice::ReadOnly)) {
        QDomDocument document;
        if(document.setContent(&file)) {
            Regola *newModel = new Regola(document, FILE_TEST, true);
            // test begin

            // 1- save as is
            if(!testSaveAs(newModel)) {
                Utils::error(tr("Test 1 failed"));
                return ;
            }
            // 2 - copy and paste (TODO)
            if(!testCopyAndPaste()) {
                Utils::error(tr("Test 2 failed"));
                return ;
            }

            // wrong test
            if(!testFailure(newModel)) {
                Utils::error(tr("Test F failed"));
                return ;
            }


            Utils::message(tr("Tests completed successfully."));
        } else {
            Utils::error(tr("Unable to parse XML"));
        }
        file.close();
    } else {
        Utils::error(QString(tr("Unable to load file.\n Error code is '%1'")).arg(file.error()));
    }
}

bool Test::testSaveAs(Regola *source)
{
    QByteArray array = source->writeMemory();
    if(array.isEmpty()) {
        return false;
    }
    CompareEngine compare;
    if(!compare.compareQuick(source, &array)) {
        return false ;
    }
    if(compare.areDifferent()) {
        return false ;
    } else {
        return true;
    }
}

bool Test::testFailure(Regola *source)
{
    CompareEngine compare;
    if(!compare.compareQuick(source, QString(FILE_TEST_FAIL))) {
        Utils::error(tr("Test F error loading file"));
        return false ;
    }
    if(compare.areDifferent()) {
        return true ;
    } else {
        return false;
    }
}

//TODO: test me
bool Test::testCopyAndPaste()
{
    // Operations on rule
    Regola *sourceRule = loadRule(FILE_TEST);
    if(NULL == sourceRule) {
        return false;
    }
    Regola *compareRule = loadRule(FILE_TEST_RESULT);
    if(NULL == compareRule) {
        delete sourceRule;
        return false;
    }
    bool isOk = false ;
    Element *rootElement = sourceRule->root();
    Element *copyElement = rootElement->getChildItems()->at(0);
    Element* toInsert = copyElement->copyToClipboard();
    // paste into nth child
    int nodi = rootElement->getChildItems()->size();
    if(nodi > 0) {
        Element *pasteTo = rootElement->getChildItems()->at(nodi - 1) ;
        sourceRule->pasteNoUI(toInsert, pasteTo);
        QByteArray array = sourceRule->writeMemory();
        if(!array.isEmpty()) {
            CompareEngine compare;
            if(compare.compareQuick(compareRule, &array)) {
                if(!compare.areDifferent()) {
                    isOk = true ;
                }
            }
        }
    }
    delete sourceRule;
    delete compareRule;
    return isOk;
}

Regola *Test::loadRule(const QString &filePath)
{
    Regola *retVal = NULL ;
    QFile file(filePath);
    if(file.open(QIODevice::ReadOnly)) {
        QDomDocument document;
        if(document.setContent(&file)) {
            retVal = new Regola(document, filePath, true);
        }
        file.close();
    }
    return retVal ;
}
