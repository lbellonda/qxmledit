/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2013 by Luca Bellonda and individual contributors       *
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


#ifndef XSLTELEMENT_H
#define XSLTELEMENT_H

#include "xmlEdit.h"
#include "modules/xslt/xsltmode.h"
#include "regola.h"

class XsltElement
{

public:
    QString _tag;
    int cardinality ;
    bool use ;
    QString thisClass;
    QStringList childrenClasses;
    XsltPosition::EPosition position;
    QString nameAttributeName;
    QString valueAttributeName;

    XsltCompletionType::ECompletion completionType;
    bool isClearAttributes;


    /**
     * @brief isOpenContext
     *if the item is opening a context or not
     */
    bool isOpenContext;

    XsltInsertable::EInsertMode _insertMode;

public:
    XsltElement(QString newTag);
    ~XsltElement();

    void setInsertMode(XsltInsertable::EInsertMode newMode);
    QString tagName();

    bool isInsertAtTop();
    bool isInsertAnywhere();
    bool isInsertableAsChild(XslContext *context, QMap<QString, XsltElement*> &elementsByTag);
    bool isAppendable(XslContext *context, QMap<QString, XsltElement*> &elementsByTag);


private:
    XsltElement* findXslParent(XslLevel *level, QMap<QString, XsltElement*> &elementsByTag);
    bool canInsertChild(XsltElement *newChild);
};

#endif // XSLTELEMENT_H

