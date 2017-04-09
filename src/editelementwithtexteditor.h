/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2017 by Luca Bellonda and individual contributors       *
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

#ifndef EDITELEMENTWITHTEXTEDITOR_H
#define EDITELEMENTWITHTEXTEDITOR_H

#include "xmlEdit.h"
#include "element.h"
#include "modules/xml/xmlsyntaxh.h"

namespace Ui
{
class EditElementWithTextEditor;
}

class EditElementWithTextEditor : public QDialog
{
    Q_OBJECT
    Element *_element;
    XMLSyntaxH *_syntaxHiglighter;

    void setData(Element *element, Regola *regola);
    bool makeItAccectped();
public:
    explicit EditElementWithTextEditor(QWidget *parent, Element *element, Regola *regola);
    ~EditElementWithTextEditor();

    virtual void accept();
#ifdef  QXMLEDIT_TEST
    friend class TestEditElements;
    friend class TestEditElements_EditHook ;
#endif

private:
    Ui::EditElementWithTextEditor *ui;
};

#endif // EDITELEMENTWITHTEXTEDITOR_H
