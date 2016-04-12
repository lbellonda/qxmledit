/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2013-2016 by Luca Bellonda and individual contributors  *
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

#include "xmlEdit.h"
#include "nodesrelationsdialog.h"
#include "ui_nodesrelationsdialog.h"
#include "utils.h"
#include "nodessax.h"
#include "qxmleditdata.h"

#include    <QFileDialog>
#include    <QDragEnterEvent>
#include    <QDropEvent>
#include    <QUrl>
#include    <QDateTime>

//----------------------------------------------------------------------------------------------------------------

NodesRelationsDialog::NodesRelationsDialog(const bool newCanLoadData, QList<TagNode*> &dataList, QWidget *parent) :
    QDialog(parent),
    controller(this),
    ui(new Ui::NodesRelationsDialog)
{
    timerInterval = TimerInterval ;
    started = false;
    isTest = false;
    canLoadData = newCanLoadData ;
    Utils::addMaximizeToDialog(this);

    ui->setupUi(this);

    ui->lblDamping->setVisible(false);
    ui->dampingSlider->setVisible(false);
    ui->dampingSlider->setEnabled(false);
    ui->lblStiffness->setVisible(false);
    ui->stiffnessSlider->setVisible(false);
    ui->stiffnessSlider->setEnabled(false);

    ui->labelFile->setVisible(canLoadData);
    ui->infoLabelFile->setVisible(canLoadData);
    ui->infoLabelFile->setToolTip(NULL);
    ui->cmdBrowseFile->setVisible(canLoadData);
    ui->lblFileName->setVisible(canLoadData);
    ui->labelFile->setText(tr("<No file - load a file using the \"...\" button >"));

    setAcceptDrops(canLoadData);

    ui->dataTable->setSortingEnabled(true);
    ui->graphicsView->setScene(&_scene);
    timerActive = false;
    connect(&shotTimer, SIGNAL(timeout()), this, SLOT(timedUpdatePositions()));
    connect(&controller, SIGNAL(recalc()), this, SLOT(startTimer()));

    ui->radiusSlider->setMinimum(10);
    ui->radiusSlider->setMaximum(400);
    ui->radiusSlider->setValue(controller.radius());
    ui->forceSlider->setMinimum(1);
    ui->forceSlider->setMaximum(40);
    ui->forceSlider->setValue(controller.force());
    //---
    ui->stiffnessSlider->setMinimum(1);
    ui->stiffnessSlider->setMaximum(100);
    ui->stiffnessSlider->setValue(controller.stiffness() * 100);
    ui->dampingSlider->setMinimum(1);
    ui->dampingSlider->setMaximum(100);
    ui->dampingSlider->setValue(controller.damping() * 100);
    ui->springLengthSlider->setMinimum(20);
    ui->springLengthSlider->setMaximum(200);
    ui->springLengthSlider->setValue(controller.springsLength());

    feedNewData(dataList);
    started = true;

    if(newCanLoadData) {
        QTimer::singleShot(200, this, SLOT(on_cmdBrowseFile_clicked()));
    }
    QToolButton *button = new QToolButton();
    QIcon icon(":/save-as/images/document-save-as.png");
    button->setAutoRaise(true);
    button->setIcon(icon);
    button->setToolTip(tr("Export data to file."));
    ui->tabWidget->setCornerWidget(button);
    connect(button, SIGNAL(clicked()), this, SLOT(onExportCmd()));
}

NodesRelationsDialog::~NodesRelationsDialog()
{
    resetData();
    delete ui;
}

void NodesRelationsDialog::resetData()
{
    inputFileName = "" ;
    controller.deleteCurrentData();
    foreach(TagNode * nd, nodes) {
        delete nd ;
    }
    nodes.clear();
}

void NodesRelationsDialog::feedNewData(QList<TagNode*> &dataList)
{
    controller.start(dataList);
    controller.loadDataList(ui->dataTable);
    if(!shotTimer.isActive()) {
        timerActive = true ;
        shotTimer.start(timerInterval);
    }
}

QGraphicsScene *NodesRelationsDialog::scene()
{
    return &_scene;
}

void NodesRelationsDialog::timedUpdatePositions()
{
    if(!controller.timedTick()) {
        shotTimer.stop();
        timerActive = false;
    }
}

void NodesRelationsDialog::startTimer()
{
    if(!shotTimer.isActive()) {
        shotTimer.start(timerInterval);
        timerActive = true ;
    }
}

//---------------------------------------------------------------------------------
bool NodesRelationsDialog::loadNodesFromFile(QIODevice *inputDevice, const QString &newInputFileName)
{
    resetData();
    QHash<QString, TagNode*> newNodes;
    NodesSax handler(&newNodes);
    QXmlSimpleReader reader;
    reader.setFeature("http://xml.org/sax/features/namespaces", false);
    reader.setFeature("http://xml.org/sax/features/namespace-prefixes", true);
    reader.setContentHandler(&handler);
    reader.setErrorHandler(&handler);

    if(!inputDevice->open(QIODevice::ReadOnly | QIODevice::Text)) {
        Utils::error(tr("An error occurred opening the file."));
        return false ;
    }
    bool isOk = true ;
    QXmlInputSource xmlInput(inputDevice);
    if(!reader.parse(xmlInput)) {
        isOk = false  ;
    }
    inputDevice->close();
    inputFileName = newInputFileName;
    if(!isOk) {
        Utils::error(tr("An error occurred loading data."));
    }
    nodes.append(newNodes.values());
    if(nodes.isEmpty()) {
        Utils::error(tr("No data found in the file."));
    }
    feedNewData(nodes);
    return true ;
}

