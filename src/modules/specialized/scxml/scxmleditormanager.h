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

#ifndef SCXMLEDITORMANAGER_H
#define SCXMLEDITORMANAGER_H

#include "modules/xsd/namespacemanager.h"
#include "modules/specialized/scxml/scxmltokenmanager.h"

class SCXMLState;
class NSContext ;

class SCXMLEditorManager : public NamespaceHandlerForEdit
{
    SCXMLTokenManager _tokenMakager;
    bool _inited;

    void init();
    bool findInfoStates(Element *element, SCXMLInfo *info, NSContext *parent, const bool rootFound, SCXMLState *currentState);
    SCXMLState *addStateForInfo(SCXMLInfo *info, SCXMLState *currentState, SCXMLState *state);

public:
    SCXMLEditorManager();
    virtual ~SCXMLEditorManager();

    virtual bool handleEdit(QWidget *parent, QTreeWidget *tree, Regola *regola, Element *element);
    virtual HandlerForInsert *handlerForInsert(Regola *regola, Element *element, const bool isChild) ;
    virtual bool handleInsert(QTreeWidget *tree, Regola *regola, Element *element, const bool isChild, const QString &itemCode) ;
    bool findInfoStates(Regola *regola, SCXMLInfo *info);
};

#endif // SCXMLEDITORMANAGER_H
