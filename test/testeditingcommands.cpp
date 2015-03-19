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

#include "testeditingcommands.h"
#include "fakeuidelegate.h"

#define INPUT_M1    "../test/data/editing_commands/append_input.xml"
#define OUTPUT_M1_1    "../test/data/editing_commands/append_output.xml"

TestEditingCommandsAppend::TestEditingCommandsAppend()
{
    ;
}

QList<int> TestEditingCommandsAppend::getSelection(const int /*operationType*/)
{
    QList<int> selection;
    selection.append(1);
    selection.append(0);
    selection.append(1);
    return selection;
}

bool TestEditingCommandsAppend::execTest(MainWindow *mainWindow, Regola *regola, Element * /*selectedElement*/, const int operationType)
{
    Element *newElement = new Element("newElement", "", regola, NULL);
    if(NULL == newElement) {
        fprintf(stderr, "TestEditingCommandsAppend::execTest: op:%d", operationType);
        return false;
    }
    regola->addBrother(mainWindow, mainWindow->getMainTreeWidget(), newElement);
    return true;
}


//-----------------------------------------------------------------------------------------------------------

TestEditingCommands::TestEditingCommands()
{
}


bool TestEditingCommands::testInnerAppendElement()
{
    TestEditingCommandsAppend testAppend;
    TestExecution execution;
    return execution.execTest(INPUT_M1, OUTPUT_M1_1, testAppend, 0);
}

