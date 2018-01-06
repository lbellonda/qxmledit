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


#ifndef XSINGLEELEMENTCONTENT_H
#define XSINGLEELEMENTCONTENT_H

#include <QList>
#include "xsdeditor/validator/schemasearchcontext.h"

class XSchemaObject ;

class XSingleElementContent
{
    static int instances ;

    int _id;
    XSchemaObject *_item;

    //---------- search
    int     _occurrences;
    //bool    _exploring ;

    /**
      \brief not only choices, but for example sequence items
    */
    QList<XSingleElementContent*> _children;
    QList<XSingleElementContent*> _attributes;
    int _minOccurs;
    int _maxOccurs;
    //bool _isSelectable; // to use as the father of a group

    void reset();
    void calculateOccurrences(XSchemaObject *object);
    void schemaSearchInit();

    SchemaSearchContext::ESearchState evaluateMatch(SchemaSearchContext * context, const bool isInAll);
    SchemaSearchContext::ESearchState scanForElement(SchemaSearchContext * context, const bool isAll);
    SchemaSearchContext::ESearchState handleOk(SchemaSearchContext * context);
    SchemaSearchContext::ESearchState handleNOk(SchemaSearchContext * context);

public:
    XSingleElementContent(XSchemaObject *newItem);
    ~XSingleElementContent();
    XSchemaObject *item();
    XSingleElementContent *addAChild(XSchemaObject *object);
    bool isChoice();
    bool isAll();
    QList<XSingleElementContent*> &children();
    int id();

    void dump(const int indent);

    //--------------- search related items

    SchemaSearchContext::ESearchState scanForPosition(SchemaSearchContext * context, const bool isInAll);
    SchemaSearchContext::ESearchState evalTerminalCondition();
    void resetOccurrencesRecursive(const bool resetSelf);
    void resetOccurrences();
    bool isOptional();
    bool isTerminal() ;
    bool isCollectable();

};

#endif // XSINGLEELEMENTCONTENT_H
