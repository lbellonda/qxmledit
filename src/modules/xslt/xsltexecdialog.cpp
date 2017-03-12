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

#include "xsltexecdialog.h"
#include "mainwindow.h"
#include "modules/xslt/xsltexecutor.h"
#include "modules/xslt/xslparametermanager.h"
#include "modules/services/systemservices.h"
#include "modules/xslt/showxslerrorsdialog.h"
#include "ui_xsltexecdialog.h"
#include "utils.h"

bool XSLTExecDialog::execDialog(QWidget *parent, ApplicationData *data)
{
    if(!data->isUseSaxonXSL()) {
        Utils::warning(parent, tr("WARNING: the support to XSL-T in Qt is currently experimental: the support to XSL is not complete.\nYou can use SAXON or another Java jar with same command line syntax (not part of QXmlEdit) if you configure it in preferences."));
    }
    XSLTExecDialog dialog(parent, data);
    if(dialog.exec() == QDialog::Accepted) {
        return true ;
    }
    return false ;
}

XSLTExecDialog::XSLTExecDialog(QWidget *parent, ApplicationData *data) :
    QDialog(parent),
    _data(data),
    ui(new Ui::XSLTExecDialog)
{
    _inputDataAsFile = true;
    _inputXSLAsFile = true;
    _inputEditor = NULL ;
    _xslEditor = NULL ;
    _outputAsFile = false;
    _running = false;
    ui->setupUi(this);
    finishSetup();
    connect(ui->buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(onBBOXlicked(QAbstractButton*)));
}

XSLTExecDialog::~XSLTExecDialog()
{
    delete ui;
}

void XSLTExecDialog::finishSetup()
{
    Utils::TODO_THIS_RELEASE("quando scelgo file in input aggiungo la cartella ai dati in output");
    //--
    ui->outputName->setText("");
    ui->inputFile->setText("");
    ui->xslName->setText("");
    //--
    ui->params->setColumnCount(2);
    QStringList headers ;
    headers << tr("Name") << tr("Value");
    ui->params->setHorizontalHeaderLabels(headers);
    ui->params->horizontalHeader()->setStretchLastSection(true);
    ui->params->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->params->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->params->setTextElideMode(Qt::ElideMiddle);
    loadSources();
    evaluateParamEnabling();
}

void XSLTExecDialog::loadSources()
{
    setupRecentFolders(ui->cmdInputFile, &_utilsInput, false, SLOT(onChooseInput()));
    setupRecentFolders(ui->cmdOutputFile, &_utilsOutput, true, SLOT(onChooseOutput()));
    setupRecentFolders(ui->cmdChooseXSL, &_utilsXSL, false, SLOT(onChooseXSL()));
}

void XSLTExecDialog::setupRecentFolders(QToolButton *button, ComboUtils *util, const bool isSave, const char *method)
{
    QList<XmlEditWidget *> widgets ;
    foreach(MainWindow *w, _data->windows()) {
        widgets.append(w->getEditor());
    }
    util->setupItemsForFile(_data, widgets, !_data->isUseSaxonXSL(), isSave);
    util->loadButtonMenu(button, this, method);
}

bool XSLTExecDialog::existsEditor(XmlEditWidget *editor)
{
    foreach(MainWindow *w, _data->windows()) {
        if(w->getEditor() == editor) {
            return true;
        }
    }
    return false;
}

void XSLTExecDialog::accept()
{
    startOperation();
}

void XSLTExecDialog::startOperation()
{
    setEnabled(false);
    XSLTExecutor xsltExecutor(_data) ;

    if(!setInput(&xsltExecutor)) {
        setEnabled(true);
        return ;
    }
    ui->stackedWidget->setCurrentIndex(ProgressView);
    // disable apply
    ui->buttonBox->setEnabled(false);
    _running = true ;

    MessagesOperationResult result;
    QHash<QString, QString> params = getParameters();
    xsltExecutor.setParameters(params);

    if(!xsltExecutor.exec(result)) {
        Utils::error(this, tr("Error executing xsl."));
        ShowXSLErrorsDialog::showErrors(this, &result);
    } else {
        if(_outputAsFile) {
            if(Utils::askYN(this, tr("Do you want to open the result file in the editor?"))) {
                openFileInEditor(_outputFile);
            } else if(Utils::askYN(this, tr("Do you want to open the destination folder?"))) {
                openFolder(_outputFile);
            }
        } else {
            openStringInEditor(_outputString);
        }
    }
    _running = false ;
    ui->stackedWidget->setCurrentIndex(NormalView);
    // disable apply
    ui->buttonBox->setEnabled(true);
    setEnabled(true);
}

void XSLTExecDialog::openFileInEditor(const QString &filePath)
{
    MainWindow *window = _data->newWindow();
    if(!window->loadFile(filePath, true, MainWindow::OpenUsingNewWindow)) {
        Utils::error(this, tr("Error opening file '%1'").arg(filePath));
        window->close();
    }
}

