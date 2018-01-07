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
#ifndef ELEMENTEDITOR_H
#define ELEMENTEDITOR_H

#include <QWidget>
#include "xsdeditor/xschema.h"
#include "xsdeditor/widgets/XEditorInterface.h"

namespace Ui
{
class ElementEditor;
};

class ElementEditor : public QWidget, public XEditorInterface
{
    Q_OBJECT
    Q_INTERFACES(XEditorInterface)

    XSchemaElement *_target;

public:
    explicit ElementEditor(QWidget *parent = 0);
    ~ElementEditor();

    void setTarget(XSchemaObject *newTarget);
    XSchemaObject *target() ;


    void setAbstract(const XEnums::XBool newValue);
    void setFixed(const QString &newValue);
    void setDefaultValue(const QString &newValue);
    void setNillable(const XEnums::XBool newValue);
    void setMaxOccurs(const int newValue);
    void setMinOccurs(const int newValue);
    void setBlock(const XSchemaObject::EFinalType newValue);
    void setFinal(const XSchemaObject::EFinalType newValue);
    void setType(const QString &newValue);
    void setRef(const QString &newValue);
    void setCategory(const XSchemaElement::ElementCategory newValue);

protected:
    void changeEvent(QEvent *e);

private:
    Ui::ElementEditor *ui;

    void applyPropertyChanged(const QString &propertyName, const bool isAll);
    void finalizeUI();

private slots:
    void onTargetPropertyChanged(const QString &propertyName);
    void on_chkAbstract_clicked();
    void on_chkNillable_clicked();
    void on_fixed_textChanged(const QString &text);
    void on_defaultValue_textChanged(const QString &text);
    void on_minOccurs_valueChanged(const int newValue);
    void on_maxOccurs_valueChanged(const int newValue);
    void on_cmbBlock_currentIndexChanged(const int newValue);
    void on_cmbFinal_currentIndexChanged(const int newValue);
    void on_type_editTextChanged(const QString & text);
    void on_cmbReference_editTextChanged(const QString & text);
    void on_cmbElementCategory_currentIndexChanged(const int newValue);
};

#endif // ELEMENTEDITOR_H
