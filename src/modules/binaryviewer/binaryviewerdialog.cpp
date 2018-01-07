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


#include "binaryviewerdialog.h"
#include "ui_binaryviewerdialog.h"
#include <QFileDialog>
#include "qxmleditdata.h"
#include "qxmleditconfig.h"
#include "utils.h"
#include <QFileInfo>

BinaryViewerDialog::BinaryViewerDialog(QStringList recentFiles, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BinaryViewerDialog)
{
    ui->setupUi(this);
    finishSetup(recentFiles);
    start();
}

BinaryViewerDialog::~BinaryViewerDialog()
{
    delete ui;
}


void BinaryViewerDialog::finishSetup(QStringList recentFiles)
{
    _recentFiles = recentFiles ;
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    Utils::setupComboEncoding(ui->cmbEncodings);
    QString selection = Config::getString(Config::KEY_BINARY_ENCODING, "ISO-8859-15");
    Utils::selectComboText(ui->cmbEncodings, selection);
    connect(&_model, SIGNAL(pageChanged(int)), this, SLOT(onCurrentPageChanged(int)));
    setAcceptDrops(true);

    ui->fileComboBox->setEnabled(false);
    ui->fileComboBox->addItems(_recentFiles);
    ui->fileComboBox->setCurrentIndex(-1);
    ui->fileComboBox->setEnabled(true);

    enableSearch();
    calcEnablePages();
    enableGoto();
}

void BinaryViewerDialog::start()
{
    _model.setCodecByName(ui->cmbEncodings->currentText());
    ui->tableView->setVisible(true);
    calcEnablePages();
    enableSearch();
    enableGoto();
}

void BinaryViewerDialog::on_cmdBrowse_clicked()
{
    QString startPath;
    if(_recentFiles.size() > 0) {
        startPath = _recentFiles.at(0);
    }

    QString filePath = QFileDialog::getOpenFileName(
                           this, tr("Open File"),
                           QXmlEditData::sysFilePathForOperation(startPath),
                           Utils::getFileFilterForOpenFile());
    if(!filePath.isEmpty()) {
        if(!_recentFiles.contains(filePath)) {
            _recentFiles.removeOne(filePath);
        }
        _recentFiles.insert(0, filePath);
    }
    ui->fileComboBox->clear();
    ui->fileComboBox->addItems(_recentFiles);
    ui->fileComboBox->setCurrentIndex(0);
    assignFile(filePath);
}

void BinaryViewerDialog::on_fileComboBox_activated(const QString & text)
{
    assignFile(text);
}

void BinaryViewerDialog::setNullModel()
{
    ui->tableView->setModel(NULL);
    ui->tableView->setEnabled(false);
    ui->lblFileSize->setText("");
    ui->lblFileUpdatedOn->setText("");
    calcEnablePages();
    enableSearch();
    enableGoto();
}

void BinaryViewerDialog::assignFile(const QString &filePath)
{
    setEnabled(false);

    QFile *file = new QFile(filePath);
    QFileInfo fileInfo(filePath);
    qint64 fileSize = fileInfo.size();
    QDateTime dtLastMod = fileInfo.lastModified();
    QString dateMod = dtLastMod.toString(Qt::DefaultLocaleLongDate);
    // update labels
    ui->lblFileSize->setText(tr("%1 bytes").arg(fileSize));
    ui->lblFileUpdatedOn->setText(dateMod);
    assignIO(file);
    setEnabled(true);
}

void BinaryViewerDialog::assignIO(QIODevice *ioDevice)
{
    BinaryViewerModel::ErrCodes resultCode = _model.setFile(ioDevice) ;
    if(BinaryViewerModel::EC_NOERROR == resultCode) {
        ui->tableView->setUpdatesEnabled(false);
        ui->tableView->setModel(NULL);
        ui->tableView->setModel(&_model);
        ui->tableView->resizeColumnsToContents();
        ui->tableView->setUpdatesEnabled(true);
        connect(ui->tableView->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
                this, SLOT(selectionChangedBinaryRow(const QItemSelection&, const QItemSelection&)));
        calcEnablePages();
        enableSearch();
        enableGoto();
    } else {
        Utils::error(this, tr("Error opening file, code:%1").arg(resultCode));
        setNullModel();
    }
}