//---------------------------------------------------------------------------------

void NodesRelationsDialog::on_cmdZoom1_clicked()
{
    ui->graphicsView->resetTransform();
}

void NodesRelationsDialog::on_cmdZoomIn_clicked()
{
    ui->graphicsView->scale(1.1, 1.1);
}

void NodesRelationsDialog::on_cmdZoomOut_clicked()
{
    ui->graphicsView->scale(0.9, 0.9);
}

void NodesRelationsDialog::on_cmdReset_clicked()
{
    ui->chkViewOnlySelected->setChecked(false);
    controller.resetData();
}

void NodesRelationsDialog::on_forceSlider_valueChanged()
{
    if(started) {
        controller.setForce(ui->forceSlider->value());
    }
}

void NodesRelationsDialog::on_radiusSlider_valueChanged()
{
    if(started) {
        controller.setRadius(ui->radiusSlider->value());
    }
}

void NodesRelationsDialog::on_stiffnessSlider_valueChanged()
{
    if(started) {
        controller.setStiffness(ui->stiffnessSlider->value() / 100);
    }
}

void NodesRelationsDialog::on_dampingSlider_valueChanged()
{
    if(started) {
        controller.setDamping(ui->dampingSlider->value() / 100);
    }
}

void NodesRelationsDialog::on_springLengthSlider_valueChanged()
{
    if(started) {
        controller.setSpringsLength(ui->springLengthSlider->value());
    }
}

void NodesRelationsDialog::on_chkViewOnlySelected_clicked()
{
    if(started) {
        controller.setViewOnlySelectedMode(ui->chkViewOnlySelected->isChecked());
    }
}

//---------------------------------------------------------------------------------

//------------------------- region(file management) ---
void NodesRelationsDialog::dragEnterEvent(QDragEnterEvent *event)
{
    if(!canLoadData) {
        return;
    }

    if(event->mimeData()->hasFormat("text/uri-list"))
        event->acceptProposedAction();
}

void NodesRelationsDialog::dropEvent(QDropEvent *event)
{
    if(!canLoadData) {
        return;
    }

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
            if(!loadFile(filePath)) {
                event->ignore();
                return ;
            }
        }
        event->acceptProposedAction();
    }
}

void NodesRelationsDialog::on_cmdBrowseFile_clicked()
{
    if(!canLoadData) {
        return;
    }
    QString filePath = QFileDialog::getOpenFileName(this, tr("Open File"),
                       QXmlEditData::sysFilePathForOperation(inputFileName), Utils::getFileFilterForOpenFile());
    if(!filePath.isEmpty()) {
        loadFile(filePath);
    }
}


bool NodesRelationsDialog::loadFile(const QString &filePath)
{
    if(filePath.isEmpty()) {
        Utils::errorFilePathInvalid(this);
        return false ;
    }
    //--- set data
    resetData();
    QFile file(filePath);
    if(!loadNodesFromFile(&file, filePath)) {
        Utils::errorAccessingFile(this);
        return false;
    }
    ui->labelFile->setText(filePath);
    //-- make tooltip
    QFileInfo fileInfo(filePath);
    qint64 fileSize = fileInfo.size();
    QDateTime dtLastMod = fileInfo.lastModified();
    QString dateMod = dtLastMod.toString(Qt::DefaultLocaleLongDate);
    QString dateAccessed = fileInfo.lastRead().toString(Qt::DefaultLocaleLongDate);
    QString toolTip = tr("%1\n  last accessed on %2\n  last updated on %3\n  size %4").arg(filePath).arg(dateAccessed).arg(dateMod).arg(Utils::getSizeForPresentation(fileSize));
    ui->infoLabelFile->setToolTip(toolTip);
    ui->infoLabelFile->setVisible(true);

    return true ;
}


NodesRelationsController* NodesRelationsDialog::getController()
{
    return &controller;
}

QTableWidget *NodesRelationsDialog::getTableWidget()
{
    return ui->dataTable ;
}

void NodesRelationsDialog::saveStatisticsToStream(QTextStream &outStream)
{
    controller.saveDataToStream(outStream);
}

void NodesRelationsDialog::onExportCmd()
{
    QString filePath = QFileDialog::getSaveFileName(this, tr("Export Statistics"),
                       QXmlEditData::sysFilePathForOperation(_saveStatsPath), tr("DAT files (*.dat);;All files (*)"));

    if(filePath.isEmpty()) {
        return ;
    }
    _saveStatsPath = filePath ;
    bool isOK = false ;
    QFile data(_saveStatsPath);
    if(data.open(QFile::WriteOnly | QFile::Truncate)) {
        QTextStream outStream(&data);
        QDateTime now = QDateTime::currentDateTime();
        outStream << tr("Statistics on %1 (ISO %2)\n").arg(now.toString(Qt::TextDate)).arg(now.toString(Qt::ISODate));
        if(!inputFileName.isEmpty()) {
            outStream << tr(" for file '%1'\n").arg(inputFileName);
        }
        outStream << tr("\n------\n");
        controller.saveDataToStream(outStream);
        outStream << tr("\n------\n");
        outStream.flush();
        data.flush();
        data.close();
        if(data.error() == QFile::NoError) {
            isOK = true;
        }
        data.close();
    }
    if(!isOK) {
        Utils::error(this, tr("Error writing data."));
    }
}
