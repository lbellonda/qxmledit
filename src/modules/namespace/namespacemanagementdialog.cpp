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

#include "namespacemanagementdialog.h"
#include "ui_namespacemanagementdialog.h"
#include "modules/namespace/choosenamespacedialog.h"
#include "element.h"
#include "xmlutils.h"
#include "utils.h"

NamespaceManagementDialog::NamespaceManagementDialog(QWidget *parent, Element *element, NamespaceManager *nsManager) :
    QDialog(parent),
    ui(new Ui::NamespaceManagementDialog)
{
    _goComboNs = false;
    _element = element ;
    _nsManager = nsManager ;
    ui->setupUi(this);
    init();
    setupData();
    updateButtonsNs();
    enableOk();
    Utils::TODO_THIS_RELEASE("check length of table columns");
    Utils::TODO_THIS_RELEASE("non riporta uri");
    Utils::TODO_THIS_RELEASE("non riporta le descrizioni");
    Utils::TODO_THIS_RELEASE("su edit non riporta il namespace");
    Utils::TODO_THIS_RELEASE("seleziona ilprimo ns e scrive il prefix");
    Utils::TODO_THIS_RELEASE("quando inserisce il parent, controlla i namespace degli attributi");
}

NamespaceManagementDialog::~NamespaceManagementDialog()
{
    delete ui;
}

NamespaceCommands *NamespaceManagementDialog::getCommands()
{
    bool isError = false;
    NamespaceCommands *commands = new NamespaceCommands();
    QString prefix = ui->prefix->text().trimmed();
    QString uri = ui->uri->currentText().trimmed();
    if(!prefix.isEmpty()) {
        commands->setUseNsElement(true);
        commands->tagSpec()->prefix = prefix;
        commands->tagSpec()->uri = uri;
        commands->setDeclareNs(ui->radioDoNotDeclare->isChecked() ? NamespaceCommands::DoNotDeclare : NamespaceCommands::DeclareInElement);
    }

    Utils::TODO_THIS_RELEASE("fare altri ns");
    int rows = ui->nsList->rowCount();
    FORINT(row, rows) {
        NamespaceResult setValues;
        _tableUtils.valueForRow(ui->nsList, row, &setValues);
        if(!commands->addNamespace(setValues.prefix, setValues.uri)) {
            isError = true ;
        }
    }
    if(isError) {
        Utils::error(this, tr("Error"));
    }
    return commands ;
}

void NamespaceManagementDialog::init()
{
    Utils::TODO_THIS_RELEASE("rem comments");
    _tableUtils.setupTable(ui->nsList);
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    ui->nsList->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
#endif
    _group.addButton(ui->radioDeclareInElement);
    _group.addButton(ui->radioDoNotDeclare);
    ui->radioDoNotDeclare->setChecked(true);
}

void NamespaceManagementDialog::setupData()
{
    decodePrefix();
    loadOtherDeclarations();
}

void NamespaceManagementDialog::enableOk()
{
    bool isEnabledOk = true ;
    Utils::TODO_THIS_RELEASE("decidere se esplorare tutti i ns leciti dal parent");
    /*if(ui->uri->currentText().trimmed().isEmpty()
            &&  !ui->prefix->text().trimmed().isEmpty()) {
        isEnabledOk = false;
    }
    if(!Utils::checkNsPrefix(ui->prefix->text().trimmed())) {
        isEnabledOk = false;
        tutti quelli dichiarati, ma il b
    }*/
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(isEnabledOk);
}

void NamespaceManagementDialog::on_uri_currentIndexChanged(int index)
{
    if(_goComboNs && (index >= 0)) {
        QString value = ui->uri->itemText(index);
        foreach(QString key, _visibleNamespaces.keys()) {
            if(value == _visibleNamespaces[key]) {
                ui->prefix->setText(key);
                break;
            }
        }
    }
}

