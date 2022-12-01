/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2014-2022 by Luca Bellonda and individual contributors  *
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


#include "anonadvdialog.h"
#include "anonaddexcelementdialog.h"
#include "anonattrexcdialog.h"
#include "anoncontext.h"
#include "anonprofilemanager.h"
#include "anoneditprofiledialog.h"
#include "anoncodealg.h"
#include "ui_anonadvdialog.h"
#include "utils.h"

#include <QTableWidgetItem>

AnonAdvDialog::AnonAdvDialog(QXmlEditData *newAppData, Regola * inputData, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AnonAdvDialog)
{
    _appData = newAppData ;
    _profileChanged = false ;
    _profile = new AnonProfile();
    _orig = inputData ;
    _lastFolder = _orig->fileName();
    _paintInfo = inputData->getPaintInfo() ;
    _profileData = NULL ;
    _profileManagerFactory = new AnonProfileManagerFactory();
    ui->setupUi(this);
    endSetup();
    updateThings(false);
}

AnonAdvDialog::~AnonAdvDialog()
{
    delete ui;
    if(NULL != _profile) {
        delete _profile ;
    }
    if(NULL != _profileData) {
        delete _profileData ;
    }
    if(NULL != _profileManagerFactory) {
        delete _profileManagerFactory ;
    }
}

AnonymizeParameters *AnonAdvDialog::params()
{
    return _profile->params();
}


void AnonAdvDialog::setProfileData(GenericPersistentData *newData)
{
    if(NULL != _profileData) {
        delete _profileData ;
    }
    _profileData = newData ;
}

void AnonAdvDialog::endSetup()
{
    ui->cmdSaveProfile->setEnabled(false);

    //-----
    params()->mode = AnonymizeParameters::UsingPatterns ;
    params()->useFixedLetter = false;
    ui->codeChk->setChecked(true);
    params()->threshold = AnonCodeAlg::Threshold;
    ui->threshold->setValue(params()->threshold);
    enableThreshold();

    //---
    _ex_in = QIcon(":/anon/an_ex_in");
    _ex_in_r = QIcon(":/anon/an_ex_in_r");
    _ex_ex_r = QIcon(":/anon/an_ex_ex_r");
    _ex_ex  = QIcon(":/anon/an_ex_ex");
    _ex_v_default = QIcon(":/anon/an_ex_v_default");
    _ex_v_fixed = QIcon(":/anon/an_ex_v_fixed");


    ui->exceptions->setColumnCount(4);
    QStringList labels;
    labels << tr("Rule", "anon exception");
    labels << tr("Transform", "anon exception");
    labels << tr("Namespaces", "anon exception");
    labels << tr("Path", "anon exception");
    ui->exceptions->setHorizontalHeaderLabels(labels);
    ui->exceptions->horizontalHeader()->setStretchLastSection(true);
    ui->exceptions->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->exceptions->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->exceptions->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->exceptions->setTextElideMode(Qt::ElideMiddle);

    ui->exceptions->horizontalHeaderItem(0)->setToolTip(tr("Exclude or include"));
    ui->exceptions->horizontalHeaderItem(1)->setToolTip(tr("How the datum is transformed"));
    ui->exceptions->horizontalHeaderItem(2)->setToolTip(tr("Process namespaces"));
    ui->exceptions->horizontalHeaderItem(3)->setToolTip(tr("XPath"));

    QString toolTipImport = tr("Import a set of exceptions loading a CSV file, one exception per row.\nThe fields are:\n\
   1- XPath.\n\
   2- Criteria (0: exclude (default), 1: exclude with children,  2: include,  3: include with children).\n\
   3- Type of data mask (0: use default 1: use a fixed value).\n\
   4- A value for the fixed value replacement.\n\
   5- Process namespace flag (0/1)\n\
Only the first field (XPath) is mandatory.");
    ui->cmdImportException->setToolTip(toolTipImport);
    ui->cmdExportExceptions->setToolTip(tr("Exports the exceptions in the clipboard in CSV format."));

    connect(ui->treeWidget, SIGNAL(treeContextMenuRequested(const QPoint &)), this, SLOT(treeContextMenu(const QPoint &)));
    loadData();
    enableItems();
    showMaximized();
}

void AnonAdvDialog::loadData()
{
    loadData(_orig->lightClone());
}

