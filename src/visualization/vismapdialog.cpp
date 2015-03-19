/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2012 by Luca Bellonda and individual contributors       *
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

//#define LOG_CONSOLE 1

#include "xmlEdit.h"

#include "vismapdialog.h"
#include "ui_vismapdialog.h"
#include "utils.h"
#include "visdatasax.h"
#include "extraction/extractfragmentsdialog.h"
#include "choosecolormap.h"
#include "modules/graph/nodesrelationsdialog.h"
#include "config.h"
#include "qxmleditdata.h"

#include <QXmlSimpleReader>
#include <QFileDialog>
#include <QTimer>
#include <QTextStream>
#include <QDateTime>
#include <QUrl>

VisMapDialog::VisMapDialog(QXmlEditData *newData, QWidget *parent, const QString &fileName) :
    QDialog(parent),
    _colorMap(tr("Color Map")),
    _stdColorMap(tr("Standard Color Map")),
    _grayColorMap("Gray level Color Map"),
    ui(new Ui::VisMapDialog)
{
    _isAutoDelete = false ;
    _appData = newData ;
    _dataRoot = NULL ;
    ui->setupUi(this);
    ui->fileName->setText(tr("--- No file ---"));

    ui->cmapWidget->setColorMap(&_stdColorMap);
    ui->dataWidget->setColorMap(&_stdColorMap);
    _filePath = fileName ;
    ui->exportStatsCmd->setEnabled(false);
    ui->cmdViewGraph->setEnabled(false);
    ui->checkAnalyzeNodes->setChecked(Config::getBool(Config::KEY_VIEW_OPTION_ANALYZENODES, false));

    ui->comboLoudness->addItem(QString("None"), QVariant(DataWidget::NoLoudness));
    ui->comboLoudness->addItem(QString("Grow"), QVariant(DataWidget::LoudnessMax));
    ui->comboLoudness->addItem(QString("Dilate"), QVariant(DataWidget::LoudnessDilate));
    ui->comboLoudness->setCurrentIndex(0);

    ui->visType->addItem(QString("Size"), QVariant(DataWidget::Size));
    ui->visType->addItem(QString("Attributes"), QVariant(DataWidget::Attributes));
    ui->visType->addItem(QString("Elements"), QVariant(DataWidget::Elements));
    ui->visType->addItem(QString("Structure"), QVariant(DataWidget::Distribution));
    ui->visType->addItem(QString("Payload"), QVariant(DataWidget::Payload));

    ui->sliceLevel->setEnabled(false);
    ui->visType->setCurrentIndex(0);
#ifndef QWT_PLOT3D
    ui->threeD->setVisible(false);
    ui->threeD->setEnabled(false);
#endif

    calcVerticalPosition();
    setAcceptDrops(true);
    if(! fileName.isEmpty()) {
        QTimer::singleShot(200, this, SLOT(onLoadFile()));
    }
}

VisMapDialog::~VisMapDialog()
{
    delete ui;
    newData(NULL);
    clearTagNodes();
}

void VisMapDialog::setAutoDelete()
{
    _isAutoDelete = true ;
}

void VisMapDialog::closeEvent(QCloseEvent * event)
{
    event->accept();
    if(_isAutoDelete) {
        deleteLater();
    }
}

void VisMapDialog::newData(ElementBase *newRoot)
{
    if(NULL != _dataRoot) {
        delete _dataRoot;
    }
    _dataRoot = newRoot ;
}

void VisMapDialog::clearTagNodes()
{
    foreach(TagNode * tagNode, _tagNodes) {
        delete tagNode;
    }
    _tagNodes.clear();
}


void VisMapDialog::onLoadFile()
{
    loadFile(_filePath);
}

