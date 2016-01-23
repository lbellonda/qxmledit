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

#include "xmlattributesindentationhelper.h"

XMLAttributesIndentationHelper::XMLAttributesIndentationHelper()
{
    _noIndent = NULL ;
    _indentCol = NULL ;
    _charsSpinBox = NULL ;
}

XMLAttributesIndentationHelper::~XMLAttributesIndentationHelper()
{
}

void XMLAttributesIndentationHelper::init(QRadioButton *noIndent, QRadioButton *indentCol, QSpinBox *charsSpinBox)
{
    _noIndent = noIndent;
    _indentCol = indentCol ;
    _charsSpinBox = charsSpinBox;
}

void XMLAttributesIndentationHelper::setUp(const QXmlEditData::EIndentAttributes value, const int columnsValue)
{
    if(QXmlEditData::AttributesIndentationNone == value) {
        _noIndent->setChecked(true);
    } else {
        _indentCol->setChecked(true);
    }
    _charsSpinBox->setValue(columnsValue);
    onSelection();
}

void XMLAttributesIndentationHelper::doPredefined()
{
    _noIndent->setChecked(true);
    onSelection();
}

void XMLAttributesIndentationHelper::onSelection()
{
    _charsSpinBox->setEnabled(_indentCol->isChecked());
}

QXmlEditData::EIndentAttributes XMLAttributesIndentationHelper::type()
{
    return _noIndent->isChecked() ? QXmlEditData::AttributesIndentationNone : QXmlEditData::AttributesIndentationMaxCols ;
}

int XMLAttributesIndentationHelper::columns()
{
    return _charsSpinBox->value();
}