void XSLTExecDialog::openFolder(const QString &outputFilePath)
{
    if(!QDesktopServices::openUrl(QUrl::fromLocalFile(outputFilePath))) {
        Utils::error(this, tr("Error opening folder."));
    }
}

void XSLTExecDialog::openStringInEditor(const QString &output)
{
    MainWindow *window = _data->newWindow();
    window->newFromString(output);
}

void XSLTExecDialog::setInputEditor(XmlEditWidget *editor)
{
    _inputDataAsFile = false;
    _inputEditor = editor;
}

void XSLTExecDialog::setInputDataFile(const QString &fileName)
{
    _inputData = fileName ;
    _inputDataAsFile = true;
}

void XSLTExecDialog::setXSLEditor(XmlEditWidget *editor)
{
    _xslEditor = editor ;
    _inputXSLAsFile = false;
}

void XSLTExecDialog::setXSLFile(const QString &fileName)
{
    _inputXSL = fileName ;
    _inputXSLAsFile = true;
}

bool XSLTExecDialog::setInput(XSLTExecutor *xsltExecutor)
{
    if(_inputDataAsFile) {
        if(_inputData.isEmpty()) {
            errorEmptyFileString(tr("source"));
            return false;
        }
        xsltExecutor->setInputFile(_inputData);
    } else {
        if(existsEditor(_inputEditor)) {
            QString regolaString = _inputEditor->getRegola()->getAsText();
            xsltExecutor->setInputLiteral(regolaString);
        } else {
            errorNotEditor(tr("source"));
            loadSources();
            return false;
        }
    }
    if(_inputXSLAsFile) {
        if(_inputXSL.isEmpty()) {
            errorEmptyFileString(tr("xsl"));
            return false;
        }
        xsltExecutor->setXSLFile(_inputXSL);
    } else {
        if(existsEditor(_xslEditor)) {
            QString regolaString = _xslEditor->getRegola()->getAsText();
            xsltExecutor->setXSLLiteral(regolaString);
        } else {
            errorNotEditor(tr("xsl"));
            loadSources();
            return false;
        }
    }
    if(_outputAsFile) {
        if(_outputFile.isEmpty()) {
            errorEmptyFileString(tr("output"));
            return false;
        }
        xsltExecutor->setOutput(_outputFile);
    } else {
        xsltExecutor->setOutput(&_outputString);
    }

    return true;
}

QString XSLTExecDialog::fileForSelection(ComboUtils::ComboItem *item, const QString &message, const bool isOpen)
{
    if((ComboUtils::TypeFile == item->code) && !item->dataString.isEmpty()) {
        return item->dataString ;
    }
    QString folder = SystemServices::userDocumentsDirectory();
    if(!item->dataString.isEmpty()) {
        folder = item->dataString ;
    }
    QString filePath ;
    if(isOpen) {
        filePath = QFileDialog::getOpenFileName(this, message,
                                                folder, Utils::getFileFilterForOpenFile());
    } else {
        filePath = QFileDialog::getSaveFileName(this, message,
                                                folder, Utils::getFileFilterForOpenFile());
    }
    return filePath;
}

void XSLTExecDialog::errorNotEditor(const QString &type)
{
    Utils::error(this, tr("The editor for %1 do not exists anymore.").arg(type));
}

void XSLTExecDialog::errorEmptyFileString(const QString &type)
{
    Utils::error(this, tr("Empty %1 file.").arg(type));
}

void XSLTExecDialog::onChooseInput()
{
    QAction *action = qobject_cast<QAction*>(sender());
    if(NULL != action) {
        ComboUtils::ComboItem *item = ComboUtils::actionData(action) ;
        if(NULL != item) {
            if(ComboUtils::TypeEditor == item->code) {
                XmlEditWidget *editor = static_cast<XmlEditWidget*>(item->data);
                if(existsEditor(editor)) {
                    setInputEditor(editor);
                    QString newText = ComboUtils::titleForEditor(editor);
                    ui->inputFile->setText(newText);
                } else {
                    errorNotEditor(tr("source"));
                    loadSources();
                }
            } else {
                QString filePath = fileForSelection(item, tr("Source File"), true);
                if(!filePath.isEmpty()) {
                    setInputDataFile(filePath);
                    ui->inputFile->setText(filePath);
                }
            }
        }
    }
}

