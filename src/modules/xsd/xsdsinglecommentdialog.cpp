/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2015 by Luca Bellonda and individual contributors       *
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


#include "xsdsinglecommentdialog.h"
#include "ui_xsdsinglecommentdialog.h"
#include "utils.h"

XSDSingleCommentDialog::XSDSingleCommentDialog(const bool argOpenedAsStarting, XInfoBase *newModel, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::XSDSingleCommentDialog)
{
    _wasAccepted = false;
    _wasOpenedAsStarting = argOpenedAsStarting ;
    _isCallExtended = false;
    _origModel = newModel;
    ui->setupUi(this);
    ui->xmlEditor->setReadOnly(true);
    ui->xmlEditor->setEncodingLabelVisible(false);
    ui->xmlEditor->showButtons(false);
    ui->xmlEditor->showToolbar(false);

    QSizePolicy policy = ui->tabWidget->sizePolicy();
    policy.setVerticalStretch(1);
    ui->tabWidget->setSizePolicy(policy);

    loadLangCodes();

    if(!argOpenedAsStarting) {
        ui->cmdAdvanced->setVisible(false);
        ui->cmdAdvanced->setEnabled(false);
    }
    ui->uri->setEditText(_origModel->source());
    if(_origModel->getType() == SchemaTypeDocumentation) {
        XDocumentation *doc = static_cast<XDocumentation*>(_origModel);
        ui->lang->setEditText(doc->language());
        ui->typeDoc->setChecked(true);
    } else {
        ui->typeAppInfo->setChecked(true);
    }
    ui->textValue->setPlainText(newModel->contentString());
    activateButtons();
    _textChanged = true ;
    ui->textValue->setFocus();
}

XSDSingleCommentDialog::~XSDSingleCommentDialog()
{
    delete ui;
}

bool XSDSingleCommentDialog::isCallExtended()
{
    return _isCallExtended ;
}

void XSDSingleCommentDialog::loadLangCodes()
{
    QComboBox *combo = ui->lang;
    combo->setUpdatesEnabled(false);
    QStringList codes;
    codes << "aa" << "ab" << "ae" << "af" << "ak" << "am" << "ar" << "an" << "as" << "av" << "ay" << "az"
          << "ba" << "bg" << "be" << "bn" << "bh" << "bi" << "bm" << "bo" << "bs" << "br"
          << "ca" << "ch" << "ce" << "cu" << "cv" << "co" << "cr" << "cy" << "cs" << "da" << "de" << "dv"
          << "dz" << "en" << "eo" << "es" << "et" << "eu" << "ee" << "el" << "fo" << "fa" << "fj"
          << "fi" << "fr" << "fr" << "fy" << "ff" << "gd" << "ga" << "gl"
          << "gv" << "gn" << "gu" << "ht" << "ha" << "he" << "hz" << "hi" << "ho"
          << "hr" << "hu" << "hy" << "ig" << "is" << "io" << "ii" << "iu" << "ie" << "ia"
          << "id" << "ik" << "is" << "it" << "jv" << "ja" << "kl" << "kn" << "ks" << "ka"
          << "kr" << "kk" << "km" << "ki" << "rw" << "ky" << "kv" << "kg" << "ko" << "kj" << "kw"
          << "ku" << "lo" << "la" << "lv" << "li" << "ln" << "lt" << "lb" << "lu" << "lg"
          << "mk" << "mh" << "ml" << "mi" << "mr" << "ms" << "mk" << "mg" << "mt" << "mn"
          << "mi" << "ms" << "my" << "na" << "nv" << "nr" << "nd" << "ng" << "ne" << "nl"
          << "nn" << "nb" << "no" << "ny" << "oc" << "oj" << "or" << "om" << "os" << "pa" << "pi" << "pl" << "pt" << "ps" << "qu" << "rm" << "ro"
          << "rn" << "ru" << "sg" << "sa" << "si" << "sk" << "sl" << "se" << "sm" << "sn" << "sd" << "so" << "st" << "sq" << "sc" << "sr" << "ss"
          << "su" << "sw" << "sv" << "ty" << "ta" << "tt" << "te" << "tg" << "tl" << "th" << "bo" << "ti" << "to" << "tn" << "ts" << "tk" << "tr" << "tw"
          << "ug" << "uk" << "ur" << "uz" << "ve" << "vi" << "vo" << "wa" << "wo" << "xh" << "yi" << "yo" << "za" << "zh" << "zu";
    combo->setInsertPolicy(QComboBox::InsertAlphabetically);
    combo->addItems(codes);
    combo->setUpdatesEnabled(true);
}

