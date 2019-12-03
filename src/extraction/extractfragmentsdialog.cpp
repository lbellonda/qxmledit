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
#include "libQXmlEdit_global.h"
#include "extractfragmentsdialog.h"
#include "extractionfrontend.h"
#include "ui_extractfragmentsdialog.h"
#include "utils.h"
#include "qxmleditdata.h"
#include <limits.h>
#include <QDesktopServices>
#include <QFileDialog>
#include <QUrl>
#include "extractionadavancedoptionsdialog.h"

#define UPDATE_TIMEOUT  1500

void LIBQXMLEDITSHARED_EXPORT extractFragments(ExtractResults *extractResult, QWidget *parent, QWidget *mainWidget)
{
    ExtractFragmentsDialog dialog(extractResult, parent, mainWidget);
    dialog.exec();
}

void LIBQXMLEDITSHARED_EXPORT extractFragmentsWindow(ExtractResults *extractResult, QWidget *parent, QWidget *mainWidget)
{
    ExtractFragmentsDialog *dialog = new ExtractFragmentsDialog(extractResult, parent, mainWidget);
    dialog->setAutoDelete();
    dialog->setModal(false);
    dialog->show();
}

ExtractFragmentsDialog::ExtractFragmentsDialog(ExtractResults *extractResult, QWidget *parent, QWidget *mainWidget) :
    QDialog(parent),
    _operation(extractResult),
    ui(new Ui::ExtractFragmentsDialog)
{
    if(NULL == mainWidget) {
        mainWidget = this;
    }
    _mainWidget = mainWidget ;
    _isAutoDelete = false ;
    delayTimer.setSingleShot(true);
    _extractResult = extractResult ;
    _extractResult->init();
    ui->setupUi(this);
    _groupExtractOptions.addButton(ui->extractAll);
    _groupExtractOptions.addButton(ui->extractRange);
    _groupExtractOptions.addButton(ui->extractWithCfr);

    _groupSplitOptions.addButton(ui->splitByDepth);
    _groupSplitOptions.addButton(ui->splitByPath);

    loadComboBoxCfrOpr(ui->cfrOperator);
    loadComboBoxOperation();
    ui->rangeMin->setMinimum(1);
    ui->rangeMax->setMinimum(1);
    ui->rangeMin->setMaximum(INT_MAX);
    ui->rangeMax->setMaximum(INT_MAX);
    ui->eachNFiles->setMaximum(INT_MAX);
    ui->depth->setRange(1, 10000);
    _operation.loadSettings();
    initUIFromOperation();
    showNaming();
    infoOnAdvancedOptions();
    connectUpdates();
}

ExtractFragmentsDialog::~ExtractFragmentsDialog()
{
    delete ui;
}

void ExtractFragmentsDialog::setAutoDelete()
{
    _isAutoDelete = true ;
}

void ExtractFragmentsDialog::closeEvent(QCloseEvent * event)
{
    event->accept();
    if(_isAutoDelete) {
        deleteLater();
    }
}

void ExtractFragmentsDialog::loadComboBoxCfrOpr(QComboBox *combo)
{
    combo->addItem(tr("equal"), ExtractionOperation::CFR_EQ);
    combo->addItem(tr("not equal"), ExtractionOperation::CFR_NEQ);
}

void ExtractFragmentsDialog::loadComboBoxOperation()
{
    ui->cbOperationType->clear();
    QStringList texts;
    QList<int> values ;
    texts  << tr("Split");
    values  << ExtractionOperation::OperationSplit;
    texts  << tr("Act as a filter producing only 1 (one) file");
    values  << ExtractionOperation::OperationFilter;
    texts  << tr("Extract and group XML format. Only the splitting element will be extracted");
    values  << ExtractionOperation::OperationExportAndGroupXML;
    texts  << tr("Extract and group, CSV format. Only the splitting element will be extracted");
    values  << ExtractionOperation::OperationExportAndGroupCSV;
    Utils::loadComboCodedArrays(ui->cbOperationType, _operation.OperationType(), texts, values);
}


void ExtractFragmentsDialog::loadComboData(QComboBox *box)
{
    box->addItem(DATE_TOKEN_PTRN);
    box->addItem(TIME_TOKEN_PTRN);
    box->addItem(COUNTER_TOKEN_PTRN);
    box->addItem(TS_TOKEN_PTRN);
    box->addItem(SPACE_TOKEN_PTRN);
    box->addItem(SEQUENCE_TOKEN_PTRN);
    box->addItem("_");
    box->addItem(".");
    box->setEditText("");
}

