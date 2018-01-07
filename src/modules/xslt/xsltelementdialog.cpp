/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2013-2018 by Luca Bellonda and individual contributors  *
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

#include "xsltelementdialog.h"
#include "ui_xsltelementdialog.h"
#include "utils.h"
#include "modules/xslt/xsltmode.h"
#include "modules/xslt/xslthelper.h"
#include "modules/copyattr/copiedattribute.h"
#include "xmlutils.h"
#ifdef QXMLEDIT_TEST
#include "../test/testhelpers/XsltElementDialogprivatetest.h"
#endif


//-------------------------

XsltElementDialogParam::XsltElementDialogParam()
{
    selectedElement = NULL ;
    xsltElement = NULL ;
    element = NULL ;
    baseElement = NULL ;
    helper = NULL ;
    path = NULL ;
    parent = NULL ;
    useParams = false;
}

XsltElementDialogParam::~XsltElementDialogParam()
{

}

//-------------------------

CopyAttributesSession *XsltElementDialog::dialogProperties(XsltElement *xsltElement, Element *element, Element *baseElement, XsltHelper *helper, QStringList *path, bool *useParam, QWidget *parent)
{
    CopyAttributesSession *ret = NULL ;
    XsltElementDialog dlg(xsltElement, element, baseElement, helper, path, parent);
    if(dlg.exec() == QDialog::Accepted) {
        ret = dlg.getAndClearResult();
        *useParam = dlg.useParam();
    } else {
        *useParam = false;
    }
    return ret;
}

CopyAttributesSession *XsltElementDialog::dialogProperties(XsltElementDialogParam *params)
{
    return dialogProperties(params->xsltElement, params->element, params->baseElement, params->helper, params->path, &params->useParams, params->parent);
}

// base is the element before the reference, if not the sibling, then the father.
// we cannot derive it from the element, because of insert operations
XsltElementDialog::XsltElementDialog(XsltElement *xsltElement, Element *element, Element *baseElement, XsltHelper *helper, QStringList *path, QWidget *parent) :
    QDialog(parent),
#ifdef QXMLEDIT_TEST
    d(new PrivateTest(this)),
#endif
    ui(new Ui::XsltElementDialog)
{
    _useParam = false;
    _hasNames = false;
    _result = NULL ;
    _helper = helper;
    _baseElement = baseElement;
    _endedWithOk = false;
    _isStarted = false ;
    ui->setupUi(this);
    _xsltElement = xsltElement ;
    _element = element ;
    _path = path;
    _isTemplate = false ;
    setTarget();
#ifdef QXMLEDIT_TEST
    QTimer::singleShot(50, d, SLOT(testStart()));
#endif
}

XsltElementDialog::~XsltElementDialog()
{
    if(NULL != _result) {
        delete _result ;
    }
    delete ui;
#ifdef QXMLEDIT_TEST
    delete d;
#endif
}

void XsltElementDialog::enableOK()
{
    bool isEnabled = true ;
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(isEnabled);
}

void XsltElementDialog::setTarget()
{
    QString path = _path->join("/");
    path = "/" + path ;
    ui->pathLabel->setText(path);
    ui->tagName->setText(_element->tag());
    bool nameEnabled = false;
    QString xName, xPrefix;
    XmlUtils::decodeQualifiedName(_element->tag(), xPrefix, xName);
    if("template" != xName) {
        ui->modeAttr->setVisible(false);
        ui->modeLabel->setVisible(false);
    } else {
        _isTemplate = true ;
        QString mode = _element->getAttributeValue("mode");
        ui->modeAttr->setText(mode);
    }
    if(!_xsltElement->nameAttributeName.isEmpty()) {
        if(XsltCompletionType::TemplateNames == _xsltElement->completionType) {
            ui->labelComboName->setText(_xsltElement->nameAttributeName);
            ui->comboName->setEnabled(true);
        }
        Attribute *attrib = _element->getAttribute(_xsltElement->nameAttributeName);
        if(NULL != attrib) {
            ui->comboName->setEditText(attrib->value);
        }
        ui->comboName->setFocus();
        nameEnabled = true;
    } else {
        ui->comboName->setEnabled(false);
    }
    if(!_xsltElement->valueAttributeName.isEmpty()) {
        ui->labelSelect->setText(_xsltElement->valueAttributeName);
        ui->selectAttr->setEnabled(true);
        Attribute *attrib = _element->getAttribute(_xsltElement->valueAttributeName);
        if(NULL != attrib) {
            ui->selectAttr->setText(attrib->value);
        }
        if(!nameEnabled) {
            ui->selectAttr->setFocus();
        }
    } else {
        ui->selectAttr->setEnabled(false);
    }

    if(!((_xsltElement->childrenClasses.contains("w")) || (_xsltElement->childrenClasses.contains("c")))) {
        ui->cmdInsParam->setEnabled(false);
    }
    setDataForCompletion();
    enableOK();
    _isStarted = true ;
}


void XsltElementDialog::setDataForCompletion()
{
    QSet<QString> newData;
    // find the relevant names : templates, all the xsl:parent so far+element names
    // all the parent, siblings of the parent and all the root children xsl that have the name attribute

    //1- templates, child of root.
    if(XsltCompletionType::TemplateNames == _xsltElement->completionType) {
        newData = _helper->templateNames();
        foreach(QString str, newData.values()) {
            ui->comboName->addItem(str);
        }
    } else {
        if(XsltCompletionType::ParameterNames == _xsltElement->completionType) {
            Element * tmpl = _helper->getTemplateParent(_baseElement);
            if(NULL != tmpl) {
                QString templateName = _helper->getXslName(tmpl);
                newData = _helper->parameterNames(templateName);
                foreach(QString str, newData.values()) {
                    ui->comboName->addItem(str);
                }
            }
        }
    }
    newData = _helper->allNames(_baseElement);
    ui->selectAttr->setData(&newData);
    if(_helper->hasAutoCompletionNames()) {
        _hasNames = true ;
    }
    ui->lblWarningNames->setVisible(!_hasNames);
}

bool XsltElementDialog::useParam()
{
    return _useParam;
}

void XsltElementDialog::fillAttributes()
{
    if(_result) {
        delete _result ;
        _result = NULL ;
    }
    _useParam = ui->cmdInsParam->isChecked();
    CopyAttributesSession* cas = new CopyAttributesSession();
    if(NULL != cas) {
        QList<Attribute *> resList;
        if(!_xsltElement->nameAttributeName.isEmpty()) {
            Attribute *attribute = new Attribute();
            attribute->name = _xsltElement->nameAttributeName;
            attribute->value = ui->comboName->currentText();
            resList.append(attribute);
        }
        if(!_xsltElement->valueAttributeName.isEmpty()) {
            Attribute *attribute = new Attribute();
            attribute->name = _xsltElement->valueAttributeName;
            attribute->value = ui->selectAttr->text();
            resList.append(attribute);
        }
        if(_isTemplate) {
            QString mode = ui->modeAttr->text().trimmed();
            Attribute *attribute = new Attribute();
            attribute->name = "mode";
            attribute->value = ui->modeAttr->text().trimmed();
            resList.append(attribute);
        }
        cas->setAttributes(resList);
        _result = cas ;
    } else {
        Utils::errorOutOfMem(this);
    }
}

void XsltElementDialog::accept()
{
    fillAttributes();
    QDialog::accept();
}

CopyAttributesSession* XsltElementDialog::getAndClearResult()
{
    CopyAttributesSession* ret = _result ;
    _result = NULL ;
    return ret ;
}