void AnonAdvDialog::loadData(Regola *input)
{
    ui->treeWidget->assignRegola(input, true);
    ui->treeWidget->setReadOnly(true);
    ui->treeWidget->setEncodingLabelVisible(false);
    ui->treeWidget->showButtons(false);
    ui->treeWidget->showToolbar(false);
}

AnonContext *AnonAdvDialog::getContext()
{
    AnonContext *context = new AnonContext(NULL, "");
    context->setAlg(params());
    context->setProfile(_profile->clone());
    return context;
}


QTableWidgetItem *AnonAdvDialog::newExItem(const int row, const int column, AnonException *exception, const QString &text)
{
    QTableWidgetItem *item = new QTableWidgetItem(text);
    item->setData(Qt::UserRole, qVariantFromValue((void*)exception));
    item->setFlags(item->flags() & (~Qt::ItemIsEditable));
    ui->exceptions->setItem(row, column, item);
    return item;
}

void AnonAdvDialog::addException(AnonException *exception, const bool update)
{
    int row = ui->exceptions->rowCount();
    ui->exceptions->setRowCount(row + 1);
    // icon (include/exclude)
    QTableWidgetItem *itemIcon = newExItem(row, 0, exception, "");
    switch(exception->criteria()) {
    default:
    case AnonInclusionCriteria::Include:
        itemIcon->setIcon(_ex_in);
        itemIcon->setToolTip(tr("Include"));
        break;
    case AnonInclusionCriteria::IncludeWithChildren:
        itemIcon->setIcon(_ex_in_r);
        itemIcon->setToolTip(tr("Include with children"));
        break;
    case AnonInclusionCriteria::Exclude:
        itemIcon->setIcon(_ex_ex);
        itemIcon->setToolTip(tr("Exclude"));
        break;
    case AnonInclusionCriteria::ExcludeWithChildren:
        itemIcon->setIcon(_ex_ex_r);
        itemIcon->setToolTip(tr("Exclude with children"));
        break;
    }

    // type (exception type)
    QTableWidgetItem *itemType = newExItem(row, 1, exception, "");
    if(exception->isInclude()) {
        switch(exception->anonType()) {
        default:
        case AnonType::UseDefault:
            itemType->setIcon(_ex_v_default);
            itemType->setToolTip(tr("Use default"));
            break;
        case AnonType::FixedValue:
            itemType->setIcon(_ex_v_fixed);
            {
                QString txt = exception->fixedValue();
                if(txt.length() > 10) {
                    txt = txt.left(8);
                    txt.append("...");
                }
                itemType->setText(txt);
                itemType->setToolTip(tr("Fixed value: %1").arg(exception->fixedValue()));
            }
            break;
        case AnonType::UseFormula:
            itemType->setIcon(_ex_v_fixed);
            itemType->setToolTip(tr("Use formula"));
            break;
        }
    } else {
        itemType->setToolTip(tr("Not used"));
    }
    // is processing namespaces?
    /*QTableWidgetItem *itemText = */newExItem(row, 2, exception, exception->isUseNamespace() ? tr("Yes") : "");

    // path (path of the exception)
    /*QTableWidgetItem *itemText = */newExItem(row, 3, exception, exception->path());
    ui->exceptions->resizeColumnsToContents();
    if(update) {
        updateThings();
    }
}

void AnonAdvDialog::updateThings(const bool changed)
{
    Regola *r = _orig->lightClone();
    AnonContext *context = getContext();
    if(NULL != context) {
        loadData(r);
        QHash<void *, QString> *origData = new QHash<void *, QString>();
        context->setOrigData(origData);
        r->anonymize(context, ui->treeWidget->getMainTreeWidget(), true);
        context->exposeErrorToUser(this);
        context->setOrigData(NULL);
        ui->treeWidget->setOrigDataForAnonPreview(origData);
        if(NULL != r->root()) {
            r->root()->expandRecursive();
        }
        delete context ;
    } else {
        Utils::error(this, tr("Error generating the preview"));
    }
    setChanged(changed) ;
}

void AnonAdvDialog::setChanged(const bool changed)
{
    _profileChanged = changed ;
    ui->cmdSaveProfile->setEnabled(changed);
}


