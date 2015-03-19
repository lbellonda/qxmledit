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

#include "extractionfrontend.h"
#include "ui_extractionfrontend.h"
#include "utils.h"
#include <QTimer>
#include <QDesktopServices>
#include <QUrl>

const int POLL_TIMEOUT = 1500;

ExtractionFrontEnd::ExtractionFrontEnd(ExtractionOperation *operation, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ExtractionFrontEnd)
{
    _operation = operation;
    _running = false ;
    ui->setupUi(this);
    _running = true ;
    _future = QtConcurrent::run(this, &ExtractionFrontEnd::extractFragmentsWorkThread);
    QTimer::singleShot(200, this, SLOT(checkIfDone()));

    // setup values
    ui->inputFile->setText(_operation->inputFile());
}

ExtractionFrontEnd::~ExtractionFrontEnd()
{
    delete ui;
}

void ExtractionFrontEnd::checkIfDone()
{
    if(!_future.isStarted()) {
        Utils::error(this, tr("Error starting operation."));
        reject();
        return ;
    }
    if(_future.isRunning()) {
        int counterDocumentsFound ;
        int counterFoldersCreated ;
        int counterOperations  ;

        QString fld ;
        _operation->_mutex.lock();
        fld = _operation->currentSubFolder ;
        counterDocumentsFound = _operation->counterDocumentsFound ;
        counterFoldersCreated = _operation->counterFoldersCreated;
        counterOperations = _operation->counterOperations ;
        _operation->_mutex.unlock();
        //----------------------------
        ui->currentSubfolder->setText(fld);
        ui->documentsNumber->setText(QString::number(counterDocumentsFound));
        ui->numFoldersCreated->setText(QString::number(counterFoldersCreated));
        ui->operationCount->setText(QString::number(counterOperations));

        QTimer::singleShot(POLL_TIMEOUT, this, SLOT(checkIfDone()));
    } else {
        endOfOperation();
    }
}

void ExtractionFrontEnd::endOfOperation()
{
    _running = false;
    if(_operation->isAborted()) {
        Utils::message(this, tr("User abort requested"));
        reject();
    } else {
        if(_operation->isError()) {
            Utils::error(this, tr("Error: %1, '%2'").arg(_operation->error()).arg(_operation->errorMessage()));
            reject();
        } else {
            if(_operation->isExtractDocuments()) {
                if(Utils::askYN(tr("Operation terminated.\nDo you want to show the extraction folder in the browser?"))) {
                    QDesktopServices::openUrl(QUrl::fromLocalFile(_operation->extractFolder()));
                }
            } else {
                Utils::message(this, tr("Operation terminated."));
            }
            accept();
        }
    }
}


void ExtractionFrontEnd::extractFragmentsWorkThread()
{
    _operation->performExtraction();
    return ;
}

void ExtractionFrontEnd::on_cmdCancel_clicked()
{
    if(_running) {
        //_operation._mutex.lock();
        _operation->setAborted(true) ;
        //_operation._mutex.unlock();
        if(_future.isStarted() && _future.isRunning()) {
            _future.waitForFinished();
        }
        endOfOperation();
        _running = false ;
    } else {
        rejected();
    }
}
