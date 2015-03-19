/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2011 by Luca Bellonda and individual contributors       *
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
/*****************************************************************************************************
the procedure to make classification work is based on the fact that the items of the
path are visited in order.
Initially all the counters are set to zero.
When we encounter the father , we reset the child
When we run into an end element, we close the item-1 counting and we reset the item
Next time we access an element, all counters are consitent.
*******************************************************************************************************/

#include "searchinfiles.h"
#include "ui_searchinfiles.h"
#include "xmlEdit.h"
#include "qxmleditdata.h"
#include "utils.h"
#include "config.h"

const int POLL_TIMEOUT = 500 ;
const int MAX_PATTERN_SIZE = 10 ;

void searchInFiles(QWidget * parent)
{
    SearchInFiles edit(parent);
    edit.exec() ;
}


SearchInFiles::SearchInFiles(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SearchInFiles)
{
    numberIcon.addPixmap(QPixmap(QString::fromUtf8(":/editText/number")), QIcon::Normal, QIcon::Off);
    groupIcon.addPixmap(QPixmap(QString::fromUtf8(":/editText/group")), QIcon::Normal, QIcon::Off);
    running = false ;
    ui->setupUi(this);
    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    if(Config::loadStringArray(Config::KEY_SEARCHINFILES_INPUTFILE, previousSearch)) {
        foreach(QString item, previousSearch) {
            ui->pattern2Search->addItem(item);
        }
    }
    ui->filePath->setText(Config::getString(Config::KEY_SEARCHINFILES_PATTERN, ""));
    ui->progressBar->setTextVisible(false);
    ui->groupChk->setChecked(Config::getBool(Config::KEY_SEARCHINFILES_GROUP, false));
    enableButtons(true);
    clearTable();
}

SearchInFiles::~SearchInFiles()
{
    delete ui;
}

void SearchInFiles::clearTable()
{
    ui->results->clear();
    ui->results->setColumnCount(2);
    ui->results->setHorizontalHeaderItem(0, new QTableWidgetItem(tr("Item")));
    ui->results->setHorizontalHeaderItem(1, new QTableWidgetItem(tr("Occurrences")));
}


void SearchInFiles::on_cmdOpenFile_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this, tr("File to scan"),
                       QXmlEditData::sysFilePathForOperation(ui->filePath->text()), tr("XML files (*.xml);;All files (*);;"));
    if(!filePath.isEmpty()) {
        ui->filePath->setText(filePath);
    }
}

void SearchInFiles::on_cmdStart_clicked()
{
    startProcessing();
}

void SearchInFiles::on_cmdCancel_clicked()
{
    if(running) {
        info.isAbort = true ;
        if(QThreadPool::globalInstance()->activeThreadCount() > 0) {
            QThreadPool::globalInstance()->waitForDone();
        }
        enableButtons(false);
        running = false ;
    }
}


void SearchInFiles::startProcessing()
{
    bool isGroup = ui->groupChk->isChecked() ;
    QString pattern = ui->pattern2Search->currentText();
    if(!info.init(pattern, isGroup)) {
        Utils::error(tr("Unable to begin the search"));
        return ;
    }
    if(!info.check()) {
        Utils::error(tr("The pattern to search is not well formed."));
        return ;
    }

    Config::saveString(Config::KEY_SEARCHINFILES_PATTERN, ui->filePath->text());

    if(!previousSearch.contains(pattern)) {
        previousSearch.insert(0, pattern);
        if(previousSearch.size() > MAX_PATTERN_SIZE) {
            previousSearch.removeAt(previousSearch.size() - 1);
        }
        Config::saveStringArray(Config::KEY_SEARCHINFILES_INPUTFILE, previousSearch);
    }

    Config::saveBool(Config::KEY_SEARCHINFILES_GROUP, isGroup);
    clearTable();

    enableButtons(false);
    running = true ;
    /*QFuture<void> future = */QtConcurrent::run(searchInFileWorking, &running, &info, ui->filePath->text());

    checkIfDone();
}