void AnonAdvDialog::enableItems()
{
    bool enableExcElementAndAttribute = false;
    bool enableExcText = false;
    Element *selection = ui->treeWidget->getSelectedItem();
    if(NULL != selection) {
        if(selection->isElement()) {
            enableExcElementAndAttribute = true ;
            enableExcText = true ;
        } else if(selection->isText()) {
            enableExcText = true ;
        }
    }
    ui->cmdExcElement->setEnabled(enableExcElementAndAttribute);
    ui->cmdExcAttribute->setEnabled(enableExcElementAndAttribute);
    ui->cmdExcText->setEnabled(enableExcText);
    bool enableDeleteExc = false;
    if(ui->exceptions->selectedItems().count() > 0) {
        enableDeleteExc = true;
    }
    ui->cmdDeleteException->setEnabled(enableDeleteExc);
}

//---------------------------
void AnonAdvDialog::on_actionExceptionElement_triggered()
{
    on_cmdExcElement_clicked();
}

void AnonAdvDialog::on_actionExceptionAttributes_triggered()
{
    on_cmdExcAttribute_clicked();
}

void AnonAdvDialog::on_actionExceptionText_triggered()
{
    on_cmdExcText_clicked();
}

void AnonAdvDialog::on_cmdExcElement_clicked()
{
    Element *el = ui->treeWidget->getSelectedItem();
    if(NULL != el) {
        AnonAddExcElementDialog dlg(el, true, this);
        if(dlg.exec() == QDialog::Accepted) {
            AnonException *exception = dlg.getException();
            _profile->addException(exception);
            addException(exception);
        }
    }
}


void AnonAdvDialog::on_cmdExcText_clicked()
{
    Element *el = ui->treeWidget->getSelectedItem();
    if(NULL != el) {
        AnonAddExcElementDialog dlg(el, false, this);
        if(dlg.exec() == QDialog::Accepted) {
            AnonException *exception = dlg.getException();
            _profile->addException(exception);
            addException(exception);
        }
    }
}

void AnonAdvDialog::on_cmdExcAttribute_clicked()
{
    Element *el = ui->treeWidget->getSelectedItem();
    if((NULL != el) && el->isElement()) {
        AnonAttrExcDialog dlg(el, this);
        if(dlg.exec() == QDialog::Accepted) {
            QList<AnonException*> list = dlg.getExceptions();
            foreach(AnonException * exception, list) {
                _profile->addException(exception);
                addException(exception, false);
            }
            updateThings();
        }
    }
}



void AnonAdvDialog::on_allChk_clicked(bool /*checked*/)
{
    if(params()->mode != AnonymizeParameters::AllText) {
        setChanged(true);
        params()->mode = AnonymizeParameters::AllText ;
        updateThings();
    }
}

void AnonAdvDialog::on_codeChk_clicked(bool /*checked*/)
{
    if(params()->mode != AnonymizeParameters::UsingPatterns) {
        setChanged(true);
        params()->mode = AnonymizeParameters::UsingPatterns ;
        updateThings();
    }
    enableThreshold();
}

void AnonAdvDialog::on_chkFixedPattern_clicked(bool /*checked*/)
{
    if(params()->useFixedLetter != ui->chkFixedPattern->isChecked()) {
        setChanged(true);
        params()->useFixedLetter = ui->chkFixedPattern->isChecked();
        updateThings();
    }
    enableThreshold();
}

void AnonAdvDialog::on_exceptions_currentItemChanged(QTableWidgetItem * /*current*/, QTableWidgetItem * /*previous*/)
{
    enableItems();
}

void AnonAdvDialog::on_treeWidget_reevaluateSelectionState()
{
    enableItems();
}

void AnonAdvDialog::on_cmdManageProfiles_clicked()
{
    onManageProfiles();
}

void AnonAdvDialog::on_cmdEditProfile_clicked()
{
    onEditProfile();
}

void AnonAdvDialog::on_cmdSaveProfile_clicked()
{
    onSaveProfile();
}

void AnonAdvDialog::on_cmdDeleteException_clicked()
{
    if(ui->exceptions->selectedItems().count() > 0) {
        QTableWidgetItem * const item = ui->exceptions->selectedItems().at(0);
        int row = item->row();
        QVariant data = item->data(Qt::UserRole);
        AnonException *exc = (AnonException*)data.value<void*>();
        ui->exceptions->removeRow(row);
        _profile->removeException(exc);
        updateThings();
    }
}

void AnonAdvDialog::accept()
{
    if(checkIfProfileChanged()) {
        QDialog::accept();
    }
}