void ExtractFragmentsDialog::initUIFromOperation()
{
    loadComboData(ui->sfp1);
    loadComboData(ui->sfp2);
    loadComboData(ui->sfp3);
    loadComboData(ui->sfp4);
    loadComboData(ui->sfp5);
    loadComboData(ui->sfn1);
    loadComboData(ui->sfn2);
    loadComboData(ui->sfn3);
    loadComboData(ui->sfn4);
    loadComboData(ui->sfn5);

    ui->chkExtractFiles->setChecked(_operation.isExtractDocuments());
    ui->splitPath->setEditText(_operation.splitPath());
    if(_operation.isExtractAllDocuments()) {
        ui->extractAll->setChecked(true);
    } else {
        if(_operation.isExtractCfr()) {
            ui->extractWithCfr->setChecked(true);
        } else {
            ui->extractRange->setChecked(true);
        }
    }
    ui->rangeMin->setValue(_operation.minDoc());
    ui->rangeMax->setValue(_operation.maxDoc());
    ui->extractDir->setText(_operation.extractFolder());
    ui->makeSubfolders->setChecked(_operation.isMakeSubFolders());
    ui->eachNFiles->setValue(_operation.subFoldersEachNFiles());
    ui->sourceFile->setText(_operation.inputFile());
    ui->reverseRange->setChecked(_operation.isReverseRange());
    Utils::selectComboValue(ui->cbOperationType, _operation.OperationType());
    ui->attrName->setText(_operation.attributeName());
    ui->compTerm->setText(_operation.comparisonTerm());
    Utils::selectComboValue(ui->cfrOperator, _operation.comparisonType());
    ui->depth->setValue(_operation.splitDepth());
    ui->chkDeleteUsingPath->setChecked(_operation.isFilterTextForPath());
    ui->pathToDeleteText->setText(_operation.pathForDeleteText());

    if(_operation.splitType() == ExtractionOperation::SplitUsingDepth) {
        ui->splitByDepth->setChecked(true);
    } else {
        ui->splitByPath->setChecked(true);
    }

    QList<QComboBox*>comboF;
    comboF.append(ui->sfp1);
    comboF.append(ui->sfp2);
    comboF.append(ui->sfp3);
    comboF.append(ui->sfp4);
    comboF.append(ui->sfp5);
    Utils::loadComboTextArray(_operation.subfolderNamePattern(), comboF);
    QList<QComboBox*>comboN;
    comboN.append(ui->sfn1);
    comboN.append(ui->sfn2);
    comboN.append(ui->sfn3);
    comboN.append(ui->sfn4);
    comboN.append(ui->sfn5);
    Utils::loadComboTextArray(_operation.filesNamePattern(), comboN);

    //-----
    enableControls();
}