void SearchInFiles::closeEvent(QCloseEvent *event)
{
    on_cmdCancel_clicked();
    event->accept();
}

void SearchInFiles::checkIfDone()
{
    QThreadPool *gInst = QThreadPool::globalInstance();
    if(gInst->activeThreadCount() > 0) {
        QTimer::singleShot(POLL_TIMEOUT, this, SLOT(checkIfDone()));
        ui->progressBar->setValue((ui->progressBar->value() + 5) % 100);
    } else {
        endOfSearch();
    }
}

void SearchInFiles::enableButtons(const bool how)
{
    this->ui->cmdStart->setEnabled(how);
    this->ui->cmdStart->setVisible(how);
    this->ui->cmdCancel->setEnabled(!how);
    this->ui->cmdCancel->setVisible(!how);
    this->ui->progressBar->setVisible(!how);
    this->ui->progressBar->setValue(0);
    this->ui->frame->setEnabled(how);
}

void SearchInFiles::endOfSearch()
{
    running = false;
    enableButtons(true);
    clearTable();
    if(! info.isError && !info.isAbort) {
        int numRows = info.size ;
        ui->results->setRowCount(numRows);
        for(int i = 0 ; i < info.size ; i ++) {
            ui->results->setItem(i, 0, new QTableWidgetItem(numberIcon, info.tokens.at(i))) ;
            ui->results->setItem(i, 1, new QTableWidgetItem(QString("%1\n").arg(info.occurrences[i])));
        }
        int index = numRows;
        if(info.isGroup) {
            for(int i = 0 ; i < info.size ; i ++) {
                numRows += info.groups[i].count();
                ui->results->setRowCount(numRows);
                QMapIterator<int, int> iterator(info.groups[i]);
                // build the string
                QString itemId ;
                for(int ic = 0 ; ic <= i ; ic ++) {
                    itemId += "/" ;
                    itemId += info.tokens.at(ic) ;
                }
                while(iterator.hasNext()) {
                    iterator.next();
                    ui->results->setItem(index, 0, new QTableWidgetItem(groupIcon,  tr("Gr.%1 - %2 having count: %3").arg(i).arg(itemId).arg(iterator.key())));
                    ui->results->setItem(index, 1, new QTableWidgetItem(QString("%1\n").arg(iterator.value())));
                    index ++ ;
                }
            }
        }
        ui->results->resizeColumnsToContents();
        ui->results->resizeRowsToContents();
    } else {
        if(info.isAbort) {
            Utils::message(tr("Operation aborted by user."));
        } else if(info.isError) {
            Utils::error(tr("Operation interrupted with error :'%1'").arg(info.errorMessage));
        }
    }
}

void SearchInFiles::searchInFileWorking(volatile bool *running, XmlScanInfo *info, const QString &filePath)
{
    if(!*running) {
        info->isError = true ;
        return  ;
    }

    ScanSax handler(*info);
    QXmlSimpleReader reader;
    reader.setFeature("http://xml.org/sax/features/namespaces", false);
    reader.setFeature("http://xml.org/sax/features/namespace-prefixes", true);
    reader.setContentHandler(&handler);
    reader.setErrorHandler(&handler);

    QFile file(filePath);
    if(!file.open(QFile::ReadOnly | QFile::Text)) {
        info->isError = true ;
        info->errorMessage = tr("Error opening input file.");
        return ;
    }
    QXmlInputSource xmlInput(&file);
    if(!reader.parse(xmlInput)) {
        info->isError = true ;
    }
    file.close();
    return ;
}


void SearchInFiles::accept()
{
    on_cmdCancel_clicked();
    QDialog::accept();
}

void SearchInFiles::reject()
{
    on_cmdCancel_clicked();
    QDialog::reject();
}