void VisMapDialog::loadFile(const QString &fileName)
{
    if(!fileName.isEmpty()) {
        setEnabled(false);
        clearTagNodes();
        QHash<QString, TagNode*> *nodes = NULL ;
        if(ui->checkAnalyzeNodes->isChecked()) {
            nodes = &_tagNodes;
        }
        VisDataSax handler(&names, nodes);
        QXmlSimpleReader reader;
        reader.setFeature("http://xml.org/sax/features/namespaces", false);
        reader.setFeature("http://xml.org/sax/features/namespace-prefixes", true);
        reader.setContentHandler(&handler);
        reader.setErrorHandler(&handler);

        QFile file(fileName);
        if(!file.open(QFile::ReadOnly | QFile::Text)) {
            Utils::error(tr("An error occurred opening the file."));
            return ;
        }
        _filePath = fileName ;

        bool isOk = true ;
        QXmlInputSource xmlInput(&file);
        if(!reader.parse(xmlInput)) {
            isOk = false  ;
        }
        file.close();
        if(!isOk) {
            Utils::error(tr("An error occurred loading data."));
        }
        ui->dataWidget->freeze();
        ui->dataWidget->setData(NULL);
        _dataMap.resetData();
        if(isOk) {
            newData(handler.root);
            _dataMap.calculate(handler.root);
            calcSize(handler.root);
            recalc();
            calcSlice(1);
            displayNumbers();
            ui->fileName->setText(_filePath);
            ui->dataWidget->setData(handler.root);
            ui->dataWidget->setDataMap(&_dataMap);
            ui->dataWidget->unfreeze();
            ui->sliceLevel->clear();
            int levels = ui->dataWidget->levels();
            for(int i = 0 ; i < levels ; i ++) {
                ui->sliceLevel->addItem(QString("%1").arg(i + 1), QVariant(i));
                if(1 == i) {
                    ui->sliceLevel->setCurrentIndex(1);
                }
            }
            ui->exportStatsCmd->setEnabled(true);
            ui->cmdViewGraph->setEnabled(_tagNodes.count() > 0);
            _appData->notifier()->notify(NULL, tr("Data ready."));
        }
        setEnabled(true);
    }
}

void VisMapDialog::on_loadFile_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(
                           this, tr("Open File"),
                           QXmlEditData::sysFilePathForOperation(_filePath),
                           Utils::getFileFilterForOpenFile()
                       );
    if(!filePath.isEmpty()) {
        loadFile(filePath);
    }
}

void VisMapDialog::calcSize(ElementBase *e)
{
    e->totalSize += e->size;
    e->totalPayload += e->payload;
    e->totalAttributesCount += e->attributesCount;
    e->totalChildrenCount = e->childrenCount;
    _dataMap.numElements ++;
    ElementBase * child = e->firstChild;
    while(child != NULL) {

        calcSize(child);
        e->totalAttributesCount += child->totalAttributesCount;
        e->totalChildrenCount += child->totalChildrenCount;
        e->totalSize += child->totalSize;
        e->totalPayload += child->totalPayload;
        child = child->next;
    }

    if(_dataMap.maxSize < e->size) {
        _dataMap.maxSize = e->size ;
    }
    if(_dataMap.maxChildrenCount < e->childrenCount) {
        _dataMap.maxChildrenCount = e->childrenCount;
    }
    if(_dataMap.maxAttributesCount < e->attributesCount) {
        _dataMap.maxAttributesCount = e->attributesCount ;
    }
    if(_dataMap.maxPayload < e->payload) {
        _dataMap.maxPayload = e->payload;
    }
}

void VisMapDialog::on_zoom_valueChanged(int value)
{
    ui->dataWidget->setZoom(value);
    calcVerticalPosition();
}


void VisMapDialog::on_sliceLevel_currentIndexChanged(int index)
{
    if(index >= 0) {
        QVariant data = ui->sliceLevel->itemData(index);
        int newSlice = data.toInt();
        ui->dataWidget->setSlice(newSlice);
        recalc();
        calcSlice(newSlice);
        displayNumbers();
    }
}

void VisMapDialog::recalc()
{
    _summary.reset();
    _summary.totalElements = _dataMap.numElements;
    _summary.totalSize = _dataMap._root->totalSize;
    _summary.levels = _dataMap.numColumns;
    _summary.totalAttributes = _dataMap._root->totalAttributesCount;
    _summary.totalPayload = _dataMap._root->totalPayload;

    _summary.maxAttributes = _dataMap.maxAttributesCount ;
    _summary.maxChildren = _dataMap.maxChildrenCount ;
    _summary.maxSize = _dataMap.maxSize ;
    _summary.maxPayload = _dataMap.maxPayload ;
}