void AnonAdvDialog::reject()
{
    if(checkIfProfileChanged()) {
        QDialog::reject();
    }
}

bool AnonAdvDialog::checkIfProfileChanged()
{
    if(_profileChanged) {
        if(Utils::askYN(this, tr("The profile has changed. Do you want to save it?"))) {
            onSaveProfile();
            return true ;
        }
    }
    return true;
}

bool AnonAdvDialog::onSaveProfile()
{
    if(NULL == _profileData) {
        return saveNewProfile();
    } else {
        return saveProfile();
    }
}

bool AnonAdvDialog::onEditProfile()
{
    if(NULL == _profileData) {
        return saveNewProfile();
    } else {
        AnonEditProfileDialog dlg(false, _profileData, this);
        if(dlg.editProfile()) {
            ui->profileName->setText(_profileData->name());
            return saveProfile();
        }
        return false;
    }
}

void AnonAdvDialog::onManageProfiles()
{
    AnonProfileManager *manager = _profileManagerFactory->newProfileManager();
    if(NULL != manager) {
        GenericPersistentData *newProfileData = manager->chooseProfile(_appData, this);
        AnonProfile *newProfile = NULL ;
        if(NULL != newProfileData) {
            newProfile = getProfileFromProfileData(newProfileData);
        } else {
            return ;
        }
        bool profileDataOwned = false ;
        if(NULL != newProfile) {
            profileDataOwned = true ;
            setProfileData(newProfileData);
            if(NULL != _profile) {
                delete _profile ;
            }
            _profile = newProfile;
            setChanged(false) ;
            updateProfileData();
        } else {
            Utils::error(this, tr("Unable to load the new profile"));
        }
        if(!profileDataOwned && (NULL != newProfileData)) {
            delete newProfileData;
        }
        delete manager ;
    }
}

void AnonAdvDialog::updateProfileData()
{
    if(NULL != _profileData) {
        ui->profileName->setText(_profileData->name());
        ui->exceptions->clearContents();
        ui->exceptions->setRowCount(0);
        // set all the exceptions.
        foreach(AnonException * e, _profile->exceptions()) {
            addException(e, false);
        }

        // set params values
        ui->chkFixedPattern->setChecked(params()->useFixedLetter);
        switch(params()->mode) {
        default:
        case  AnonymizeParameters::UsingPatterns:
            ui->codeChk->setChecked(true);
            break;

        case  AnonymizeParameters::AllText:
            ui->allChk->setChecked(true);
        }
        ui->threshold->setValue(params()->threshold);
        updateThings(false);
        enableThreshold();
    } else {
        ui->profileName->setText(tr("<No profile>"));
    }
}

AnonProfile* AnonAdvDialog::getProfileFromProfileData(GenericPersistentData *input)
{
    AnonProfile * result = new AnonProfile() ;
    bool flag = result->readFromSerializedXmlString(input->payload());
    if(!flag) {
        Utils::error(tr("Unable to parse the profile data."));
        delete result;
        result = NULL ;
    }
    return result;
}

bool AnonAdvDialog::saveProfile()
{
    bool result = true ;
    AnonProfileManager *manager = _profileManagerFactory->newProfileManager();
    if(NULL != manager) {
        QString newData = _profile->toXMLSerializedString();
        _profileData->setPayload(newData);
        if(manager->saveProfile(this, _appData, _profileData)) {
            setChanged(false);
        } else {
            Utils::error(this, tr("Unable to save the profile"));
        }
        delete manager ;
    }
    return result;
}

bool AnonAdvDialog::saveNewProfile()
{
    bool result = true ;
    AnonProfileManager *manager = _profileManagerFactory->newProfileManager();
    if(NULL != manager) {
        GenericPersistentData *newData = manager->insertProfile(this, _appData, _profile) ;
        if(NULL != newData) {
            ui->profileName->setText(newData->name());
            setChanged(false);
            setProfileData(newData);
        }
        delete manager ;
    }
    return result;
}

void AnonAdvDialog::treeContextMenu(const QPoint &pt)
{
    QMenu contextMenu(this);
    Element *selection = ui->treeWidget->getSelectedItem();
    if(NULL == selection) {
        return ;
    }
    if(selection->isElement()) {
        contextMenu.addAction(ui->actionExceptionElement);
        contextMenu.addAction(ui->actionExceptionAttributes);
        contextMenu.addAction(ui->actionExceptionText);
    } else if(selection->isText()) {
        contextMenu.addAction(ui->actionExceptionText);
    } else {
        return ;
    }
    contextMenu.exec(ui->treeWidget->mapToGlobal(pt));
}

