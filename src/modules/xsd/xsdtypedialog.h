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


#ifndef XSDTYPEDIALOG_H
#define XSDTYPEDIALOG_H

#include <QDialog>
#include "modules/xsd/xsdoperationparameters.h"
namespace Ui
{
class XSDTypeDialog;
}

class Element;

class XSDTypeDialog : public QDialog
{
    Q_OBJECT

    XSDOperationParameters _params;
    XSDOperationParameters::EObjectType _objectType;
    bool _started ;


    void updateFromData();
    void onUpdate();
    void checkEnable();
    void setPanelSimpleComplexType(const bool isComplexType, const bool isShowPanel);
    void setupSelection(Element *selection, const bool isInsert);
    XSDOperationParameters::EObjectType decodeElement(Element *inputElement);

public:
    explicit XSDTypeDialog(const bool isInsert, const XSDOperationParameters::EObjectType objectType, const QString &startName, Element *selection, QWidget *parent = 0);
    virtual ~XSDTypeDialog();

    XSDOperationParameters* getParameters();


protected:
    Ui::XSDTypeDialog *ui;

private slots:
    void on_typeRef_clicked(bool clicked);
    void on_typeComplex_clicked(bool clicked);
    void on_typeSimple_clicked(bool clicked);
    void on_contentSimple_clicked(bool clicked);
    void on_contentComplex_clicked(bool clicked);
    void on_cNone_clicked(bool clicked);
    void on_simpleTypeNone_clicked(bool clicked);
    void on_cExtension_clicked(bool clicked);
    void on_cRestriction_clicked(bool clicked);
    void on_cAll_clicked(bool clicked);
    void on_cGroup_clicked(bool clicked);
    void on_cSeq_clicked(bool clicked);
    void on_cChoice_clicked(bool clicked);
    void on_contentNone_clicked(bool clicked);
    void on_ename_textEdited(const QString & text);

    void on_reftype_editTextChanged(const QString & text);
    void on_simpleTypeUnion_clicked(bool clicked);
    void on_simpleTypeList_clicked(bool clicked);
    void on_simpleTypeRestriction_clicked(bool clicked);
    void on_simpleTypeBase_editTextChanged(const QString & text);
    void on_cBaseType_editTextChanged(const QString & text);
};

#endif // XSDTYPEDIALOG_H