void VisMapDialog::displayNumbers()
{
    ui->numbers->setRowCount(0);
    newNumbersItem(tr("Fragments"), QString("%1").arg(_summary.totalFragments));
    newNumbersItem(tr("Size"), QString("%1").arg(_summary.totalSize));
    newNumbersItem(tr("Levels"), QString("%1").arg(_summary.levels));
    newNumbersItem(tr("Elements"), QString("%1").arg(_summary.totalElements));
    newNumbersItem(tr("Attributes"), QString("%1").arg(_summary.totalAttributes));
    newNumbersItem(tr("Payload"), QString("%1").arg(_summary.totalPayload));

    newNumbersItem(tr("Max attributes"), QString("%1").arg(_summary.maxAttributes));
    newNumbersItem(tr("Max children"), QString("%1").arg(_summary.maxChildren));
    newNumbersItem(tr("Max size"), QString("%1").arg(_summary.maxSize));
    newNumbersItem(tr("Max Payload"), QString("%1").arg(_summary.maxPayload));


    /*newNumbersItem(tr("Mean size per fr."), QString("%1").arg(_summary.sizePerFragmentMean));
    newNumbersItem(tr("Median size per fr."), QString("%1").arg(_summary.sizePerFragmentMedian));
    newNumbersItem(tr("Mean element # per fr."), QString("%1").arg(_summary.elementsPerFragmentMean));
    newNumbersItem(tr("Mean element # per fr."), QString("%1").arg(_summary.elementsPerFragmentMean));
    newNumbersItem(tr("Median attributes # per fr."), QString("%1").arg(_summary.attributesPerFragmentMean));
    newNumbersItem(tr("Median attributes # per fr."), QString("%1").arg(_summary.attributesPerFragmentMedian));*/
}

void VisMapDialog::newNumbersItem(const QString &label, const QString &data)
{
    QTableWidgetItem *newLabel = new QTableWidgetItem(label);
    int row = ui->numbers->rowCount();
    ui->numbers->setRowCount(row + 1);
    ui->numbers->setItem(row, 0, newLabel);
    QTableWidgetItem *newData = new QTableWidgetItem(data);
    ui->numbers->setItem(row, 1, newData);
}

void VisMapDialog::on_verticalPosition_valueChanged(int value)
{
    ui->dataWidget->setWindowPerc(value);
}

void VisMapDialog::on_visType_currentIndexChanged(int index)
{
    QVariant newVisType = ui->visType->itemData(index);
    DataWidget::EMeasurementType newType = (DataWidget::EMeasurementType)newVisType.toInt();
    ui->dataWidget->setVisType(newType);
    bool enableSliceCombo = false;
    if(DataWidget::Distribution == newType) {
        enableSliceCombo = true ;
    }
    ui->sliceLevel->setEnabled(enableSliceCombo);
}

void VisMapDialog::on_cumulative_stateChanged(int /*state*/)
{
    if(ui->cumulative->checkState() == Qt::Checked) {
        ui->dataWidget->setCumulative(true);
    } else {
        ui->dataWidget->setCumulative(false);
    }
}

void VisMapDialog::on_comboLoudness_currentIndexChanged(int index)
{
    QVariant newLoudnessType = ui->comboLoudness->itemData(index);
    DataWidget::ELoudness newLoudness = (DataWidget::ELoudness)newLoudnessType.toInt();
    ui->dataWidget->setLoudness(newLoudness);
}


void VisMapDialog::calcVerticalPosition()
{
    int zoom = ui->zoom->value();
    if(zoom < 1) {
        zoom = 1 ;
    }
    int pageStep = (int)(100.0 / zoom) ;
    if(pageStep > 100) {
        pageStep = 100 ;
    } else if(pageStep < 0)  {
        pageStep = 0 ;
    }
    int maximum = 100 - pageStep ;
    // documentLen is 100 by definition
    ui->verticalPosition->setMaximum(maximum) ;
    // Zoom is the percentual of the document
    ui->verticalPosition->setPageStep(pageStep);
    //TRACEQ(QString("Doc Len (page_step) %1 max val %2").arg(pageStep).arg(maximum));
}

ElementBase *VisMapDialog::getElement(const int x, const int y)
{
    if((y >= 0) && (y < _dataMap.rows.count())) {
        VisDataRow *row = _dataMap.rows.at(y);
        if((x >= 0) && (row->_numColumns > x)) {
            return row->_columns[x];
        }
    }
    return NULL ;
}