void AnonAdvDialog::on_cmdImportException_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(
                           this, tr("Import Exceptions"),
                           QXmlEditData::sysFilePathForOperation(_lastFolder),
                           Utils::getFileFilterForCSV());
    if(!filePath.isEmpty()) {
        if(importUsingFile(filePath)) {
            updateThings(true);
        }
    }
}

bool AnonAdvDialog::importUsingFile(const QString &filePath)
{
    bool isOk = false;
    if(!filePath.isEmpty()) {
        setEnabled(false);
        _lastFolder = filePath ;
        QFile file(filePath);
        if(file.open(QIODevice::ReadOnly)) {
            QTextStream textStream(&file);
            textStream.setCodec("UTF-8");
            int imported = import(textStream);
            if(file.error() != QFile::NoError) {
                Utils::error(this, tr("Error importing data"));
            } else {
                isOk = true ;
                Utils::message(this, tr("Imported %n definition(s)", "", imported));
            }
            file.close();
        } else {
            Utils::error(this, tr("Error opening file"));
        }
        setEnabled(true);
    }
    return isOk ;
}

QString AnonAdvDialog::cleanParmCSV(const QString &val)
{
    QString result = val;
    result.replace("\"", "");
    result = result.trimmed();
    return result;
}

int AnonAdvDialog::import(QTextStream &stream)
{
    int numImported = 0 ;
    while(!stream.atEnd()) {
        QString row = stream.readLine();
        QStringList values = row.split(",");
        if(values.size() > 0) {
            QString path = cleanParmCSV(values.at(0));
            AnonException *exception = new AnonException();
            exception->setPath(path);
            QString crit ;
            QString anType ;
            QString fixedValue ;
            if(values.size() > 1) {
                crit = cleanParmCSV(values.at(1));
                if(!crit.isEmpty()) {
                    bool isOk = false;
                    int cr = crit.toInt(&isOk);
                    if(isOk && (cr < AnonInclusionCriteria::EndValue) && (cr >= AnonInclusionCriteria::StartValue)) {
                        exception->setCriteria((AnonInclusionCriteria::Criteria)cr);
                    }
                }
                if(values.size() > 2) {
                    anType = cleanParmCSV(values.at(2));
                    bool isOk = false;
                    int at = anType.toInt(&isOk);
                    if(isOk && (at < AnonType::EndValue) && (at >= AnonType::StartValue)) {
                        exception->setAnonType((AnonType::Type)at);
                    }
                    if(values.size() > 3) {
                        fixedValue = cleanParmCSV(values.at(3));
                        exception->setFixedValue(fixedValue);
                        if(values.size() > 4) {
                            QString ns = cleanParmCSV(values.at(4));
                            bool isOk = false;
                            int ax = ns.toInt(&isOk);
                            if(isOk) {
                                exception->setUseNamespace(ax != 0);
                            }
                        }
                    }
                }
            }
            _profile->addException(exception);
            addException(exception, false);
            numImported++;
        }
    }
    if(numImported > 0) {
        setChanged(true) ;
    }
    return numImported ;
}


void AnonAdvDialog::on_cmdExportExceptions_clicked()
{
    QString result;
    foreach(AnonException * e, _profile->exceptions()) {
        result += "\"" ;
        result += e->path() ;
        result += "\",";
        result += QString("%1,%2,\"%3\",%4\n").arg(e->criteria()).arg(e->anonType()).arg(e->fixedValue()).arg(e->isUseNamespace());
    }
    QClipboard *clipboard = QApplication::clipboard();
    if((NULL != clipboard) && !result.isEmpty()) {
        clipboard->setText(result);
        Utils::message(this, tr("Exceptions exported in the clipboard."));
    } else {
        Utils::warning(this, tr("Unable to export exceptions in the clipboard."));
    }
}

void AnonAdvDialog::enableThreshold()
{
    ui->threshold->setEnabled(ui->codeChk->isChecked());
}

void AnonAdvDialog::on_threshold_valueChanged(int /*i*/)
{
    int newValue = ui->threshold->value();
    if(params()->threshold != newValue) {
        setChanged(true);
        params()->threshold = ui->threshold->value();
        updateThings();
    }
}
