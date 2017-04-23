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

#include "editelementwithtexteditor.h"
#include "ui_editelementwithtexteditor.h"
#include "regola.h"
#include "utils.h"

EditElementWithTextEditor::EditElementWithTextEditor(QWidget *parent, Element *element, Regola *regola) :
    QDialog(parent),
    ui(new Ui::EditElementWithTextEditor)
{
    Utils::TODO_THIS_RELEASE("finire");
    Utils::TODO_THIS_RELEASE("serve un flag per gestire le entita' e i caratteri raw o meno amp quote");
    _syntaxHiglighter = NULL ;
    _element = element;
    ui->setupUi(this);
    setData(element, regola);
}

EditElementWithTextEditor::~EditElementWithTextEditor()
{
    delete ui;
}

void EditElementWithTextEditor::setData(Element *element, Regola *regola)
{
    ui->editor->setLineWrapMode(QPlainTextEdit::WidgetWidth);
    ui->editor->setWordWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
    QString value = element->getInTextualForm();
    int indexOfFirst = value.indexOf("<");
    if(indexOfFirst >= 0) {
        value = value.mid(indexOfFirst + 1);
    }
    int indexOfLast = value.lastIndexOf("/>");
    if(indexOfLast >= 0) {
        value = value.mid(0, indexOfLast);
    }
    ui->editor->setPlainText(value);
    PaintInfo *paintInfo = NULL ;
    if(NULL != regola) {
        paintInfo = regola->getPaintInfo();
    }
    _syntaxHiglighter = new XMLSyntaxH(ui->editor->document(), paintInfo);
}

void EditElementWithTextEditor::accept()
{
    if(makeItAccectped()) {
        QDialog::accept();
    }
}

bool EditElementWithTextEditor::makeItAccectped()
{
    QString text = "<root><";
    text += ui->editor->toPlainText();
    text += "/></root>";
    QList<Element*> result = Regola::decodeXMLFromString(text, false, true);
    if(result.isEmpty()) {
        Utils::error(this, tr("The text cannot be converted to a valid element (1)."));
        return false ;
    }
    if(result.size() > 2) {
        Utils::error(this, tr("The text cannot be converted to a valid element (2)."));
        EMPTYPTRLIST(result, Element);
        return false ;
    }
    Element *first = result.at(0);
    if(first->getChildItemsCount() > 0) {
        Utils::error(this, tr("The text cannot be converted to a valid element (3)."));
        EMPTYPTRLIST(result, Element);
        return false ;
    }
    _element->setTagAuto(first->tag());
    _element->clearAttributes();
    foreach(Attribute *attribute, first->getAttributesList()) {
        _element->addAttribute(attribute->name, attribute->value);
    }
    EMPTYPTRLIST(result, Element);
    _element->markEdited();
    return true;
}