void VisMapDialog::calcSlice(const int nSlice)
{
    QSet<int> slices;
    int rowMax = _dataMap.rows.size();
    for(int y = 0 ; y < rowMax ; y ++) {
        ElementBase *el = getElement(nSlice, y);
        if(NULL != el) {
            slices.insert(el->id);
        }
    }
    _summary.totalFragments = slices.size();
}

void VisMapDialog::on_exportStatsCmd_clicked()
{
    QString filePath = QFileDialog::getSaveFileName(this, tr("Export Statistics"),
                       QXmlEditData::sysFilePathForOperation(_saveStatsPath), tr("DAT files (*.dat);;All files (*);;"));

    if(filePath.isEmpty()) {
        return ;
    }
    _saveStatsPath = filePath ;
    bool isOK = false ;
    QFile data(_saveStatsPath);
    if(data.open(QFile::WriteOnly | QFile::Truncate)) {
        QTextStream outStream(&data);
        QDateTime now = QDateTime::currentDateTime();
        outStream << tr("Statistics on %1 ( ISO %2)\n").arg(now.toString(Qt::TextDate)).arg(now.toString(Qt::ISODate));
        outStream << tr(" for file '%1'\n").arg(_filePath);
        outStream << tr(" total fragments '%1'\n").arg(_summary.totalFragments) ;
        outStream << tr(" total attributes '%1'\n").arg(_summary.totalAttributes) ;
        outStream << tr(" total elements '%1'\n").arg(_summary.totalElements) ;
        outStream << tr(" total size '%1'\n").arg(_summary.totalSize) ;
        outStream << tr(" total payload '%1'\n").arg(_summary.totalPayload) ;
        outStream << tr(" levels '%1'\n").arg(_summary.levels) ;
        outStream << tr("\n------\n");

        ui->dataWidget->writeDetails(outStream);
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


void VisMapDialog::on_dataWidget_extractFragment(const int fragment, const int depth)
{
    // save settings for extraction
    ExtractionOperation::saveSettingsForExtractionFragmentNumber(_filePath, fragment, depth);

    ExtractResults extractResult;
    ExtractFragmentsDialog dialog(&extractResult, this);
    dialog.exec();
}


void VisMapDialog::on_extractFragment_clicked()
{
    Utils::message(this, tr("To extract a fragment, move the nouse over the data in the image and use the contextual menu."));
}

void VisMapDialog::dropEvent(QDropEvent *event)
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
            loadFile(filePath);
        }
        event->acceptProposedAction();
    }
}

void VisMapDialog::dragEnterEvent(QDragEnterEvent *event)
{
    if(event->mimeData()->hasFormat("text/uri-list"))
        event->acceptProposedAction();
}


void VisMapDialog::on_changeColorMap_clicked()
{
    QList<ColorMap *>colorMaps;
    colorMaps.append(&_stdColorMap);
    colorMaps.append(&_colorMap);
    colorMaps.append(&_grayColorMap);
    ChooseColorMap chooseColorMap(colorMaps);
    if(chooseColorMap.exec() == QDialog::Accepted) {
        if(NULL != chooseColorMap.choosenMap) {
            ui->cmapWidget->setColorMap(chooseColorMap.choosenMap);
            ui->dataWidget->setColorMap(chooseColorMap.choosenMap);
        }
    }
}

void VisMapDialog::on_threeD_stateChanged(int /*state*/)
{
#ifdef QWT_PLOT3D
    if(ui->threeD->checkState() == Qt::Checked) {
        ui->dataWidget->set3D(true);
    } else {
        ui->dataWidget->set3D(false);
    }
#endif
}

void VisMapDialog::on_cmdViewGraph_clicked()
{
    if(_tagNodes.count() > 0) {
        QList<TagNode*> nodesList ;
        nodesList.append(_tagNodes.values());
        NodesRelationsDialog dialog(false, nodesList);
        dialog.exec();
    } else {
        Utils::error(this, tr("No data to show."));
    }
}

void VisMapDialog::on_checkAnalyzeNodes_stateChanged(int /*newState*/)
{
    Config::saveBool(Config::KEY_VIEW_OPTION_ANALYZENODES, ui->checkAnalyzeNodes->isChecked());
}
