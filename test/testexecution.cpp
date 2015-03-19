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
#include "testexecution.h"
#include "comparexml.h"


TestExecution::TestExecution(QObject *parent) :
    QObject(parent)
{
    _showXML = false ;
}

bool TestExecution::execTest(const QString &inputFileName, const QString &outputFileName, TestExecutionCustomization &transformer, const int operationType)
{
    App app;
    if(!app.initNoWindow() ) {
        return false;
    }
    MainWindow mainWindow(false, qApp, app.data());

    mainWindow.loadFile(inputFileName);

    QList<int> selection = transformer.getSelection(operationType);
    Element *elementToTransform = mainWindow.getRegola()->findElementByArray(selection);

    if(NULL == elementToTransform) {
        fprintf(stderr, "%s\n", QString("Test:%1 No selected element").arg(operationType).toLatin1().data());
        return false;
    }
    QTreeWidget *tree = mainWindow.getMainTreeWidget();
    tree->setCurrentItem(elementToTransform->getUI());
    if(!transformer.execTest(&mainWindow, mainWindow.getRegola(), elementToTransform, operationType)) {
        fprintf(stderr, "%s\n", QString("Test:%1 Exec failed").arg(operationType).toLatin1().data());
        return false ;
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

