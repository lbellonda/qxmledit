/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2016-2018 by Luca Bellonda and individual contributors  *
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

#include "xincludedialog.h"
#include "ui_xincludedialog.h"
#include "modules/specialized/xinclude/xincludeeditormanager.h"
#include "element.h"
#include "utils.h"

static const QString XIncludeLNSDecl = "xmlns:xila" ;

static const QString AttrHREF = "href" ;
static const QString AttrPARSE  = "parse" ;
static const QString AttrXPOINTER  = "xpointer" ;
static const QString AttrFRAGID  = "fragid" ;
static const QString AttrSETXMLID  = "set-xml-id" ;
static const QString AttrENCODING  = "encoding" ;
static const QString AttrACCEPT  = "accept" ;
static const QString AttrACCEPTLANG  = "accept-language" ;

XIncludeAttr::XIncludeAttr()
{
    useSetXMLid = false ;
    declareLocalA = false;
}

XIncludeAttr::~XIncludeAttr()
{
}

//----

XIncludeDialog::XIncludeDialog(QWidget *parent, Element *element) :
    QDialog(parent),
    ui(new Ui::XIncludeDialog)
{
    _element = element;
    readAttributesFromElement();
    ui->setupUi(this);
    postSetupUI();
    setAttributesToUI();
}

XIncludeDialog::~XIncludeDialog()
{
    delete ui;
}

void XIncludeDialog::postSetupUI()
{
    QComboBox *parse = ui->parse;
    parse->clear();
    QStringList parseOpts ;
    parseOpts << "" << "xml" << "application/xml" << "text" << "text/plain" ;
    Utils::loadComboTextArrays(parse, "", parseOpts, parseOpts);
}

void XIncludeDialog::accept()
{
    if(checkValues()) {
        setValuesToElement();
        QDialog::accept();
    }
}

bool XIncludeDialog::isXMLProcessing()
{
    const QString &parse = _attr.parse ;
    if(parse.endsWith("+xml") || (parse == "xml") || (parse == "application/xml")) {
        return true ;
    }
    return false;
}

void XIncludeDialog::readAttributesFromElement()
{
    _attr.href = _element->getAttributeValue(AttrHREF);
    _attr.xpointer = _element->getAttributeValue(AttrXPOINTER);
    _attr.fragid  = _element->getAttributeValue(AttrFRAGID);
    _attr.parse = _element->getAttributeValue(AttrPARSE);
    _attr.setxmlid = _element->getAttributeValue(AttrSETXMLID) ;
    _attr.useSetXMLid = (NULL != _element->getAttribute(AttrSETXMLID)) ;
    bool isDeclLN = false ;
    if(XIncludeEditorManager::XIncLocaAttrNS == _element->getAttributeValue(XIncludeLNSDecl)) {
        isDeclLN = true ;
    }
    _attr.declareLocalA = isDeclLN;
    _attr.accept = _element->getAttributeValue(AttrACCEPT);
    _attr.language = _element->getAttributeValue(AttrACCEPTLANG);
    _attr.encoding = _element->getAttributeValue(AttrENCODING);
}

void XIncludeDialog::setAttributesToUI()
{
    ui->href->setText(_attr.href);
    ui->xpointer->setText(_attr.xpointer);
    ui->fragid->setText(_attr.fragid);
    Utils::setComboCurrentText(ui->parse, _attr.parse);
    ui->setxmlid->setText(_attr.setxmlid) ;
    ui->useSetXMLId->setChecked(_attr.useSetXMLid);
    ui->declareLocalA->setChecked(_attr.declareLocalA);
    ui->accept->setText(_attr.accept);
    ui->language->setText(_attr.language);
    ui->encoding->setText(_attr.encoding);
}

void XIncludeDialog::readAttributesFromUI()
{
    _attr.href = ui->href->text();
    _attr.xpointer = ui->xpointer->text() ;
    _attr.fragid  = ui->fragid->text();
    _attr.parse = ui->parse->currentText();
    _attr.setxmlid = ui->setxmlid->text() ;
    _attr.useSetXMLid = ui->useSetXMLId->isChecked();
    _attr.declareLocalA = ui->declareLocalA->isChecked();
    _attr.accept = ui->accept->text();
    _attr.language = ui->language->text();
    _attr.encoding = ui->encoding->text();
}

void XIncludeDialog::setValuesToElement()
{
    setAttr(AttrHREF, _attr.href);
    setAttr(AttrXPOINTER, _attr.xpointer);
    setAttr(AttrFRAGID, _attr.fragid);
    setAttr(AttrPARSE, _attr.parse);
    setAttrByBool(AttrSETXMLID, _attr.setxmlid, _attr.useSetXMLid);
    if(_attr.declareLocalA) {
        setAttrByBool(XIncludeLNSDecl, XIncludeEditorManager::XIncLocaAttrNS, true);
    }
    setAttr(AttrACCEPT, _attr.accept);
    setAttr(AttrACCEPTLANG, _attr.language);
    setAttr(AttrENCODING, _attr.encoding);
}

void XIncludeDialog::setAttr(const QString &attr, const QString &value)
{
    if(!value.isEmpty()) {
        _element->setAttribute(attr, value);
    } else {
        _element->removeAttribute(attr);
    }
}

void XIncludeDialog::setAttrByBool(const QString &attr, const QString &value, const bool useIt)
{
    if(useIt) {
        _element->setAttribute(attr, value);
    } else {
        _element->removeAttribute(attr);
    }
}

bool XIncludeDialog::check20To7E(const QString &in)
{
    const int len = in.length();
    FORINT(i, len) {
        const ushort ch = in[i].unicode();
        if((ch < 0x20) || (ch > 0x7E)) {
            return false;
        }
    }
    return true;
}

bool XIncludeDialog::checkValues()
{
    readAttributesFromUI();
    if(isXMLProcessing()) {
        if(_attr.href.isEmpty() && (_attr.xpointer.isEmpty() && _attr.fragid.isEmpty())) {
            Utils::error(this, tr("When XML processing is enabled and href is empty, xpointer or fragid must be present"));
            return false;
        }
        if(_attr.href.endsWith("#")) {
            Utils::error(this, tr("URI ending in '#' is an error."));
            return false;
        }
    } else {
        if(!_attr.setxmlid.isEmpty()) {
            Utils::error(this, tr("The 'set-xml-id' attribute is not allowed if XML processing is not used."));
            return false;
        }
    }
    if(!_attr.parse.isEmpty() && _attr.useSetXMLid) {
        if(!(("xml" == _attr.parse) || ("text" == _attr.parse) || Utils::isRFC4288(_attr.parse))) {
            Utils::error(this, tr("The 'parse' attribute is not valid (RFC4288)."));
            return false;
        }
    }
    if(!_attr.accept.isEmpty()) {
        if(!check20To7E(_attr.accept)) {
            Utils::error(this, tr("The 'accept' should contain only caracters in range 20 to 7E."));
            return false;
        }
    }
    if(!_attr.language.isEmpty()) {
        if(!check20To7E(_attr.language)) {
            Utils::error(this, tr("The 'accept-language' should contain only caracters in range 20 to 7E."));
            return false;
        }
    }

    // remember: xmlns:xila
    return true ;
}