void XSDSingleCommentDialog::on_textValue_modificationChanged(bool changed)
{
    if(changed) {
        _textChanged = true ;
    }
}

void XSDSingleCommentDialog::on_textValue_textChanged()
{
    _textChanged = true ;
}

void XSDSingleCommentDialog::on_textValue_undoAvailable(bool /*available*/)
{
    _textChanged = true ;
}

void XSDSingleCommentDialog::on_cmdAdvanced_clicked()
{
    if(!_wasOpenedAsStarting) {
        return ;
    }
    if(Utils::askYN(this, tr("Confirm changes and open the full panel?"))) {
        _isCallExtended = true ;
        if(!doAccept()) {
            _isCallExtended = false ;
        }
    }
}

bool XSDSingleCommentDialog::doAccept()
{
    if(!isXML()) {
        Utils::error(this, tr("The annotation must be not null and contain valid XML."));
        return false;
    }
    QDialog::accept();
    _wasAccepted = true ;
    return true ;
}

void XSDSingleCommentDialog::accept()
{
    doAccept();
}

bool XSDSingleCommentDialog::isXML()
{
    QString userData = ui->textValue->toPlainText();
    QString text = "<root>";
    text += userData ;
    text += "</root>";
    QDomDocument document;
    if(document.setContent(text)) {
        return true ;
    }
    return false;
}

void XSDSingleCommentDialog::on_tabWidget_currentChanged(int index)
{
    if(1 == index) {
        if(_textChanged) {
            bool isXMLData = isXML();
            ui->xmlEditor->setVisible(isXMLData);
            ui->warningNoXML->setVisible(!isXMLData);
            if(isXMLData) {
                // load data into the editor.
                QString userData = ui->textValue->toPlainText();
                QString text = "<PARENT>";
                text += userData ;
                text += "</PARENT>";
                QDomDocument document;
                if(document.setContent(text)) {
                    ui->xmlEditor->setDocument(document, "", false);
                    ui->xmlEditor->setReadOnly(true);
                    ui->xmlEditor->showButtons(false);
                    ui->xmlEditor->getRegola()->root()->_tag = "";
                }
            }
            _textChanged = false ;
        }
    }
}

XInfoBase* XSDSingleCommentDialog::createModel()
{
    if(!_wasAccepted) {
        return NULL ;
    }
    XInfoBase* result = NULL ;
    if(ui->typeAppInfo->isChecked()) {
        XAppInfo *appInfo = new XAppInfo(NULL, NULL);
        result = appInfo ;
    } else {
        XDocumentation *docInfo = new XDocumentation(NULL, NULL);
        QString lang = ui->lang->currentText();
        if(!lang.isEmpty()) {
            docInfo->setLanguage(lang);
        }
        result = docInfo ;
    }
    QString uri = ui->uri->currentText();
    if(!uri.isEmpty()) {
        result->setSource(uri);
    }
    result->setContentString(ui->textValue->toPlainText());
    return result ;
}

void XSDSingleCommentDialog::activateButtons()
{
    bool isDoc = ui->typeDoc->isChecked();
    ui->lang->setEnabled(isDoc);
}

void XSDSingleCommentDialog::on_typeAppInfo_clicked()
{
    activateButtons();
}

void XSDSingleCommentDialog::on_typeDoc_clicked()
{
    activateButtons();
}