void BinaryViewerDialog::calcEnablePages()
{
    if(NULL == ui->tableView->model()) {
        ui->cmdPageNext->setEnabled(false);
        ui->cmdPagePrev->setEnabled(false);
        ui->lblPage->setText(tr("No data exists."));
        ui->tableView->setEnabled(false);
        return ;
    }
    ui->tableView->setEnabled(true);
    bool enablePrevPage = true;
    bool enableNextPage = true;
    if(_model.isFirstPage()) {
        enablePrevPage = false;
    }
    if(_model.isLastPage()) {
        enableNextPage = false;
    }
    ui->cmdPageNext->setEnabled(enableNextPage);
    ui->cmdPagePrev->setEnabled(enablePrevPage);
    if(_model.numPages() == 0) {
        ui->lblPage->setText(tr("No data exists."));
    } else {
        QString lblText = tr("Page %1 of %2").arg(_model.currentPage() + 1).arg(_model.numPages());
        ui->lblPage->setText(lblText);
    }
}

void BinaryViewerDialog::moveToPage(const bool isNext)
{
    setEnabled(false);
    _model.goToPage(isNext);
    ui->tableView->setUpdatesEnabled(false);
    ui->tableView->setModel(NULL);
    ui->tableView->setModel(&_model);
    ui->tableView->setUpdatesEnabled(true);
    ui->tableView->resizeColumnsToContents();
    calcEnablePages();
    setEnabled(true);
}

void BinaryViewerDialog::moveToPageAbs(const int newPage)
{
    _model.goToPageAbs(newPage);
    ui->tableView->setUpdatesEnabled(false);
    ui->tableView->setModel(NULL);
    ui->tableView->setModel(&_model);
    ui->tableView->setUpdatesEnabled(true);
    ui->tableView->resizeColumnsToContents();
    calcEnablePages();
}

void BinaryViewerDialog::on_cmdPageNext_clicked()
{
    moveToPage(true);
}

void BinaryViewerDialog::on_cmdPagePrev_clicked()
{
    moveToPage(false);
}

void BinaryViewerDialog::onCurrentPageChanged(int /*currentPage*/)
{
    calcEnablePages();
}

void BinaryViewerDialog::on_cmbEncodings_activated(const QString & text)
{
    QString codecName = text ;
    _model.setCodecByName(codecName);
    Config::saveString(Config::KEY_BINARY_ENCODING, text);
    refreshData();
}

void BinaryViewerDialog::refreshData()
{
    setEnabled(false);
    ui->tableView->setUpdatesEnabled(false);
    ui->tableView->setModel(NULL);
    ui->tableView->setModel(&_model);
    ui->tableView->resizeColumnsToContents();
    ui->tableView->setUpdatesEnabled(true);
    setEnabled(true);
}

void BinaryViewerDialog::enableSearch()
{
    if(NULL == ui->tableView->model()) {
        ui->cmdSearchNext->setEnabled(false);
        ui->cmdSearchPrev->setEnabled(false);
        return ;
    }
    bool enable = !ui->searchBox->text().isEmpty();
    bool isFirstRow = selRow() == 0;
    bool isLastRow = _model.isLastPage() && (selRow() == lastRow());
    ui->cmdSearchNext->setEnabled(enable && !isLastRow);
    ui->cmdSearchPrev->setEnabled(enable && !isFirstRow);
}

void BinaryViewerDialog::on_searchBox_textChanged(const QString & /*text*/)
{
    enableSearch();
}

void BinaryViewerDialog::on_cmdSearchNext_clicked()
{
    search(true);
}

void BinaryViewerDialog::on_cmdSearchPrev_clicked()
{
    search(false);
}

