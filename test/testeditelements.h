/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2012-2018 by Luca Bellonda and individual contributors  *
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


#ifndef TESTEDITELEMENTS_H
#define TESTEDITELEMENTS_H

#include "app.h"
#include "regola.h"
#include "testbase.h"
#include "xmleditwidgetprivate.h"

class NamespaceCommands;

class TestEditElements : public TestBase
{
    App app;
    Element *selectedElement ;
    Regola *regola ;

    static bool error1();
    bool start(const bool isNew, QList<int> &selection);
    bool compare(const QString &referenceFileName);
    bool compareDocuments(const QString &filename, Regola *regola);
    Element *newProcessingInstruction();
    Element *createElement();

    bool insertCommentEmpty();
    bool insertCommentAtRootNoSel();
    bool insertCommentUnderRoot();
    bool appendCommentToRoot();
    bool insertCommentAsChild();
    bool appendCommentAsSibling();
    bool insertProcessingInstructionEmpty();
    bool insertProcessingInstructionAtRootNoSel();
    bool insertProcessingInstructionUnderRoot();
    bool appendProcessingInstructionToRoot();
    bool insertProcessingInstructionAsChild();
    bool appendProcessingInstructionAsSibling();

    bool insertElementEmpty();
    bool insertElementAtRootNoSel();
    bool insertElementUnderRoot();
    bool insertElementAsChild();
    bool appendElementAsSibling();
    bool insertElementAsChildWithAttributes();
    bool appendElementAsSiblingWithAttributes();
    bool insertElementAsChildWithAttributesAndText();
    bool appendElementAsSiblingWithAttributesAndText();
    //
    bool testNamespaces();
    //
    bool tagXtoX0();
    bool tagAXtoX1();
    bool tagXtoAX2();
    bool tagAXtoBX3();
    bool tagXtoX4();
    bool tagAXtoX5();
    bool tagXtoAX6();
    bool tagAXtoBX7();
    bool tagInsNsAltri();
    bool tagRemoveNsAltri();
    bool tagNsInsModDel();
    bool tagNsChangeNs();
    bool tagNsNoMod();
    //
    NamespaceCommands* makeOperation(const bool useNs, const QString &prefix, const QString &uri);
    NamespaceCommands* makeOperationOther(const bool useNs, const QString &prefix, const QString &uri, const QString &otherNs);
    NamespaceCommands* makeOperationDeclare(const bool useNs, const QString &prefix, const QString &uri);
    bool doCheckElementNamespaceOp(Element *sourceElement, Element *resultElement, NamespaceCommands *cmd );
    //
    bool testLoadElm(const QString &fileStart, const QString& expected, QList<int> &sel, const bool isInsert );
    bool testLoadElmText(const QString &fileStart, const QString& fileToCompare,
                                           QList<int> &sel,
                                           const QString& newValue,
                                           const bool expectedResult,
                                           const QString& expectedTag, const QString& expectedAttributes );
    bool verifyTestActivationEdit(XmlEditWidgetPrivate *target, App &app, const bool baseEditModeForm, const bool isNormalMode, const XmlEditWidgetPrivate::EEditMode expected );
    bool testActivationEdit();
public:
    TestEditElements();
    virtual ~TestEditElements();

    static bool test();
    bool testFast();
    bool testUnit();
    bool testInsertElement();
    bool testEditText();
};

#endif // TESTEDITELEMENTS_H