void ExtractFragmentsDialog::fillOperationFromUI()
{
    _operation.setInputFile(ui->sourceFile->text());
    _operation.setExtractDocuments(ui->chkExtractFiles->isChecked());
    _operation.setSplitPath(ui->splitPath->currentText().trimmed());

    _operation.setSplitDepth(ui->depth->value());

    if(ui->extractAll->isChecked()) {
        _operation.setExtractAllDocuments();
    } else if(ui->extractRange->isChecked()) {
        _operation.setExtractRange();
    } else {
        _operation.setExtractCfr();
    }

    _operation.setSplitDepth(ui->depth->value());
    if(ui->splitByDepth->isChecked()) {
        _operation.setSplitType(ExtractionOperation::SplitUsingDepth);
    } else {
        _operation.setSplitType(ExtractionOperation::SplitUsingPath);
    }

    _operation.setMinDoc(ui->rangeMin->value());
    _operation.setMaxDoc(ui->rangeMax->value());
    _operation.setExtractFolder(ui->extractDir->text().trimmed());
    _operation.setIsMakeSubFolders(ui->makeSubfolders->isChecked());
    _operation.setSubFoldersEachNFiles(ui->eachNFiles->value());
    _operation.setReverseRange(ui->reverseRange->isChecked());
    _operation.setOperationType(static_cast<ExtractionOperation::EOperationType>(Utils::comboSelectedCodeAsInt(ui->cbOperationType, -1)));
    _operation.setAttributeName(ui->attrName->text());
    _operation.setComparisonTerm(ui->compTerm->text());
    _operation.setComparisonType((ExtractionOperation::ECfrOp)Utils::comboSelectedCodeAsInt(ui->cfrOperator, ExtractionOperation::CFR_EQ));
    _operation.setFilterTextForPath(ui->chkDeleteUsingPath->isChecked());
    _operation.setPathForDeleteText(ui->pathToDeleteText->text().trimmed());

    QStringList dummy;
    _operation.setSubfolderNamePattern(dummy);
    _operation.addFolderPattern(ui->sfp1->currentText().trimmed());
    _operation.addFolderPattern(ui->sfp2->currentText().trimmed());
    _operation.addFolderPattern(ui->sfp3->currentText().trimmed());
    _operation.addFolderPattern(ui->sfp4->currentText().trimmed());
    _operation.addFolderPattern(ui->sfp5->currentText().trimmed());

    _operation.setFilesNamePattern(dummy);
    _operation.addFileNamePattern(ui->sfn1->currentText().trimmed());
    _operation.addFileNamePattern(ui->sfn2->currentText().trimmed());
    _operation.addFileNamePattern(ui->sfn3->currentText().trimmed());
    _operation.addFileNamePattern(ui->sfn4->currentText().trimmed());
    _operation.addFileNamePattern(ui->sfn5->currentText().trimmed());
}

void ExtractFragmentsDialog::on_buttonBox_accepted()
{
    //accept();
}

bool ExtractFragmentsDialog::checkOperationParameters()
{
    ExtractionOperation::EParamErrors error = _operation.checkParameters();
    if(ExtractionOperation::ParamNoError ==  error) {
        return true ;
    }
    switch(error) {
    default:
        _errorMessage = tr("Unknown error.");
        break;
    case ExtractionOperation::ParamErrorNoInputFile:
        _errorMessage = tr("No input file.");
        break;
    case ExtractionOperation::ParamErrorNoSplitPath:
        _errorMessage = tr("No split path.");
        break;
    case ExtractionOperation::ParamErrorNoMinRange:
        _errorMessage = tr("No minimum range specified.");
        break;
    case ExtractionOperation::ParamErrorNoMaxRange:
        _errorMessage = tr("No maximum range specified.");
        break;
    case ExtractionOperation::ParamErrorNoExtractFolder:
        _errorMessage = tr("No extraction folder.");
        break;
    case ExtractionOperation::ParamErrorSubFolderEach:
        _errorMessage = tr("No subfolder specified.");
        break;
    case ExtractionOperation::ParamErrorSubFolderNamePattern:
        _errorMessage = tr("No subfolder name pattern specified.");
        break;
    case ExtractionOperation::ParamErrorNoRange:
        _errorMessage = tr("The documents range is not correct.");
        break;
    case ExtractionOperation::ParamErrorCfrType:
        _errorMessage = tr("The type of the comparaison is not legal.");
        break;
    case ExtractionOperation::ParamErrorExtractionType:
        _errorMessage = tr("The type of the extraction is not legal.");
        break;
    case ExtractionOperation::ParamErrorCfrAttr:
        _errorMessage = tr("The name of the comparison attribute is not valid.");
        break;
    case ExtractionOperation::ParamErrorSplitDepth:
        _errorMessage = tr("The value of the depth field is invalid.");
        break;
    case ExtractionOperation::ParamErrorSplitType:
        _errorMessage = tr("The type of split not valid.");
        break;
    case ExtractionOperation::ParamErrorNoDeleteTextPath:
        _errorMessage = tr("Delete text has been seletced but no path or an invalid path was given.");
        break;
    case ExtractionOperation::ParamErrorBadOperationType:
        _errorMessage = tr("Unknown operation type.");
        break;
    case ExtractionOperation::ParamErrorFilesNamePattern:
        _errorMessage = tr("The output file name is not valid.");
        break;
    }
    Utils::error(this, _errorMessage);
    return false;
}

void ExtractFragmentsDialog::accept()
{
    fillOperationFromUI();
    _errorMessage = "" ;
    if(!checkOperationParameters()) {
        return ;
    }
    if(_operation.isFilterTextForPath()) {
        if(!Utils::askYN(this, tr("This operation will take out some text from the result. Do you want to continue?"))) {
            return ;
        }
    }
    _operation.saveSettings();
    ExtractionFrontEnd frontEnd(&_operation, this, _mainWidget);
    frontEnd.exec();
    QDialog::accept();
}

