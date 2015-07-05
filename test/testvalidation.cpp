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

#include <QTest>
#include "app.h"
#include "testvalidation.h"
#include "fakeuidelegate.h"
#include "../src/mainwindow.h"
#include "../src/config.h"
#include "../src/xmleditwidgetprivate.h"
#include "../src/xsdeditor/validator/xelementcontent.h"

#define VALIDATION_FILE_IN_0    "../test/data/xsd/test0.xml"
#define VALIDATION_RESULT_0    "../test/data/xsd/result0.xml"
#define VALIDATION_XSD_0    "../test/data/xsd/test.xsd"


TestValidation::TestValidation()
{
}


bool TestValidation::test()
{
    App app;
    if(!app.initNoWindow() ) {
        return error("init");
    }
    MainWindow mainWindow(false, qApp, app.data());
    XmlEditWidget *editor = mainWindow.getEditor();
    mainWindow.loadFile(VALIDATION_FILE_IN_0);
    Regola *regola = mainWindow.getRegola();
    if(NULL == regola) {
        return error("Null rule");
    }
    //FakeNetworkAccessManager mockNetworkManager(FILE_TEST_LOAD_SIMPLE);
    editor->loadSchema(VALIDATION_XSD_0);

    bool found = false;
    int index = 10;
    do {
        if(editor->schema() != NULL) {
            found = true ;
            break;
        }
        QTest::qWait(250);
        index -- ;
    } while(index >= 0);

    if(!found) {
        return error(QString("Schema not loaded '%1'").arg(VALIDATION_XSD_0));
    }

    // select the position
    QList<int> selection;
    selection.append(0);
    selection.append(2);

    Element *element = mainWindow.getRegola()->findElementByArray(selection);

    //Controlla con il file campione, nota che gli elementi non sono inseriti, ma appartengono ad una lista.
    bool startAsRoot ;
    XElementContent content;
    if(! editor->getPrivate()->findAllowedItemsElement(&content, element, startAsRoot)) {
        return error(QString("findAllowedItemsElement bad result"));
    }
    if(content.allowedItems().size() != 1) {
        return error("findAllowedItemsElement bad size");
    }
    XSingleElementContent * targetContent = content.allowedItems().at(0);
    if(targetContent->item()->name() != "plane") {
        return error("findAllowedItemsElement bad element");
    }
    return true ;
}