void BinaryViewerDialog::search(const bool searchForward)
{
    setEnabled(false);
    BinaryViewerOperationResult result;
    QString text = ui->searchBox->text();
    if(!text.isEmpty()) {
        int selLine = selRow();
        if(selLine < 0) {
            selLine = 0 ;
        }
        bool found = _model.findOccurrences(result, text, _model.currentPage(), selLine, searchForward);
        if(!result.ok) {
            Utils::error(this, tr("Error while searching :%1.").arg(result.errorCode));
        } else {
            if(found) {
                moveToPageAbs(result.page);
                selectRowAndEnsureIsVisible(result.row);
            } else {
                Utils::message(this, tr("Text not found."));
            }
        }
    } else {
        Utils::error(this, tr("Insert a text to search."));
    }
    setEnabled(true);
}


int BinaryViewerDialog::selRow()
{
    QModelIndexList list = ui->tableView->selectionModel()->selectedRows();
    if(list.isEmpty()) {
        return -1 ;
    }
    QModelIndex index = list.at(0);
    return index.row();
}

int BinaryViewerDialog::lastRow()
{
    return ui->tableView->model()->rowCount() - 1;
}

void BinaryViewerDialog::selectRowAndEnsureIsVisible(const int newRow, const bool setIsVisible)
{
    ui->tableView->selectRow(newRow);
    if(setIsVisible) {
        QModelIndex rowIndex =  _model.index(newRow, 0, QModelIndex());
        ui->tableView->scrollTo(rowIndex);
    }
}

void BinaryViewerDialog::selectionChangedBinaryRow(const QItemSelection& /*selected*/, const QItemSelection& /*deselected*/)
{
    if(NULL == ui->tableView->model()) {
        return ;
    }
    enableSearch();
}

void BinaryViewerDialog::msgNoModel()
{
    Utils::error(this, tr("Do data exist."));
}

void BinaryViewerDialog::msgInvalidAddress()
{
    Utils::error(this, tr("The specified address is not valid."));
}

void BinaryViewerDialog::on_cmdGoToAddress_clicked()
{
    if(NULL == ui->tableView->model()) {
        msgNoModel();
        return ;
    }
    QString addressText = ui->gotoAddress->text().trimmed();
    if(addressText.isEmpty()) {
        msgInvalidAddress();
        return ;
    }
    int base = 10;
    if(ui->chkHexOption->isChecked()) {
        base = 16 ;
    }
    bool ok = true ;
    quint64 address = addressText.toULongLong(&ok, base);
    if(!ok) {
        msgInvalidAddress();
        return ;
    }
    BinaryViewerOperationResult result;
    _model.findPageOfAddress(result, address);
    if(!result.ok) {
        msgInvalidAddress();
        return ;
    }
    moveToPageAbs(result.page);
    selectRowAndEnsureIsVisible(result.row);
}

void BinaryViewerDialog::on_gotoAddress_textChanged(const QString & /*text*/)
{
    enableGoto();
}

void BinaryViewerDialog::enableGoto()
{
    ui->cmdGoToAddress->setEnabled(!ui->gotoAddress->text().isEmpty() && (NULL != ui->tableView->model())) ;
}

void BinaryViewerDialog::keyPressEvent(QKeyEvent *e)
{
    int key = e->key() ;
    if((Qt::Key_Enter == key) || (Qt::Key_Return == key)) {
        QWidget *focus = focusWidget();
        if(focus == ui->searchBox) {
            ui->cmdSearchNext->animateClick();
            return;
        }
        if(focus == ui->gotoAddress) {
            ui->cmdGoToAddress->animateClick();
            return;
        }
    }
    QDialog::keyPressEvent(e);
}

void BinaryViewerDialog::dragEnterEvent(QDragEnterEvent *event)
{
    if(event->mimeData()->hasFormat("text/uri-list")) {
        event->acceptProposedAction();
    }
}

void BinaryViewerDialog::dropEvent(QDropEvent *event)
{
    if(event->mimeData()->hasFormat("text/uri-list")) {
        QString filePath = "" ;
        event->acceptProposedAction();
        if(event->mimeData()->hasUrls()) {
            foreach(QUrl url, event->mimeData()->urls()) {
                filePath = url.toLocalFile();
                break;
            }
        }
        if(filePath.length() > 0) {
            assignFile(filePath);
        }
        event->acceptProposedAction();
    }
}