//------------------------- slots
void ExtractFragmentsDialog::on_extractAll_clicked()
{
    enableControls();
}

void ExtractFragmentsDialog::on_extractRange_clicked()
{
    enableControls();
}

void ExtractFragmentsDialog::on_extractWithCfr_clicked()
{
    enableControls();
}

void ExtractFragmentsDialog::on_splitByPath_clicked()
{
    enableControls();
}

void ExtractFragmentsDialog::on_splitByDepth_clicked()
{
    enableControls();
}

void ExtractFragmentsDialog::enableControls()
{
    bool isEnabledSplitByPath = ui->splitByPath->isChecked();
    bool enableExtraction = ui->chkExtractFiles->isChecked();
    bool isRange = ui->extractRange->isChecked();
    bool isCfr = ui->extractWithCfr->isChecked();
    bool isFilterTextUsingPath = ui->chkDeleteUsingPath->isChecked();
    ui->extractAll->setEnabled(enableExtraction);
    ui->extractRange->setEnabled(enableExtraction);
    ui->extractWithCfr->setEnabled(enableExtraction);
    ui->rangeMin->setEnabled(enableExtraction && isRange);
    ui->rangeMax->setEnabled(enableExtraction && isRange);
    ui->reverseRange->setEnabled(enableExtraction && isRange);
    ui->attrName->setEnabled(enableExtraction && isCfr);
    ui->cfrOperator->setEnabled(enableExtraction && isCfr);
    ui->compTerm->setEnabled(enableExtraction && isCfr);
    ui->extractDir->setEnabled(enableExtraction);
    ui->cmdDirScan->setEnabled(enableExtraction);
    ui->cmdViewInBrowser->setEnabled(enableExtraction);
    ui->pathToDeleteText->setEnabled(isFilterTextUsingPath);
    ui->makeSubfolders->setEnabled(enableExtraction);
    bool isMakeSubFolders = ui->makeSubfolders->isChecked();
    ui->eachNFiles->setEnabled(enableExtraction && isMakeSubFolders);
    ui->sfp1->setEnabled(enableExtraction && isMakeSubFolders);
    ui->sfp2->setEnabled(enableExtraction && isMakeSubFolders);
    ui->sfp3->setEnabled(enableExtraction && isMakeSubFolders);
    ui->sfp4->setEnabled(enableExtraction && isMakeSubFolders);
    ui->sfp5->setEnabled(enableExtraction && isMakeSubFolders);
    ui->sfn1->setEnabled(enableExtraction);
    ui->sfn2->setEnabled(enableExtraction);
    ui->sfn3->setEnabled(enableExtraction);
    ui->sfn4->setEnabled(enableExtraction);
    ui->sfn5->setEnabled(enableExtraction);

    ui->splitPath->setEnabled(isEnabledSplitByPath);
    ui->depth->setEnabled(!isEnabledSplitByPath);
}


void ExtractFragmentsDialog::on_makeSubfolders_clicked()
{
    enableControls();
}

void ExtractFragmentsDialog::on_cmdViewInBrowser_clicked()
{
    QDesktopServices::openUrl(QUrl::fromLocalFile(ui->extractDir->text()));
}

void ExtractFragmentsDialog::on_cmdDirScan_clicked()
{
    QString dirPath = QFileDialog::getExistingDirectory(this, tr("Choose the directory"),
                      ui->extractDir->text(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if(!dirPath.isEmpty()) {
        ui->extractDir->setText(dirPath);
    }
}

void ExtractFragmentsDialog::on_sourceFIleSearch_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this, tr("Open File"),
                       QXmlEditData::sysFilePathForOperation(ui->sourceFile->text()), Utils::getFileFilterForOpenFile());
    if(!filePath.isEmpty()) {
        ui->sourceFile->setText(filePath);
    }
}

void ExtractFragmentsDialog::on_chkExtractFiles_clicked()
{
    enableControls();
}

void ExtractFragmentsDialog::on_chkDeleteUsingPath_clicked()
{
    enableControls();
}


void ExtractFragmentsDialog::reject()
{
    _extractResult->setAborted(true);
    QDialog::reject();
}