void NamespaceManagementDialog::decodePrefix()
{
    QString prefix;
    QString localName;
    XmlUtils::decodeQualifiedName(_element->tag(), prefix, localName);
    ui->prefix->setText(prefix);

    _visibleNamespaces = _element->findVisibleNamespaces();
    foreach(QString uri, _visibleNamespaces.values()) {
        ui->uri->addItem(uri);
    }
    _goComboNs = true;
    // select the current one
    QString currNs = _visibleNamespaces[prefix];
    ui->uri->setEditText(currNs);
}

void NamespaceManagementDialog::loadOtherDeclarations()
{
    foreach(Attribute * attribute, _element->attributes) {
        if(XmlUtils::isDeclaringNS(attribute->name)) {
            QString nsPrefix = XmlUtils::namespacePrefix(attribute->name);
            QString description ;
            if(NULL != _nsManager) {
                Utils::TODO_THIS_RELEASE("trova descrizione");
            }
            _tableUtils.insNsInList(ui->nsList, -1, nsPrefix, attribute->value, description, NULL);
        }
    }
}

void NamespaceManagementDialog::updateButtonsNs()
{
    int selRow = ui->nsList->currentRow();
    bool enableRow = selRow >= 0 ;
    ui->cmdModNs->setEnabled(enableRow);
    ui->cmdRemNs->setEnabled(enableRow);
}

void NamespaceManagementDialog::on_cmdRemNs_clicked()
{
    int selRow = ui->nsList->currentRow();
    if(selRow >= 0) {
        if(Utils::askYN(this, tr("Do you really want to delete this declaration?"))) {
            ui->nsList->removeRow(selRow);
        }
    }
}

void NamespaceManagementDialog::on_nsList_itemSelectionChanged()
{
    updateButtonsNs();
}

void NamespaceManagementDialog::on_cmdAddNs_clicked()
{
    if(NULL != _nsManager) {
        ChooseNamespaceDialog dlg(this, _nsManager, NULL);
        dlg.setModal(true);
        if(dlg.exec() == QDialog::Accepted) {
            NamespaceResult result;
            dlg.nsChosen(&result);
            _tableUtils.insNsInList(ui->nsList, -1, result.prefix, result.uri, result.description, NULL);
        }
    }
}

void NamespaceManagementDialog::on_cmdModNs_clicked()
{
    int row = ui->nsList->currentRow();
    if(row >= 0) {
        if(NULL != _nsManager) {
            NamespaceResult setValues;
            _tableUtils.valueForRow(ui->nsList, row, &setValues);
            ChooseNamespaceDialog dlg(this, _nsManager, &setValues);
            dlg.setModal(true);
            if(dlg.exec() == QDialog::Accepted) {
                NamespaceResult result;
                dlg.nsChosen(&result);
                _tableUtils.insNsInList(ui->nsList, row, result.prefix, result.uri, result.description, NULL);
            }
        }
    }
}


void NamespaceManagementDialog::on_cmdSelect_clicked()
{
    Utils::TODO_THIS_RELEASE("finire");
    if(NULL != _nsManager) {
        NamespaceResult setValues;
        setValues.description = ui->prefix->text().trimmed();
        setValues.uri = ui->uri->currentText().trimmed();
        setValues.prefix = ui->nsDescription->text().trimmed();
        ChooseNamespaceDialog dlg(this, _nsManager, &setValues);
        dlg.setModal(true);
        if(dlg.exec() == QDialog::Accepted) {
            NamespaceResult result;
            dlg.nsChosen(&result);
            ui->prefix->setText(result.prefix);
            ui->uri->setEditText(result.uri);
            ui->nsDescription->setText(result.description);
        }
    }
    enableOk();
}

void NamespaceManagementDialog::on_nsList_itemDoubleClicked(QTableWidgetItem * /*item*/)
{
    on_cmdModNs_clicked();
}

void NamespaceManagementDialog::on_cmdClear_clicked()
{
    ui->prefix->setText("");
    ui->uri->setCurrentIndex(-1);
    ui->uri->setEditText("");
    ui->nsDescription->setText("");
    enableOk();
}

void NamespaceManagementDialog::on_prefix_textChanged(const QString & /*text*/)
{
    enableOk();
}

void NamespaceManagementDialog::on_uri_editTextChanged(const QString & /*text*/)
{
    enableOk();
}