void XSLTExecDialog::onChooseXSL()
{
    QAction *action = qobject_cast<QAction*>(sender());
    if(NULL != action) {
        ComboUtils::ComboItem *item = ComboUtils::actionData(action) ;
        if(NULL != item) {
            QString newText;
            bool isNewData = false ;
            if(ComboUtils::TypeEditor == item->code) {
                XmlEditWidget *editor = static_cast<XmlEditWidget*>(item->data);
                if(existsEditor(editor)) {
                    setXSLEditor(editor);
                    newText = ComboUtils::titleForEditor(editor);
                    isNewData = true ;
                } else {
                    errorNotEditor(tr("xsl"));
                    loadSources();
                }
            } else {
                QString filePath = fileForSelection(item, tr("XSL File"), true);
                if(!filePath.isEmpty()) {
                    setXSLFile(filePath);
                    newText = filePath ;
                    isNewData = true ;
                }
            }
            if(isNewData) {
                ui->xslName->setText(newText);
                mergeParameters();
            }
        }
    }
}

void XSLTExecDialog::onChooseOutput()
{
    QAction *action = qobject_cast<QAction*>(sender());
    if(NULL != action) {
        ComboUtils::ComboItem *item = ComboUtils::actionData(action) ;
        if(NULL != item) {
            if(ComboUtils::TypeOutputEditor == item->code) {
                _outputAsFile = false;
                ui->outputName->setText(item->text);
            } else {
                QString filePath = fileForSelection(item, tr("Output File"), false);
                if(!filePath.isEmpty()) {
                    _outputAsFile = true;
                    _outputFile = filePath;
                    ui->outputName->setText(filePath);
                }
            }
        }
    }
}

void XSLTExecDialog::onBBOXlicked(QAbstractButton* button)
{
    if(NULL != button) {
        if(ui->buttonBox->buttonRole(button) == QDialogButtonBox::ApplyRole) {
            accept();
        }
    }
}

void XSLTExecDialog::mergeParameters()
{
    XSLParameterManager manager;
    bool ok = false;
    if(_inputXSLAsFile) {
        ok = manager.extractParametersFromFile(_inputXSL);
    } else {
        if(existsEditor(_xslEditor)) {
            setXSLEditor(_xslEditor);
            QString regolaString = _xslEditor->getRegola()->getAsText();
            ok = manager.extractParametersFromString(regolaString);
        }
    }
    if(ok) {
        const int rows = ui->params->rowCount();
        QSet<QString> toAddItems ;
        _parameters = manager.parameters() ;
        toAddItems.unite(_parameters);
        FORINT(row, rows) {
            QTableWidgetItem *item = ui->params->item(row, 0);
            toAddItems.remove(item->text());
        }
        foreach(const QString &key, toAddItems) {
            addParamRow(key);
        }
    } else {
        Utils::error(this, tr("Error retrieving parameters."));
    }
}

void XSLTExecDialog::on_params_itemSelectionChanged()
{
    evaluateParamEnabling();
}

void XSLTExecDialog::evaluateParamEnabling()
{
    bool isSelection = ui->params->currentRow() != -1 ;
    ui->cmdModParam->setEnabled(isSelection);
    ui->cmdDelParam->setEnabled(isSelection);
}

void XSLTExecDialog::on_cmdInsParam_clicked()
{
    const int row = addParamRow("");
    ui->params->setCurrentCell(row, 0);
    on_cmdModParam_clicked();
}

void XSLTExecDialog::on_cmdModParam_clicked()
{
    const int row = ui->params->currentRow();
    if(row != -1) {
        ui->params->editItem(ui->params->item(row, 1));
    }
}

void XSLTExecDialog::on_cmdDelParam_clicked()
{
    const int row = ui->params->currentRow() ;
    bool isSelection = row != -1 ;
    if(isSelection) {
        ui->params->removeRow(row);
    }
}

void XSLTExecDialog::on_cmdResetParam_clicked()
{
    resetParameters();
}

void XSLTExecDialog::resetParameters()
{
    QSet<QString> existing;
    const int rows = ui->params->rowCount();
    for(int row = rows - 1 ; row >= 0 ; row --) {
        QTableWidgetItem *item = ui->params->item(row, 0);
        if(!_parameters.contains(item->text())) {
            ui->params->removeRow(row);
        } else {
            existing.insert(item->text());
        }
    }
    foreach(const QString &key, _parameters) {
        if(!existing.contains(key)) {
            addParamRow(key);
        }
    }
}

int XSLTExecDialog::addParamRow(const QString &text, const QString &value)
{
    const int rowCount = ui->params->rowCount();
    ui->params->setRowCount(rowCount + 1);
    QTableWidgetItem *item = new QTableWidgetItem(text);
    ui->params->setItem(rowCount, 0, item);
    QTableWidgetItem *item1 = new QTableWidgetItem(value);
    ui->params->setItem(rowCount, 1, item1);
    return rowCount ;
}

QHash<QString, QString> XSLTExecDialog::getParameters()
{
    QHash<QString, QString> result ;
    const int rows = ui->params->rowCount();
    FORINT(row, rows) {
        QTableWidgetItem *item = ui->params->item(row, 0);
        QString key = item->text().trimmed();
        item = ui->params->item(row, 1);
        QString value = item->text().trimmed();
        if(!key.isEmpty()) {
            result[key] = value ;
        }
    }
    return result ;
}
