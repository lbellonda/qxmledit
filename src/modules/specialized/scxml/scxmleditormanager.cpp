/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2016 by Luca Bellonda and individual contributors       *
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

#include "scxmleditormanager.h"
#include "regola.h"
#include "undo/elinsertcommand.h"
#include "xmlutils.h"
#include "modules/namespace/nscontext.h"
#include "utils.h"

SCXMLEditorManager::SCXMLEditorManager()
{
    _inited = false;
}

SCXMLEditorManager::~SCXMLEditorManager()
{
}

//bool SCXMLEditorManager::handleEdit(QWidget *parent, QTreeWidget * tree, Regola *regola, Element *element)
bool SCXMLEditorManager::handleEdit(QWidget *, QTreeWidget *, Regola *, Element *)
{
    init();
    /*QXName qName ;
    element->qName(&qName);*/
    Utils::TODO_THIS_RELEASE("fare");
    /*if((qName.ns == NamespaceManager::XIncludeNamespace) && (qName.name == XINCLUDE_TAG)) {
        Element *newElement = new Element(NULL);
        element->copyTo(*newElement, false);
        XIncludeDialog dlg(parent, newElement);
        if(dlg.exec() == QDialog::Accepted) {
            if(!regola->editElementWrapper(tree, newElement, element)) {
                Utils::error(parent, QObject::tr("Error applying the editing."));
                delete newElement ;
            }
        }
        return true;
    }*/
    return false;
}

bool SCXMLEditorManager::handleInsert(QTreeWidget *tree, Regola *regola, Element *element, const bool isChild, const QString & itemCode)
{
    Utils::TODO_THIS_RELEASE("fare");
    init();
    Element *newElement = new Element(NULL);
    bool goAhead = false;
    SCXMLToken *token = _tokenMakager.tokenForName(itemCode);
    if(NULL == token) {
        return false;
    }
    Utils::TODO_THIS_RELEASE("if(token->editToken(tree->window(), newElement, element))"); {
        //goAhead = true ;
    }
    if(goAhead) {
        QList<int> destPath = element->indexPathOfNewRelative(isChild);
        ElInsertCommand *cmd = new ElInsertCommand(tree, regola, newElement, destPath);
        regola->addUndo(cmd);
        return true ;
    }
    delete newElement ;
    return false ;
}

HandlerForInsert *SCXMLEditorManager::handlerForInsert(Regola *, Element *element, const bool isChild)
{
    init();
    Element *theParent = element ;
    if(!isChild) {
        theParent = element->parent();
    }
    QList<SCXMLTokenChild*> tokens ;
    bool parentIsSCXML = false;
    QList<NSContext*> contexts;
    NSContext *lastContext = NULL;

    if(NULL != theParent) {
        lastContext = Regola::buildContextInfo(contexts, theParent);
        QXName qName ;
        element->qName(&qName);
        if(qName.ns == NamespaceManager::SCXMLNamespace) {
            tokens = _tokenMakager.tokensForParentAsList(qName.name, theParent->getChildItems(), lastContext);
            parentIsSCXML = true;
        }
    }
    if(!parentIsSCXML) {
        QVector<Element*> emptyList;
        tokens = _tokenMakager.tokensForParentAsList("", &emptyList, NULL);
    }
    EMPTYPTRLIST(contexts, NSContext);

    const QString SCXML = QObject::tr("SCXML 1.1 state machine");

    HandlerForInsert *category = new HandlerForInsert();
    category->handler = this;
    category->nameSpace = NamespaceManager::XIncludeNamespace ;
    category->name = SCXML ;
    foreach(SCXMLTokenChild *tokenChild, tokens) {
        SingleHandlerForInsert *s = new SingleHandlerForInsert();
        s->name = tokenChild->name();
        s->id = tokenChild->name();
        s->description = tokenChild->description();
        category->elements.append(s);
    }
    return category ;
}

void SCXMLEditorManager::init()
{
    if(_inited) {
        return ;
    }
    _inited = true;
    _tokenMakager.init();
    Utils::TODO_THIS_RELEASE("fare");
}