QString ExtractFragmentsDialog::showSingleNaming(QStringList &names)
{
    QString title;
    title += _operation.makeAName("", 1, names, 1);
    return title;
}

void ExtractFragmentsDialog::addNamePattern(QStringList &list, const QString &str)
{
    if(!str.isEmpty()) {
        list.append(str);
    }
}

void ExtractFragmentsDialog::showNaming()
{
    QString naming = tr("Folder names are like: '");
    QStringList folderNames;
    addNamePattern(folderNames, ui->sfp1->currentText().trimmed());
    addNamePattern(folderNames, ui->sfp2->currentText().trimmed());
    addNamePattern(folderNames, ui->sfp3->currentText().trimmed());
    addNamePattern(folderNames, ui->sfp4->currentText().trimmed());
    addNamePattern(folderNames, ui->sfp5->currentText().trimmed());
    naming += showSingleNaming(folderNames);
    naming += tr("', files like: '");

    QStringList fileNames;
    addNamePattern(fileNames, ui->sfn1->currentText().trimmed());
    addNamePattern(fileNames, ui->sfn2->currentText().trimmed());
    addNamePattern(fileNames, ui->sfn3->currentText().trimmed());
    addNamePattern(fileNames, ui->sfn4->currentText().trimmed());
    addNamePattern(fileNames, ui->sfn5->currentText().trimmed());
    naming += showSingleNaming(fileNames);
    const ExtractionOperation::EOperationType currentOperation = static_cast<ExtractionOperation::EOperationType>(Utils::comboSelectedCodeAsInt(ui->cbOperationType, -1));
    if(currentOperation == ExtractionOperation::OperationExportAndGroupCSV) {
        naming += "'.csv";
    } else {
        naming += "'.xml";
    }
    ui->lblLike->setText(naming);
}

void ExtractFragmentsDialog::editTextChanged(const QString & /*text*/)
{
    if(delayTimer.isActive()) {
        delayTimer.stop();
    }
    delayTimer.start(UPDATE_TIMEOUT);
}

void ExtractFragmentsDialog::connectUpdates()
{
    connect(ui->sfp1, SIGNAL(editTextChanged(const QString &)), this, SLOT(editTextChanged(const QString &)));
    connect(ui->sfp2, SIGNAL(editTextChanged(const QString &)), this, SLOT(editTextChanged(const QString &)));
    connect(ui->sfp3, SIGNAL(editTextChanged(const QString &)), this, SLOT(editTextChanged(const QString &)));
    connect(ui->sfp4, SIGNAL(editTextChanged(const QString &)), this, SLOT(editTextChanged(const QString &)));
    connect(ui->sfp5, SIGNAL(editTextChanged(const QString &)), this, SLOT(editTextChanged(const QString &)));

    connect(ui->sfn1, SIGNAL(editTextChanged(const QString &)), this, SLOT(editTextChanged(const QString &)));
    connect(ui->sfn2, SIGNAL(editTextChanged(const QString &)), this, SLOT(editTextChanged(const QString &)));
    connect(ui->sfn3, SIGNAL(editTextChanged(const QString &)), this, SLOT(editTextChanged(const QString &)));
    connect(ui->sfn4, SIGNAL(editTextChanged(const QString &)), this, SLOT(editTextChanged(const QString &)));
    connect(ui->sfn5, SIGNAL(editTextChanged(const QString &)), this, SLOT(editTextChanged(const QString &)));

    connect(&delayTimer, SIGNAL(timeout()), this, SLOT(showNaming()));
}

void ExtractFragmentsDialog::on_cmdAdvanced_clicked()
{
    ExtractionAdavancedOptionsDialog advancedOptions(&_operation, this);
    advancedOptions.setModal(true);
    if(advancedOptions.exec() == QDialog::Accepted) {
        infoOnAdvancedOptions();
    }
}

void ExtractFragmentsDialog::infoOnAdvancedOptions()
{
    if(!_operation.isUseNamespaces() || !_operation.filtersId().isEmpty()) {
        ui->lblAdvancedOptions->setText(tr("options set"));
        ui->lblAdvancedOptions->setToolTip(tr("Advanced options present."));
    } else {
        ui->lblAdvancedOptions->setText("");
        ui->lblAdvancedOptions->setToolTip("");
    }
}
