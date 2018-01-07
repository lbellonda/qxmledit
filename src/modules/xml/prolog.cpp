/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2014-2018 by Luca Bellonda and individual contributors  *
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

#include "regoladefinitions.h"
#include "modules/metadata/prologparser.h"

/********************************************************************************

Rules for encoding:
    the only data are inside the prolog, if any
    no data -> default encoding is used (utf-8 so far)

When choosing an encoding, the document is modified
 if a prolog exists, it is modified.
 if no prolog, the user is asked about inserting one.
        If the answer is negative, the new encoding is not applied.

When saving, the prolog is scanned to read the encoding.
 Whatever encoding is found, it is applied on the output data.

Modifiying the data in the editor, the prolog is checked to scan for new ecoding settings.

If a document is read only, the encoding can not be changed.

********************************************************************************/

const QString Regola::DefaultEncoding("UTF-8");

Element *Regola::prolog()
{
    // 2.1 Well-Formed XML Documents
    // document ::= prolog element Misc*
    // decl is the first
    if(childItems.size() > 0) {
        Element *first = childItems.at(0);
        PrologParser parser;
        if(parser.isProlog(first)) {
            return first;
        }
    }
    return NULL ;
}

bool Regola::hasProlog()
{
    Element *theProlog = prolog();
    if(NULL != theProlog) {
        return true;
    }
    return false;
}

/**
 * @brief Regola::insertProlog inserts XML declarations if missing without worry about undo
 */
bool Regola::insertProlog(QTreeWidget *tree, const QString &encoding, const bool undoableOperation)
{
    if(!hasProlog()) {
        Element *newProlog = insertPrologData(encoding);
        newProlog->markEdited();
        newProlog->caricaFigli(tree, NULL, paintInfo, true, 0);
        afterInsertHousekeeping(newProlog, tree, undoableOperation);
        checkEncoding();
        return true;
    }
    return false;
}

QString Regola::makePrologData(const QString &encoding)
{
    QString theEncoding = encoding;
    if(theEncoding.isEmpty()) {
        theEncoding = "UTF-8" ;
    }
    return QString(" version=\"1.0\" encoding=\"%1\"").arg(theEncoding);
}

Element *Regola::insertPrologData(const QString &encoding)
{
    QString newData = makePrologData(encoding);
    Element *newProlog = new Element(this, Element::ET_PROCESSING_INSTRUCTION, NULL);
    newProlog->setPITarget(XML_TARGET);
    newProlog->setPIData(newData);
    childItems.insert(0, newProlog);
    newProlog->markEdited();
    return newProlog;
}

QString Regola::encoding()
{
    QString theEncoding = "" ;
    if(childItems.size() > 0) {
        XmlProlog prolog;
        Element *first = childItems.at(0);
        PrologParser parser;
        if(parser.parse(first, &prolog)) {
            theEncoding = prolog.encoding();
        }
    }
    if(theEncoding.isEmpty()) {
        theEncoding = DefaultEncoding;
    }
    return theEncoding;
}

QString Regola::originalEncoding()
{
    return _originalEncoding ;
}

void Regola::checkEncoding(const bool isFirstTime)
{
    QString theEncoding = encoding();
    if(isFirstTime) {
        _originalEncoding = theEncoding ;
    }
    emit encodingChanged(theEncoding);
}

void Regola::checkEncodingByEl(Element *element)
{
    if((NULL == element) || (NULL != element->parent())) {
        return ;
    }
    return checkEncoding();
}

/**
 * @brief Regola::checkEncoding
 *
 * checks the encoding and compare it with the set encoding.
 * If it is different, it updates the set encoding and write it
 */
bool Regola::setEncoding(QTreeWidget *tree, UIDelegate *uiDelegate, const QString &newEncoding)
{
    if(!hasProlog()) {
        if(!uiDelegate->askYN(tr("The element is missing a prolog. In order to change the encoding a prolog must be inserted. Do you want to continue?"))) {
            return false;
        }
        Element *newProlog = insertPrologData(newEncoding);
        newProlog->markEdited();
        newProlog->caricaFigli(tree, NULL, paintInfo, true, 0);
        afterInsertHousekeeping(newProlog, tree, true);
        _forceDOM = false;
        emit encodingChanged(newEncoding);
        return true;
    } else {
        QString theEncoding = "" ;
        if(childItems.size() > 0) {
            XmlProlog prolog;
            Element *first = childItems.at(0);
            PrologParser parser;
            if(parser.parse(first, &prolog)) {
                theEncoding = prolog.encoding();
                if(theEncoding !=  newEncoding) {
                    prolog.setEncoding(newEncoding);
                    UndoEditCommand *undoCommand = new UndoEditCommand(tree, this, first->indexPath());
                    undoCommand->setOriginalElement(first);
                    QString newData = prolog.generateData();
                    first->setPIData(newData);
                    first->display(first->getUI(), paintInfo);
                    first->markEdited();
                    undoCommand->setModifiedElement(first);
                    _undoStack.push(undoCommand);
                    _forceDOM = false;
                    emit encodingChanged(newEncoding);
                    return true;
                }
            }
        }
    }
    return false;
}


void Regola::addUndo(QUndoCommand *undoGroup)
{
    _undoStack.push(undoGroup);
    setModified(true);
}
